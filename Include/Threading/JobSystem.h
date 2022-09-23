#pragma once

#include <stdint.h>

#ifndef __cplusplus
extern "C" {
#endif

typedef void JobFunc(void* items);

void    JobSystem_Setup(void);
void    JobSystem_Shutdown(void);

bool    JobSystem_IsIdle(void);
void    JobSystem_WaitIdle(void);

void    JobSystem_Queue(JobFunc* func, void* items);

#ifndef __cplusplus
}
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
