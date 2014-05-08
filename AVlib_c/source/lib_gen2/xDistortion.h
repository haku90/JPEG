#pragma once
#ifndef _xDistortionH_
#define _xDistortionH_

#include "xLicense.h"
#include "xCommon.h"

namespace AVlib {

class xDistortion
{
public:
  static uint64 xCalcSADContinuous_STD(int16* Org, int16* Dist, int32 Area); //Fast SAD calculation for continuous memory blocks
  static uint64 xCalcSADContinuous_SSE(int16* Org, int16* Dist, int32 Area); //Fast SAD calculation for continuous memory blocks
  static uint64 xCalcSADContinuous_AVX(int16* Org, int16* Dist, int32 Area); //Fast SAD calculation for continuous memory blocks
  static uint64 xCalcSAD_STD          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height); //rectangular blocks
  static uint64 xCalcSAD_SSE          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height); //rectangular blocks
  static uint64 xCalcSAD_AVX          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height); //rectangular blocks
  
  static uint64 xCalcSSDContinuous_STD(int16* Org, int16* Dist, int32 Area); //Fast SSD calculation for continuous memory blocks
  static uint64 xCalcSSDContinuous_SSE(int16* Org, int16* Dist, int32 Area); //Fast SSD calculation for continuous memory blocks
  static uint64 xCalcSSDContinuous_AVX(int16* Org, int16* Dist, int32 Area); //Fast SSD calculation for continuous memory blocks
  static uint64 xCalcSSD_STD          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height); //rectangular blocks
  static uint64 xCalcSSD_SSE          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height); //rectangular blocks
  static uint64 xCalcSSD_AVX          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height); //rectangular blocks

  static uint64 xCalcSATD_2x2_STD         (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATDContinous_4x4_STD(int16* Org,                int16* Dist               );
  static uint64 xCalcSATDContinous_4x4_SSE(int16* Org,                int16* Dist               );
  static uint64 xCalcSATD_4x4_STD         (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATD_4x4_SSE         (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATD_8x8_STD         (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATD_8x8_SSE         (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATD_4x16_STD        (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATD_4x16_SSE        (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATD_16x4_STD        (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATD_16x4_SSE        (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 xCalcSATD_16x16_STD       (int16* Org, int32 OStride, int16* Dist, int32 DStride);

//=========================================================================================================================================
#if X_USE_AVX && X_AVX2
  static uint64 calcSADContinuous(int16* Org, int16* Dist, int32 Area) { return xCalcSADContinuous_AVX(Org, Dist, Area); }
  static uint64 calcSAD          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height) { return xCalcSAD_AVX(Org, OStride, Dist, DStride, Width, Height); }
#elif X_USE_SSE && X_SSE3
  static uint64 calcSADContinuous(int16* Org, int16* Dist, int32 Area) { return xCalcSADContinuous_SSE(Org, Dist, Area); }
  static uint64 calcSAD          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height) { return xCalcSAD_SSE(Org, OStride, Dist, DStride, Width, Height); }
#else
  static uint64 calcSADContinuous(int16* Org, int16* Dist, int32 Area) { return xCalcSADContinuous_STD(Org, Dist, Area); }
  static uint64 calcSAD          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height) { return xCalcSAD_STD(Org, OStride, Dist, DStride, Width, Height); }
#endif

#if X_USE_AVX && X_AVX2
  static uint64 calcSSDContinuous(int16* Org, int16* Dist, int32 Area) { return xCalcSSDContinuous_AVX(Org, Dist, Area); }
  static uint64 calcSSD          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height) { return xCalcSSD_AVX(Org, OStride, Dist, DStride, Width, Height); }
#elif X_USE_SSE && X_SSE2
  static uint64 calcSSDContinuous(int16* Org, int16* Dist, int32 Area) { return xCalcSSDContinuous_SSE(Org, Dist, Area); }
  static uint64 calcSSD          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height) { return xCalcSSD_SSE(Org, OStride, Dist, DStride, Width, Height); }
#else
  static uint64 calcSSDContinuous(int16* Org, int16* Dist, int32 Area) { return xCalcSSDContinuous_STD(Org, Dist, Area); }
  static uint64 calcSSD          (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height) { return xCalcSSD_STD(Org, OStride, Dist, DStride, Width, Height); }
#endif

//=========================================================================================================================================

  static uint64 calcSATD         (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height, bool NonSquareSATD); //rectangular blocks
  static uint64 calcSATDContinous(int16* Org, int16* Dist, int32 Size); //Fast SSD calculation for continuous memory blocks
  static uint64 calcSATD         (int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Size); //square blocks

#if X_USE_SSE && X_SSE2
  static uint64 calcSATD_2x2         (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_2x2_STD(Org, OStride, Dist, DStride);  }
  static uint64 calcSATD_4x4         (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_4x4_SSE(Org, OStride, Dist, DStride);  }
  static uint64 calcSATDContinous_4x4(int16* Org, int16* Dist)                               { return xCalcSATDContinous_4x4_SSE(Org, Dist);           }
  static uint64 calcSATD_8x8         (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_8x8_SSE(Org, OStride, Dist, DStride);  }
  static uint64 calcSATD_4x16        (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_4x16_SSE(Org, OStride, Dist, DStride); }
  static uint64 calcSATD_16x4        (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_16x4_SSE(Org, OStride, Dist, DStride); }
#else
  static uint64 calcSATD_2x2         (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_2x2_STD(Org, OStride, Dist, DStride);  }
  static uint64 calcSATD_4x4         (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_4x4_STD(Org, OStride, Dist, DStride);  }
  static uint64 calcSATDContinous_4x4(int16* Org, int16* Dist)                               { return xCalcSATDContinous_4x4_STD(Org, Dist);           }
  static uint64 calcSATD_8x8         (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_8x8_STD(Org, OStride, Dist, DStride);  }
  static uint64 calcSATD_4x16        (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_4x16_STD(Org, OStride, Dist, DStride); }
  static uint64 calcSATD_16x4        (int16* Org, int32 OStride, int16* Dist, int32 DStride) { return xCalcSATD_16x4_STD(Org, OStride, Dist, DStride); }
#endif
  static uint64 calcSATD_16x16       (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 calcSATD_32x32       (int16* Org, int32 OStride, int16* Dist, int32 DStride);
  static uint64 calcSATD_64x64       (int16* Org, int32 OStride, int16* Dist, int32 DStride);
};

} //end of namespace AVLib

#endif //_xDistortionH_