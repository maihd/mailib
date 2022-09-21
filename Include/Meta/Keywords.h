#pragma once

#ifndef __cplusplus
#define constexpr       static const
#endif

#ifndef __cplusplus
#define nullptr         0
#endif

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
