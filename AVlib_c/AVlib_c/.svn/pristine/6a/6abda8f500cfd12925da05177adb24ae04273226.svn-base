#pragma once
#ifndef _xCompilerH_
#define _xCompilerH_

#include "xLicense.h"

#ifdef __INTEL_COMPILER
#define X_COMPILER      ICC
#define X_COMPILER_VER  __INTEL_COMPILER
#define X_COMPILER_NAME TEXT("Intel Compiler")
#endif

#ifdef __GNUC__
#define X_COMPILER      GCC
#define X_COMPILER_VER  __GNUC__
#define X_COMPILER_NAME TEXT("GNU Compiler Collection")
#endif

#ifdef __clang__
#define X_COMPILER      CLANG
#define X_COMPILER_VER  __clang_major__
#define X_COMPILER_NAME TEXT("Clang")
#endif

#ifdef __llvm__
#define X_COMPILER      LLVM
#define X_COMPILER_VER  
#define X_COMPILER_NAME TEXT("LLVM")
#endif

#ifdef __MINGW32__
#define X_COMPILER      MINGW
#define X_COMPILER_VER  _MINGW32_MAJOR_VERSION
#define X_COMPILER_NAME TEXT("MinGW 32")
#endif

#ifdef __MINGW64__
#define X_COMPILER      MINGW
#define X_COMPILER_VER  __MINGW64_VERSION_MAJOR
#define X_COMPILER_NAME TEXT("MinGW 64")
#endif

#ifndef X_COMPILER
#ifdef _MSC_VER
#define X_COMPILER      VS
#define X_COMPILER_VER  _MSC_VER
#define X_COMPILER_NAME TEXT("MS Visual Studio")
#endif
#endif

#ifndef X_COMPILER
#define X_COMPILER      UNKNOWN
#define X_COMPILER_VER  
#define X_COMPILER_NAME TEXT("unknown")
#endif

#endif //_xCompilerH_