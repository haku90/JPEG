#pragma once
#ifndef _xDefinesH_
#define _xDefinesH_

#include <windows.h>
#include <assert.h>
#include "xLicense.h"
#include "xType.h"
#include "xLanguage.h"
#include "xCompiler.h"

//================================================================================================
// DEBUG/RELEASE section
//================================================================================================
#ifdef _DEBUG
#define X_DEBUG
#endif
#ifdef NDEBUG
#define X_RELEASE
#endif

#ifdef X_DEBUG
#define xAssert(expression) assert(expression)
#else
#define xAssert(expression)
#endif

//================================================================================================
// Architecture section
//================================================================================================
#if ( defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64) ))
#define X_ARCH_X86_64 1
#endif
#ifndef X_ARCH_X86_64
#define X_ARCH_X86_32 1
#endif

//================================================================================================
// SSE & AVX SIMD section - set your processor capabilities
//================================================================================================
#define X_USE_SSE 1
#if X_USE_SSE
#define X_SSE1   1    //Pentium III
#define X_SSE2   1    //Pentium 4    (Willamette, Northwood, Gallatin)
#define X_SSE3   1    //Pentium 4    (Prescott, Cedar Mill)
#define X_SSSE3  1    //Core 2       (Conroe, Merom)
#define X_SSE4_1 1    //Core 2       (Penryn, Wolfdale)
#define X_SSE4_2 1    //Core iX nnn  (Nehalem, Westmere)
#endif
#define X_USE_AVX 0 
#if X_USE_AVX
#define X_AVX1   0    //Core iX nnnn (Sandy Bridge, Ivy Bridge)
#define X_AVX2   0    //Core iX nnnn (Haswell, Broadwell)
#endif


//================================================================================================
// Memmory and alignment section
//================================================================================================
#define xMMXAlign   8              //MMX =  64 bit
#define xSSEAlign   16             //SSE = 128 bit
#define xAVXAlign   32             //AVX = 256 bit
#define xRegAlign   xAVXAlign      //current register size selection
#define xPageAlign  4096           //memmory page size = 4kB

#define xPelAlign   xRegAlign      //pel origin alignment
#define xDefAlign   xRegAlign

#define xAlignedMalloc(size, alignment)       _aligned_malloc((size), (alignment))
#define xAlignedCalloc(num, size, alignment)  _aligned_recalloc(nullptr, (num), (size), (alignment))
#define xAlignedFree(memmory)                 _aligned_free((memmory))

#define xMalloc(size)       xAlignedMalloc(size, xDefAlign)
#define xCalloc(num, size)  xAlignedCalloc(num, size, xDefAlign)
#define xFree(memmory)      xAlignedFree(memmory)

//================================================================================================
// OpenMP section
//================================================================================================
#define X_USE_OPENMP 0
#define X_USE_OPENMP_IN_DEBUG   0
#define X_USE_OPENMP_IN_RELEASE 1

#if X_USE_OPENMP && !X_OMP_SUPPORT
#error "Turn on OpenMP support (or change the compiler) !!!!"
#endif

#if X_USE_OPENMP && ((defined(X_DEBUG) && X_USE_OPENMP_IN_DEBUG) || (defined(X_RELEASE) && X_USE_OPENMP_IN_RELEASE))
#define X_OMP_BAYER 1
#define X_OMP_CMP   1
#define X_OMP_THRESHOLD 524288   // 0.5 Mpix
#endif


#endif //_xDefinesH_