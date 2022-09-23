#pragma once

#include <stdbool.h>
#include <Threading/ThreadTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void            Threading_Setup(void);
void            Threading_Shutdown(void);
bool            Threading_IsMainThread(void);
uint32_t        Threading_GetMainThreadId(void);

int32_t         Threading_GetCpuCores(void);
int64_t         Threading_GetCpuTicks(void);
int64_t         Threading_GetCpuFrequency(void);

void            Threading_Sleep(int64_t miliseconds);
void            Threading_MicroSleep(int64_t microseconds);

bool            Thread_Run(ThreadFunc* func, void* data, ThreadDesc* outThreadDesc);
void            Thread_Join(ThreadDesc* threadDesc);

void            ThreadMutex_Init(ThreadMutex* mutex);
void            ThreadMutex_Release(ThreadMutex* mutex);

void            ThreadMutex_Lock(ThreadMutex* mutex);
void            ThreadMutex_Unlock(ThreadMutex* mutex);
bool            ThreadMutex_TryLock(ThreadMutex* mutex);

void            ThreadSignal_Init(ThreadSignal* signal);
void            ThreadSignal_Release(ThreadSignal* signal);

void            ThreadSignal_Wait(ThreadSignal* signal, ThreadMutex* mutex);
void            ThreadSignal_Signal(ThreadSignal* signal);
void            ThreadSignal_Broadcast(ThreadSignal* signal);

void            ThreadSemaphore_Init(ThreadSemaphore* semaphore, int32_t value);
void            ThreadSemaphore_Release(ThreadSemaphore* semaphore);

void            ThreadSemaphore_Post(ThreadSemaphore* semaphore);
void            ThreadSemaphore_Wait(ThreadSemaphore* semaphore);
bool            ThreadSemaphore_TryWait(ThreadSemaphore* semaphore);

#ifdef __cplusplus
}
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
