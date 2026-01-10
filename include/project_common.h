#pragma once
#ifndef PROJECT_COMMON_H
#define PROJECT_COMMON_H

#if defined(_MSC_VER)
#define TODO(msg) __pragma(message(__FILE__ "(" STRINGIZE(__LINE__) "): TODO: " msg))
#else // #if defined(_MSC_VER)
#define TODO(msg) _Pragma("message(\"TODO: " msg "\")")
#endif // #if defined(_MSC_VER)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#if defined(_DEBUG) || !defined(NDEBUG)
#define IS_DEBUG 1
#define IS_RELEASE 0
#else // #if defined(_DEBUG) || !defined(NDEBUG)
#define IS_DEBUG 0
#define IS_RELEASE 1
#endif // #if defined(_DEBUG) || !defined(NDEBUG)

#if IS_DEBUG
#define DEBUG_ONLY(code) code
#else // #if IS_DEBUG
#define DEBUG_ONLY(code)
#endif // #if IS_DEBUG

#endif // PROJECT_COMMON_H