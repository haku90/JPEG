#pragma once
#ifndef _xPixelH_
#define _xPixelH_

#include "xLicense.h"
#include "xCommon.h"

namespace AVlib {

class xPixel
{
public:
  static void xAddContinuous_STD         (int16* Dst, int16* Src0, int16* Src1, int32 Area);
  static void xAddContinuous_SSE         (int16* Dst, int16* Src0, int16* Src1, int32 Area);
  static void xAdd_STD                   (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height);
  static void xAdd_SSE                   (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height);

  static void xAddAndClipContinuous_STD  (int16* Dst, int16* Src0, int16* Src1, int32 Area, int16 ClippingRange);
  static void xAddAndClipContinuous_SSE  (int16* Dst, int16* Src0, int16* Src1, int32 Area, int16 ClippingRange);
  static void xAddAndClip_STD            (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange);
  static void xAddAndClip_SSE            (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange);

  static void xSubContinuous_STD         (int16* Dst, int16* Src0, int16* Src1, int32 Area);
  static void xSubContinuous_SSE         (int16* Dst, int16* Src0, int16* Src1, int32 Area);
  static void xSub_STD                   (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height);
  static void xSub_SSE                   (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height);

  static void xBiPredRefEstContinuous_STD(int16* Dst, int16* Src0, int16* Src1, int32 Area);
  static void xBiPredRefEstContinuous_SSE(int16* Dst, int16* Src0, int16* Src1, int32 Area);
  static void xBiPredRefEst_STD          (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height);  
  static void xBiPredRefEst_SSE          (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height);

  static void xAvgContinuous_STD         (int16* Dst, int16* Src0, int16* Src1, int32 Area);
  static void xAvgContinuous_SSE         (int16* Dst, int16* Src0, int16* Src1, int32 Area);
  static void xAvg_STD                   (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height);
  static void xAvg_SSE                   (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height);

  static void xAvgAndClipContinuous_STD  (int16* Dst, int16* Src0, int16* Src1, int32 Area, int16 ClippingRange);
  static void xAvgAndClipContinuous_SSE  (int16* Dst, int16* Src0, int16* Src1, int32 Area, int16 ClippingRange);
  static void xAvgAndClip_STD            (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange);
  static void xAvgAndClip_SSE            (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange);

  static void xClipContinuous_STD        (int16* Dst, int16* Src, int32 Area, int16 ClippingRangeLow, int16 ClippingRangeHigh);
  static void xClipContinuous_SSE        (int16* Dst, int16* Src, int32 Area, int16 ClippingRangeLow, int16 ClippingRangeHigh);
  static void xClip_STD                  (int16* Dst, int16* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height, int16 ClippingRangeLow, int16 ClippingRangeHigh);
  static void xClip_SSE                  (int16* Dst, int16* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height, int16 ClippingRangeLow, int16 ClippingRangeHigh);
  
  static void xPackI16ToU8Continuous_STD   (int8*  Dst, int16* Src, int32 Area);
  static void xPackI16ToU8Continuous_SSE   (int8*  Dst, int16* Src, int32 Area);
  static void xPackI16ToU8_STD             (int8*  Dst, int16* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height);
  static void xPackI16ToU8_SSE             (int8*  Dst, int16* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height);

  static void xUnpackU8ToI16Continuous_STD (int16* Dst, int8*  Src, int32 Area);
  static void xUnpackU8ToI16Continuous_SSE (int16* Dst, int8*  Src, int32 Area);
  static void xUnpackU8ToI16_STD           (int16* Dst, int8*  Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height);
  static void xUnpackU8ToI16_SSE           (int16* Dst, int8*  Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height);
  
//=========================================================================================================================================

#if X_USE_SSE && X_SSE2
  static void AddContinuous             (int16* Dst, int16* Src0, int16* Src1, int32 Area) { xAddContinuous_SSE(Dst, Src0, Src1, Area); }
  static void Add                       (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height) { xAdd_SSE(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height); }
  static void AddAndClipContinuous      (int16* Dst, int16* Src0, int16* Src1, int32 Area, int16 ClippingRange) { xAddAndClipContinuous_SSE(Dst, Src0, Src1, Area, ClippingRange); }
  static void AddAndClip                (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange) { xAddAndClip_SSE(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height, ClippingRange); }
  static void SubContinuous             (int16* Dst, int16* Src0, int16* Src1, int32 Area) { xSubContinuous_SSE(Dst, Src0, Src1, Area); }
  static void Sub                       (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height) { xSub_SSE(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height); }
  static void BiPredRefEstContinuous    (int16* Dst, int16* Src0, int16* Src1, int32 Area) { xBiPredRefEstContinuous_SSE(Dst, Src0, Src1, Area); }
  static void BiPredRefEst              (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height) { xBiPredRefEst_SSE(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height); }
  static void AvgContinuous             (int16* Dst, int16* Src0, int16* Src1, int32 Area) { xAvgContinuous_SSE(Dst, Src0, Src1, Area); }
  static void Avg                       (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height) { xAvg_SSE(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height); }
  static void AvgAndClipContinuous      (int16* Dst, int16* Src0, int16* Src1, int32 Area, int16 ClippingRange) { xAvgAndClipContinuous_SSE(Dst, Src0, Src1, Area, ClippingRange); }
  static void AvgAndClip                (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange) { xAvgAndClip_SSE(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height, ClippingRange); }
  static void ClipContinuous            (int16* Dst, int16* Src, int32 Area, int16 ClippingRangeLow, int16 ClippingRangeHigh) { xClipContinuous_SSE(Dst, Src, Area, ClippingRangeLow, ClippingRangeHigh); }
  static void Clip                      (int16* Dst, int16* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height, int16 ClippingRangeLow, int16 ClippingRangeHigh) { xClip_SSE(Dst, Src, DstStride, SrcStride, Width, Height, ClippingRangeLow, ClippingRangeHigh); }
#else
  static void AddContinuous             (int16* Dst, int16* Src0, int16* Src1, int32 Area) { xAddContinuous_STD(Dst, Src0, Src1, Area); }
  static void Add                       (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height) { xAdd_STD(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height); }
  static void AddAndClipContinuous      (int16* Dst, int16* Src0, int16* Src1, int32 Area, int16 ClippingRange) { xAddAndClipContinuous_STD(Dst, Src0, Src1, Area, ClippingRange); }
  static void AddAndClip                (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange) { xAddAndClip_STD(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height, ClippingRange); }
  static void SubContinuous             (int16* Dst, int16* Src0, int16* Src1, int32 Area) { xSubContinuous_STD(Dst, Src0, Src1, Area); }
  static void Sub                       (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height) { xSub_STD(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height); }
  static void BiPredRefEstContinuous    (int16* Dst, int16* Src0, int16* Src1, int32 Area) { xBiPredRefEstContinuous_STD(Dst, Src0, Src1, Area); }
  static void BiPredRefEst              (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height) { xBiPredRefEst_STD(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height); }
  static void AvgContinuous             (int16* Dst, int16* Src0, int16* Src1, int32 Area) { xAvgContinuous_STD(Dst, Src0, Src1, Area); }
  static void Avg                       (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height) { xAvg_STD(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height); }
  static void AvgAndClipContinuous      (int16* Dst, int16* Src0, int16* Src1, int32 Area, int16 ClippingRange) { xAvgAndClipContinuous_STD(Dst, Src0, Src1, Area, ClippingRange); }
  static void AvgAndClip                (int16* Dst, int16* Src0, int16* Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange) { xAvgAndClip_STD(Dst, Src0, Src1, DstStride, Src0Stride, Src1Stride, Width, Height, ClippingRange); }
  static void ClipContinuous            (int16* Dst, int16* Src, int32 Area, int16 ClippingRangeLow, int16 ClippingRangeHigh) { xClipContinuous_STD(Dst, Src, Area, ClippingRangeLow, ClippingRangeHigh); }
  static void Clip                      (int16* Dst, int16* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height, int16 ClippingRangeLow, int16 ClippingRangeHigh) { xClip_STD(Dst, Src, DstStride, SrcStride, Width, Height, ClippingRangeLow, ClippingRangeHigh); }

#endif


};

} //end of namespace AVLib

#endif //_xPixelH_