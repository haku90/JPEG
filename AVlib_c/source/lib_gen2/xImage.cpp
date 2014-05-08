#include "xImage.h"
#include "xColorSpace.h"

namespace AVlib {

//base functions
xImg::xImg()
{
  m_Cmp          = nullptr;    
  m_CmpSpace     = -1;
  m_CmpTotal     = -1;
  m_BitDepth     = -1;

  m_ImageType    = ImgTp_UNKNOWN;
  m_ChromaFormat = CrF_UNKNOWN;
  m_ColorSpace   = ClrSpc_Unknown;
  m_Id           = -1;

  m_Name[CmpNameLen];
}

void xImg::create(int32 SizeX, int32 SizeY, int32 Margin, int32 BitDepth, eImgTp ImageType, eCrF ChromaFormat, eClrSpc ColorSpace, int32 SpareCmpNum)
{
  m_ImageType    = ImageType;
  m_ChromaFormat = ChromaFormat;
  m_ColorSpace   = ColorSpace;
  m_BitDepth     = BitDepth;
  int32 CmpIdx = 0;

  switch (ImageType)
  {
  case ImgTp_UNKNOWN:
    m_Cmp = (xCmp**)xCalloc(SpareCmpNum, sizeof(xCmp*));
    break;
  case ImgTp_RGB:
    m_Cmp = (xCmp**)xCalloc(3+SpareCmpNum, sizeof(xCmp*));
    m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[0]->setName(TEXT("R"));
    m_Cmp[1] = new xCmp; m_Cmp[1]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[1]->setName(TEXT("G"));
    m_Cmp[2] = new xCmp; m_Cmp[2]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[2]->setName(TEXT("B"));
    CmpIdx = 3;
    break;
  case ImgTp_RGBA:
    m_Cmp = (xCmp**)xCalloc(4+SpareCmpNum, sizeof(xCmp*));
    m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[0]->setName(TEXT("R"));
    m_Cmp[1] = new xCmp; m_Cmp[1]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[1]->setName(TEXT("G"));
    m_Cmp[2] = new xCmp; m_Cmp[2]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[2]->setName(TEXT("B"));
    m_Cmp[3] = new xCmp; m_Cmp[3]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[3]->setName(TEXT("A"));
    CmpIdx = 4;
    break;
  case ImgTp_XYZ:
    m_Cmp = (xCmp**)xCalloc(3+SpareCmpNum, sizeof(xCmp*));
    m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[0]->setName(TEXT("X"));
    m_Cmp[1] = new xCmp; m_Cmp[1]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[1]->setName(TEXT("Y"));
    m_Cmp[2] = new xCmp; m_Cmp[2]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[2]->setName(TEXT("Z"));
    CmpIdx = 3;
    break;
  case ImgTp_YUV:  
    xAssert(ColorSpace>=ClrSpc_BT601 && ColorSpace<=ClrSpc_SMPTE240M);
    switch (ChromaFormat)
    {
    case CrF_444:
      m_Cmp = (xCmp**)xCalloc(3+SpareCmpNum, sizeof(xCmp*));
      m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[0]->setName(TEXT("Y"));
      m_Cmp[1] = new xCmp; m_Cmp[1]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[1]->setName(TEXT("U"));
      m_Cmp[2] = new xCmp; m_Cmp[2]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[2]->setName(TEXT("V"));
      CmpIdx = 3;
      break;
    case CrF_422:
      m_Cmp = (xCmp**)xCalloc(3+SpareCmpNum, sizeof(xCmp*));
      m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth);   m_Cmp[0]->setName(TEXT("Y"));
      m_Cmp[1] = new xCmp; m_Cmp[1]->create(SizeX/2, SizeY, Margin, BitDepth); m_Cmp[1]->setName(TEXT("U"));
      m_Cmp[2] = new xCmp; m_Cmp[2]->create(SizeX/2, SizeY, Margin, BitDepth); m_Cmp[2]->setName(TEXT("V"));
      CmpIdx = 3;
      break;
    case CrF_420:
      m_Cmp = (xCmp**)xCalloc(3+SpareCmpNum, sizeof(xCmp*));
      m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth);     m_Cmp[0]->setName(TEXT("Y"));
      m_Cmp[1] = new xCmp; m_Cmp[1]->create(SizeX/2, SizeY/2, Margin, BitDepth); m_Cmp[1]->setName(TEXT("U"));
      m_Cmp[2] = new xCmp; m_Cmp[2]->create(SizeX/2, SizeY/2, Margin, BitDepth); m_Cmp[2]->setName(TEXT("V"));
      CmpIdx = 3;
      break;
    case CrF_411:
      m_Cmp = (xCmp**)xCalloc(3+SpareCmpNum, sizeof(xCmp*));
      m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth);   m_Cmp[0]->setName(TEXT("Y"));
      m_Cmp[1] = new xCmp; m_Cmp[1]->create(SizeX/4, SizeY, Margin, BitDepth); m_Cmp[1]->setName(TEXT("U"));
      m_Cmp[2] = new xCmp; m_Cmp[2]->create(SizeX/4, SizeY, Margin, BitDepth); m_Cmp[2]->setName(TEXT("V"));
      CmpIdx = 3;
      break;
    case CrF_410:
      m_Cmp = (xCmp**)xCalloc(3+SpareCmpNum, sizeof(xCmp*));
      m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth);     m_Cmp[0]->setName(TEXT("Y"));
      m_Cmp[1] = new xCmp; m_Cmp[1]->create(SizeX/4, SizeY/2, Margin, BitDepth); m_Cmp[1]->setName(TEXT("U"));
      m_Cmp[2] = new xCmp; m_Cmp[2]->create(SizeX/4, SizeY/2, Margin, BitDepth); m_Cmp[2]->setName(TEXT("V"));
      CmpIdx = 3;
      break;
    case CrF_400:
      m_Cmp = (xCmp**)xCalloc(3+SpareCmpNum, sizeof(xCmp*));
      m_Cmp[0] = new xCmp; m_Cmp[0]->create(SizeX, SizeY, Margin, BitDepth); m_Cmp[0]->setName(TEXT("Y"));
      CmpIdx = 1;
      break;
    case CrF_UNKNOWN:
    default:
      xAssert(0);
      break;
    }
    break;
  default:
    xAssert(0);
    break;
  }

  m_CmpSpace = CmpIdx;
  m_CmpTotal = CmpIdx + SpareCmpNum;

  for(; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx] = new xCmp;
    m_Cmp[CmpIdx]->create(SizeX, SizeY, Margin, BitDepth);
  }
}

void xImg::destroy()
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->destroy();
    delete m_Cmp[CmpIdx];
  }
}

void xImg::createCompatible(xImg* RefImg)
{
  m_ImageType    = RefImg->m_ImageType;
  m_ChromaFormat = RefImg->m_ChromaFormat;
  m_ColorSpace   = RefImg->m_ColorSpace;
  m_BitDepth     = RefImg->m_BitDepth;
  m_Id           = RefImg->m_ChromaFormat;
  m_CmpSpace     = RefImg->m_CmpSpace;
  m_CmpTotal     = RefImg->m_CmpTotal;
  
  ::memcpy(m_Name, RefImg->m_Name, sizeof(m_Name)); 

  m_Cmp = (xCmp**)xCalloc(RefImg->m_CmpTotal, sizeof(xCmp*));

  for(int32 CmpIdx=0; CmpIdx<RefImg->m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx] = new xCmp;
    m_Cmp[CmpIdx]->createCompatible(RefImg->m_Cmp[CmpIdx]);
  }
}

void xImg::duplicate(xImg* RefImg)
{
  createCompatible(RefImg);
  copy(RefImg);
}

void xImg::clear()
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->clear();
  }
}

void xImg::copy(xImg* Src)
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->copy(Src->m_Cmp[CmpIdx]);
  }
}

void xImg::copyEx(xImg* Src, int32 DstX, int32 DstY, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY)
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->copyEx(Src->m_Cmp[CmpIdx], DstX, DstY, SrcSX, SrcSY, SrcEX, SrcEY);
  }
}

void xImg::set(int16 Value)
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->set(Value);
  }
}

void xImg::setChroma(int16 Value)
{
  xAssert(m_ImageType==ImgTp_YUV && m_ChromaFormat!=CrF_400);
  m_Cmp[1]->set(Value);
  m_Cmp[2]->set(Value);
}

void xImg::setName(xchar* Name)
{
  xAssert(Name!=nullptr);
  int32 Len = xClipU((int32)xstrlen(Name), CmpNameLen-1);
  ::memcpy(m_Name, Name, Len*sizeof(xchar));
}

//resize and rescale
void xImg::rescaleFast(xImg* Src)
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->rescaleFast(Src->m_Cmp[CmpIdx]);
  }
}

void xImg::rescaleChroma(xImg* Src)
{
  xAssert(m_ImageType==ImgTp_YUV && m_ChromaFormat!=CrF_400);
  m_Cmp[1]->rescaleChroma(Src->m_Cmp[1]);
  m_Cmp[2]->rescaleChroma(Src->m_Cmp[2]);
}

//precision
void xImg::bitDepthDecrease(xImg* Src, int32 DeltaBits)
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->bitDepthDecrease(Src->m_Cmp[CmpIdx], DeltaBits);
  }
}

void xImg::bitDepthIncrease(xImg* Src, int32 DeltaBits)
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->bitDepthIncrease(Src->m_Cmp[CmpIdx], DeltaBits);
  }
}

//crop & extend
void xImg::crop(xImg* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY)
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->crop(Src->m_Cmp[CmpIdx], SrcSX, SrcSY, SrcEX, SrcEY);
  }
}

void xImg::extend(xImg* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY, eCmpExtMode Mode)
{
  for(int32 CmpIdx=0; CmpIdx<m_CmpTotal; CmpIdx++)
  {
    m_Cmp[CmpIdx]->extend(Src->m_Cmp[CmpIdx], SrcSX, SrcSY, SrcEX, SrcEY, Mode);
  }
}

//color space conversion
void xImg::convertRGB2YUV(xImg* Src)
{
  xColorSpace::convertRGB2YUV_Cont(Src->m_Cmp[0]->m_PelBuffer, Src->m_Cmp[1]->m_PelBuffer, Src->m_Cmp[2]->m_PelBuffer,
                                   m_Cmp[0]->m_PelBuffer, m_Cmp[1]->m_PelBuffer, m_Cmp[2]->m_PelBuffer,
                                   m_Cmp[0]->m_BufferSize, m_Cmp[0]->m_BitDepth, m_ColorSpace);
}

void xImg::convertYUV2RGB(xImg* Src)
{
  xColorSpace::convertYUV2RGB_Cont(Src->m_Cmp[0]->m_PelBuffer, Src->m_Cmp[1]->m_PelBuffer, Src->m_Cmp[2]->m_PelBuffer,
                                   m_Cmp[0]->m_PelBuffer, m_Cmp[1]->m_PelBuffer, m_Cmp[2]->m_PelBuffer,
                                   m_Cmp[0]->m_BufferSize, m_Cmp[0]->m_BitDepth, Src->m_ColorSpace);
}

//load and store form RAW YUV
void xImg::getRAW(FILE_HANDLE File)
{
  xAssert(File!=INVALID_HANDLE_VALUE && File!=NULL);
  int32 BufferSize = calcSizeRAW(m_Cmp[0]->m_SizeX, m_Cmp[0]->m_SizeY, m_BitDepth, m_ChromaFormat);
  uint8* Buff = (uint8*)xAlignedMalloc(BufferSize*sizeof(uint8),xRegAlign);
  int32 Read = x_fread(File, (void*)Buff, BufferSize);
  xAssert(Read==BufferSize);
  unpackRAW(Buff);
  xAlignedFree(Buff);
}

void xImg::putRAW(FILE_HANDLE File)
{
  xAssert(File!=INVALID_HANDLE_VALUE && File!=NULL);
  int32 BufferSize = calcSizeRAW(m_Cmp[0]->m_SizeX, m_Cmp[0]->m_SizeY, m_BitDepth, m_ChromaFormat);
  uint8* Buff = (uint8*)xAlignedMalloc(BufferSize*sizeof(uint8),xRegAlign);
  packRAW(Buff);
  int32 Written = x_fwrite(File, (void*)Buff, BufferSize);
  xAssert(Written==BufferSize);
  xAlignedFree(Buff);
}

void xImg::unpackRAW(uint8* Src)
{
  __m128i Zero = _mm_setzero_si128();
  int32 BitDepthMultiplier = (m_BitDepth<=8) ? 1 : 2;

  for(int32 k=0; k<3; k++)
  {    
    int32 Points = m_Cmp[k]->m_SizeY*m_Cmp[k]->m_SizeX;
    int32 PointsDivBy16 = Points-(Points%16);    

    switch (BitDepthMultiplier)
    {
    case 1:      
      for(int32 i=0; i<PointsDivBy16; i+=16)
      {
        __m128i in = _mm_loadu_si128((__m128i*)&(Src[i]));
        __m128i out1 = _mm_unpacklo_epi8(in, Zero);
        __m128i out2 = _mm_unpackhi_epi8(in, Zero);
        _mm_store_si128 ((__m128i*)(&(m_Cmp[k]->m_PelOrg[i]))  , out1);
        _mm_store_si128 ((__m128i*)(&(m_Cmp[k]->m_PelOrg[i+8])), out2);
      }

      if(Points != PointsDivBy16)
      {
        for(int32 i=PointsDivBy16; i<Points; i++)
        {
          m_Cmp[k]->m_PelOrg[i] = (int16)((uint16)Src[i]);
        }
      }
      break;
    case 2:
      memcpy(m_Cmp[k]->m_PelOrg, Src, Points*BitDepthMultiplier);
      break;
    }  

    Src += Points*BitDepthMultiplier;
  }
}

void xImg::packRAW(uint8* Dst)
{
  int32 BitDepthMultiplier = (m_BitDepth<=8) ? 1 : 2;

  for(int32 k=0; k<3; k++)
  {    
    int32 Points = m_Cmp[k]->m_SizeY*m_Cmp[k]->m_SizeX;
    int32 PointsDivBy16 = Points-(Points%16);    

    switch(BitDepthMultiplier)
    {
    case 1:
      for(int32 i=0; i<PointsDivBy16; i+=16)
      {
        __m128i In1 = _mm_load_si128((__m128i*)&(m_Cmp[k]->m_PelOrg[i  ]));
        __m128i In2 = _mm_load_si128((__m128i*)&(m_Cmp[k]->m_PelOrg[i+8]));
        __m128i Out = _mm_packus_epi16(In1, In2);
        _mm_storeu_si128 ((__m128i*)(&(Dst[i])), Out);
      }
      if(Points != PointsDivBy16)
      {
        for(int32 i=PointsDivBy16; i<Points; i++)
        {
          Dst[i] = (uint8)xClipU8(m_Cmp[k]->m_PelOrg[i]);
        }
      }    
      break;
    case 2:
      memcpy(Dst, m_Cmp[k]->m_PelOrg, Points*BitDepthMultiplier);
      break;
    }

    Dst += Points*BitDepthMultiplier;
  }
}

int32 xImg::calcSizeRAW(int32 SizeX, int32 SizeY, int32 BitDepth, eCrF ChromaFormat)
{
  int32 SamplingReference = (int32)ChromaFormat/100;
  int32 ChromaSamplesIn1stRow = (ChromaFormat-SamplingReference*100)/10;
  int32 ChromaSamplesIn2ndRow = ChromaFormat%10;

  int32 LumaSamples = 2*SamplingReference;
  int32 ChromaSamples = ChromaSamplesIn1stRow + ChromaSamplesIn2ndRow;

  int32 BitDepthMultiplier = (BitDepth<=8) ? 1 : 2;

  int32 FrameSize = (SizeX*SizeY*BitDepthMultiplier*(LumaSamples+(2*ChromaSamples)))/LumaSamples;

  return FrameSize;
}

//load and store BMP
#pragma pack (2)
__declspec(align(2)) struct xBitmapFileHeader
{
  uint16  Type;
  uint32  FileSize;
  uint16  Reserved1;
  uint16  Reserved2;
  uint32  Offset;
};

#pragma pack (2)
__declspec(align(2)) struct xBitmapInfoHeader
{
  uint32  HeaderSize;
  int32   Width;
  int32   Height;
  uint16  Planes;
  uint16  BitsPerPixel;
  uint32  Compression;
  uint32  ImageSize;
  int32   PelsPerMeterX;
  int32   PelsPerMeterY;
  uint32  ColorsUsed;
  uint32  ColorsImportant;
};

void xImg::getBMP(FILE_HANDLE File)
{
  xBitmapFileHeader BitmapFileHeader;
  x_fread(File, &BitmapFileHeader, sizeof(xBitmapFileHeader));

  xAssert(BitmapFileHeader.Type==0x4d42 && BitmapFileHeader.Offset >= (sizeof(xBitmapFileHeader)+sizeof(xBitmapInfoHeader)));

  xBitmapInfoHeader BitmapInfoHeader;
  x_fread(File, &BitmapInfoHeader, sizeof(xBitmapInfoHeader));

  xAssert(BitmapInfoHeader.HeaderSize==40);

  int32 v    = BitmapInfoHeader.Width;
  int32 h    = BitmapInfoHeader.Height;
  int32 Lev  = BitmapInfoHeader.BitsPerPixel;
  int32 AbsV = xAbs(v);
  int32 AbsH = xAbs(h);
  int32 NumComponents = Lev==32 ? 4 : 3;
  uint8* LineBuff = (uint8*)calloc(v*4, sizeof(uint8)); //line buffer
  uint8* PaletteBuff = nullptr;
  
  //Inicjacja obrazu
  if(m_CmpTotal>0) { destroy(); }
  if(NumComponents==3) { create(AbsV, AbsH, 0, 8, ImgTp_RGB,  CrF_UNKNOWN, ClrSpc_Unknown, 0); }
  else                 { create(AbsV, AbsH, 0, 8, ImgTp_RGBA, CrF_UNKNOWN, ClrSpc_Unknown, 0); }

  //Load Paletee
  if((Lev==1)||(Lev==4)||(Lev==8))
  {
    int32 NumOfColors = 1<<Lev;
    PaletteBuff = (uint8*)xMalloc(4*NumOfColors*sizeof(uint8));
    x_fread(File, (void*)PaletteBuff, 4*NumOfColors*sizeof(uint8));
  }

  //Seek to the image beginning
  x_fseek(File,(uint64) BitmapFileHeader.Offset, X_SEEK_SET);
  
  //Read image
  switch(Lev)
  {
  case 1: //Paletee 1bit bitmap
    {
      int32 ls = ((v-1)/8+4)&0xFFFFFFFC;
      if(h>0)
      {
        for(int32 j=(h-1); j>=0; j--)
        {
          x_fread(File, LineBuff, ls);
          for(int32 i=0,l=0; i<v-v%2; i+=2,l++)
          {
            for(int32 k=7; k>=0; k--)
            {
              m_Cmp[0]->m_Pel[j][i+7]= PaletteBuff[((LineBuff[l]&1)<<2)+2];
              m_Cmp[1]->m_Pel[j][i+7]= PaletteBuff[((LineBuff[l]&1)<<2)+1];
              m_Cmp[2]->m_Pel[j][i+7]= PaletteBuff[((LineBuff[l]&1)<<2)+0];
              LineBuff[l]>>=1; 
            }
          }
        }
      }
      else
      {
        for(int32 j=0; j<AbsH; j++)
        {
          x_fread(File, LineBuff, ls);
          for(int32 i=0,l=0; i<v-v%2; i+=2,l++)
          {
            for(int32 k=7; k>=0; k--)
            {
              m_Cmp[0]->m_Pel[j][i+7]= PaletteBuff[((LineBuff[l]&1)<<2)+2];
              m_Cmp[1]->m_Pel[j][i+7]= PaletteBuff[((LineBuff[l]&1)<<2)+1];
              m_Cmp[2]->m_Pel[j][i+7]= PaletteBuff[((LineBuff[l]&1)<<2)+0];
              LineBuff[l]>>=1; 
            }
          }
        }
      }
      break;
    }

  case 4: //Paletee 4bit bitmap
    {
      int32 ls = ((v-1)/2+4)&0xFFFFFFFC;
      if(h>0)
      {
        for(int32 j=(h-1); j>=0; j--)
        {
          x_fread(File, LineBuff, ls);
          for(int32 i=0,l=0; i<v-v%2; i+=2,l++)
          {
            m_Cmp[0]->m_Pel[j][i+1]= PaletteBuff[((LineBuff[l]&15)<<2)+2];
            m_Cmp[1]->m_Pel[j][i+1]= PaletteBuff[((LineBuff[l]&15)<<2)+1];
            m_Cmp[2]->m_Pel[j][i+1]= PaletteBuff[((LineBuff[l]&15)<<2)+0];
            LineBuff[l]>>=4; 
            m_Cmp[0]->m_Pel[j][i]= PaletteBuff[((LineBuff[l]&15)<<2)+2];
            m_Cmp[1]->m_Pel[j][i]= PaletteBuff[((LineBuff[l]&15)<<2)+1];
            m_Cmp[2]->m_Pel[j][i]= PaletteBuff[((LineBuff[l]&15)<<2)+0];
          }
        }
      }
      else
      {
        for(int32 j=0; j<AbsH; j++)
        {
          x_fread(File, LineBuff, ls);
          for(int32 i=0,l=0; i<v-v%2; i+=2,l++)
          {
            m_Cmp[0]->m_Pel[j][i+1]= PaletteBuff[((LineBuff[l]&15)<<2)+2];
            m_Cmp[1]->m_Pel[j][i+1]= PaletteBuff[((LineBuff[l]&15)<<2)+1];
            m_Cmp[2]->m_Pel[j][i+1]= PaletteBuff[((LineBuff[l]&15)<<2)+0];
            LineBuff[l]>>=4; 
            m_Cmp[0]->m_Pel[j][i]= PaletteBuff[((LineBuff[l]&15)<<2)+2];
            m_Cmp[1]->m_Pel[j][i]= PaletteBuff[((LineBuff[l]&15)<<2)+1];
            m_Cmp[2]->m_Pel[j][i]= PaletteBuff[((LineBuff[l]&15)<<2)+0];
          }
        }
      }
      break;
    }
  case 8: //Paletee 8bit bitmap
    {
      int32 ls = (v+3)&0xFFFFFFFC; //ls = v%4 //4byte alignment
      if(h>0)
      {
        for(int32 j=(h-1); j>=0; j--)
        {
          x_fread(File, LineBuff, ls);
          for(int32 i=0,l=0; i<v; i++) 
          {
            m_Cmp[0]->m_Pel[j][i]= PaletteBuff[(LineBuff[i]<<2)+2];//R
            m_Cmp[1]->m_Pel[j][i]= PaletteBuff[(LineBuff[i]<<2)+1];//G
            m_Cmp[2]->m_Pel[j][i]= PaletteBuff[(LineBuff[i]<<2)+0];//B
          }
        }
      }
      else
      {
        for(int32 j=0; j<AbsH; j++)
        {
          x_fread(File, LineBuff, ls);
          for(int32 i=0,l=0; i<v; i++) 
          {
            m_Cmp[0]->m_Pel[j][i]= PaletteBuff[(LineBuff[i]<<2)+2];//R
            m_Cmp[1]->m_Pel[j][i]= PaletteBuff[(LineBuff[i]<<2)+1];//G
            m_Cmp[2]->m_Pel[j][i]= PaletteBuff[(LineBuff[i]<<2)+0];//B
          }
        }
      }
      break;
    }

  case 24: //RGB 24bit bitmap
    {      
      int32 ls = (v*3+3)&0xFFFFFFFC; //4byte alignment
      if(h>0)
      {
        for(int32 j=(h-1); j>=0; j--)
        {
          x_fread(File, LineBuff, ls);
          for(int32 i=0,l=0; i<v; i++,l+=3) 
          {
            m_Cmp[2]->m_Pel[j][i] = LineBuff[l+0];
            m_Cmp[1]->m_Pel[j][i] = LineBuff[l+1];
            m_Cmp[0]->m_Pel[j][i] = LineBuff[l+2];
          }
        }
      }
      else
      {
        for(int32 j=0; j<AbsH; j++)
        {
          x_fread(File, LineBuff, ls);
          for(int32 i=0,l=0; i<v; i++,l+=3) 
          {
            m_Cmp[2]->m_Pel[j][i] = LineBuff[l+0];
            m_Cmp[1]->m_Pel[j][i] = LineBuff[l+1];
            m_Cmp[0]->m_Pel[j][i] = LineBuff[l+2];
          }
        }
      }
      break;
    }

  case 32: //RGBA 32bit bitmap 
    {
      if(h>0)
      {
        for(int32 j=(h-1); j>=0; j--)
        {
          x_fread(File, LineBuff, v*4);
          for(int32 i=0,l=0; i<v; i++,l+=4) 
          {
            m_Cmp[2]->m_Pel[j][i] = LineBuff[l+0]; //B
            m_Cmp[1]->m_Pel[j][i] = LineBuff[l+1]; //G
            m_Cmp[0]->m_Pel[j][i] = LineBuff[l+2]; //R
            m_Cmp[3]->m_Pel[j][i] = LineBuff[l+3]; //A
          }
        }
      }
      else
      {
        for(int32 j=0; j<AbsH; j++)
        {
          x_fread(File, LineBuff, v*4);
          for(int32 i=0,l=0; i<v; i++,l+=4) 
          {
            m_Cmp[2]->m_Pel[j][i] = LineBuff[l+0]; //B
            m_Cmp[1]->m_Pel[j][i] = LineBuff[l+1]; //G
            m_Cmp[0]->m_Pel[j][i] = LineBuff[l+2]; //R
            m_Cmp[3]->m_Pel[j][i] = LineBuff[l+3]; //A
          }
        }
      }
      break;
    }

  default:
      xAssert(0);
      break;
  }

  free(LineBuff); 
  if(PaletteBuff) free(PaletteBuff);
}

void xImg::putBMP(FILE_HANDLE File)
{
  int32 h = m_Cmp[0]->m_SizeY;
  int32 v = m_Cmp[0]->m_SizeX;

  int32 NumComponents = m_ImageType == ImgTp_RGBA ? 4 : 3;
  int32 LineSize = NumComponents==4 ? v*4 : (v*3+3)&0xFFFFFFFC;
  
  xBitmapFileHeader BitmapFileHeader;
  BitmapFileHeader.Type               = 0x4d42;
  BitmapFileHeader.FileSize           = sizeof(xBitmapFileHeader)+sizeof(xBitmapInfoHeader)+LineSize*h;
  BitmapFileHeader.Reserved1          = 0;
  BitmapFileHeader.Reserved2          = 0;
  BitmapFileHeader.Offset             = sizeof(xBitmapFileHeader)+sizeof(xBitmapInfoHeader);
  x_fwrite(File, &BitmapFileHeader, sizeof(xBitmapFileHeader));

  xBitmapInfoHeader BitmapInfoHeader;
  BitmapInfoHeader.HeaderSize         = sizeof(xBitmapInfoHeader);
  BitmapInfoHeader.Width              = v;
  BitmapInfoHeader.Height             = h;
  BitmapInfoHeader.Planes             = 1;
  BitmapInfoHeader.BitsPerPixel       = 8*NumComponents;
  BitmapInfoHeader.Compression        = 0; //RGB
  BitmapInfoHeader.ImageSize          = 0;
  BitmapInfoHeader.PelsPerMeterX      = 0;
  BitmapInfoHeader.PelsPerMeterY      = 0;
  BitmapInfoHeader.ColorsUsed         = 0;
  BitmapInfoHeader.ColorsImportant    = 0;  
  x_fwrite(File, &BitmapInfoHeader, sizeof(xBitmapInfoHeader));

  uint8* LineBuff = (uint8*)xCalloc(LineSize, sizeof(uint8));

  if(NumComponents==3)
  {
    for(int32 j=(h-1); j>=0; j--)
    {
      for(int32 i=0,l=0; i<v; i++,l+=3)
      {
        LineBuff[l+0] = (uint8)m_Cmp[2]->m_Pel[j][i];
        LineBuff[l+1] = (uint8)m_Cmp[1]->m_Pel[j][i];
        LineBuff[l+2] = (uint8)m_Cmp[0]->m_Pel[j][i];
      }
      x_fwrite(File, LineBuff, LineSize);
    }
  }
  else
  {
    for(int32 j=(h-1); j>=0; j--)
    {
      for(int32 i=0,l=0; i<v; i++,l+=4)
      {
        LineBuff[l+0] = (uint8)m_Cmp[2]->m_Pel[j][i];
        LineBuff[l+1] = (uint8)m_Cmp[1]->m_Pel[j][i];
        LineBuff[l+2] = (uint8)m_Cmp[0]->m_Pel[j][i];
        LineBuff[l+3] = (uint8)m_Cmp[3]->m_Pel[j][i];
      }
      x_fwrite(File, LineBuff, LineSize);
    }
  }

  xFree(LineBuff);
}

} //end of namespace AVLib