#pragma once

#include <stdbool.h>
#include <threading/thread_types.h>

#ifdef __cplusplus
extern "C" {
#endif

void            threading_setup(void);
void            threading_shutdown(void);
bool            threading_is_main_thread(void);
uint32_t        threading_get_main_thread_id(void);

int32_t         threading_get_cpu_cores(void);
int64_t         threading_get_cpu_ticks(void);
int64_t         threading_get_cpu_frequency(void);

void            threading_sleep(int64_t miliseconds);
void            threading_micro_sleep(int64_t microseconds);

bool            thread_run(ThreadFunc* func, void* data, ThreadDesc* out_thread_desc);
void            thread_join(ThreadDesc* thread_desc);

void            thread_mutex_init(ThreadMutex* mutex);
void            thread_mutex_release(ThreadMutex* mutex);

void            thread_mutex_lock(ThreadMutex* mutex);
void            thread_mutex_unlock(ThreadMutex* mutex);
bool            thread_mutex_try_lock(ThreadMutex* mutex);

void            thread_signal_init(ThreadSignal* signal);
void            thread_signal_release(ThreadSignal* signal);

void            thread_signal_wait(ThreadSignal* signal, ThreadMutex* mutex);
void            thread_signal_send(ThreadSignal* signal);
void            thread_signal_broadcast(ThreadSignal* signal);

void            thread_semaphore_init(ThreadSemaphore* semaphore, int32_t value);
void            thread_semaphore_release(ThreadSemaphore* semaphore);

void            thread_semaphore_post(ThreadSemaphore* semaphore);
void            thread_semaphore_wait(ThreadSemaphore* semaphore);
bool            thread_semaphore_try_wait(ThreadSemaphore* semaphore);

#ifdef __cplusplus
}
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
