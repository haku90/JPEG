#include "xPixel.h"

namespace AVlib {

void xPixel::xAddContinuous_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area)
{
  for(int32 i=0; i<Area; i++)
  { 
    Dst[i] = Src0[i] + Src1[i];   
  }
}
void xPixel::xAddContinuous_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area)
{
  for(int32 i=0; i<Area; i+=8)
  {
    __m128i Src0Block  = _mm_load_si128((__m128i*)&Src0[i]);
    __m128i Src1Block  = _mm_load_si128((__m128i*)&Src1[i]);
    __m128i Add        = _mm_adds_epi16(Src0Block, Src1Block); //
    _mm_store_si128((__m128i*)&Dst[i], Add);
  }
}
void xPixel::xAdd_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {      
      Dst[x] = Src0[x] + Src1[x];
    }
    Src0 += Src0Stride;
    Src1 += Src1Stride;
    Dst  += DstStride;  
  }
}
void xPixel::xAdd_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height)
{
  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i Src0Block  = _mm_load_si128((__m128i*)&Src0[x]);
        __m128i Src1Block  = _mm_load_si128((__m128i*)&Src1[x]);
        __m128i Add        = _mm_adds_epi16(Src0Block, Src1Block); //
        _mm_store_si128((__m128i*)&Dst[x], Add);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i Src0Block  = _mm_loadl_epi64((__m128i*)&Src0[x]);
        __m128i Src1Block  = _mm_loadl_epi64((__m128i*)&Src1[x]);
        __m128i Add        = _mm_adds_epi16(Src0Block, Src1Block); //
        _mm_storel_epi64((__m128i*)&Dst[x], Add);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
  else
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {      
        Dst[x] = Src0[x] + Src1[x];
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
}

void xPixel::xAddAndClipContinuous_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area, int16 ClippingRange)
{
  for(int32 i=0; i<Area; i++)
  { 
    Dst[i] = (int16)xClipU<int32>((int32)Src0[i] + (int32)Src1[i], (int32)ClippingRange);   
  }
}
void xPixel::xAddAndClipContinuous_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area, int16 ClippingRange)
{
  const __m128i Zero   = _mm_setzero_si128();
  const __m128i MaxVal = _mm_set1_epi16(ClippingRange);

  for(int32 i=0; i<Area; i+=8)
  {
    __m128i Src0Block  = _mm_load_si128((__m128i*)&Src0[i]);
    __m128i Src1Block  = _mm_load_si128((__m128i*)&Src1[i]);
    __m128i AddClip    = _mm_adds_epi16(Src0Block, Src1Block); //
    AddClip = _mm_min_epi16(AddClip, MaxVal);
    AddClip = _mm_max_epi16(AddClip, Zero);
    _mm_store_si128((__m128i*)&Dst[i], AddClip);
  }
}
void xPixel::xAddAndClip_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {      
      Dst[x] = xClipU<int16>(Src0[x] + Src1[x], ClippingRange);
    }
    Src0 += Src0Stride;
    Src1 += Src1Stride;
    Dst  += DstStride;  
  }
}
void xPixel::xAddAndClip_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange)
{
  const __m128i Zero   = _mm_setzero_si128();
  const __m128i MaxVal = _mm_set1_epi16(ClippingRange);

  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i Src0Block  = _mm_load_si128((__m128i*)&Src0[x]);
        __m128i Src1Block  = _mm_load_si128((__m128i*)&Src1[x]);
        __m128i AddClip    = _mm_adds_epi16(Src0Block, Src1Block); //
        AddClip = _mm_min_epi16(AddClip, MaxVal);
        AddClip = _mm_max_epi16(AddClip, Zero);
        _mm_store_si128((__m128i*)&Dst[x], AddClip);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i Src0Block  = _mm_loadl_epi64((__m128i*)&Src0[x]);
        __m128i Src1Block  = _mm_loadl_epi64((__m128i*)&Src1[x]);
        __m128i AddClip    = _mm_adds_epi16(Src0Block, Src1Block); //
        AddClip = _mm_min_epi16(AddClip, MaxVal);
        AddClip = _mm_max_epi16(AddClip, Zero);
        _mm_storel_epi64((__m128i*)&Dst[x], AddClip);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
  else
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {      
        Dst[x] = xClipU<int16>(Src0[x] + Src1[x], ClippingRange);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
}

void xPixel::xSubContinuous_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area)
{
  for(int32 i=0; i<Area; i++)
  {
    Dst[i] = Src0[i] - Src1[i];
  }
}
void xPixel::xSubContinuous_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area)
{
  for(int32 i=0; i<Area; i+=8)
  {
    __m128i Src0Block  = _mm_load_si128((__m128i*)&Src0[i]);
    __m128i Src1Block  = _mm_load_si128((__m128i*)&Src1[i]);
    __m128i Sub        = _mm_subs_epi16(Src0Block, Src1Block);
    _mm_store_si128((__m128i*)&Dst[i], Sub);
  }
}
void xPixel::xSub_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {      
      Dst[x] = Src0[x] - Src1[x];
    }
    Src0 += Src0Stride;
    Src1 += Src1Stride;
    Dst  += DstStride;  
  }
}
void xPixel::xSub_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height)
{
  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i Src0Block  = _mm_load_si128((__m128i*)&Src0[x]);
        __m128i Src1Block  = _mm_load_si128((__m128i*)&Src1[x]);
        __m128i Sub       = _mm_subs_epi16(Src0Block, Src1Block);
        _mm_store_si128((__m128i*)&Dst[x], Sub);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i Src0Block  = _mm_loadl_epi64((__m128i*)&Src0[x]);
        __m128i Src1Block  = _mm_loadl_epi64((__m128i*)&Src1[x]);
        __m128i Sub        = _mm_subs_epi16(Src0Block, Src1Block);
        _mm_storel_epi64((__m128i*)&Dst[x], Sub);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
  else
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {      
        Dst[x] = Src0[x] - Src1[x];
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;  
    }
  }
}

void xPixel::xBiPredRefEstContinuous_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area)
{
  for(int32 i=0; i<Area; i++)
  {
    Dst[i] = (Src0[i]<<1) - Src1[i];
  }
}
void xPixel::xBiPredRefEstContinuous_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area)
{
  for(int32 i=0; i<Area; i+=8)
  {
    __m128i Src0Block  = _mm_load_si128((__m128i*)&Src0[i]);
    __m128i Src1Block  = _mm_load_si128((__m128i*)&Src1[i]);
    Src0Block          = _mm_slli_epi16(Src0Block, 1);
    __m128i Sub        = _mm_subs_epi16(Src0Block, Src1Block);
    _mm_store_si128((__m128i*)&Dst[i], Sub);
  }
}
void xPixel::xBiPredRefEst_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {
      Dst[x] = (Src0[x]<<1) - Src1[x];
    }
    Src0 += Src0Stride;
    Src1 += Src1Stride;
    Dst  += DstStride;    
  }
}
void xPixel::xBiPredRefEst_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height)
{
  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i Src0Block  = _mm_load_si128((__m128i*)&Src0[x]);
        __m128i Src1Block  = _mm_load_si128((__m128i*)&Src1[x]);
        Src0Block          = _mm_slli_epi16(Src0Block, 1);
        __m128i Sub        = _mm_subs_epi16(Src0Block, Src1Block);
        _mm_store_si128((__m128i*)&Dst[x], Sub);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i Src0Block  = _mm_loadl_epi64((__m128i*)&Src0[x]);
        __m128i Src1Block  = _mm_loadl_epi64((__m128i*)&Src1[x]);
        Src0Block          = _mm_slli_epi16(Src0Block, 1);
        __m128i Sub        = _mm_subs_epi16(Src0Block, Src1Block);
        _mm_storel_epi64((__m128i*)&Dst[x], Sub);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
  else
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        Dst[x] = (Src0[x]<<1) - Src1[x];
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
}

void xPixel::xAvgContinuous_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area)
{
  for(int32 i=0; i<Area; i++)
  { 
    Dst[i] = (Src0[i] + Src1[i] + 1)>>1;
  }
}
void xPixel::xAvgContinuous_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area)
{
  const __m128i ZeroV   = _mm_setzero_si128();
  const __m128i OffsetV = _mm_set1_epi16(1);

  for(int32 i=0; i<Area; i+=8)
  {
    __m128i Src0BlockV  = _mm_load_si128((__m128i*)&Src0[i]);
    __m128i Src1BlockV  = _mm_load_si128((__m128i*)&Src1[i]);
    __m128i AddV        = _mm_adds_epi16(_mm_adds_epi16(Src0BlockV, Src1BlockV), OffsetV);
    __m128i ShiftV      = _mm_srai_epi16(AddV, 1);
    _mm_store_si128((__m128i*)&Dst[i], ShiftV);
  }
}
void xPixel::xAvg_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {
      Dst[x] = (Src0[x] + Src1[x] + 1)>>1;
    }
    Src0 += Src0Stride;
    Src1 += Src1Stride;
    Dst  += DstStride;    
  }
}
void xPixel::xAvg_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height)
{
  const __m128i ZeroV   = _mm_setzero_si128();
  const __m128i OffsetV = _mm_set1_epi16(1);

  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i Src0BlockV  = _mm_load_si128((__m128i*)&Src0[x]);
        __m128i Src1BlockV  = _mm_load_si128((__m128i*)&Src1[x]);
        __m128i AddV        = _mm_adds_epi16(_mm_adds_epi16(Src0BlockV, Src1BlockV), OffsetV);
        __m128i ShiftV      = _mm_srai_epi16(AddV, 1);
        _mm_store_si128((__m128i*)&Dst[x], ShiftV);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i Src0BlockV  = _mm_loadl_epi64((__m128i*)&Src0[x]);
        __m128i Src1BlockV  = _mm_loadl_epi64((__m128i*)&Src1[x]);
        __m128i AddV        = _mm_adds_epi16(_mm_adds_epi16(Src0BlockV, Src1BlockV), OffsetV);
        __m128i ShiftV      = _mm_srai_epi16(AddV, 1);
        _mm_storel_epi64((__m128i*)&Dst[x], ShiftV);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
  else
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        Dst[x] = (Src0[x] + Src1[x] + 1)>>1;
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
}

void xPixel::xAvgAndClipContinuous_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area, int16 ClippingRange)
{
  for(int32 i=0; i<Area; i++)
  { 
    Dst[i] = xClipU<int16>(((Src0[i] + Src1[i] + 1)>>1), ClippingRange);
  }
}
void xPixel::xAvgAndClipContinuous_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 Area, int16 ClippingRange)
{
  const __m128i ZeroV   = _mm_setzero_si128();
  const __m128i OffsetV = _mm_set1_epi16(1);
  const __m128i MaxValV = _mm_set1_epi16(ClippingRange);

  for(int32 i=0; i<Area; i+=8)
  {
    __m128i Src0BlockV  = _mm_load_si128((__m128i*)&Src0[i]);
    __m128i Src1BlockV  = _mm_load_si128((__m128i*)&Src1[i]);
    __m128i AddV        = _mm_adds_epi16(_mm_adds_epi16(Src0BlockV, Src1BlockV), OffsetV);
    __m128i ShiftV      = _mm_srai_epi16(AddV, 1);
    __m128i ClipV       = _mm_max_epi16(_mm_min_epi16(ShiftV, MaxValV),ZeroV);
    _mm_store_si128((__m128i*)&Dst[i], ClipV);
  }
}
void xPixel::xAvgAndClip_STD(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {
      Dst[x] = xClipU<int16>(((Src0[x] + Src1[x] + 1)>>1), ClippingRange);
    }
    Src0 += Src0Stride;
    Src1 += Src1Stride;
    Dst  += DstStride;    
  }
}
void xPixel::xAvgAndClip_SSE(int16* restrict Dst, int16* restrict Src0, int16* restrict Src1, int32 DstStride, int32 Src0Stride, int32 Src1Stride, int32 Width, int32 Height, int16 ClippingRange)
{
  const __m128i ZeroV   = _mm_setzero_si128();
  const __m128i OffsetV = _mm_set1_epi16(1);
  const __m128i MaxValV = _mm_set1_epi16(ClippingRange);

  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i Src0BlockV  = _mm_load_si128((__m128i*)&Src0[x]);
        __m128i Src1BlockV  = _mm_load_si128((__m128i*)&Src1[x]);
        __m128i AddV        = _mm_adds_epi16(_mm_adds_epi16(Src0BlockV, Src1BlockV), OffsetV);
        __m128i ShiftV      = _mm_srai_epi16(AddV, 1);
        __m128i ClipV       = _mm_max_epi16(_mm_min_epi16(ShiftV, MaxValV), ZeroV);
        _mm_store_si128((__m128i*)&Dst[x], ClipV);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i Src0BlockV  = _mm_loadl_epi64((__m128i*)&Src0[x]);
        __m128i Src1BlockV  = _mm_loadl_epi64((__m128i*)&Src1[x]);
        __m128i AddV        = _mm_adds_epi16(_mm_adds_epi16(Src0BlockV, Src1BlockV), OffsetV);
        __m128i ShiftV      = _mm_srai_epi16(AddV, 1);
        __m128i ClipV       = _mm_max_epi16(_mm_min_epi16(ShiftV, MaxValV),ZeroV);
        _mm_storel_epi64((__m128i*)&Dst[x], ClipV);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
  else
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        Dst[x] = xClipU<int16>(((Src0[x] + Src1[x] + 1)>>1), ClippingRange);
      }
      Src0 += Src0Stride;
      Src1 += Src1Stride;
      Dst  += DstStride;    
    }
  }
}

void xPixel::xClipContinuous_STD(int16* restrict Dst, int16* restrict Src, int32 Area, int16 ClippingRangeLow, int16 ClippingRangeHigh)
{
  for(int32 i=0; i<Area; i++)
  { 
    Dst[i] = xClip<int16>(Src[i], ClippingRangeLow, ClippingRangeHigh);
  }
}
void xPixel::xClipContinuous_SSE(int16* restrict Dst, int16* restrict Src, int32 Area, int16 ClippingRangeLow, int16 ClippingRangeHigh)
{
  const __m128i RangeLowV  = _mm_set1_epi16(ClippingRangeLow );
  const __m128i RangeHighV = _mm_set1_epi16(ClippingRangeHigh);

  for(int32 i=0; i<Area; i+=8)
  {
    __m128i SrcBlockV  = _mm_load_si128((__m128i*)&Src[i]);
    __m128i ClipV      = _mm_max_epi16(_mm_min_epi16(SrcBlockV, RangeHighV), RangeLowV);
    _mm_store_si128((__m128i*)&Dst[i], ClipV);
  }
}
void xPixel::xClip_STD(int16* restrict Dst, int16* restrict Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height, int16 ClippingRangeLow, int16 ClippingRangeHigh)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {
      Dst[x] = xClip<int16>(Src[x], ClippingRangeLow, ClippingRangeHigh);
    }
    Src += SrcStride;
    Dst += DstStride;    
  }
}
void xPixel::xClip_SSE(int16* restrict Dst, int16* restrict Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height, int16 ClippingRangeLow, int16 ClippingRangeHigh)
{
  const __m128i RangeLowV  = _mm_set1_epi16(ClippingRangeLow );
  const __m128i RangeHighV = _mm_set1_epi16(ClippingRangeHigh);

  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i SrcBlockV  = _mm_load_si128((__m128i*)&Src[x]);
        __m128i ClipV      = _mm_max_epi16(_mm_min_epi16(SrcBlockV, RangeHighV), RangeLowV);
        _mm_store_si128((__m128i*)&Dst[x], ClipV);
      }
      Src += SrcStride;
      Dst += DstStride;    
    }
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i SrcBlockV  = _mm_loadl_epi64((__m128i*)&Src[x]);
        __m128i ClipV      = _mm_max_epi16(_mm_min_epi16(SrcBlockV, RangeHighV), RangeLowV);
        _mm_storel_epi64((__m128i*)&Dst[x], ClipV);
      }
      Src += SrcStride;
      Dst += DstStride;    
    }
  }
  else
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        Dst[x] = xClip<int16>(Src[x], ClippingRangeLow, ClippingRangeHigh);
      }
      Src += SrcStride;
      Dst += DstStride;    
    }
  }
}

void xPixel::xPackI16ToU8Continuous_STD(int8* Dst, int16* Src, int32 Area)
{
  for(int32 i=0; i<Area; i++)
  { 
    Dst[i] = (uint8)xClipU8(Src[i]);   
  }
}
void xPixel::xPackI16ToU8Continuous_SSE(int8* Dst, int16* Src, int32 Area)
{
  uint32 i;
  uint32 Area16 = (uint32)Area & (uint32)0xFFFFFFF0;
  
  for(i=0; i<Area16; i+=16)
  { 
    __m128i In1 = _mm_load_si128((__m128i*)&(Src[i  ]));
    __m128i In2 = _mm_load_si128((__m128i*)&(Src[i+8]));
    __m128i Out = _mm_packus_epi16(In1, In2);
    _mm_storeu_si128 ((__m128i*)(&(Dst[i])), Out);
  }
  for( ; i<Area; i++)
  {
    Dst[i] = (uint8)xClipU8(Src[i]);
  }
}
void xPixel::xPackI16ToU8_STD(int8* Dst, int16* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {      
      Dst[x] = (uint8)xClipU8(Src[x]);
    }
    Src += SrcStride;
    Dst += DstStride;  
  }
}
void xPixel::xPackI16ToU8_SSE(int8* Dst, int16* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height)
{
  if(((uint32)Width & (uint32)0xF)==0) //Width%16==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=16)
      {
        __m128i In1 = _mm_load_si128((__m128i*)&(Src[x  ]));
        __m128i In2 = _mm_load_si128((__m128i*)&(Src[x+8]));
        __m128i Out = _mm_packus_epi16(In1, In2);
        _mm_storeu_si128 ((__m128i*)(&(Dst[x])), Out);
      }
      Src += SrcStride;
      Dst += DstStride;    
    }
  }
  else if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i In = _mm_load_si128((__m128i*)&(Src[x  ]));
        __m128i Out = _mm_packus_epi16(In, In);
        _mm_storel_epi64 ((__m128i*)(&(Dst[x])), Out);
      }
      Src += SrcStride;
      Dst += DstStride;    
    }
  }
  else
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        Dst[x] = (uint8)xClipU8(Src[x]);
      }
      Src += SrcStride;
      Dst += DstStride;    
    }
  }
}

void xPixel::xUnpackU8ToI16Continuous_STD(int16* Dst, int8* Src, int32 Area)
{
  for(int32 i=0; i<Area; i++)
  { 
    Dst[i] = (int16)(Src[i]);   
  }
}
void xPixel::xUnpackU8ToI16Continuous_SSE(int16* Dst, int8* Src, int32 Area)
{
  __m128i Zero = _mm_setzero_si128();

  for(int32 i=0; i<Area; i+=16)
  { 
    __m128i In = _mm_loadu_si128((__m128i*)&(Src[i]));
    __m128i Out1 = _mm_unpacklo_epi8(In, Zero);
    __m128i Out2 = _mm_unpackhi_epi8(In, Zero);
    _mm_store_si128 ((__m128i*)(&(Dst[i  ])), Out1);
    _mm_store_si128 ((__m128i*)(&(Dst[i+8])), Out2);
  }
}
void xPixel::xUnpackU8ToI16_STD(int16* Dst, int8* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height)
{
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {      
      Dst[x] = (int16)(Src[x]);
    }
    Src += SrcStride;
    Dst += DstStride;  
  }
}
void xPixel::xUnpackU8ToI16_SSE(int16* Dst, int8* Src, int32 DstStride, int32 SrcStride, int32 Width, int32 Height)
{

}


} //end of namespace AVLib


