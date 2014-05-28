#pragma once
#ifndef _xColorSpaceH_
#define _xColorSpaceH_

#include "xLicense.h"
#include "xCommon.h"

namespace AVlib {

class xColorSpace
{
public:
  static const int32 m_RGB2YUV_int32[4][9];
  static const int32 m_YUV2RGB_int32[4][9];
  static const int32 m_RGB2XYZ_int32[1][9];

  static void convertRGB2YUV     (int16* R, int16* G, int16* B, uint32 RGBStride, int16* Y, int16* U, int16* V, uint32 YUVStride, uint32 Width, uint32 Height, uint32 BitDepth, eClrSpc YUVColorSpace);
  static void convertRGB2YUV_Cont(int16* R, int16* G, int16* B,                   int16* Y, int16* U, int16* V,                   uint32 Length,               uint32 BitDepth, eClrSpc YUVColorSpace);

  static void convertYUV2RGB     (int16* Y, int16* U, int16* V, uint32 YUVStride, int16* R, int16* G, int16* B, uint32 RGBStride, uint32 Width, uint32 Height, uint32 BitDepth, eClrSpc YUVColorSpace);
  static void convertYUV2RGB_Cont(int16* Y, int16* U, int16* V,                   int16* R, int16* G, int16* B,                   uint32 Length,               uint32 BitDepth, eClrSpc YUVColorSpace);

protected:
  static void xConvertDEF2ABC_INT32     (int16* D, int16* E, int16* F, uint32 DEFStride, int16* A, int16* B, int16* C, uint32 ABCStride, uint32 Width, uint32 Height, const int32 ConvertMatrix[9]);
  static void xConvertDEF2ABC_INT32_Cont(int16* D, int16* E, int16* F,                   int16* A, int16* B, int16* C,                   uint32 Length,               const int32 ConvertMatrix[9]);
};

} //end of namespace AVLib

#endif //_xColorSpaceH_