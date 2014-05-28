#pragma once
#ifndef _xLanguageH_
#define _xLanguageH_

#include "xLicense.h"

//================================================================================================
// OpenMP support
//================================================================================================
#if defined _OPENMP
  #define X_OMP_SUPPORT 1
  #define X_OMP_DATE _OPENMP
  #if X_OMP_DATE == 199710
    #define X_OMP_VERSION 100 //1.0
  #elif X_OMP_DATE == 200203
    #define X_OMP_VERSION 200 //2.0
  #elif X_OMP_DATE == 200805
    #define X_OMP_VERSION 300 //3.0
  #elif X_OMP_DATE == 201107
    #define X_OMP_VERSION 310 //3.1
  #elif X_OMP_DATE == 201307
    #define X_OMP_VERSION 400 //4.0
  #endif
#else
  #define X_OMP_SUPPORT 0
  #define X_OMP_DATE 0
  #define X_OMP_VERSION 0
#endif

//================================================================================================
// C99 restrict pointers support
//================================================================================================
#if defined(_MSC_VER) && _MSC_VER >= 1400 //MSVC
  #define restrict __restrict
#endif

#if defined(__GNUC__) && ((__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)) //GCC
  #ifdef __cplusplus
    #define restrict __restrict
  #endif
  #ifdef __STDC_VERSION__ //STANDARD_C_1990
    #define restrict __restrict
  #endif
  #if (__STDC_VERSION__ >= 199409L) //STANDARD_C_1994
    #define restrict __restrict
  #endif
  #if (__STDC_VERSION__ >= 199901L) //STANDARD_C_1999
  //restrict defined
  #endif
#endif

#ifdef __STDC__ //STANDARD_C_1989
  #define restrict
#endif



//================================================================================================
// MSVC issues 
//================================================================================================

//MSVC doesn't accept more than 3 __m128i arguments in 32bit mode.
#if (defined(_MSC_VER) && !defined(_M_X64))
#define MSVC_32BIT_M128I_ISSUE 1
#else
#define MSVC_32BIT_M128I_ISSUE 0
#endif

//stupid warning conected with strdup
#ifdef _MSC_VER
#define strdup _strdup
#endif

#endif //_xLanguageH_