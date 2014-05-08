#pragma once
#ifndef _xImageH_
#define _xImageH_

#include "xLicense.h"
#include "xCommon.h"
#include "xCmp.h"

namespace AVlib {

class xImg
{
public:
  xCmp**      m_Cmp;    
  int32       m_CmpSpace;
  int32       m_CmpTotal;
  int32       m_BitDepth;

  eImgTp      m_ImageType;
  eCrF        m_ChromaFormat;
  eClrSpc     m_ColorSpace;
  int32       m_Id;

  xchar       m_Name[CmpNameLen];

public:
  //base functions
  xImg                      ();
  void     create           (int32 SizeX, int32 SizeY, int32 Margin, int32 BitDepth, eImgTp ImageType, eCrF ChromaFormat, eClrSpc ColorSpace, int32 SpareCmpNum);
  void     destroy          ();
  void     createCompatible (xImg* RefImg);
  void     duplicate        (xImg* RefImg);
  void     clear            ();
  void     copy             (xImg* Src);
  void     copyEx           (xImg* Src, int32 DstX, int32 DstY, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY);
  void     set              (int16 Value);
  void     setChroma        (int16 Value);
  void     setName          (xchar* Name);

  //resize and rescale
  void     rescaleFast      (xImg* Src);
  void     rescaleChroma    (xImg* Src); // 4:2:0 <--> 4:4:4 only

  //precision
  void     bitDepthDecrease (xImg* Src, int32 DeltaBits);
  void     bitDepthIncrease (xImg* Src, int32 DeltaBits);

  //crop & extend
  void     crop             (xImg* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY);
  void     extend           (xImg* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY, eCmpExtMode Mode);

  //color space conversion
  void     convertRGB2YUV   (xImg* Src);
  void     convertYUV2RGB   (xImg* Src);

  //load and store RAW YUV
  void     getRAW           (FILE_HANDLE File);
  void     putRAW           (FILE_HANDLE File);
  void     unpackRAW        (uint8* Src);
  void     packRAW          (uint8* Dst);
  static int32 calcSizeRAW  (int32 SizeX, int32 SizeY, int32 BitDepth, eCrF ChromaFormat);

  //load and store BMP
  void     getBMP           (FILE_HANDLE File);
  void     putBMP           (FILE_HANDLE File);

};

} //end of namespace AVLib

#endif //_xImageH_