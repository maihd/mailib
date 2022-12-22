#include <assert.h>

#define VC_EXTRA_CLEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <sysinfoapi.h>

#include <meta/keywords.h>
#include <threading/thread_functions.h>

static DWORD g_main_thread_id;

void threading_setup(void)
{
    g_main_thread_id = GetCurrentThreadId();
}

void threading_shutdown(void)
{
    assert(g_main_thread_id == GetCurrentThreadId());

    g_main_thread_id = 0;
}

bool threading_is_main_thread(void)
{
    return g_main_thread_id == GetCurrentThreadId();
}

uint32_t threading_get_main_thread_id(void)
{
    return (uint32_t)g_main_thread_id;
}

int threading_get_cpu_cores(void)
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    return systemInfo.dwNumberOfProcessors;
}

int64_t threading_get_cpu_ticks(void)
{
    LARGE_INTEGER value;
    return QueryPerformanceCounter(&value) ? (int64_t)value.QuadPart : (int64_t)GetTickCount64();
}

int64_t threading_get_cpu_frequency(void)
{
    static int64_t savedValue;
    if (savedValue > 0)
    {
        return savedValue;
    }

    LARGE_INTEGER value;
    return QueryPerformanceFrequency(&value) ? (savedValue = (int64_t)value.QuadPart) : (savedValue = 1000);
}

void threading_sleep(int64_t miliseconds)
{
    Sleep((DWORD)(miliseconds & MAXDWORD32));
}

void threading_micro_sleep(int64_t microseconds)
{
    /* 'NTSTATUS NTAPI NtDelayExecution(BOOL Alerted, PLARGE_INTEGER time);' */
    /* 'typedef LONG NTSTATUS;' =)) */
    /* '#define NTAPI __stdcall' =)) */
    typedef LONG(__stdcall * NtDelayExecutionFN)(BOOL, PLARGE_INTEGER);
    
    static int done_finding;
    static NtDelayExecutionFN NtDelayExecution;
    
    if (!NtDelayExecution && !done_finding)
    {
        done_finding = 1;
        HMODULE module = GetModuleHandleA("ntdll.dll");
        const char* func = "NtDelayExecution";
        NtDelayExecution = (NtDelayExecutionFN)GetProcAddress(module, func);
    }
    
    if (NtDelayExecution)
    {
        LARGE_INTEGER times;
        times.QuadPart = (LONGLONG)(-microseconds * 10); // Timer is precise as 100 nanoseconds
        NtDelayExecution(FALSE, &times);
    }
    else
    {
        Sleep((DWORD)(microseconds / (1000)));
    }
}

bool thread_run(ThreadFunc* func, void* data, ThreadDesc* out_thread)
{
    DWORD thread_id;
    HANDLE thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, data, 0, &thread_id);

    if (out_thread)
    {
        out_thread->id      = (uint32_t)thread_id;
        out_thread->func    = func;
        out_thread->data    = data;
        out_thread->handle  = (uintptr_t)thread_handle;
    }

    return thread_handle != NULL;
}

void thread_join(ThreadDesc* thread)
{
    HANDLE thread_handle = thread ? (HANDLE)thread->handle : GetCurrentThread();
    WaitForSingleObject(thread_handle, INFINITE);
    CloseHandle(thread_handle);

    if (thread)
    {
        thread->id = 0;
        thread->handle = 0;
    }
}

void thread_mutex_init(ThreadMutex* mutex)
{
    InitializeCriticalSection((LPCRITICAL_SECTION)mutex);
}

void thread_mutex_release(ThreadMutex* mutex)
{
    DeleteCriticalSection((LPCRITICAL_SECTION)mutex);
}

void thread_mutex_lock(ThreadMutex* mutex)
{
    EnterCriticalSection((LPCRITICAL_SECTION)mutex);
}

void thread_mutex_unlock(ThreadMutex* mutex)
{
    LeaveCriticalSection((LPCRITICAL_SECTION)mutex);
}

bool thread_mutex_try_lock(ThreadMutex* mutex)
{
    return TryEnterCriticalSection((LPCRITICAL_SECTION)mutex);
}

void thread_signal_init(ThreadSignal* signal)
{
    InitializeConditionVariable((CONDITION_VARIABLE*)signal);
}

void thread_signal_release(ThreadSignal* signal)
{
    __unused(signal);
}

void thread_signal_wait(ThreadSignal* signal, ThreadMutex* mutex)
{
    if (SleepConditionVariableCS((CONDITION_VARIABLE*)signal, (CRITICAL_SECTION*)mutex, INFINITE))
    {
        //process::abort();
    }
}

void thread_signal_send(ThreadSignal* signal)
{
    WakeConditionVariable((CONDITION_VARIABLE*)signal);
}

void thread_signal_broadcast(ThreadSignal* signal)
{
    WakeAllConditionVariable((CONDITION_VARIABLE*)signal);
}

void thread_semaphore_init(ThreadSemaphore* semaphore, int32_t count)
{
    semaphore->count = count;
    thread_mutex_init(&semaphore->mutex);
    thread_signal_init(&semaphore->signal);
}

void thread_semaphore_release(ThreadSemaphore* semaphore)
{
    semaphore->count = 0;
    thread_mutex_release(&semaphore->mutex);
    thread_signal_release(&semaphore->signal);
}

void thread_semaphore_post(ThreadSemaphore* semaphore)
{
    thread_mutex_lock(&semaphore->mutex);

    while (semaphore->count == 1)
    {
        thread_signal_wait(&semaphore->signal, &semaphore->mutex);
    }
    semaphore->count++;

    thread_mutex_unlock(&semaphore->mutex);
}

void thread_semaphore_wait(ThreadSemaphore* semaphore)
{
    thread_mutex_lock(&semaphore->mutex);

    while (semaphore->count <= 0)
    {
        thread_signal_wait(&semaphore->signal, &semaphore->mutex);
    }
    semaphore->count--;

    thread_mutex_unlock(&semaphore->mutex);
}

bool thread_semaphore_try_wait(ThreadSemaphore* semaphore)
{
    if (!thread_mutex_try_lock(&semaphore->mutex))
    {
        return false;
    }

    if (semaphore->count == 0)
    {
        thread_mutex_unlock(&semaphore->mutex);
        return false;
    }

    semaphore->count--;
    thread_mutex_unlock(&semaphore->mutex);
    return true;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
