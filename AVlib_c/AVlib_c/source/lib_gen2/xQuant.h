#pragma once
#ifndef _xQuantH_
#define _xQuantH_

#include "xLicense.h"
#include "xCommon.h"

namespace AVlib {

class xQuant
{
private:
  static const int32 m_QuantScales[6];   //HEVC Quant
  static const int32 m_DeQuantScales[6]; //HEVC Quant

  static const int8  m_LumaToChromaQP[58];
  static const int8  m_QPPer[58];
  static const int8  m_QPRem[58];

public:
  static void xQuant_STD  (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);
  static void xQuant_SSE  (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);
  static void xQuant_AVX  (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);
  static void xDeQuant_STD(int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);
  static void xDeQuant_SSE(int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);
  static void xDeQuant_AVX(int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);

  static void xQuant_STD  (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);
  static void xQuant_SSE  (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);
  static void xDeQuant_STD(int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);
  static void xDeQuant_SSE(int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth);

//=========================================================================================================================================

#if X_USE_AVX && X_AVX2
  static void Quant      (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xQuant_AVX(Src, Dst, Log2TrSize, QP, BitDepth); }
  static void DeQuant    (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xQuant_AVX(Src, Dst, Log2TrSize, QP, BitDepth); }
#elif X_USE_SSE && X_SSE4_1
  static void Quant      (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xQuant_SSE(Src, Dst, Log2TrSize, QP, BitDepth); }
  static void DeQuant    (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xQuant_SSE(Src, Dst, Log2TrSize, QP, BitDepth); }
#else
  static void Quant      (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xQuant_STD(Src, Dst, Log2TrSize, QP, BitDepth); }
  static void DeQuant    (int16* Src, int16* Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xQuant_STD(Src, Dst, Log2TrSize, QP, BitDepth); }
#endif

#if X_USE_SSE && X_SSE4_1
  static void Quant      (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xQuant_SSE(Src, Dst, SrcStride, DstStride, Log2TrSize, QP, BitDepth); }
  static void DeQuant    (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xDeQuant_SSE(Src, Dst, SrcStride, DstStride, Log2TrSize, QP, BitDepth); }
#else
  static void Quant      (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xQuant_STD(Src, Dst, SrcStride, DstStride, Log2TrSize, QP, BitDepth); }
  static void DeQuant    (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth) { xDeQuant_STD(Src, Dst, SrcStride, DstStride, Log2TrSize, QP, BitDepth); }
#endif

  static uint32 CalcNumSigCoeff();


};

} //end of namespace AVLib

#endif
