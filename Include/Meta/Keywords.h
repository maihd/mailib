#pragma once

#ifndef __cplusplus
#define constexpr       static const
#endif

/// nullptr
/// Simple define `nullptr` keyword for C
#ifndef __cplusplus
#define nullptr         0
#endif

/// __forceinline
/// Force thoses functions is always inline
/// Instead of use gcc/clang support builtin
/// I find that __forceinline is very readable
#if !defined(_MSC_VER) && !defined(__forceinline)
#   if defined(__GNUC__)
#       define __forceinline    static __attribute__((always_inline))
#   elif defined(__cplusplus)
#       define __forceinline    static inline
#   else
#       define __forceinline    inline
#   endif
#endif

/// __unused
/// Instead of use ((void*)X) idiom, we should make an 'prebuilt' operator
/// to clear out what it do, power the readability
#ifndef __unused
#define __unused(variable) ((void*)(variable))
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
