#pragma once

#if !defined(_WIN32) && !defined(__linux__) && !defined(__darwin__)
#define PLATFORM_BAREBONE
#else
#define PLATFORM_HOSTED
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
