#pragma once
#ifndef _xTransformH_
#define _xTransformH_

#include "xLicense.h"
#include "xCommon.h"

namespace AVlib {

class xTransform
{
private:
  static const int16 m_TrM4x4  [4][4];   //HEVC DCT
  static const int16 m_TrM8x8  [8][8];   //HEVC DCT
  static const int16 m_TrM16x16[16][16]; //HEVC DCT
  static const int16 m_TrM32x32[32][32]; //HEVC DCT
  static const int16 m_TrM64x64[64][64]; //HEVC DCT

public:
  //Hadamard transform
  static void TransformHadamard_4x4_STD     (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);
  static void TransformHadamard_8x8_STD     (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);
  static void TransformHadamard_8x8_SSE     (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);
  static void TransformHadamard_16x16_STD   (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);
  static void TransformHadamard_32x32_STD   (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);

  static void InvTransformHadamard_4x4_STD  (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);
  static void InvTransformHadamard_8x8_STD  (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);
  static void InvTransformHadamard_16x16_STD(int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);
  static void InvTransformHadamard_32x32_STD(int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride);

  //HEVC DCT transform
  static void TransformDCT_4x4_STD_C     (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - hardcoded coefficients
  static void TransformDCT_4x4_STD_M     (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)
  static void TransformDCT_4x4_SSE       (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - SSE, horizontal transform->transpose->vertical transform->transpose
  static void TransformDCT_4x4_SSE       (int16* Src, int16* Dst,                   uint32 BitDepth);  //vectorized - SSE, no src stride, horizontal transform->transpose->vertical transform->transpose
  static void TransformDCT_4x4_AVX       (int16* Src, int16* Dst,                   uint32 BitDepth);  //vectorized - AVX, horizontal transform->transpose->vertical transform->transpose
  static void TransformDCT_8x8_STD_C     (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - hardcoded coefficients
  static void TransformDCT_8x8_STD_M     (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)
  static void TransformDCT_8x8_SSE       (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - SSE, horizontal transform->transpose->vertical transform->transpose
  static void TransformDCT_8x8_SSEv2     (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - SSE, horizontal transform->vertical transform using 1 symetry
  static void TransformDCT_16x16_STD_C   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - hardcoded coefficients
  static void TransformDCT_16x16_STD_M   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)
  static void TransformDCT_16x16_SSE     (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - horizontal transform->transpose->vertical transform->transpose
  static void TransformDCT_16x16_SSEv2   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - horizonal transform uses one symmetry, vertical transform uses three symmetries
  static void TransformDCT_16x16_SSEv3   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - slower than SSEv2, vertical transf. uses 4 symmetries
  static void TransformDCT_32x32_STD_C   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - hardcoded coefficients
  static void TransformDCT_32x32_STD_M   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)
  static void TransformDCT_32x32_SSE     (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - horizontal transform->transpose->vertical transform->transpose
  static void TransformDCT_32x32_SSEv2   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - horizontal transform 1 symmetry, vertical transform 2 symmetries
  static void TransformDCT_32x32_SSEv3   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //vectorized - horizontal transform 2 symmetry (xT32e_DCTreduction, vertical transform 3 symmetries. slower than DCT_32x32_SSEv2
  static void TransformDCT_64x64_STD_M   (int16* Src, int16* Dst, uint32 SrcStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)

  static void InvTransformDCT_4x4_STD_C  (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - hardcoded coefficients
  static void InvTransformDCT_4x4_STD_M  (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)
  static void InvTransformDCT_4x4_SSE    (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //vectorized - SSE, horizontal transform->transpose->vertical transform->transpose
  static void InvTransformDCT_4x4_SSE    (int16* Src, int16* Dst,                   uint32 BitDepth);  //vectorized - SSE, no dst stride, horizontal transform->transpose->vertical transform->transpose
  static void InvTransformDCT_8x8_STD_C  (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - hardcoded coefficients
  static void InvTransformDCT_8x8_STD_M  (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)
  static void InvTransformDCT_8x8_SSE    (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //vectorized - SSE, transpose->vertical transform->->horizontal transform
  static void InvTransformDCT_8x8_SSEv2  (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_8x8_SSEv3  (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_16x16_STD_C(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - hardcoded coefficients
  static void InvTransformDCT_16x16_STD_M(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)
  static void InvTransformDCT_16x16_SSE  (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_16x16_SSEv2(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_16x16_SSEv3(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_16x16_SSEv4(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_32x32_STD_C(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - hardcoded coefficients
  static void InvTransformDCT_32x32_STD_M(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)
  static void InvTransformDCT_32x32_SSE  (int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_32x32_SSEv2(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_32x32_SSEv3(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_32x32_SSEv4(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDCT_64x64_STD_M(int16* Src, int16* Dst, uint32 DstStride, uint32 BitDepth);  //scalar - coefficients readed from matrix (slower)

  //HEVC DST transform
  static void TransformDST_4x4_STD_C     (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 BitDepth);
  static void TransformDST_4x4_SSE       (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDST_4x4_STD_C  (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 BitDepth);
  static void InvTransformDST_4x4_SSE    (int16* Src, int16* Dst, uint32 SrcStride, uint32 DstStride, uint32 BitDepth);
};

} //end of namespace AVLib

#endif




