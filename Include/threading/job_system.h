#pragma once

#include <stdint.h>

#ifndef __cplusplus
extern "C" {
#endif

typedef void JobFunc(void* items);

void    job_system_setup(void);
void    job_system_shutdown(void);

bool    job_system_is_idle(void);
void    job_system_wait_idle(void);

void    job_system_queue(JobFunc* func, void* items);

#ifndef __cplusplus
}
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
