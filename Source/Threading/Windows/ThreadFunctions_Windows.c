#include <assert.h>

#define VC_EXTRA_CLEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <sysinfoapi.h>

#include <Meta/Keywords.h>
#include <Threading/ThreadFunctions.h>

static DWORD gMainThreadId;

void Threading_Setup(void)
{
    gMainThreadId = GetCurrentThreadId();
}

void Threading_Shutdown(void)
{
    assert(gMainThreadId == GetCurrentThreadId());

    gMainThreadId = 0;
}

bool Threading_IsMainThread(void)
{
    return gMainThreadId == GetCurrentThreadId();
}

uint32_t Threading_GetMainThreadId(void)
{
    return (uint32_t)gMainThreadId;
}

int Threading_GetCpuCores(void)
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    return systemInfo.dwNumberOfProcessors;
}

int64_t Threading_GetCpuTicks(void)
{
    LARGE_INTEGER value;
    return QueryPerformanceCounter(&value) ? (int64_t)value.QuadPart : (int64_t)GetTickCount64();
}

int64_t Threading_GetCpuFrequency(void)
{
    static int64_t savedValue;
    if (savedValue > 0)
    {
        return savedValue;
    }

    LARGE_INTEGER value;
    return QueryPerformanceFrequency(&value) ? (savedValue = (int64_t)value.QuadPart) : (savedValue = 1000);
}

void Threading_Sleep(int64_t miliseconds)
{
    Sleep((DWORD)(miliseconds & MAXDWORD32));
}

void Threading_MicroSleep(int64_t microseconds)
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

bool Thread_Run(ThreadFunc* func, void* data, ThreadDesc* outThread)
{
    DWORD threadId;
    HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, data, 0, &threadId);

    if (outThread)
    {
        outThread->id = (uint32_t)threadId;
        outThread->func = func;
        outThread->data = data;
        outThread->handle = (uintptr_t)threadHandle;
    }

    return threadHandle != NULL;
}

void Thread_Join(ThreadDesc* thread)
{
    HANDLE threadHandle = thread ? (HANDLE)thread->handle : GetCurrentThread();
    WaitForSingleObject(threadHandle, INFINITE);
    CloseHandle(threadHandle);

    if (thread)
    {
        thread->id = 0;
        thread->handle = 0;
    }
}

void ThreadMutex_Init(ThreadMutex* mutex)
{
    InitializeCriticalSection((LPCRITICAL_SECTION)mutex);
}

void ThreadMutex_Release(ThreadMutex* mutex)
{
    DeleteCriticalSection((LPCRITICAL_SECTION)mutex);
}

void ThreadMutex_Lock(ThreadMutex* mutex)
{
    EnterCriticalSection((LPCRITICAL_SECTION)mutex);
}

void ThreadMutex_Unlock(ThreadMutex* mutex)
{
    LeaveCriticalSection((LPCRITICAL_SECTION)mutex);
}

bool ThreadMutex_TryLock(ThreadMutex* mutex)
{
    return TryEnterCriticalSection((LPCRITICAL_SECTION)mutex);
}

void ThreadSignal_Init(ThreadSignal* signal)
{
    InitializeConditionVariable((CONDITION_VARIABLE*)signal);
}

void ThreadSignal_Release(ThreadSignal* signal)
{
    __unused(signal);
}

void ThreadSignal_Wait(ThreadSignal* signal, ThreadMutex* mutex)
{
    if (SleepConditionVariableCS((CONDITION_VARIABLE*)signal, (CRITICAL_SECTION*)mutex, INFINITE))
    {
        //process::abort();
    }
}

void ThreadSignal_Signal(ThreadSignal* signal)
{
    WakeConditionVariable((CONDITION_VARIABLE*)signal);
}

void ThreadSignal_Broadcast(ThreadSignal* signal)
{
    WakeAllConditionVariable((CONDITION_VARIABLE*)signal);
}

void ThreadSemaphore_Init(ThreadSemaphore* semaphore, int32_t count)
{
    semaphore->count = count;
    ThreadMutex_Init(&semaphore->mutex);
    ThreadSignal_Init(&semaphore->signal);
}

void ThreadSemaphore_Release(ThreadSemaphore* semaphore)
{
    semaphore->count = 0;
    ThreadMutex_Release(&semaphore->mutex);
    ThreadSignal_Release(&semaphore->signal);
}

void ThreadSemaphore_Post(ThreadSemaphore* semaphore)
{
    ThreadMutex_Lock(&semaphore->mutex);

    while (semaphore->count == 1)
    {
        ThreadSignal_Wait(&semaphore->signal, &semaphore->mutex);
    }
    semaphore->count++;

    ThreadMutex_Unlock(&semaphore->mutex);
}

void ThreadSemaphore_Wait(ThreadSemaphore* semaphore)
{
    ThreadMutex_Lock(&semaphore->mutex);

    while (semaphore->count <= 0)
    {
        ThreadSignal_Wait(&semaphore->signal, &semaphore->mutex);
    }
    semaphore->count--;

    ThreadMutex_Unlock(&semaphore->mutex);
}

bool ThreadSemaphore_TryWait(ThreadSemaphore* semaphore)
{
    if (!ThreadMutex_TryLock(&semaphore->mutex))
    {
        return false;
    }

    if (semaphore->count == 0)
    {
        ThreadMutex_Unlock(&semaphore->mutex);
        return false;
    }

    semaphore->count--;
    ThreadMutex_Unlock(&semaphore->mutex);
    return true;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
