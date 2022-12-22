#pragma once

#include <stdint.h>

typedef int32_t (ThreadFunc)(void*);

typedef struct ThreadDesc
{
    uint32_t        id;
    uintptr_t       handle;

    //void*           stack;
    //const char*     name;
    //int32_t         prefered_core;
    //int32_t         migrate_enabled;

    ThreadFunc*     func;
    void*           data;
} ThreadDesc;

typedef struct ThreadMutex
{
    uint8_t         internal[40];
} ThreadMutex;

typedef struct ThreadSignal
{
    uint8_t         internal[8];
} ThreadSignal;

typedef struct ThreadSemaphore
{
    int32_t         count;
    ThreadMutex     mutex;
    ThreadSignal    signal;
} ThreadSemaphore;

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
