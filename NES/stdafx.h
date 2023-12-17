#pragma once

// CRT
#include <cassert>
#include <cstdint>
#include <cstring>

// C++
#include <iostream>

// C++14
#include <memory>

// C++20
#include <filesystem>

// Macros
#if !defined(ARRAYSIZE)
#define ARRAYSIZE(arr)    (sizeof(arr)/sizeof(arr[0]))
#endif

#define STRINGIFY(token)            (#token)
#define CONCATENATE_STR(lhs, rhs)   STRINGIFY(lhs##rhs)