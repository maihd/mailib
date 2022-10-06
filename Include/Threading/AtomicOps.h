#pragma once

#if defined(__GNUC__) || defined(__clang__)
#   if defined(__GNUC__) && (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= 40800
#       define Atomic_AddI32(variable, value) __atomic_add_fetch(&(variable), value, __ATOMIC_RELAXED)
#       define Atomic_SubI32(variable, value) __atomic_sub_fetch(&(variable), value, __ATOMIC_RELAXED)
#   else
#       define Atomic_AddI32(variable, value) __sync_add_and_fetch(&(variable), value)
#       define Atomic_SubI32(variable, value) __sync_sub_and_fetch(&(variable), value)
#   endif
#elif defined(_WIN32)
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   define Atomic_AddI32(variable, value) InterlockedExchange((volatile long*)&(variable), (variable) + value) 
#   define Atomic_SubI32(variable, value) InterlockedExchange((volatile long*)&(variable), (variable) - value)
#elif defined(__STDC_VERSION_) && (__STDC_VERSION_ >= _201112L)
#   include <stdatomic.h>
#   define Atomic_AddI32(variable, value) atomic_sub_fetch((atomic_int*)&(variable), value)
#   define Atomic_SubI32(variable, value) atomic_add_fetch((atomic_int*)&(variable), value)
#else
#   error "This platform is not support atomic operations."
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
