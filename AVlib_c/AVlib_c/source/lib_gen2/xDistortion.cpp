#include "xDistortion.h"

namespace AVlib {

uint64 xDistortion::xCalcSADContinuous_STD(int16* restrict Org, int16* restrict Dist, int32 Area)
{
  uint64 SSD = 0;
  for(int32 i=0; i<Area; i++)
  {
    SSD += xAbs(Org[i] - Dist[i]);
  }
  return SSD;
}
uint64 xDistortion::xCalcSADContinuous_SSE(int16* restrict Org, int16* restrict Dist, int32 Area)
{
  __m128i SADBlock = _mm_setzero_si128();
  __m128i Zero     = _mm_setzero_si128();

  for(int32 i=0; i<Area; i+=8)
  {
    __m128i OrgBlock  = _mm_load_si128((__m128i*)&Org[i]);
    __m128i DistBlock = _mm_load_si128((__m128i*)&Dist[i]);
    __m128i DiffBlock = _mm_sub_epi16(OrgBlock, DistBlock);
    __m128i AbsBlock  = _mm_abs_epi16(DiffBlock);
    __m128i AbsBlock1 = _mm_unpacklo_epi16(AbsBlock, Zero);
    __m128i AbsBlock2 = _mm_unpackhi_epi32(AbsBlock, Zero);
    __m128i SumBlock  = _mm_add_epi32(AbsBlock1, AbsBlock2);
    SADBlock = _mm_add_epi32(SADBlock, SumBlock);
  }  

  SADBlock = _mm_hadd_epi32(SADBlock, SADBlock);
  SADBlock = _mm_hadd_epi32(SADBlock, SADBlock);

  uint64 SAD = _mm_extract_epi32(SADBlock, 0);
  return SAD;
}
uint64 xDistortion::xCalcSADContinuous_AVX(int16* restrict Org, int16* restrict Dist, int32 Area)
{
  __m256i SADBlock = _mm256_setzero_si256();
  __m256i Zero     = _mm256_setzero_si256();

  for(int32 i=0; i<Area; i+=16)
  {
    __m256i OrgBlock  = _mm256_load_si256((__m256i*)&Org[i]);
    __m256i DistBlock = _mm256_load_si256((__m256i*)&Dist[i]);
    __m256i DiffBlock = _mm256_sub_epi16(OrgBlock, DistBlock);
    __m256i AbsBlock  = _mm256_abs_epi16(DiffBlock);
    __m256i AbsBlock1 = _mm256_unpacklo_epi16(AbsBlock, Zero);
    __m256i AbsBlock2 = _mm256_unpackhi_epi16(AbsBlock, Zero);
    __m256i SumBlock  = _mm256_add_epi32(AbsBlock1, AbsBlock2);
    SADBlock = _mm256_add_epi32(SADBlock, SumBlock);
  }  

  __m128i SADBlock1 = _mm256_extracti128_si256(SADBlock, 1);
  __m128i SADBlock2 = _mm256_castsi256_si128(SADBlock);
  __m128i SADBlockA = _mm_add_epi32(SADBlock1, SADBlock2);
  SADBlockA = _mm_hadd_epi32(SADBlockA, SADBlockA);
  SADBlockA = _mm_hadd_epi32(SADBlockA, SADBlockA); 
  uint64 SAD = _mm_extract_epi32(SADBlockA, 0);
  return SAD;
}
uint64 xDistortion::xCalcSAD_STD(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride, int32 Width, int32 Height)
{
  int32 SAD = 0;
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {
      SAD += xAbs(Org[x] - Dist[x]);
    }
    Org  += OStride;
    Dist += DStride;
  }
  return SAD;
}
uint64 xDistortion::xCalcSAD_SSE(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride, int32 Width, int32 Height)
{
  int32 SAD = 0;
  __m128i SADV  = _mm_setzero_si128();
  __m128i ZeroV = _mm_setzero_si128();

  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i OrgV  = _mm_loadu_si128((__m128i*)&Org[x]);
        __m128i DistV = _mm_loadu_si128((__m128i*)&Dist[x]);
        __m128i DiffV = _mm_sub_epi16(OrgV, DistV);
        __m128i AbsV  = _mm_abs_epi16(DiffV);
        __m128i AbsV1 = _mm_unpacklo_epi16(AbsV, ZeroV);
        __m128i AbsV2 = _mm_unpackhi_epi16(AbsV, ZeroV);
        __m128i SumV  = _mm_add_epi32(AbsV1, AbsV2);
        SADV = _mm_add_epi32(SADV, SumV);
      }
      Org  += OStride;
      Dist += DStride;
    }
    SADV = _mm_hadd_epi32(SADV, SADV);
    SADV = _mm_hadd_epi32(SADV, SADV);
    SAD  = _mm_extract_epi32(SADV, 0);
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i OrgV  = _mm_loadl_epi64((__m128i*)&Org[x]);
        __m128i DistV = _mm_loadl_epi64((__m128i*)&Dist[x]);
        __m128i DiffV = _mm_sub_epi16(OrgV, DistV);
        __m128i AbsV  = _mm_abs_epi16(DiffV);
        __m128i AbsV1 = _mm_unpacklo_epi16(AbsV, ZeroV);
        SADV = _mm_add_epi32(SADV, AbsV1);
      }
      Org  += OStride;
      Dist += DStride;
    }
    SADV = _mm_hadd_epi32(SADV, SADV);
    SADV = _mm_hadd_epi32(SADV, SADV);
    SAD  = _mm_extract_epi32(SADV, 0);
  }
  else //any other
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        SAD += xAbs(Org[x] - Dist[x]);
      }
      Org  += OStride;
      Dist += DStride;
    }
  }
  return SAD;
}
uint64 xDistortion::xCalcSAD_AVX(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride, int32 Width, int32 Height)
{
  uint64 SAD = 0;

  if(((uint32)Width & (uint32)0x15)==0) //Width%16==0
  {
    __m256i SADV = _mm256_setzero_si256();
    __m256i Zero = _mm256_setzero_si256();

    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=16)
      {
        __m256i OrgV   = _mm256_load_si256((__m256i*)&Org[x]);
        __m256i DistV  = _mm256_load_si256((__m256i*)&Dist[x]);
        __m256i DiffV  = _mm256_sub_epi16(OrgV, DistV);
        __m256i AbsV   = _mm256_abs_epi16(DiffV);
        __m256i AbsV_1 = _mm256_unpacklo_epi16(AbsV, Zero);
        __m256i AbsV_2 = _mm256_unpackhi_epi16(AbsV, Zero);
        __m256i SumBlock  = _mm256_add_epi32(AbsV_1, AbsV_2);
        SADV = _mm256_add_epi32(SADV, SumBlock);
      }
      Org  += OStride;
      Dist += DStride;
    }
    __m128i SADV_1 = _mm256_extracti128_si256(SADV, 1);
    __m128i SADV_2 = _mm256_castsi256_si128(SADV);
    __m128i SADV_A = _mm_add_epi32(SADV_1, SADV_2);
    SADV_A = _mm_hadd_epi32(SADV_A, SADV_A);
    SADV_A = _mm_hadd_epi32(SADV_A, SADV_A); 
    SAD = _mm_extract_epi32(SADV_A, 0);
  }
  else if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    __m128i SADV  = _mm_setzero_si128();
    __m128i ZeroV = _mm_setzero_si128();

    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i OrgV  = _mm_loadu_si128((__m128i*)&Org[x]);
        __m128i DistV = _mm_loadu_si128((__m128i*)&Dist[x]);
        __m128i DiffV = _mm_sub_epi16(OrgV, DistV);
        __m128i AbsV  = _mm_abs_epi16(DiffV);
        __m128i AbsV1 = _mm_unpacklo_epi16(AbsV, ZeroV);
        __m128i AbsV2 = _mm_unpackhi_epi16(AbsV, ZeroV);
        __m128i SumV  = _mm_add_epi32(AbsV1, AbsV2);
        SADV = _mm_add_epi32(SADV, SumV);
      }
      Org  += OStride;
      Dist += DStride;
    }
    SADV = _mm_hadd_epi32(SADV, SADV);
    SADV = _mm_hadd_epi32(SADV, SADV);
    SAD  = _mm_extract_epi32(SADV, 0);
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    __m128i SADV  = _mm_setzero_si128();
    __m128i ZeroV = _mm_setzero_si128();

    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i OrgV  = _mm_loadl_epi64((__m128i*)&Org[x]);
        __m128i DistV = _mm_loadl_epi64((__m128i*)&Dist[x]);
        __m128i DiffV = _mm_sub_epi16(OrgV, DistV);
        __m128i AbsV  = _mm_abs_epi16(DiffV);
        __m128i AbsV1 = _mm_unpacklo_epi16(AbsV, ZeroV);
        SADV = _mm_add_epi32(SADV, AbsV1);
      }
      Org  += OStride;
      Dist += DStride;
    }
    SADV = _mm_hadd_epi32(SADV, SADV);
    SADV = _mm_hadd_epi32(SADV, SADV);
    SAD  = _mm_extract_epi32(SADV, 0);
  }
  else //any other
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        SAD += xAbs(Org[x] - Dist[x]);
      }
      Org  += OStride;
      Dist += DStride;
    }
  }
  return SAD;
}

uint64 xDistortion::xCalcSSDContinuous_STD(int16* restrict Org, int16* restrict Dist, int32 Area)
{
  uint64 SSD = 0;
  for(int32 i=0; i<Area; i++)
  {
    SSD += xPow2(Org[i] - Dist[i]);
  }
  return SSD;
}
uint64 xDistortion::xCalcSSDContinuous_SSE(int16* restrict Org, int16* restrict Dist, int32 Area)
{
  __m128i SSDBlock = _mm_setzero_si128();
  __m128i Zero     = _mm_setzero_si128();

  for(int32 i=0; i<Area; i+=8)
  {
    __m128i OrgBlock  = _mm_load_si128((__m128i*)&Org[i]);
    __m128i DistBlock = _mm_load_si128((__m128i*)&Dist[i]);
    __m128i DiffBlock = _mm_sub_epi16(OrgBlock, DistBlock);
    __m128i PowBlock  = _mm_madd_epi16(DiffBlock, DiffBlock);
    __m128i Block0    = _mm_unpacklo_epi32(PowBlock, Zero);
    __m128i Block1    = _mm_unpackhi_epi32(PowBlock, Zero);
    __m128i SumBlock  = _mm_add_epi64(Block0, Block1);
    SSDBlock = _mm_add_epi64(SSDBlock, SumBlock);
  }  
  uint64 SSD = _mm_extract_epi64(SSDBlock, 0) + _mm_extract_epi64(SSDBlock, 1);
  return SSD;
}
uint64 xDistortion::xCalcSSDContinuous_AVX(int16* restrict Org, int16* restrict Dist, int32 Area)
{
  __m256i SSDBlock = _mm256_setzero_si256();
  __m256i Zero     = _mm256_setzero_si256();

  for(int32 i=0; i<Area; i+=16)
  {
    __m256i OrgBlock  = _mm256_load_si256((__m256i*)&Org[i]);
    __m256i DistBlock = _mm256_load_si256((__m256i*)&Dist[i]);
    __m256i DiffBlock = _mm256_sub_epi16(OrgBlock, DistBlock);
    __m256i PowBlock  = _mm256_madd_epi16(DiffBlock, DiffBlock);
    __m256i Block0    = _mm256_unpacklo_epi32(PowBlock, Zero);
    __m256i Block1    = _mm256_unpackhi_epi32(PowBlock, Zero);
    __m256i SumBlock  = _mm256_add_epi64(Block0, Block1);
    SSDBlock = _mm256_add_epi64(SSDBlock, SumBlock);
  } 

  __m128i SSDBlock1 = _mm256_extracti128_si256(SSDBlock, 1);
  __m128i SSDBlock2 = _mm256_castsi256_si128(SSDBlock);
  __m128i SSDBlockA = _mm_add_epi64(SSDBlock1, SSDBlock2);
  uint64 SSD = _mm_extract_epi64(SSDBlockA, 0) + _mm_extract_epi64(SSDBlockA, 1);
  return SSD;
}
uint64 xDistortion::xCalcSSD_STD(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride, int32 Width, int32 Height)
{
  uint64 SSD = 0;
  for(int32 y=0; y<Height; y++)
  {
    for(int32 x=0; x<Width; x++)
    {
      SSD += xPow2((int32)Org[x] - (int32)Dist[x]);
    }
    Org  += OStride;
    Dist += DStride;
  }
  return SSD;
}
uint64 xDistortion::xCalcSSD_SSE(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride, int32 Width, int32 Height)
{
  uint64 SSD = 0;
  __m128i SSDV  = _mm_setzero_si128();
  __m128i ZeroV = _mm_setzero_si128();

  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i OrgV  = _mm_load_si128((__m128i*)&Org[x]);
        __m128i DistV = _mm_load_si128((__m128i*)&Dist[x]);
        __m128i DiffV = _mm_sub_epi16(OrgV, DistV);
        __m128i PowV  = _mm_madd_epi16(DiffV,DiffV);
        __m128i PowV1 = _mm_unpacklo_epi32(PowV, ZeroV);
        __m128i PowV2 = _mm_unpackhi_epi32(PowV, ZeroV);
        __m128i SumV  = _mm_add_epi64(PowV1, PowV2);
        SSDV = _mm_add_epi64(SSDV, SumV);
      }
      Org  += OStride;
      Dist += DStride;
    }
    SSD = _mm_extract_epi64(SSDV, 0) + _mm_extract_epi64(SSDV, 1);
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i OrgV  = _mm_loadl_epi64((__m128i*)&Org[x]);
        __m128i DistV = _mm_loadl_epi64((__m128i*)&Dist[x]);
        __m128i DiffV = _mm_sub_epi16(OrgV, DistV);
        __m128i PowV  = _mm_madd_epi16(DiffV,DiffV);
        __m128i PowV1 = _mm_unpacklo_epi32(PowV, ZeroV);
        SSDV = _mm_add_epi64(SSDV, PowV1);
      }
      Org  += OStride;
      Dist += DStride;
    }
    SSD = _mm_extract_epi64(SSDV, 0) + _mm_extract_epi64(SSDV, 1);
  }
  else //any other
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        SSD += xPow2((int32)Org[x] - (int32)Dist[x]);
      }
      Org  += OStride;
      Dist += DStride;
    }
  }
  return SSD;
}
uint64 xDistortion::xCalcSSD_AVX(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride, int32 Width, int32 Height)
{
  uint64 SSD = 0;
  
  if(((uint32)Width & (uint32)0x15)==0) //Width%16==0
  {
    __m256i SSDBlock = _mm256_setzero_si256();
    __m256i Zero     = _mm256_setzero_si256();
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=16)
      {
        __m256i OrgBlock  = _mm256_load_si256((__m256i*)&Org[x]);
        __m256i DistBlock = _mm256_load_si256((__m256i*)&Dist[x]);
        __m256i DiffBlock = _mm256_sub_epi16(OrgBlock, DistBlock);
        __m256i PowBlock  = _mm256_madd_epi16(DiffBlock, DiffBlock);
        __m256i Block0    = _mm256_unpacklo_epi32(PowBlock, Zero);
        __m256i Block1    = _mm256_unpackhi_epi32(PowBlock, Zero);
        __m256i SumBlock  = _mm256_add_epi64(Block0, Block1);
        SSDBlock = _mm256_add_epi64(SSDBlock, SumBlock);
      }
      Org  += OStride;
      Dist += DStride;
    }
    __m128i SSDBlock1 = _mm256_extracti128_si256(SSDBlock, 1);
    __m128i SSDBlock2 = _mm256_castsi256_si128(SSDBlock);
    __m128i SSDBlockA = _mm_add_epi64(SSDBlock1, SSDBlock2);
    SSD = _mm_extract_epi64(SSDBlockA, 0) + _mm_extract_epi64(SSDBlockA, 1);
  }
  if(((uint32)Width & (uint32)0x7)==0) //Width%8==0
  {
    __m128i SSDV  = _mm_setzero_si128();
    __m128i ZeroV = _mm_setzero_si128();
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        __m128i OrgV  = _mm_load_si128((__m128i*)&Org[x]);
        __m128i DistV = _mm_load_si128((__m128i*)&Dist[x]);
        __m128i DiffV = _mm_sub_epi16(OrgV, DistV);
        __m128i PowV  = _mm_madd_epi16(DiffV,DiffV);
        __m128i PowV1 = _mm_unpacklo_epi32(PowV, ZeroV);
        __m128i PowV2 = _mm_unpackhi_epi32(PowV, ZeroV);
        __m128i SumV  = _mm_add_epi64(PowV1, PowV2);
        SSDV = _mm_add_epi64(SSDV, SumV);
      }
      Org  += OStride;
      Dist += DStride;
    }
    SSD = _mm_extract_epi64(SSDV, 0) + _mm_extract_epi64(SSDV, 1);
  }
  else if(((uint32)Width & (uint32)0x3)==0) //Width%4==0
  {
    __m128i SSDV  = _mm_setzero_si128();
    __m128i ZeroV = _mm_setzero_si128();
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        __m128i OrgV  = _mm_loadl_epi64((__m128i*)&Org[x]);
        __m128i DistV = _mm_loadl_epi64((__m128i*)&Dist[x]);
        __m128i DiffV = _mm_sub_epi16(OrgV, DistV);
        __m128i PowV  = _mm_madd_epi16(DiffV,DiffV);
        __m128i PowV1 = _mm_unpacklo_epi32(PowV, ZeroV);
        SSDV = _mm_add_epi64(SSDV, PowV1);
      }
      Org  += OStride;
      Dist += DStride;
    }
    SSD = _mm_extract_epi64(SSDV, 0) + _mm_extract_epi64(SSDV, 1);
  }
  else //any other
  {
    for(int32 y=0; y<Height; y++)
    {
      for(int32 x=0; x<Width; x++)
      {
        SSD += xPow2((int32)Org[x] - (int32)Dist[x]);
      }
      Org  += OStride;
      Dist += DStride;
    }
  }
  return SSD;
}

uint64 xDistortion::xCalcSATD_2x2_STD(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  int16  Diff[4];
  int32  M[4];

  Diff[0] = Org[0          ] - Dist[0          ];
  Diff[1] = Org[1          ] - Dist[1          ];
  Diff[2] = Org[OStride    ] - Dist[0 + DStride];
  Diff[3] = Org[OStride + 1] - Dist[1 + DStride];
  
  M[0] = Diff[0] + Diff[2];
  M[1] = Diff[1] + Diff[3];
  M[2] = Diff[0] - Diff[2];
  M[3] = Diff[1] - Diff[3];

  uint64 SATD = xAbs(M[0] + M[1]) + xAbs(M[0] - M[1]) + xAbs(M[2] + M[3]) + xAbs(M[2] - M[3]);  
  return SATD;
}
uint64 xDistortion::xCalcSATDContinous_4x4_STD(int16* restrict Org, int16* restrict Dist)
{
  uint64 SATD = 0;
  int16  Diff[16];
  int32  M1[16], M2[16];
  
  int16* DiffLine = Diff;
  for(int32 y=0; y<4; y++)
  {    
    DiffLine[0] = Org[0] - Dist[0];
    DiffLine[1] = Org[1] - Dist[1];
    DiffLine[2] = Org[2] - Dist[2];
    DiffLine[3] = Org[3] - Dist[3];   
    Org  += 4;
    Dist += 4;
    DiffLine += 4;
  }

  M1[ 0] = Diff[ 0] + Diff[12];
  M1[ 1] = Diff[ 1] + Diff[13];
  M1[ 2] = Diff[ 2] + Diff[14];
  M1[ 3] = Diff[ 3] + Diff[15];
  M1[ 4] = Diff[ 4] + Diff[ 8];
  M1[ 5] = Diff[ 5] + Diff[ 9];
  M1[ 6] = Diff[ 6] + Diff[10];
  M1[ 7] = Diff[ 7] + Diff[11];
  M1[ 8] = Diff[ 4] - Diff[ 8];
  M1[ 9] = Diff[ 5] - Diff[ 9];
  M1[10] = Diff[ 6] - Diff[10];
  M1[11] = Diff[ 7] - Diff[11];
  M1[12] = Diff[ 0] - Diff[12];
  M1[13] = Diff[ 1] - Diff[13];
  M1[14] = Diff[ 2] - Diff[14];
  M1[15] = Diff[ 3] - Diff[15];
  
  M2[ 0] = M1[ 0] + M1[ 4];
  M2[ 1] = M1[ 1] + M1[ 5];
  M2[ 2] = M1[ 2] + M1[ 6];
  M2[ 3] = M1[ 3] + M1[ 7];
  M2[ 4] = M1[ 8] + M1[12];
  M2[ 5] = M1[ 9] + M1[13];
  M2[ 6] = M1[10] + M1[14];
  M2[ 7] = M1[11] + M1[15];
  M2[ 8] = M1[ 0] - M1[ 4];
  M2[ 9] = M1[ 1] - M1[ 5];
  M2[10] = M1[ 2] - M1[ 6];
  M2[11] = M1[ 3] - M1[ 7];
  M2[12] = M1[12] - M1[ 8];
  M2[13] = M1[13] - M1[ 9];
  M2[14] = M1[14] - M1[10];
  M2[15] = M1[15] - M1[11];
  
  M1[ 0] = M2[ 0] + M2[ 3];
  M1[ 1] = M2[ 1] + M2[ 2];
  M1[ 2] = M2[ 1] - M2[ 2];
  M1[ 3] = M2[ 0] - M2[ 3];
  M1[ 4] = M2[ 4] + M2[ 7];
  M1[ 5] = M2[ 5] + M2[ 6];
  M1[ 6] = M2[ 5] - M2[ 6];
  M1[ 7] = M2[ 4] - M2[ 7];
  M1[ 8] = M2[ 8] + M2[11];
  M1[ 9] = M2[ 9] + M2[10];
  M1[10] = M2[ 9] - M2[10];
  M1[11] = M2[ 8] - M2[11];
  M1[12] = M2[12] + M2[15];
  M1[13] = M2[13] + M2[14];
  M1[14] = M2[13] - M2[14];
  M1[15] = M2[12] - M2[15];
  
  M2[ 0] = M1[ 0] + M1[ 1];
  M2[ 1] = M1[ 0] - M1[ 1];
  M2[ 2] = M1[ 2] + M1[ 3];
  M2[ 3] = M1[ 3] - M1[ 2];
  M2[ 4] = M1[ 4] + M1[ 5];
  M2[ 5] = M1[ 4] - M1[ 5];
  M2[ 6] = M1[ 6] + M1[ 7];
  M2[ 7] = M1[ 7] - M1[ 6];
  M2[ 8] = M1[ 8] + M1[ 9];
  M2[ 9] = M1[ 8] - M1[ 9];
  M2[10] = M1[10] + M1[11];
  M2[11] = M1[11] - M1[10];
  M2[12] = M1[12] + M1[13];
  M2[13] = M1[12] - M1[13];
  M2[14] = M1[14] + M1[15];
  M2[15] = M1[15] - M1[14];
  
  for (int i = 0; i < 16; i++)
  {       
    SATD += xAbs(M2[i]); 
  } 
  return SATD;
}
uint64 xDistortion::xCalcSATDContinous_4x4_SSE(int16* restrict Org, int16* restrict Dist)
{
  //load
  __m128i OrigA = _mm_load_si128((__m128i*)(Org   ));
  __m128i OrigB = _mm_load_si128((__m128i*)(Org +8));
  __m128i DistA = _mm_load_si128((__m128i*)(Dist  ));
  __m128i DistB = _mm_load_si128((__m128i*)(Dist+8));
  //diff
  __m128i DiffA = _mm_sub_epi16(OrigA, DistA);
  __m128i DiffB = _mm_sub_epi16(OrigB, DistB);

  //int16->int32
  __m128i M1A = _mm_cvtepi16_epi32(DiffA);
  __m128i M1B = _mm_cvtepi16_epi32(_mm_srli_si128(DiffA,8));
  __m128i M1C = _mm_cvtepi16_epi32(DiffB);
  __m128i M1D = _mm_cvtepi16_epi32(_mm_srli_si128(DiffB,8));

  //1st transform
  __m128i M2A = _mm_add_epi32(M1A, M1D);
  __m128i M2B = _mm_add_epi32(M1B, M1C);
  __m128i M2C = _mm_sub_epi32(M1B, M1C);
  __m128i M2D = _mm_sub_epi32(M1A, M1D);
  M1A = _mm_add_epi32(M2A, M2B);
  M1B = _mm_add_epi32(M2C, M2D);
  M1C = _mm_sub_epi32(M2A, M2B);
  M1D = _mm_sub_epi32(M2D, M2C);

  //transpose
  M2A = _mm_unpacklo_epi32(M1A, M1B);
  M2B = _mm_unpacklo_epi32(M1C, M1D);
  M2C = _mm_unpackhi_epi32(M1A, M1B);
  M2D = _mm_unpackhi_epi32(M1C, M1D);
  M1A = _mm_unpacklo_epi64(M2A, M2B);
  M1B = _mm_unpackhi_epi64(M2A, M2B);
  M1C = _mm_unpacklo_epi64(M2C, M2D);
  M1D = _mm_unpackhi_epi64(M2C, M2D);

  //2nd transform
  M2A = _mm_add_epi32(M1A, M1D);
  M2B = _mm_add_epi32(M1B, M1C);
  M2C = _mm_sub_epi32(M1B, M1C);
  M2D = _mm_sub_epi32(M1A, M1D);
  M1A = _mm_add_epi32(M2A, M2B);
  M1B = _mm_add_epi32(M2C, M2D);
  M1C = _mm_sub_epi32(M2A, M2B);
  M1D = _mm_sub_epi32(M2D, M2C);

  //abs
  M2A = _mm_abs_epi32(M1A);
  M2B = _mm_abs_epi32(M1B);
  M2C = _mm_abs_epi32(M1C);
  M2D = _mm_abs_epi32(M1D);

  //sum
  __m128i Sum = _mm_add_epi32(_mm_add_epi32(M2A,M2B),_mm_add_epi32(M2C,M2D));
  Sum = _mm_hadd_epi32(Sum, Sum);
  Sum = _mm_hadd_epi32(Sum, Sum);

  uint64 SATD = (uint64)_mm_extract_epi32(Sum, 0);
  return SATD;
}
uint64 xDistortion::xCalcSATD_4x4_STD(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  uint64 SATD = 0;
  int16  Diff[16];
  int32  M1[16], M2[16];
  
  int16* DiffLine = Diff;
  for(int32 y=0; y<4; y++)
  {    
    DiffLine[0] = Org[0] - Dist[0];
    DiffLine[1] = Org[1] - Dist[1];
    DiffLine[2] = Org[2] - Dist[2];
    DiffLine[3] = Org[3] - Dist[3];   
    Org  += OStride;
    Dist += DStride;
    DiffLine += 4;
  }

  M1[ 0] = Diff[ 0] + Diff[12];
  M1[ 1] = Diff[ 1] + Diff[13];
  M1[ 2] = Diff[ 2] + Diff[14];
  M1[ 3] = Diff[ 3] + Diff[15];
  M1[ 4] = Diff[ 4] + Diff[ 8];
  M1[ 5] = Diff[ 5] + Diff[ 9];
  M1[ 6] = Diff[ 6] + Diff[10];
  M1[ 7] = Diff[ 7] + Diff[11];
  M1[ 8] = Diff[ 4] - Diff[ 8];
  M1[ 9] = Diff[ 5] - Diff[ 9];
  M1[10] = Diff[ 6] - Diff[10];
  M1[11] = Diff[ 7] - Diff[11];
  M1[12] = Diff[ 0] - Diff[12];
  M1[13] = Diff[ 1] - Diff[13];
  M1[14] = Diff[ 2] - Diff[14];
  M1[15] = Diff[ 3] - Diff[15];
  
  M2[ 0] = M1[ 0] + M1[ 4];
  M2[ 1] = M1[ 1] + M1[ 5];
  M2[ 2] = M1[ 2] + M1[ 6];
  M2[ 3] = M1[ 3] + M1[ 7];
  M2[ 4] = M1[ 8] + M1[12];
  M2[ 5] = M1[ 9] + M1[13];
  M2[ 6] = M1[10] + M1[14];
  M2[ 7] = M1[11] + M1[15];
  M2[ 8] = M1[ 0] - M1[ 4];
  M2[ 9] = M1[ 1] - M1[ 5];
  M2[10] = M1[ 2] - M1[ 6];
  M2[11] = M1[ 3] - M1[ 7];
  M2[12] = M1[12] - M1[ 8];
  M2[13] = M1[13] - M1[ 9];
  M2[14] = M1[14] - M1[10];
  M2[15] = M1[15] - M1[11];
  
  M1[ 0] = M2[ 0] + M2[ 3];
  M1[ 1] = M2[ 1] + M2[ 2];
  M1[ 2] = M2[ 1] - M2[ 2];
  M1[ 3] = M2[ 0] - M2[ 3];
  M1[ 4] = M2[ 4] + M2[ 7];
  M1[ 5] = M2[ 5] + M2[ 6];
  M1[ 6] = M2[ 5] - M2[ 6];
  M1[ 7] = M2[ 4] - M2[ 7];
  M1[ 8] = M2[ 8] + M2[11];
  M1[ 9] = M2[ 9] + M2[10];
  M1[10] = M2[ 9] - M2[10];
  M1[11] = M2[ 8] - M2[11];
  M1[12] = M2[12] + M2[15];
  M1[13] = M2[13] + M2[14];
  M1[14] = M2[13] - M2[14];
  M1[15] = M2[12] - M2[15];
  
  M2[ 0] = M1[ 0] + M1[ 1];
  M2[ 1] = M1[ 0] - M1[ 1];
  M2[ 2] = M1[ 2] + M1[ 3];
  M2[ 3] = M1[ 3] - M1[ 2];
  M2[ 4] = M1[ 4] + M1[ 5];
  M2[ 5] = M1[ 4] - M1[ 5];
  M2[ 6] = M1[ 6] + M1[ 7];
  M2[ 7] = M1[ 7] - M1[ 6];
  M2[ 8] = M1[ 8] + M1[ 9];
  M2[ 9] = M1[ 8] - M1[ 9];
  M2[10] = M1[10] + M1[11];
  M2[11] = M1[11] - M1[10];
  M2[12] = M1[12] + M1[13];
  M2[13] = M1[12] - M1[13];
  M2[14] = M1[14] + M1[15];
  M2[15] = M1[15] - M1[14];
  
  for (int i = 0; i < 16; i++)
  {       
    SATD += xAbs(M2[i]); 
  } 
  return SATD;
}
uint64 xDistortion::xCalcSATD_4x4_SSE(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  //load
  __m128i OrigA = _mm_loadl_epi64((__m128i*)Org);  Org += OStride;
  __m128i OrigB = _mm_loadl_epi64((__m128i*)Org);  Org += OStride;
  __m128i OrigC = _mm_loadl_epi64((__m128i*)Org);  Org += OStride;
  __m128i OrigD = _mm_loadl_epi64((__m128i*)Org);

  __m128i DistA = _mm_loadl_epi64((__m128i*)Dist);  Dist += DStride;
  __m128i DistB = _mm_loadl_epi64((__m128i*)Dist);  Dist += DStride;
  __m128i DistC = _mm_loadl_epi64((__m128i*)Dist);  Dist += DStride;
  __m128i DistD = _mm_loadl_epi64((__m128i*)Dist);

  OrigA = _mm_unpacklo_epi64(OrigA, OrigB);
  OrigB = _mm_unpacklo_epi64(OrigC, OrigD);
  DistA = _mm_unpacklo_epi64(DistA, DistB);
  DistB = _mm_unpacklo_epi64(DistC, DistD);

  //diff
  __m128i DiffA = _mm_sub_epi16(OrigA, DistA);
  __m128i DiffB = _mm_sub_epi16(OrigB, DistB);

  //int16->int32
  __m128i M1A = _mm_cvtepi16_epi32(DiffA);
  __m128i M1B = _mm_cvtepi16_epi32(_mm_srli_si128(DiffA,8));
  __m128i M1C = _mm_cvtepi16_epi32(DiffB);
  __m128i M1D = _mm_cvtepi16_epi32(_mm_srli_si128(DiffB,8));

  //1st transform
  __m128i M2A = _mm_add_epi32(M1A, M1D);
  __m128i M2B = _mm_add_epi32(M1B, M1C);
  __m128i M2C = _mm_sub_epi32(M1B, M1C);
  __m128i M2D = _mm_sub_epi32(M1A, M1D);
  M1A = _mm_add_epi32(M2A, M2B);
  M1B = _mm_add_epi32(M2C, M2D);
  M1C = _mm_sub_epi32(M2A, M2B);
  M1D = _mm_sub_epi32(M2D, M2C);

  //transpose
  M2A = _mm_unpacklo_epi32(M1A, M1B);
  M2B = _mm_unpacklo_epi32(M1C, M1D);
  M2C = _mm_unpackhi_epi32(M1A, M1B);
  M2D = _mm_unpackhi_epi32(M1C, M1D);
  M1A = _mm_unpacklo_epi64(M2A, M2B);
  M1B = _mm_unpackhi_epi64(M2A, M2B);
  M1C = _mm_unpacklo_epi64(M2C, M2D);
  M1D = _mm_unpackhi_epi64(M2C, M2D);

  //2nd transform
  M2A = _mm_add_epi32(M1A, M1D);
  M2B = _mm_add_epi32(M1B, M1C);
  M2C = _mm_sub_epi32(M1B, M1C);
  M2D = _mm_sub_epi32(M1A, M1D);
  M1A = _mm_add_epi32(M2A, M2B);
  M1B = _mm_add_epi32(M2C, M2D);
  M1C = _mm_sub_epi32(M2A, M2B);
  M1D = _mm_sub_epi32(M2D, M2C);

  //abs
  M2A = _mm_abs_epi32(M1A);
  M2B = _mm_abs_epi32(M1B);
  M2C = _mm_abs_epi32(M1C);
  M2D = _mm_abs_epi32(M1D);

  //sum
  __m128i Sum = _mm_add_epi32(_mm_add_epi32(M2A,M2B),_mm_add_epi32(M2C,M2D));
  Sum = _mm_hadd_epi32(Sum, Sum);
  Sum = _mm_hadd_epi32(Sum, Sum);

  uint64 SATD = (uint64)_mm_extract_epi32(Sum, 0);
  return SATD;
}
uint64 xDistortion::xCalcSATD_8x8_STD(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  int16  Diff[8][8];
  int32  M1[8][8], M2[8][8];
  int16* DiffLine = &Diff[0][0];
  uint64 SATD = 0;
  for(int32 y=0; y<8; y++)
  {    
    DiffLine[0] = Org[0] - Dist[0];
    DiffLine[1] = Org[1] - Dist[1];
    DiffLine[2] = Org[2] - Dist[2];
    DiffLine[3] = Org[3] - Dist[3];
    DiffLine[4] = Org[4] - Dist[4];
    DiffLine[5] = Org[5] - Dist[5];
    DiffLine[6] = Org[6] - Dist[6];
    DiffLine[7] = Org[7] - Dist[7];
    Org  += OStride;
    Dist += DStride;
    DiffLine += 8;
  }

  //horizontal
  for (int32 y=0; y<8; y++)
  {
    M2[y][0] = Diff[y][0] + Diff[y][4];
    M2[y][1] = Diff[y][1] + Diff[y][5];
    M2[y][2] = Diff[y][2] + Diff[y][6];
    M2[y][3] = Diff[y][3] + Diff[y][7];
    M2[y][4] = Diff[y][0] - Diff[y][4];
    M2[y][5] = Diff[y][1] - Diff[y][5];
    M2[y][6] = Diff[y][2] - Diff[y][6];
    M2[y][7] = Diff[y][3] - Diff[y][7];
    
    M1[y][0] = M2[y][0] + M2[y][2];
    M1[y][1] = M2[y][1] + M2[y][3];
    M1[y][2] = M2[y][0] - M2[y][2];
    M1[y][3] = M2[y][1] - M2[y][3];
    M1[y][4] = M2[y][4] + M2[y][6];
    M1[y][5] = M2[y][5] + M2[y][7];
    M1[y][6] = M2[y][4] - M2[y][6];
    M1[y][7] = M2[y][5] - M2[y][7];
    
    M2[y][0] = M1[y][0] + M1[y][1];
    M2[y][1] = M1[y][0] - M1[y][1];
    M2[y][2] = M1[y][2] + M1[y][3];
    M2[y][3] = M1[y][2] - M1[y][3];
    M2[y][4] = M1[y][4] + M1[y][5];
    M2[y][5] = M1[y][4] - M1[y][5];
    M2[y][6] = M1[y][6] + M1[y][7];
    M2[y][7] = M1[y][6] - M1[y][7];
  }
  
  //vertical
  for (int32 x=0; x < 8; x++)
  {
    M1[0][x] = M2[0][x] + M2[4][x];
    M1[1][x] = M2[1][x] + M2[5][x];
    M1[2][x] = M2[2][x] + M2[6][x];
    M1[3][x] = M2[3][x] + M2[7][x];
    M1[4][x] = M2[0][x] - M2[4][x];
    M1[5][x] = M2[1][x] - M2[5][x];
    M1[6][x] = M2[2][x] - M2[6][x];
    M1[7][x] = M2[3][x] - M2[7][x];
    
    M2[0][x] = M1[0][x] + M1[2][x];
    M2[1][x] = M1[1][x] + M1[3][x];
    M2[2][x] = M1[0][x] - M1[2][x];
    M2[3][x] = M1[1][x] - M1[3][x];
    M2[4][x] = M1[4][x] + M1[6][x];
    M2[5][x] = M1[5][x] + M1[7][x];
    M2[6][x] = M1[4][x] - M1[6][x];
    M2[7][x] = M1[5][x] - M1[7][x];
    
    M1[0][x] = M2[0][x] + M2[1][x];
    M1[1][x] = M2[0][x] - M2[1][x];
    M1[2][x] = M2[2][x] + M2[3][x];
    M1[3][x] = M2[2][x] - M2[3][x];
    M1[4][x] = M2[4][x] + M2[5][x];
    M1[5][x] = M2[4][x] - M2[5][x];
    M1[6][x] = M2[6][x] + M2[7][x];
    M1[7][x] = M2[6][x] - M2[7][x];
  }
  
  for (int y = 0; y < 8; y++)
  {       
    SATD += xAbs(M1[y][0]); 
    SATD += xAbs(M1[y][1]); 
    SATD += xAbs(M1[y][2]); 
    SATD += xAbs(M1[y][3]); 
    SATD += xAbs(M1[y][4]); 
    SATD += xAbs(M1[y][5]); 
    SATD += xAbs(M1[y][6]); 
    SATD += xAbs(M1[y][7]); 
  }
  return SATD;
}
uint64 xDistortion::xCalcSATD_8x8_SSE(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  __m128i tmp[8];
  __m128i sum = _mm_setzero_si128();
  const __m128i zero = _mm_setzero_si128();

  //load and horizontal transform
  for(int32 j=0; j<8; j++)
  {
    __m128i org_line  = _mm_loadu_si128((__m128i*)Org );
    __m128i dist_line = _mm_loadu_si128((__m128i*)Dist);
    Org  += OStride;
    Dist += DStride;

    __m128i line      = _mm_sub_epi16(org_line, dist_line);

    __m128i line_0  = _mm_cvtepi16_epi32(line);
    __m128i line_1  = _mm_cvtepi16_epi32(_mm_srli_si128(line,8));

    __m128i h_L0_0 = _mm_add_epi32(line_0, line_1);
    __m128i h_L0_1 = _mm_sub_epi32(line_0, line_1);
    __m128i h_L1_0 = _mm_unpacklo_epi64(h_L0_0, h_L0_1);
    __m128i h_L1_1 = _mm_unpackhi_epi64(h_L0_0, h_L0_1);
    __m128i h_L2_0 = _mm_add_epi32(h_L1_0, h_L1_1);
    __m128i h_L2_1 = _mm_sub_epi32(h_L1_0, h_L1_1);
    __m128i h_L3_0 = _mm_unpacklo_epi32(h_L2_0, h_L2_1);
    __m128i h_L3_1 = _mm_unpackhi_epi32(h_L2_0, h_L2_1);
    __m128i h_L4_0 = _mm_unpacklo_epi64(h_L3_0, h_L3_1);
    __m128i h_L4_1 = _mm_unpackhi_epi64(h_L3_0, h_L3_1);
    __m128i h_L5_0 = _mm_add_epi32(h_L4_0, h_L4_1);
    __m128i h_L5_1 = _mm_sub_epi32(h_L4_0, h_L4_1);
    __m128i h_L6_0 = _mm_unpacklo_epi32(h_L5_0, h_L5_1);
    __m128i h_L6_1 = _mm_unpackhi_epi32(h_L5_0, h_L5_1);

    tmp[j] = _mm_packs_epi32(h_L6_0, h_L6_1);
  }

  //transpose  
  {
    __m128i tr_A0 = _mm_unpacklo_epi16(tmp[0], tmp[4]);
    __m128i tr_A1 = _mm_unpackhi_epi16(tmp[0], tmp[4]);
    __m128i tr_A2 = _mm_unpacklo_epi16(tmp[1], tmp[5]);
    __m128i tr_A3 = _mm_unpackhi_epi16(tmp[1], tmp[5]);
    __m128i tr_A4 = _mm_unpacklo_epi16(tmp[2], tmp[6]);
    __m128i tr_A5 = _mm_unpackhi_epi16(tmp[2], tmp[6]);
    __m128i tr_A6 = _mm_unpacklo_epi16(tmp[3], tmp[7]);
    __m128i tr_A7 = _mm_unpackhi_epi16(tmp[3], tmp[7]);

    __m128i tr_B0 = _mm_unpacklo_epi16(tr_A0, tr_A4);
    __m128i tr_B1 = _mm_unpackhi_epi16(tr_A0, tr_A4);
    __m128i tr_B2 = _mm_unpacklo_epi16(tr_A1, tr_A5);
    __m128i tr_B3 = _mm_unpackhi_epi16(tr_A1, tr_A5);
    __m128i tr_B4 = _mm_unpacklo_epi16(tr_A2, tr_A6);
    __m128i tr_B5 = _mm_unpackhi_epi16(tr_A2, tr_A6);
    __m128i tr_B6 = _mm_unpacklo_epi16(tr_A3, tr_A7);
    __m128i tr_B7 = _mm_unpackhi_epi16(tr_A3, tr_A7);

    tmp[0] = _mm_unpacklo_epi16(tr_B0, tr_B4);
    tmp[1] = _mm_unpackhi_epi16(tr_B0, tr_B4);
    tmp[2] = _mm_unpacklo_epi16(tr_B1, tr_B5);
    tmp[3] = _mm_unpackhi_epi16(tr_B1, tr_B5);
    tmp[4] = _mm_unpacklo_epi16(tr_B2, tr_B6);
    tmp[5] = _mm_unpackhi_epi16(tr_B2, tr_B6);
    tmp[6] = _mm_unpacklo_epi16(tr_B3, tr_B7);
    tmp[7] = _mm_unpackhi_epi16(tr_B3, tr_B7);
  }

  //vertical transform (without last unpacking), abs, add
  for(int j=0; j<8; j++)
  {
    __m128i line = tmp[j]; 
    __m128i line_0  = _mm_unpacklo_epi16(zero, line);
    __m128i line_1  = _mm_unpackhi_epi16(zero, line);
    line_0 = _mm_srai_epi32(line_0, 16);
    line_1 = _mm_srai_epi32(line_1, 16);

    __m128i h_L0_0 = _mm_add_epi32(line_0, line_1);
    __m128i h_L0_1 = _mm_sub_epi32(line_0, line_1);
    __m128i h_L1_0 = _mm_unpacklo_epi64(h_L0_0, h_L0_1);
    __m128i h_L1_1 = _mm_unpackhi_epi64(h_L0_0, h_L0_1);
    __m128i h_L2_0 = _mm_add_epi32(h_L1_0, h_L1_1);
    __m128i h_L2_1 = _mm_sub_epi32(h_L1_0, h_L1_1);
    __m128i h_L3_0 = _mm_unpacklo_epi32(h_L2_0, h_L2_1);
    __m128i h_L3_1 = _mm_unpackhi_epi32(h_L2_0, h_L2_1);
    __m128i h_L4_0 = _mm_unpacklo_epi64(h_L3_0, h_L3_1);
    __m128i h_L4_1 = _mm_unpackhi_epi64(h_L3_0, h_L3_1);
    __m128i h_L5_0 = _mm_add_epi32(h_L4_0, h_L4_1);
    __m128i h_L5_1 = _mm_sub_epi32(h_L4_0, h_L4_1);
    
    __m128i h_L6_0 = _mm_abs_epi32(h_L5_0);
    __m128i h_L6_1 = _mm_abs_epi32(h_L5_1);
    sum = _mm_add_epi32(sum, h_L6_0);
    sum = _mm_add_epi32(sum, h_L6_1);
  }

  sum = _mm_hadd_epi32(sum, sum);
  sum = _mm_hadd_epi32(sum, sum);
  int32 SATD = _mm_extract_epi32(sum, 0);
  return SATD;
}
uint64 xDistortion::xCalcSATD_4x16_STD(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  int16  Diff[16][4];
  int32  M1[16][4], M2[16][4];
  int16* DiffLine = &Diff[0][0];
  uint64 SATD = 0;
  for(int32 y=0; y<16; y++)
  {    
    DiffLine[ 0] = Org[ 0] - Dist[ 0];
    DiffLine[ 1] = Org[ 1] - Dist[ 1];
    DiffLine[ 2] = Org[ 2] - Dist[ 2];
    DiffLine[ 3] = Org[ 3] - Dist[ 3];

    Org  += OStride;
    Dist += DStride;
    DiffLine += 4;
  }

  //horizontal
  for(int32 y=0; y<4; y++)
  {
    M2[y][0] = Diff[y][0] + Diff[y][2];
    M2[y][1] = Diff[y][1] + Diff[y][3];
    M2[y][2] = Diff[y][0] - Diff[y][2];
    M2[y][3] = Diff[y][1] - Diff[y][3];

    M1[y][0] = M2[y][0] + M2[y][1];
    M1[y][1] = M2[y][0] - M2[y][1];
    M1[y][2] = M2[y][2] + M2[y][3];
    M1[y][3] = M2[y][2] - M2[y][3];
  }
  
  //vertical
  for(int32 x=0; x < 16; x++)
  {
    M2[ 0][x] = M1[0][x] + M1[ 8][x];
    M2[ 1][x] = M1[1][x] + M1[ 9][x];
    M2[ 2][x] = M1[2][x] + M1[10][x];
    M2[ 3][x] = M1[3][x] + M1[11][x];
    M2[ 4][x] = M1[4][x] + M1[12][x];
    M2[ 5][x] = M1[5][x] + M1[13][x];
    M2[ 6][x] = M1[6][x] + M1[14][x];
    M2[ 7][x] = M1[7][x] + M1[15][x];
    M2[ 8][x] = M1[0][x] - M1[ 8][x];
    M2[ 9][x] = M1[1][x] - M1[ 9][x];
    M2[10][x] = M1[2][x] - M1[10][x];
    M2[11][x] = M1[3][x] - M1[11][x];
    M2[12][x] = M1[4][x] - M1[12][x];
    M2[13][x] = M1[5][x] - M1[13][x];
    M2[14][x] = M1[6][x] - M1[14][x];
    M2[15][x] = M1[7][x] - M1[15][x];

    M1[ 0][x] = M2[ 0][x] + M2[ 4][x];
    M1[ 1][x] = M2[ 1][x] + M2[ 5][x];
    M1[ 2][x] = M2[ 2][x] + M2[ 6][x];
    M1[ 3][x] = M2[ 3][x] + M2[ 7][x];
    M1[ 4][x] = M2[ 0][x] - M2[ 4][x];
    M1[ 5][x] = M2[ 1][x] - M2[ 5][x];
    M1[ 6][x] = M2[ 2][x] - M2[ 6][x];
    M1[ 7][x] = M2[ 3][x] - M2[ 7][x];
    M1[ 8][x] = M2[ 8][x] + M2[12][x];
    M1[ 9][x] = M2[ 9][x] + M2[13][x];
    M1[10][x] = M2[10][x] + M2[14][x];
    M1[11][x] = M2[11][x] + M2[15][x];
    M1[12][x] = M2[ 8][x] - M2[12][x];
    M1[13][x] = M2[ 9][x] - M2[13][x];
    M1[14][x] = M2[10][x] - M2[14][x];
    M1[15][x] = M2[11][x] - M2[15][x];

    M2[ 0][x] = M1[ 0][x] + M1[ 2][x];
    M2[ 1][x] = M1[ 1][x] + M1[ 3][x];
    M2[ 2][x] = M1[ 0][x] - M1[ 2][x];
    M2[ 3][x] = M1[ 1][x] - M1[ 3][x];
    M2[ 4][x] = M1[ 4][x] + M1[ 6][x];
    M2[ 5][x] = M1[ 5][x] + M1[ 7][x];
    M2[ 6][x] = M1[ 4][x] - M1[ 6][x];
    M2[ 7][x] = M1[ 5][x] - M1[ 7][x];
    M2[ 8][x] = M1[ 8][x] + M1[10][x];
    M2[ 9][x] = M1[ 9][x] + M1[11][x];
    M2[10][x] = M1[ 8][x] - M1[10][x];
    M2[11][x] = M1[ 9][x] - M1[11][x];
    M2[12][x] = M1[12][x] + M1[14][x];
    M2[13][x] = M1[13][x] + M1[15][x];
    M2[14][x] = M1[12][x] - M1[14][x];
    M2[15][x] = M1[13][x] - M1[15][x];

    M1[ 0][x] = M2[ 0][x] + M2[ 1][x];
    M1[ 1][x] = M2[ 0][x] - M2[ 1][x];
    M1[ 2][x] = M2[ 2][x] + M2[ 3][x];
    M1[ 3][x] = M2[ 2][x] - M2[ 3][x];
    M1[ 4][x] = M2[ 4][x] + M2[ 5][x];
    M1[ 5][x] = M2[ 4][x] - M2[ 5][x];
    M1[ 6][x] = M2[ 6][x] + M2[ 7][x];
    M1[ 7][x] = M2[ 6][x] - M2[ 7][x];
    M1[ 8][x] = M2[ 8][x] + M2[ 9][x];
    M1[ 9][x] = M2[ 8][x] - M2[ 9][x];
    M1[10][x] = M2[10][x] + M2[11][x];
    M1[11][x] = M2[10][x] - M2[11][x];
    M1[12][x] = M2[12][x] + M2[13][x];
    M1[13][x] = M2[12][x] - M2[13][x];
    M1[14][x] = M2[14][x] + M2[15][x];
    M1[15][x] = M2[14][x] - M2[15][x];
  }
  
  for(int32 y=0; y<16; y++)
  {       
    for (int32 x=0; x<4; x++)
    {
      SATD += xAbs(M1[y][x]); 
    }
  }
  return SATD;
}
uint64 xDistortion::xCalcSATD_4x16_SSE(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  __m128i buff[16];
  __m128i sum = _mm_setzero_si128();
  const __m128i zero = _mm_setzero_si128();

  //load and horizontal transform
  for(int32 j=0; j<16; j+=2)
  {
    // load odd line
    __m128i org_line1  = _mm_loadl_epi64((__m128i*)Org);
    __m128i dist_line1 = _mm_loadl_epi64((__m128i*)Dist);
    Org  += OStride;
    Dist += DStride;
    
    // load even line
	  __m128i org_line2  = _mm_loadl_epi64((__m128i*)Org);
    __m128i dist_line2 = _mm_loadl_epi64((__m128i*)Dist);
    Org  += OStride;
    Dist += DStride;

    __m128i diff1    = _mm_sub_epi16(org_line1, dist_line1);
	  __m128i diff2    = _mm_sub_epi16(org_line2, dist_line2);

    __m128i line1 = _mm_cvtepi16_epi32(diff1);
    __m128i line2 = _mm_cvtepi16_epi32(diff2);

	  __m128i h_L0_0 = _mm_unpacklo_epi64(line1, line2);
	  __m128i h_L0_1 = _mm_unpackhi_epi64(line1, line2);
    __m128i h_L1_0 = _mm_add_epi32(h_L0_0, h_L0_1);
    __m128i h_L1_1 = _mm_sub_epi32(h_L0_0, h_L0_1);
	  __m128i h_L2_0 = _mm_unpacklo_epi64(h_L1_0, h_L1_1);
	  __m128i h_L2_1 = _mm_unpackhi_epi64(h_L1_0, h_L1_1);
	  __m128i h_L3_0 = _mm_hadd_epi32(h_L2_0, h_L2_1);
	  __m128i h_L3_1 = _mm_hsub_epi32(h_L2_0, h_L2_1);

    buff[j  ] = _mm_unpacklo_epi32(h_L3_0, h_L3_1);
	  buff[j+1] = _mm_unpackhi_epi32(h_L3_0, h_L3_1);
  }

  //transpose
  for(int i=0; i<16; i+=4)
  {
    __m128i* buffptr =& buff[i];

    __m128i tr_A0 = _mm_unpacklo_epi32(buffptr[0], buffptr[2]);
    __m128i tr_A1 = _mm_unpackhi_epi32(buffptr[0], buffptr[2]);
    __m128i tr_A2 = _mm_unpacklo_epi32(buffptr[1], buffptr[3]);
    __m128i tr_A3 = _mm_unpackhi_epi32(buffptr[1], buffptr[3]);
    
    buffptr[0] = _mm_unpacklo_epi32(tr_A0, tr_A2);
    buffptr[1] = _mm_unpackhi_epi32(tr_A0, tr_A2);
    buffptr[2] = _mm_unpacklo_epi32(tr_A1, tr_A3);
    buffptr[3] = _mm_unpackhi_epi32(tr_A1, tr_A3);
  }

  //vertical transform (without last unpacking), abs, add
  for(int j=0; j<4; j++)
  {
    __m128i* line =& buff[j];
 
    __m128i h_L0_0 = _mm_add_epi32(line[0], line[8]);
    __m128i h_L0_1 = _mm_add_epi32(line[4], line[12]);
    __m128i h_L0_2 = _mm_sub_epi32(line[0], line[8]);
    __m128i h_L0_3 = _mm_sub_epi32(line[4], line[12]);

    __m128i h_L1_0 = _mm_add_epi32(h_L0_0, h_L0_1);
    __m128i h_L1_1 = _mm_sub_epi32(h_L0_0, h_L0_1);
    __m128i h_L1_2 = _mm_add_epi32(h_L0_2, h_L0_3);
    __m128i h_L1_3 = _mm_sub_epi32(h_L0_2, h_L0_3);

    __m128i h_L2_0 = _mm_unpacklo_epi64(h_L1_0, h_L1_1);
    __m128i h_L2_1 = _mm_unpackhi_epi64(h_L1_0, h_L1_1);
    __m128i h_L2_2 = _mm_unpacklo_epi64(h_L1_2, h_L1_3);
    __m128i h_L2_3 = _mm_unpackhi_epi64(h_L1_2, h_L1_3);

    __m128i h_L3_0 = _mm_add_epi32(h_L2_0, h_L2_1);
    __m128i h_L3_1 = _mm_sub_epi32(h_L2_0, h_L2_1);
    __m128i h_L3_2 = _mm_add_epi32(h_L2_2, h_L2_3);
    __m128i h_L3_3 = _mm_sub_epi32(h_L2_2, h_L2_3);

    __m128i h_L4_0 = _mm_hadd_epi32(h_L3_0, h_L3_1);
    __m128i h_L4_1 = _mm_hsub_epi32(h_L3_0, h_L3_1);
    __m128i h_L4_2 = _mm_hadd_epi32(h_L3_2, h_L3_3);
    __m128i h_L4_3 = _mm_hsub_epi32(h_L3_2, h_L3_3);

    __m128i h_L5_0 = _mm_abs_epi32(h_L4_0);
    __m128i h_L5_1 = _mm_abs_epi32(h_L4_1);
    __m128i h_L5_2 = _mm_abs_epi32(h_L4_2);
    __m128i h_L5_3 = _mm_abs_epi32(h_L4_3);

    __m128i h_L6_0 = _mm_add_epi32(h_L5_0, h_L5_1);
    __m128i h_L6_1 = _mm_add_epi32(h_L5_2, h_L5_3);

    __m128i h_L7_0 = _mm_add_epi32(h_L6_0, h_L6_1);
    
    sum = _mm_add_epi32(sum, h_L7_0);
  }

  sum = _mm_hadd_epi32(sum, sum);
  sum = _mm_hadd_epi32(sum, sum);
  int32 SATD = _mm_extract_epi32(sum, 0);
  return SATD;
}
uint64 xDistortion::xCalcSATD_16x4_STD(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  int16  Diff[4][16];
  int32  M1[4][16], M2[4][16];
  int16* DiffLine = &Diff[0][0];
  uint64 SATD = 0;
  for(int32 y=0; y<4; y++)
  {    
    DiffLine[ 0] = Org[ 0] - Dist[ 0];
    DiffLine[ 1] = Org[ 1] - Dist[ 1];
    DiffLine[ 2] = Org[ 2] - Dist[ 2];
    DiffLine[ 3] = Org[ 3] - Dist[ 3];
    DiffLine[ 4] = Org[ 4] - Dist[ 4];
    DiffLine[ 5] = Org[ 5] - Dist[ 5];
    DiffLine[ 6] = Org[ 6] - Dist[ 6];
    DiffLine[ 7] = Org[ 7] - Dist[ 7];
    DiffLine[ 8] = Org[ 8] - Dist[ 8];
    DiffLine[ 9] = Org[ 9] - Dist[ 9];
    DiffLine[10] = Org[10] - Dist[10];
    DiffLine[11] = Org[11] - Dist[11];
    DiffLine[12] = Org[12] - Dist[12];
    DiffLine[13] = Org[13] - Dist[13];
    DiffLine[14] = Org[14] - Dist[14];
    DiffLine[15] = Org[15] - Dist[15];

    Org  += OStride;
    Dist += DStride;
    DiffLine += 16;
  }

  //horizontal
  for(int32 y=0; y<4; y++)
  {
    M2[y][ 0] = Diff[y][0] + Diff[y][ 8];
    M2[y][ 1] = Diff[y][1] + Diff[y][ 9];
    M2[y][ 2] = Diff[y][2] + Diff[y][10];
    M2[y][ 3] = Diff[y][3] + Diff[y][11];
    M2[y][ 4] = Diff[y][4] + Diff[y][12];
    M2[y][ 5] = Diff[y][5] + Diff[y][13];
    M2[y][ 6] = Diff[y][6] + Diff[y][14];
    M2[y][ 7] = Diff[y][7] + Diff[y][15];
    M2[y][ 8] = Diff[y][0] - Diff[y][ 8];
    M2[y][ 9] = Diff[y][1] - Diff[y][ 9];
    M2[y][10] = Diff[y][2] - Diff[y][10];
    M2[y][11] = Diff[y][3] - Diff[y][11];
    M2[y][12] = Diff[y][4] - Diff[y][12];
    M2[y][13] = Diff[y][5] - Diff[y][13];
    M2[y][14] = Diff[y][6] - Diff[y][14];
    M2[y][15] = Diff[y][7] - Diff[y][15];

    M1[y][ 0] = M2[y][ 0] + M2[y][ 4];
    M1[y][ 1] = M2[y][ 1] + M2[y][ 5];
    M1[y][ 2] = M2[y][ 2] + M2[y][ 6];
    M1[y][ 3] = M2[y][ 3] + M2[y][ 7];
    M1[y][ 4] = M2[y][ 0] - M2[y][ 4];
    M1[y][ 5] = M2[y][ 1] - M2[y][ 5];
    M1[y][ 6] = M2[y][ 2] - M2[y][ 6];
    M1[y][ 7] = M2[y][ 3] - M2[y][ 7];
    M1[y][ 8] = M2[y][ 8] + M2[y][12];
    M1[y][ 9] = M2[y][ 9] + M2[y][13];
    M1[y][10] = M2[y][10] + M2[y][14];
    M1[y][11] = M2[y][11] + M2[y][15];
    M1[y][12] = M2[y][ 8] - M2[y][12];
    M1[y][13] = M2[y][ 9] - M2[y][13];
    M1[y][14] = M2[y][10] - M2[y][14];
    M1[y][15] = M2[y][11] - M2[y][15];

    M2[y][ 0] = M1[y][ 0] + M1[y][ 2];
    M2[y][ 1] = M1[y][ 1] + M1[y][ 3];
    M2[y][ 2] = M1[y][ 0] - M1[y][ 2];
    M2[y][ 3] = M1[y][ 1] - M1[y][ 3];
    M2[y][ 4] = M1[y][ 4] + M1[y][ 6];
    M2[y][ 5] = M1[y][ 5] + M1[y][ 7];
    M2[y][ 6] = M1[y][ 4] - M1[y][ 6];
    M2[y][ 7] = M1[y][ 5] - M1[y][ 7];
    M2[y][ 8] = M1[y][ 8] + M1[y][10];
    M2[y][ 9] = M1[y][ 9] + M1[y][11];
    M2[y][10] = M1[y][ 8] - M1[y][10];
    M2[y][11] = M1[y][ 9] - M1[y][11];
    M2[y][12] = M1[y][12] + M1[y][14];
    M2[y][13] = M1[y][13] + M1[y][15];
    M2[y][14] = M1[y][12] - M1[y][14];
    M2[y][15] = M1[y][13] - M1[y][15];

    M1[y][ 0] = M2[y][ 0] + M2[y][ 1];
    M1[y][ 1] = M2[y][ 0] - M2[y][ 1];
    M1[y][ 2] = M2[y][ 2] + M2[y][ 3];
    M1[y][ 3] = M2[y][ 2] - M2[y][ 3];
    M1[y][ 4] = M2[y][ 4] + M2[y][ 5];
    M1[y][ 5] = M2[y][ 4] - M2[y][ 5];
    M1[y][ 6] = M2[y][ 6] + M2[y][ 7];
    M1[y][ 7] = M2[y][ 6] - M2[y][ 7];
    M1[y][ 8] = M2[y][ 8] + M2[y][ 9];
    M1[y][ 9] = M2[y][ 8] - M2[y][ 9];
    M1[y][10] = M2[y][10] + M2[y][11];
    M1[y][11] = M2[y][10] - M2[y][11];
    M1[y][12] = M2[y][12] + M2[y][13];
    M1[y][13] = M2[y][12] - M2[y][13];
    M1[y][14] = M2[y][14] + M2[y][15];
    M1[y][15] = M2[y][14] - M2[y][15];
  }
  
  //vertical
  for(int32 x=0; x < 16; x++)
  {
    M2[0][x] = M1[0][x] + M1[2][x];
    M2[1][x] = M1[1][x] + M1[3][x];
    M2[2][x] = M1[0][x] - M1[2][x];
    M2[3][x] = M1[1][x] - M1[3][x];

    M1[0][x] = M2[0][x] + M2[1][x];
    M1[1][x] = M2[0][x] - M2[1][x];
    M1[2][x] = M2[2][x] + M2[3][x];
    M1[3][x] = M2[2][x] - M2[3][x];
  }
  
  for(int32 y = 0; y < 4; y++)
  {       
    for (int32 x = 0; x<16; x++)
    {
      SATD += xAbs(M1[y][x]); 
    }
  }
  return SATD;
}
uint64 xDistortion::xCalcSATD_16x4_SSE(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  __m128i buff[16];
  __m128i sum = _mm_setzero_si128();
  const __m128i zero = _mm_setzero_si128();

  //load and horizontal transform
  for(int j=0; j<4; j++)
  {
    __m128i org_line_0  = _mm_loadu_si128((__m128i*)Org   );
    __m128i org_line_1  = _mm_loadu_si128((__m128i*)(Org +8));
    __m128i dist_line_0 = _mm_loadu_si128((__m128i*)Dist  );
    __m128i dist_line_1 = _mm_loadu_si128((__m128i*)(Dist+8));
    Org  += OStride;
    Dist += DStride;

    __m128i diff_0      = _mm_sub_epi16(org_line_0, dist_line_0);
    __m128i diff_1      = _mm_sub_epi16(org_line_1, dist_line_1);

    __m128i line_0  = _mm_cvtepi16_epi32(diff_0);
    __m128i line_1  = _mm_cvtepi16_epi32(_mm_srli_si128(diff_0,8));
    __m128i line_2  = _mm_cvtepi16_epi32(diff_1);
    __m128i line_3  = _mm_cvtepi16_epi32(_mm_srli_si128(diff_1,8));

    __m128i h_L0_0 = _mm_add_epi32(line_0, line_2);
    __m128i h_L0_1 = _mm_add_epi32(line_1, line_3);
    __m128i h_L0_2 = _mm_sub_epi32(line_0, line_2);
    __m128i h_L0_3 = _mm_sub_epi32(line_1, line_3);

    __m128i h_L1_0 = _mm_add_epi32(h_L0_0, h_L0_1);
    __m128i h_L1_1 = _mm_sub_epi32(h_L0_0, h_L0_1);
    __m128i h_L1_2 = _mm_add_epi32(h_L0_2, h_L0_3);
    __m128i h_L1_3 = _mm_sub_epi32(h_L0_2, h_L0_3);

    __m128i h_L2_0 = _mm_unpacklo_epi64(h_L1_0, h_L1_1);
    __m128i h_L2_1 = _mm_unpackhi_epi64(h_L1_0, h_L1_1);
    __m128i h_L2_2 = _mm_unpacklo_epi64(h_L1_2, h_L1_3);
    __m128i h_L2_3 = _mm_unpackhi_epi64(h_L1_2, h_L1_3);

    __m128i h_L3_0 = _mm_add_epi32(h_L2_0, h_L2_1);
    __m128i h_L3_1 = _mm_sub_epi32(h_L2_0, h_L2_1);
    __m128i h_L3_2 = _mm_add_epi32(h_L2_2, h_L2_3);
    __m128i h_L3_3 = _mm_sub_epi32(h_L2_2, h_L2_3);

    __m128i h_L4_0 = _mm_unpacklo_epi64(h_L3_0, h_L3_1);
    __m128i h_L4_1 = _mm_unpackhi_epi64(h_L3_0, h_L3_1);
    __m128i h_L4_2 = _mm_unpacklo_epi64(h_L3_2, h_L3_3);
    __m128i h_L4_3 = _mm_unpackhi_epi64(h_L3_2, h_L3_3);
    
    __m128i h_L5_0 = _mm_hadd_epi32(h_L4_0, h_L4_1);
    __m128i h_L5_1 = _mm_hsub_epi32(h_L4_0, h_L4_1);
    __m128i h_L5_2 = _mm_hadd_epi32(h_L4_2, h_L4_3);
    __m128i h_L5_3 = _mm_hsub_epi32(h_L4_2, h_L4_3);

    __m128i* buffptr =& buff[j];
    buffptr[0 ] = _mm_unpacklo_epi32(h_L5_0, h_L5_1);
    buffptr[4 ] = _mm_unpackhi_epi32(h_L5_0, h_L5_1);
    buffptr[8 ] = _mm_unpacklo_epi32(h_L5_2, h_L5_3);
    buffptr[12] = _mm_unpackhi_epi32(h_L5_2, h_L5_3);
  }
  
  //transpose
  for(int i=0; i<16; i+=4)
  {
    __m128i* buffptr =& buff[i];

    __m128i tr_A0 = _mm_unpacklo_epi32(buffptr[0], buffptr[2]);
    __m128i tr_A1 = _mm_unpackhi_epi32(buffptr[0], buffptr[2]);
    __m128i tr_A2 = _mm_unpacklo_epi32(buffptr[1], buffptr[3]);
    __m128i tr_A3 = _mm_unpackhi_epi32(buffptr[1], buffptr[3]);
    
    buffptr[0] = _mm_unpacklo_epi32(tr_A0, tr_A2);
    buffptr[1] = _mm_unpackhi_epi32(tr_A0, tr_A2);
    buffptr[2] = _mm_unpacklo_epi32(tr_A1, tr_A3);
    buffptr[3] = _mm_unpackhi_epi32(tr_A1, tr_A3);
  }

  //vertical transform (without last unpacking), abs, add
  for(int32 j=0; j<16; j+=2)
  {
	  __m128i* buffptr =& buff[j];

    __m128i h_L0_0 = _mm_unpacklo_epi64(buffptr[0], buffptr[1]);
	  __m128i h_L0_1 = _mm_unpackhi_epi64(buffptr[0], buffptr[1]);
    __m128i h_L1_0 = _mm_add_epi32(h_L0_0, h_L0_1);
    __m128i h_L1_1 = _mm_sub_epi32(h_L0_0, h_L0_1);
	  __m128i h_L2_0 = _mm_unpacklo_epi64(h_L1_0, h_L1_1);
	  __m128i h_L2_1 = _mm_unpackhi_epi64(h_L1_0, h_L1_1);
	  __m128i h_L3_0 = _mm_hadd_epi32(h_L2_0, h_L2_1);
	  __m128i h_L3_1 = _mm_hsub_epi32(h_L2_0, h_L2_1);

    __m128i h_L4_0 = _mm_abs_epi32(h_L3_0);
    __m128i h_L4_1 = _mm_abs_epi32(h_L3_1);
    __m128i h_L5_0 = _mm_add_epi32(h_L4_0, h_L4_1);

    sum = _mm_add_epi32(sum, h_L5_0);
  }

  sum = _mm_hadd_epi32(sum, sum);
  sum = _mm_hadd_epi32(sum, sum);
  int32 SATD = _mm_extract_epi32(sum, 0);
  return SATD;
}
uint64 xDistortion::xCalcSATD_16x16_STD(int16* restrict Org, int32 OStride, int16* restrict Dist, int32 DStride)
{
  int16  Diff[16][16];
  int32  M1[16][16], M2[16][16];
  int16* DiffLine = &Diff[0][0];
  uint64 SATD = 0;

  //diff
  for(int32 y=0; y<16; y++)
  {    
    DiffLine[ 0] = Org[ 0] - Dist[ 0];
    DiffLine[ 1] = Org[ 1] - Dist[ 1];
    DiffLine[ 2] = Org[ 2] - Dist[ 2];
    DiffLine[ 3] = Org[ 3] - Dist[ 3];
    DiffLine[ 4] = Org[ 4] - Dist[ 4];
    DiffLine[ 5] = Org[ 5] - Dist[ 5];
    DiffLine[ 6] = Org[ 6] - Dist[ 6];
    DiffLine[ 7] = Org[ 7] - Dist[ 7];
    DiffLine[ 8] = Org[ 8] - Dist[ 8];
    DiffLine[ 9] = Org[ 9] - Dist[ 9];
    DiffLine[10] = Org[10] - Dist[10];
    DiffLine[11] = Org[11] - Dist[11];
    DiffLine[12] = Org[12] - Dist[12];
    DiffLine[13] = Org[13] - Dist[13];
    DiffLine[14] = Org[14] - Dist[14];
    DiffLine[15] = Org[15] - Dist[15];

    Org  += OStride;
    Dist += DStride;
    DiffLine += 16;
  }

  //horizontal
  for(int32 y=0; y<16; y++)
  {
    M2[y][ 0] = Diff[y][0] + Diff[y][ 8];
    M2[y][ 1] = Diff[y][1] + Diff[y][ 9];
    M2[y][ 2] = Diff[y][2] + Diff[y][10];
    M2[y][ 3] = Diff[y][3] + Diff[y][11];
    M2[y][ 4] = Diff[y][4] + Diff[y][12];
    M2[y][ 5] = Diff[y][5] + Diff[y][13];
    M2[y][ 6] = Diff[y][6] + Diff[y][14];
    M2[y][ 7] = Diff[y][7] + Diff[y][15];
    M2[y][ 8] = Diff[y][0] - Diff[y][ 8];
    M2[y][ 9] = Diff[y][1] - Diff[y][ 9];
    M2[y][10] = Diff[y][2] - Diff[y][10];
    M2[y][11] = Diff[y][3] - Diff[y][11];
    M2[y][12] = Diff[y][4] - Diff[y][12];
    M2[y][13] = Diff[y][5] - Diff[y][13];
    M2[y][14] = Diff[y][6] - Diff[y][14];
    M2[y][15] = Diff[y][7] - Diff[y][15];

    M1[y][ 0] = M2[y][ 0] + M2[y][ 4];
    M1[y][ 1] = M2[y][ 1] + M2[y][ 5];
    M1[y][ 2] = M2[y][ 2] + M2[y][ 6];
    M1[y][ 3] = M2[y][ 3] + M2[y][ 7];
    M1[y][ 4] = M2[y][ 0] - M2[y][ 4];
    M1[y][ 5] = M2[y][ 1] - M2[y][ 5];
    M1[y][ 6] = M2[y][ 2] - M2[y][ 6];
    M1[y][ 7] = M2[y][ 3] - M2[y][ 7];
    M1[y][ 8] = M2[y][ 8] + M2[y][12];
    M1[y][ 9] = M2[y][ 9] + M2[y][13];
    M1[y][10] = M2[y][10] + M2[y][14];
    M1[y][11] = M2[y][11] + M2[y][15];
    M1[y][12] = M2[y][ 8] - M2[y][12];
    M1[y][13] = M2[y][ 9] - M2[y][13];
    M1[y][14] = M2[y][10] - M2[y][14];
    M1[y][15] = M2[y][11] - M2[y][15];

    M2[y][ 0] = M1[y][ 0] + M1[y][ 2];
    M2[y][ 1] = M1[y][ 1] + M1[y][ 3];
    M2[y][ 2] = M1[y][ 0] - M1[y][ 2];
    M2[y][ 3] = M1[y][ 1] - M1[y][ 3];
    M2[y][ 4] = M1[y][ 4] + M1[y][ 6];
    M2[y][ 5] = M1[y][ 5] + M1[y][ 7];
    M2[y][ 6] = M1[y][ 4] - M1[y][ 6];
    M2[y][ 7] = M1[y][ 5] - M1[y][ 7];
    M2[y][ 8] = M1[y][ 8] + M1[y][10];
    M2[y][ 9] = M1[y][ 9] + M1[y][11];
    M2[y][10] = M1[y][ 8] - M1[y][10];
    M2[y][11] = M1[y][ 9] - M1[y][11];
    M2[y][12] = M1[y][12] + M1[y][14];
    M2[y][13] = M1[y][13] + M1[y][15];
    M2[y][14] = M1[y][12] - M1[y][14];
    M2[y][15] = M1[y][13] - M1[y][15];

    M1[y][ 0] = M2[y][ 0] + M2[y][ 1];
    M1[y][ 1] = M2[y][ 0] - M2[y][ 1];
    M1[y][ 2] = M2[y][ 2] + M2[y][ 3];
    M1[y][ 3] = M2[y][ 2] - M2[y][ 3];
    M1[y][ 4] = M2[y][ 4] + M2[y][ 5];
    M1[y][ 5] = M2[y][ 4] - M2[y][ 5];
    M1[y][ 6] = M2[y][ 6] + M2[y][ 7];
    M1[y][ 7] = M2[y][ 6] - M2[y][ 7];
    M1[y][ 8] = M2[y][ 8] + M2[y][ 9];
    M1[y][ 9] = M2[y][ 8] - M2[y][ 9];
    M1[y][10] = M2[y][10] + M2[y][11];
    M1[y][11] = M2[y][10] - M2[y][11];
    M1[y][12] = M2[y][12] + M2[y][13];
    M1[y][13] = M2[y][12] - M2[y][13];
    M1[y][14] = M2[y][14] + M2[y][15];
    M1[y][15] = M2[y][14] - M2[y][15];
  }
  
  //vertical
  for(int32 x=0; x < 16; x++)
  {
    M2[ 0][x] = M1[0][x] + M1[ 8][x];
    M2[ 1][x] = M1[1][x] + M1[ 9][x];
    M2[ 2][x] = M1[2][x] + M1[10][x];
    M2[ 3][x] = M1[3][x] + M1[11][x];
    M2[ 4][x] = M1[4][x] + M1[12][x];
    M2[ 5][x] = M1[5][x] + M1[13][x];
    M2[ 6][x] = M1[6][x] + M1[14][x];
    M2[ 7][x] = M1[7][x] + M1[15][x];
    M2[ 8][x] = M1[0][x] - M1[ 8][x];
    M2[ 9][x] = M1[1][x] - M1[ 9][x];
    M2[10][x] = M1[2][x] - M1[10][x];
    M2[11][x] = M1[3][x] - M1[11][x];
    M2[12][x] = M1[4][x] - M1[12][x];
    M2[13][x] = M1[5][x] - M1[13][x];
    M2[14][x] = M1[6][x] - M1[14][x];
    M2[15][x] = M1[7][x] - M1[15][x];

    M1[ 0][x] = M2[ 0][x] + M2[ 4][x];
    M1[ 1][x] = M2[ 1][x] + M2[ 5][x];
    M1[ 2][x] = M2[ 2][x] + M2[ 6][x];
    M1[ 3][x] = M2[ 3][x] + M2[ 7][x];
    M1[ 4][x] = M2[ 0][x] - M2[ 4][x];
    M1[ 5][x] = M2[ 1][x] - M2[ 5][x];
    M1[ 6][x] = M2[ 2][x] - M2[ 6][x];
    M1[ 7][x] = M2[ 3][x] - M2[ 7][x];
    M1[ 8][x] = M2[ 8][x] + M2[12][x];
    M1[ 9][x] = M2[ 9][x] + M2[13][x];
    M1[10][x] = M2[10][x] + M2[14][x];
    M1[11][x] = M2[11][x] + M2[15][x];
    M1[12][x] = M2[ 8][x] - M2[12][x];
    M1[13][x] = M2[ 9][x] - M2[13][x];
    M1[14][x] = M2[10][x] - M2[14][x];
    M1[15][x] = M2[11][x] - M2[15][x];

    M2[ 0][x] = M1[ 0][x] + M1[ 2][x];
    M2[ 1][x] = M1[ 1][x] + M1[ 3][x];
    M2[ 2][x] = M1[ 0][x] - M1[ 2][x];
    M2[ 3][x] = M1[ 1][x] - M1[ 3][x];
    M2[ 4][x] = M1[ 4][x] + M1[ 6][x];
    M2[ 5][x] = M1[ 5][x] + M1[ 7][x];
    M2[ 6][x] = M1[ 4][x] - M1[ 6][x];
    M2[ 7][x] = M1[ 5][x] - M1[ 7][x];
    M2[ 8][x] = M1[ 8][x] + M1[10][x];
    M2[ 9][x] = M1[ 9][x] + M1[11][x];
    M2[10][x] = M1[ 8][x] - M1[10][x];
    M2[11][x] = M1[ 9][x] - M1[11][x];
    M2[12][x] = M1[12][x] + M1[14][x];
    M2[13][x] = M1[13][x] + M1[15][x];
    M2[14][x] = M1[12][x] - M1[14][x];
    M2[15][x] = M1[13][x] - M1[15][x];

    M1[ 0][x] = M2[ 0][x] + M2[ 1][x];
    M1[ 1][x] = M2[ 0][x] - M2[ 1][x];
    M1[ 2][x] = M2[ 2][x] + M2[ 3][x];
    M1[ 3][x] = M2[ 2][x] - M2[ 3][x];
    M1[ 4][x] = M2[ 4][x] + M2[ 5][x];
    M1[ 5][x] = M2[ 4][x] - M2[ 5][x];
    M1[ 6][x] = M2[ 6][x] + M2[ 7][x];
    M1[ 7][x] = M2[ 6][x] - M2[ 7][x];
    M1[ 8][x] = M2[ 8][x] + M2[ 9][x];
    M1[ 9][x] = M2[ 8][x] - M2[ 9][x];
    M1[10][x] = M2[10][x] + M2[11][x];
    M1[11][x] = M2[10][x] - M2[11][x];
    M1[12][x] = M2[12][x] + M2[13][x];
    M1[13][x] = M2[12][x] - M2[13][x];
    M1[14][x] = M2[14][x] + M2[15][x];
    M1[15][x] = M2[14][x] - M2[15][x];
  }  

  for(int32 y = 0; y < 16; y++)
  {       
    for (int32 x = 0; x<16; x++)
    {
      SATD += xAbs(M1[y][x]); 
    }
  }
  return SATD;
}


//=========================================================================================================================================

uint64 xDistortion::calcSATD(int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Width, int32 Height, bool NonSquareSATD)
{
  uint64 SATD = 0;

  if(((uint32)Width & (uint32)0x7)==0 && ((uint32)Height & (uint32)0x7)==0 && (!NonSquareSATD || Width==Height)) //Width%8==0 && Height%8==0 && Width==Height
  {
    for(int32 y=0; y<Height; y+=8)
    {
      for(int32 x=0; x<Width; x+=8)
      {
        SATD += calcSATD_8x8(Org + x, OStride, Dist + x, DStride);
      }
    }
    Org  += OStride<<3;
    Dist += DStride<<3;
  }
  else if (NonSquareSATD && Height>=16 && Height>Width) 
  {
    for(int32 y=0; y<Height; y+=4)
    {
      for(int32 x=0; x<Width; x+=16)
      {
        SATD += calcSATD_16x4(Org + x, OStride, Dist + x, DStride);
      }
    }
    Org  += OStride<<2;
    Dist += DStride<<2;
  }
  else if (NonSquareSATD && Width>=16 && Height>Width) 
  {
    for(int32 y=0; y<Height; y+=16)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        SATD += calcSATD_4x16(Org + x, OStride, Dist + x, DStride);
      }
    }
    Org  += OStride<<4;
    Dist += DStride<<4;
  }
  else if(((uint32)Width & (uint32)0x3)==0 && ((uint32)Height & (uint32)0x3)==0) //Width%4==0 && Height%4==0
  {
    for(int32 y=0; y<Height; y+=4)
    {
      for(int32 x=0; x<Width; x+=4)
      {
        SATD += calcSATD_4x4(Org + x, OStride, Dist + x, DStride);
      }
    }
    Org  += OStride<<2;
    Dist += DStride<<2;
  }
  else if(((uint32)Width & (uint32)0x1)==0 && ((uint32)Height & (uint32)0x1)==0) //Width%2==0 && Height%2==0
  {
    for(int32 y=0; y<Height; y+=2)
    {
      for(int32 x=0; x<Width; x+=2)
      {
        SATD += calcSATD_2x2(Org + x, OStride, Dist + x, DStride);
      }
    }
    Org  += OStride<<1;
    Dist += DStride<<1;
  }
  else
  {
    xAssert(0);
  }

  return SATD;
}
uint64 xDistortion::calcSATDContinous(int16* Org, int16* Dist, int32 Size)
{
  switch(Size)
  {
  case 4:
    return calcSATDContinous_4x4(Org, Dist);    
  case 8:
    return calcSATD_8x8(Org, Size, Dist, Size);    
  case 16:
    return calcSATD_16x16(Org, Size, Dist, Size);    
  case 32:
    return calcSATD_32x32(Org, Size, Dist, Size);    
  case 64:
    return calcSATD_64x64(Org, Size, Dist, Size);    
  default:
    xAssert(0);
    return int64_max;
  }
}
uint64 xDistortion::calcSATD(int16* Org, int32 OStride, int16* Dist, int32 DStride, int32 Size)
{
  switch(Size)
  {
  case 4:
    return calcSATD_4x4(Org, OStride, Dist, DStride);    
  case 8:
    return calcSATD_8x8(Org, OStride, Dist, DStride);    
  case 16:
    return calcSATD_16x16(Org, OStride, Dist, DStride);    
  case 32:
    return calcSATD_32x32(Org, OStride, Dist, DStride);    
  case 64:
    return calcSATD_64x64(Org, OStride, Dist, DStride);    
  default:
    xAssert(0);
    return int64_max;
  }
}

uint64 xDistortion::calcSATD_16x16(int16* Org, int32 OStride, int16* Dist, int32 DStride)
{
  uint64 SATD = 0;
  for(int32 y=0; y<16; y+=8)
  {
    for(int32 x=0; x<16; x+=8)
    {
      SATD += calcSATD_8x8(Org + x, OStride, Dist + x, DStride);
    }    
    Org  += OStride<<3;
    Dist += DStride<<3;
  }
  return SATD;
}
uint64 xDistortion::calcSATD_32x32(int16* Org, int32 OStride, int16* Dist, int32 DStride)
{
  uint64 SATD = 0;
  for(int32 y=0; y<32; y+=8)
  {
    for(int32 x=0; x<32; x+=8)
    {
      SATD += calcSATD_8x8(Org + x, OStride, Dist + x, DStride);
    }   
    Org  += OStride<<3;
    Dist += DStride<<3;
  }
  return SATD;
}
uint64 xDistortion::calcSATD_64x64(int16* Org, int32 OStride, int16* Dist, int32 DStride)
{
  uint64 SATD = 0;
  for(int32 y=0; y<64; y+=8)
  {
    for(int32 x=0; x<64; x+=8)
    {
      SATD += calcSATD_8x8(Org + x, OStride, Dist + x, DStride);
    }    
    Org  += OStride<<3;
    Dist += DStride<<3;
  }
  return SATD;
}

} //end of namespace AVLib