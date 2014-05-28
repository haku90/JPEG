#pragma once
#ifndef _x_small_ops_
#define _x_small_ops_

#include "xLicense.h"
#include "xType.h"
#include <intrin.h>
#include <memory>

//================================================================================================
// Small ops
//================================================================================================
template <class XXX> static inline XXX xAbs(XXX a) {return (XXX)abs(a);}

template <class XXX> static inline XXX xMin(XXX a, XXX b) {return a<b ? a : b;}
template <class XXX> static inline XXX xMax(XXX a, XXX b) {return a>b ? a : b;}
template <class XXX> static inline XXX xMin3(XXX a, XXX b, XXX c) {return xMin(xMin(a,b),c);}
template <class XXX> static inline XXX xMax3(XXX a, XXX b, XXX c) {return xMax(xMax(a,b),c);}
template <class XXX> static inline XXX xMin4(XXX a, XXX b, XXX c, XXX d) {return xMin(xMin(a,b),xMin(c,d));}
template <class XXX> static inline XXX xMax4(XXX a, XXX b, XXX c, XXX d) {return xMax(xMax(a,b),xMax(c,d));}

template <class XXX> static inline XXX xMedian3(XXX a, XXX b, XXX c) {return(a>b?a>c?b>c?b:c:a:b>c?a>c?a:c:b);} //3-point median

template <class XXX> static inline XXX xClip    (XXX x, XXX min, XXX max) {return xMax(min,xMin(x,max));}
template <class XXX> static inline XXX xClipU   (XXX x, XXX max) {return xMax((XXX)0, xMin(x,max));}
template <class XXX> static inline XXX xClipU8  (XXX x) {return xMax((XXX)0,xMin(x,(XXX)255));}
template <class XXX> static inline XXX xClipS8  (XXX x) {return xMax((XXX)-128,xMin(x,(XXX)127));}
template <class XXX> static inline XXX xClipU16 (XXX x) {return xMax((XXX)0,xMin(x,(XXX)65536));}
template <class XXX> static inline XXX xClipS16 (XXX x) {return xMax((XXX)-32768,xMin(x,(XXX)32767));}
template <class XXX> static inline XXX xAutoClip(XXX x, XXX a, XXX b) {return (a<b ? xClip(x,a,b) : xClip(x,b,a));}
template <class XXX> static inline XXX xClipMV(XXX mv) {return xMax(-32768,xMin(mv,32767));}

template <class XXX> static inline XXX xPow2(XXX x) {return x*x;}
template <class XXX> static inline XXX xPow3(XXX x) {return x*x*x;}

template <class XXX> static inline XXX xThreshold(XXX x, XXX t) {return (x)<(t) ? 0 : 1;}

//=====================================================================================================
// Memset
//=====================================================================================================
template <class XXX> static inline void xMemsetX(XXX*   Dst, XXX   Val, uint32 Size) { for(uint32 i=0; i<Size; i++) Dst[i] = Val; }
template <int8     > static inline void xMemsetX(int8*  Dst, int8  Val, uint32 Size) { ::memset(Dst, Val, Size); }
template <uint8    > static inline void xMemsetX(uint8* Dst, uint8 Val, uint32 Size) { ::memset(Dst, Val, Size); }

static inline void xMemset8 (int8*   Dst, int8   Val, uint32 Size) { xMemsetX<int8  >(Dst, Val, Size); }
static inline void xMemset8 (uint8*  Dst, uint8  Val, uint32 Size) { xMemsetX<uint8 >(Dst, Val, Size); }
static inline void xMemset16(int16*  Dst, int16  Val, uint32 Size) { xMemsetX<int16 >(Dst, Val, Size); }
static inline void xMemset16(uint16* Dst, uint16 Val, uint32 Size) { xMemsetX<uint16>(Dst, Val, Size); }
static inline void xMemset32(int32*  Dst, int32  Val, uint32 Size) { xMemsetX<int32 >(Dst, Val, Size); }
static inline void xMemset32(uint32* Dst, uint32 Val, uint32 Size) { xMemsetX<uint32>(Dst, Val, Size); }
static inline void xMemset64(int64*  Dst, int64  Val, uint32 Size) { xMemsetX<int64 >(Dst, Val, Size); }
static inline void xMemset64(uint64* Dst, uint64 Val, uint32 Size) { xMemsetX<uint64>(Dst, Val, Size); }
static inline void xMemsetF (float*  Dst, float  Val, uint32 Size) { xMemsetX<float >(Dst, Val, Size); }
static inline void xMemsetD (double* Dst, double Val, uint32 Size) { xMemsetX<double>(Dst, Val, Size); }

//================================================================================================
// Fast iteger Log2 using bsr (Bit Scan Reverse) x86/x64, ARM or MIPS instruction
//================================================================================================
#if defined __GNUC__
static inline uint32 xLog2(uint32 val) { return __builtin_clzl(val); }
static inline int32 xLog2(int32 val)   { return __builtin_clzl((uint32)val); }
static inline uint64 xLog2(uint64 val) { return __builtin_clzll(val); }
static inline int64 xLog2(int64 val)   { return __builtin_clzll((uint64)val); }
static inline uint32 xBitScanReverse(uint32 val) { return __builtin_clzl(val); }
static inline uint32 xSwapBytes32(uint32 Val) { return __builtin_bswap32(Val); }
static inline uint32 xSwapBytes64(uint32 Val) { return __builtin_bswap64(Val); }

#elif defined _MSC_VER
static inline uint32 xLog2(uint32 Value) { unsigned long Log2; _BitScanReverse  (&Log2, (uint32)Value); return Log2; }
static inline int32  xLog2(int32  Value) { unsigned long Log2; _BitScanReverse  (&Log2, (uint32)Value); return Log2; }
static inline uint64 xLog2(uint64 Value) { unsigned long Log2; _BitScanReverse64(&Log2, (uint64)Value); return Log2; }
static inline int64  xLog2(int64  Value) { unsigned long Log2; _BitScanReverse64(&Log2, (uint64)Value); return Log2; }

static inline uint32 xBitScanReverse(uint32 Value) { unsigned long BSR; _BitScanReverse  (&BSR, (uint32)Value); return BSR; }
static inline int32  xBitScanReverse(int32  Value) { unsigned long BSR; _BitScanReverse  (&BSR, (uint32)Value); return BSR; }
static inline uint64 xBitScanReverse(uint64 Value) { unsigned long BSR; _BitScanReverse64(&BSR, (uint64)Value); return BSR; }
static inline int64  xBitScanReverse(int64  Value) { unsigned long BSR; _BitScanReverse64(&BSR, (uint64)Value); return BSR; }

static inline uint32 xBitScanForward(uint32 Value) { unsigned long BSF; _BitScanForward  (&BSF, (uint32)Value); return BSF; }
static inline int32  xBitScanForward(int32  Value) { unsigned long BSF; _BitScanForward  (&BSF, (uint32)Value); return BSF; }
static inline uint64 xBitScanForward(uint64 Value) { unsigned long BSF; _BitScanForward64(&BSF, (uint64)Value); return BSF; }
static inline int64  xBitScanForward(int64  Value) { unsigned long BSF; _BitScanForward64(&BSF, (uint64)Value); return BSF; }

static inline uint32 xSwapBytes32(uint32 Value) { return _byteswap_ulong (Value); }
static inline uint64 xSwapBytes64(uint64 Value) { return _byteswap_uint64(Value); }

#else
#error bsr (Bit Scan Reverse) intrinsic unknown
#endif

#endif