#pragma once
#ifndef _xTypeH_
#define _xTypeH_

#include "xLicense.h"
#include <limits>
#include <float.h>
#include <string>
#include <windows.h>

//==========================================================================
// Windows 32/64
//==========================================================================
#ifdef WIN32
#ifdef _MSC_VER

typedef HANDLE FILE_HANDLE;
typedef HANDLE THREAD_HANDLE;
typedef HANDLE EVENT_HANDLE;
typedef HANDLE SEMAPHORE_HANDLE;

typedef   signed __int64 int64;
typedef   signed __int32 int32;
typedef   signed __int16 int16;
typedef   signed __int8  int8;

typedef   signed __int64 sint64;
typedef   signed __int32 sint32;
typedef   signed __int16 sint16;
typedef   signed __int8  sint8;

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;
typedef unsigned __int8  uint8;

typedef unsigned char    byte;

typedef uint16           float16;
typedef float            float32;
typedef double           float64;

#define int64_max  _I64_MAX
#define int64_min  _I64_MIN
#define int32_max  _I32_MAX
#define int32_min  _I32_MIN
#define int16_max  _I16_MAX
#define int16_min  _I16_MIN
#define int8_max   _I8_MAX
#define int8_min   _I8_MIN

#define uint64_max _UI64_MAX
#define uint64_min 0
#define uint32_max _UI32_MAX
#define uint32_min 0
#define uint16_max _UI16_MAX
#define uint16_min 0
#define uint8_max  _UI8_MAX
#define uint8_min  0

#define double_max DBL_MAX
#define double_min DBL_MIN
#define double_eps DBL_EPSILON

#define float_max FLT_MAX
#define float_min FLT_MIN
#define float_eps FLT_EPSILON

#else //_MSC_VER

typedef   signed long long int64;
typedef   signed long      int32;
typedef   signed short     int16;
typedef   signed char      int8;

typedef   signed long long sint64;
typedef   signed long      sint32;
typedef   signed short     sint16;
typedef   signed char      sint8;

typedef unsigned long long uint64;
typedef unsigned long      uint32;
typedef unsigned short     uint16;
typedef unsigned char      uint8;

typedef unsigned char      byte;

#define int64_max  LLONG_MAX
#define int64_min  LLONG_MIN
#define int32_max  LONG_MAX
#define int32_min  LONG_MIN
#define int16_max  SHORT_MAX
#define int16_min  SHORT_MIN
#define int8_max   SCHAR_MAX
#define int8_min   SCHAR_MIN

#define uint64_max ULLONG_MAX
#define uint64_min 0
#define uint32_max ULONG_MAX
#define uint32_min 0
#define uint16_max USHORT_MAX
#define uint16_min 0
#define uint8_max  UCHAR_MAX
#define uint8_min  0

#define double_max DBL_MAX
#define double_min DBL_MIN
#define double_epsilon DBL_EPSILON

#define float_max FLT_MAX
#define float_min FLT_MIN
#define float_epsilon FLT_EPSILON

#endif //_MSC_VER
#endif //WIN32

//==========================================================================
// Universal type 'xchar' for unicode and not unicode compilation
//==========================================================================
#ifdef UNICODE
  typedef wchar_t      xchar;
  typedef std::wstring xstring;
  #define xstrlen wcslen
  #define xstrdup wcsdup
#else //no UNICODE
  typedef char         xchar;
  typedef std::string  xstring;
  #define xstrlen strlen
  #define xstrdup strdup
#endif

//==========================================================================
// NULL type definition
//==========================================================================
#if (!defined(NULL) && !defined(_MSC_VER))
  #ifdef __cplusplus
    #define NULL    0
  #else
    #define NULL    ((void *)0)
  #endif
#endif //no NULL

//==========================================================================
// Enums
//==========================================================================

enum ePelType : uint8 //1b-int/float, 1b-sign, 6b - length 
{
  PelType_NotDeff = 0,
  PelType_uint8   = 0x01,
  PelType_int8    = 0x41,
  PelType_uint16  = 0x02,
  PelType_int16   = 0x42,
  PelType_uint32  = 0x04,
  PelType_int32   = 0x44,
  PelType_uint64  = 0x48,
  PelType_int64   = 0x48,
  PelType_half    = 0x82,
  PelType_float   = 0x84,
  PelType_double  = 0x88,
};

enum eImgTp : uint8 //Image Type
{
  ImgTp_UNKNOWN = 0,
  ImgTp_YUV,
  ImgTp_RGB,
  ImgTp_RGBA,
  ImgTp_XYZ,  
};

enum eCrF : uint32 //Chroma Format
{
  CrF_UNKNOWN = 0,
  CrF_444     = 444,
  CrF_422     = 422,
  CrF_420     = 420,
  CrF_411     = 411,
  CrF_410     = 410,
  CrF_400     = 400
};

enum eClrSpc : uint8 //Color Space
{  
  ClrSpc_Unknown = 0,
  ClrSpc_BT601,
  ClrSpc_BT709,
  ClrSpc_JPEG2000,
  ClrSpc_SMPTE240M,
};

enum eCmpExtMode : uint8
{
  CmpExtMode_FillGrey,
  CmpExtMode_EdgePixel,
  CmpExtMode_Mirror,
};

enum eSeqTp : int32 //sequence type
{
  SeqTp_yuv,  //raw yuv file
  SeqTp_xseq, //extended xseq file with optional lossles compresion (LZ4)
};

enum eSeqCmpr : uint8
{
  SeqCmpr_None = 0,
  SeqCmpr_LZ4,
  SeqCmpr_LZ4HC,
  SeqCmpr_DEFLATE_CL1,
  SeqCmpr_DEFLATE_CL2,
  SeqCmpr_DEFLATE_CL3,
  SeqCmpr_DEFLATE_CL4,
  SeqCmpr_DEFLATE_CL5,
  SeqCmpr_DEFLATE_CL6,
  SeqCmpr_DEFLATE_CL7,
  SeqCmpr_DEFLATE_CL8,
  SeqCmpr_DEFLATE_CL9,
  SeqCmpr_DEFLATE_CL10,
};

enum eSeqPred : uint8
{
  SeqPred_None = 0,
  SeqPred_Linear,
};

enum eIntraPred : int32
{
  I_INVALID   = -1, // invalid mode identifier
  I_PLANAR    = 0,     
  I_DC        = 1,
  I_HOR_DOWN  = 2,
  I_HOR_D7,
  I_HOR_D6,
  I_HOR_D5,
  I_HOR_D4,
  I_HOR_D3,
  I_HOR_D2,
  I_HOR_D1,
  I_HOR       = 10,
  I_HOR_U1,
  I_HOR_U2,
  I_HOR_U3,
  I_HOR_U4,
  I_HOR_U5,
  I_HOR_U6,
  I_HOR_U7,
  I_HOR_VER   = 18,
  I_VER_L7,
  I_VER_L6,
  I_VER_L5,
  I_VER_L4,
  I_VER_L3,
  I_VER_L2,
  I_VER_L1,
  I_VER       = 26,
  I_VER_R1,
  I_VER_R2,
  I_VER_R3,
  I_VER_R4,
  I_VER_R5,
  I_VER_R6,
  I_VER_R7,
  I_VER_RIGHT = 34,
};




#endif //_X_TYPE_