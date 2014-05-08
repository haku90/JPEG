#pragma once
#ifndef _xComponentH_
#define _xComponentH_

#include "xLicense.h"
#include "xCommon.h"

namespace AVlib {

class xCmp
{
public:
  int32      m_SizeX;
  int32      m_SizeY;
  int32      m_Margin;
  int32      m_BitDepth;      //bits per sample for int cmp

  int16*     m_PelBuffer;     //picture buffer
  int16**    m_PelPtrBuffer;  //buffer for line pointers
  int32      m_BufferSize;    //size of picture buffer (including margin)

  int16**    m_Pel;       //[y][x] pel access -> m_Pel[y][x]
  int16*     m_PelOrg;    //pel origin, pel access -> m_PelOrg[y*m_PelStride + x]
  int32      m_PelStride; //pel stride

  int32      m_Type;
  int32      m_Id;

  xchar      m_Name[CmpNameLen];         

public:
  //base functions
  xCmp                      ();
  void     create           (int32 SizeX, int32 SizeY, int32 Margin, int32 BitDepth);
  void     destroy          ();
  void     createCompatible (xCmp* RefCmp);
  void     duplicate        (xCmp* RefCmp);
  void     clear            ();
  void     resize           (int32 SizeX, int32 SizeY, int32 Margin, int32 BitDepth);
  void     copy             (xCmp* Src);
  void     copyEx           (xCmp* Src, int32 DstX, int32 DstY, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY);
  void     set              (int16 Value);
  void     setName          (xchar* Name);

  //tool functions
  bool     isCompatible     (xCmp* Cmp) { return (Cmp!=nullptr && m_BitDepth==Cmp->m_BitDepth); }
  bool     isSameSize       (xCmp* Cmp) { return (Cmp!=nullptr && m_SizeX==Cmp->m_SizeX && m_SizeY==Cmp->m_SizeY && m_Margin==Cmp->m_Margin); }

  //resize and rescale
  void     rescaleFast             (xCmp* Src);
  void     rescaleChroma           (xCmp* Src); // 4:2:0 <--> 4:4:4 only
  void     rescaleBilinear         (xCmp* Src);
  void     rescaleU2Nearest        (xCmp* Src); 
  void     rescaleU2BilinearChroma (xCmp* Src); 
  void     rescaleU2Bilinear       (xCmp* Src);
  void     rescaleD2Avg            (xCmp* Src);

  //precision
  void     bitDepthDecrease (xCmp* Src, int32 DeltaBits);
  void     bitDepthIncrease (xCmp* Src, int32 DeltaBits);

  //crop & extend
  void     crop             (xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY);
  void     extend           (xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY, eCmpExtMode Mode);
  void     extendGrey       (xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY);
  void     extendEdge       (xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY);
  void     extendMirror     (xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY);

  //arithmetic operations
  void     add              (xCmp* Src0, xCmp* Src1);
  void     addAndClip       (xCmp* Src0, xCmp* Src1);
  void     sub              (xCmp* Src0, xCmp* Src1);
  void     average          (xCmp* Src0, xCmp* Src1);
  void     averageAndClip   (xCmp* Src0, xCmp* Src1);
  void     clip             (xCmp* Src, int16 ClippingRangeLow, int16 ClippingRangeHigh);

  //distortion & PSNR
  int64    calcSAD          (xCmp* ReferenceCmp);
  int64    calcSATD         (xCmp* ReferenceCmp);
  int64    calcSSD          (xCmp* ReferenceCmp);
  double   calcPSNR         (xCmp* ReferenceCmp);
  
  //histogram
  void     calcHistogram    (int32* DstVector);

  //transform
  uint64   transform_4x4    (xCmp* Src);
  uint64   transform_8x8    (xCmp* Src);
  uint64   transform_16x16  (xCmp* Src);
  uint64   transform_32x32  (xCmp* Src);
  uint64   transform_64x64  (xCmp* Src);

  //sharpness
  //float    x_cmp_sharpness         (sCmp* src);

};

} //end of namespace AVLib

#endif //_xComponentH_