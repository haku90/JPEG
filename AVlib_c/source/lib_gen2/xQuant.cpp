#include "xQuant.h"

namespace AVlib {

const int32 xQuant::m_QuantScales[6]   = {26214, 23302, 20560, 18396, 16384, 14564};
const int32 xQuant::m_DeQuantScales[6] = {40, 45, 51, 57, 64, 72};

void xQuant::xQuant_STD(int16* restrict Src, int16* restrict Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Area  = 1<<(Log2TrSize<<1);
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Q     = m_QuantScales[QPRem];
  const int32 Shift = 29 + QPPer - BitDepth - Log2TrSize;
  const int32 Add   = 256 << (Shift-9); // 256 = no dead zone !!!!  //171 - HEVC in I slices

  for(int32 i=0; i<Area; i++)
  {    
    int32 Coeff = Src[i];
    int32  Sign  = (Coeff < 0 ? -1 : 1);
    Coeff = Sign * ((Coeff * Sign * Q + Add ) >> Shift);      
    Dst[i] = Coeff;   
  }
}
void xQuant::xQuant_SSE(int16* restrict Src, int16* restrict Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Area  = 1<<(Log2TrSize<<1);
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Q     = m_QuantScales[QPRem];
  const int32 Shift = 29 + QPPer - BitDepth - Log2TrSize;
  const int32 Add   = 256 << (Shift-9); // 256 = no dead zone !!!!  //171 - HEVC in I slices

  const __m128i Add_V  = _mm_set1_epi32(Add);
  const __m128i Q_V    = _mm_set1_epi32(Q);
  const __m128i One_V  = _mm_set1_epi32(1);

  for(int32 i=0; i<Area; i+=8)
  {    
    //load
    __m128i Line    = _mm_loadu_si128((__m128i*)&(Src[i]));
    __m128i Line_0  = _mm_cvtepi16_epi32(Line);
    __m128i Line_1  = _mm_cvtepi16_epi32(_mm_srli_si128(Line,8));
    //extract sign
    __m128i Sign_0  = _mm_sign_epi32(One_V, Line_0);
    __m128i Sign_1  = _mm_sign_epi32(One_V, Line_1);
    //calc abs and quant
    Line_0 = _mm_abs_epi32(Line_0);
    Line_1 = _mm_abs_epi32(Line_1);
    Line_0 = _mm_mullo_epi32(Line_0, Q_V);
    Line_1 = _mm_mullo_epi32(Line_1, Q_V);
    Line_0 = _mm_add_epi32(Line_0, Add_V);
    Line_1 = _mm_add_epi32(Line_1, Add_V);
    Line_0 = _mm_srai_epi32(Line_0, Shift);
    Line_1 = _mm_srai_epi32(Line_1, Shift);
    //restore sign
    Line_0 = _mm_sign_epi32(Line_0, Sign_0);
    Line_1 = _mm_sign_epi32(Line_1, Sign_1);
    //write
    Line = _mm_packs_epi32(Line_0, Line_1);
    _mm_storeu_si128((__m128i*)&(Dst[i]), Line);  
  }
}
void xQuant::xQuant_AVX(int16* restrict Src, int16* restrict Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Area  = 1<<(Log2TrSize<<1);
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Q     = m_QuantScales[QPRem];
  const int32 Shift = 29 + QPPer - BitDepth - Log2TrSize;
  const int32 Add   = 256 << (Shift-9); // 256 = no dead zone !!!!  //171 - HEVC in I slices

  const __m256i Add_V  = _mm256_set1_epi32(Add);
  const __m256i Q_V    = _mm256_set1_epi32(Q);
  const __m256i One_V  = _mm256_set1_epi32(1);

  for(int32 i=0; i<Area; i+=16)
  {    
    //load
    __m256i src_v  = _mm256_loadu_si256((__m256i*)&(Src[i]));
    __m256i data0_v  = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(src_v,0));
    __m256i data1_v  = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(src_v,1));
    //extract sign
    __m256i sign0_v  = _mm256_sign_epi32(One_V, data0_v);
    __m256i sign1_v  = _mm256_sign_epi32(One_V, data1_v);
    //calc abs and quant
    data0_v = _mm256_abs_epi32(data0_v);
    data1_v = _mm256_abs_epi32(data1_v);
    data0_v = _mm256_mullo_epi32(data0_v, Q_V);
    data1_v = _mm256_mullo_epi32(data1_v, Q_V);
    data0_v = _mm256_add_epi32(data0_v, Add_V);
    data1_v = _mm256_add_epi32(data1_v, Add_V);
    data0_v = _mm256_srai_epi32(data0_v, Shift);
    data1_v = _mm256_srai_epi32(data1_v, Shift);
    //restore sign
    data0_v = _mm256_sign_epi32(data0_v, sign0_v);
    data1_v = _mm256_sign_epi32(data1_v, sign1_v);
    //write
    __m256i dst_v = _mm256_packs_epi32(data0_v, data1_v);
    _mm256_storeu_si256((__m256i*)&(Dst[i]), dst_v);  
  }
}
void xQuant::xDeQuant_STD(int16* restrict Src, int16* restrict Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Area  = 1<<(Log2TrSize<<1);
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Shift = BitDepth + Log2TrSize - 9;
  const int32 Add   = 1<<(Shift-1);
  const int32 Scale = m_DeQuantScales[QPRem] << QPPer;

  for(int32 i=0; i<Area; i++)
  {    
    int32 Coeff = Src[i];
    Coeff = ((Coeff * Scale + Add ) >> Shift);      
    Dst[i] = Coeff;   
  }
}
void xQuant::xDeQuant_SSE(int16* restrict Src, int16* restrict Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Area  = 1<<(Log2TrSize<<1);
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Shift = BitDepth + Log2TrSize - 9;
  const int32 Add   = 1<<(Shift-1);
  const int32 Scale = m_DeQuantScales[QPRem] << QPPer;

  const __m128i Add_V   = _mm_set1_epi32(Add);
  const __m128i Scale_V = _mm_set1_epi32(Scale);

  for(int32 i=0; i<Area; i+=8)
  {    
    //load
    __m128i Line  = _mm_loadu_si128((__m128i*)&(Src[i]));
    __m128i Line_0  = _mm_cvtepi16_epi32(Line);
    __m128i Line_1  = _mm_cvtepi16_epi32(_mm_srli_si128(Line,8));
    //dequant
    Line_0 = _mm_mullo_epi32(Line_0, Scale_V);
    Line_1 = _mm_mullo_epi32(Line_1, Scale_V);
    Line_0 = _mm_add_epi32(Line_0, Add_V);
    Line_1 = _mm_add_epi32(Line_1, Add_V);
    Line_0 = _mm_srai_epi32(Line_0, Shift);
    Line_1 = _mm_srai_epi32(Line_1, Shift);
    Line = _mm_packs_epi32(Line_0, Line_1);
    //store
    _mm_storeu_si128((__m128i*)&(Dst[i]), Line);   
  }
}
void xQuant::xDeQuant_AVX(int16* restrict Src, int16* restrict Dst, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Area  = 1<<(Log2TrSize<<1);
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Shift = BitDepth + Log2TrSize - 9;
  const int32 Add   = 1<<(Shift-1);
  const int32 Scale = m_DeQuantScales[QPRem] << QPPer;

  const __m256i Add_V   = _mm256_set1_epi32(Add);
  const __m256i Scale_V = _mm256_set1_epi32(Scale);

  for(int32 i=0; i<Area; i+=16)
  {    
    //load
    __m256i src_v  = _mm256_loadu_si256((__m256i*)&(Src[i]));
    __m256i data0_v  = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(src_v,0));
    __m256i data1_v  = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(src_v,1));
    //dequant
    data0_v = _mm256_mullo_epi32(data0_v, Scale_V);
    data1_v = _mm256_mullo_epi32(data1_v, Scale_V);
    data0_v = _mm256_add_epi32(data0_v, Add_V);
    data1_v = _mm256_add_epi32(data1_v, Add_V);
    data0_v = _mm256_srai_epi32(data0_v, Shift);
    data1_v = _mm256_srai_epi32(data1_v, Shift);    
    //store
    __m256i dst_v = _mm256_packs_epi32(data0_v, data1_v);
    _mm256_storeu_si256((__m256i*)&(Dst[i]), dst_v);   
  }
}

void xQuant::xQuant_STD  (int16* restrict Src, int16* restrict Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Size  = 1<<Log2TrSize;
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Q     = m_QuantScales[QPRem];
  const int32 Shift = 29 + QPPer - BitDepth - Log2TrSize;
  const int32 Add   = 256 << (Shift-9); // 256 = no dead zone !!!!  //171 - HEVC in I slices

  for(int32 y=0; y<Size; y++)
  {    
    for(int32 x=0; x<Size; x++)
    {
      int32 Coeff = Src[x];
      int32  Sign  = (Coeff < 0 ? -1 : 1);
      Coeff = Sign * ((Coeff * Sign * Q + Add ) >> Shift);      
      Dst[x] = Coeff;   
    }
    Src += SrcStride;
    Dst += DstStride;
  }
}
void xQuant::xQuant_SSE  (int16* restrict Src, int16* restrict Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Size  = 1<<Log2TrSize;
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Q     = m_QuantScales[QPRem];
  const int32 Shift = 29 + QPPer - BitDepth - Log2TrSize;
  const int32 Add   = 256 << (Shift-9); // 256 = no dead zone !!!!  //171 - HEVC in I slices

  const __m128i Add_V  = _mm_set1_epi32(Add);
  const __m128i Q_V    = _mm_set1_epi32(Q);
  const __m128i One_V  = _mm_set1_epi32(1);

  if(Log2TrSize>=3) //>=8x8
  {
    for(int32 y=0; y<Size; y++)
    {    
      for(int32 x=0; x<Size; x+=8)
      {
        //load
        __m128i Line    = _mm_loadu_si128((__m128i*)&(Src[x]));
        __m128i Line_0  = _mm_cvtepi16_epi32(Line);
        __m128i Line_1  = _mm_cvtepi16_epi32(_mm_srli_si128(Line,8));
        //extract sign
        __m128i Sign_0  = _mm_sign_epi32(One_V, Line_0);
        __m128i Sign_1  = _mm_sign_epi32(One_V, Line_1);
        //calc abs and quant
        Line_0 = _mm_abs_epi32(Line_0);
        Line_1 = _mm_abs_epi32(Line_1);
        Line_0 = _mm_mullo_epi32(Line_0, Q_V);
        Line_1 = _mm_mullo_epi32(Line_1, Q_V);
        Line_0 = _mm_add_epi32(Line_0, Add_V);
        Line_1 = _mm_add_epi32(Line_1, Add_V);
        Line_0 = _mm_srai_epi32(Line_0, Shift);
        Line_1 = _mm_srai_epi32(Line_1, Shift);
        //restore sign
        Line_0 = _mm_sign_epi32(Line_0, Sign_0);
        Line_1 = _mm_sign_epi32(Line_1, Sign_1);
        //write
        Line = _mm_packs_epi32(Line_0, Line_1);
        _mm_storeu_si128((__m128i*)&(Dst[x]), Line);  
      }
      Src += SrcStride;
      Dst += DstStride;
    }  
  }
  else if(Log2TrSize==2) //>=4x4
  {
    for(int32 y=0; y<4; y++)
    { 
      __m128i Line  = _mm_loadl_epi64((__m128i*)Src);    
      Line  = _mm_cvtepi16_epi32(Line);
      __m128i Sign  = _mm_sign_epi32(One_V, Line);
      Line = _mm_abs_epi32(Line);
      Line = _mm_mullo_epi32(Line, Q_V);
      Line = _mm_add_epi32(Line, Add_V);
      Line = _mm_srai_epi32(Line, Shift);
      Line = _mm_sign_epi32(Line, Sign);
      Line = _mm_packs_epi32(Line, Line);
      _mm_storel_epi64((__m128i*)Dst, Line);  
      Src += SrcStride;
      Dst += DstStride;
    }
  }
}
void xQuant::xDeQuant_STD(int16* restrict Src, int16* restrict Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Size  = 1<<Log2TrSize;
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Shift = BitDepth + Log2TrSize - 9;
  const int32 Add   = 1<<(Shift-1);
  const int32 Scale = m_DeQuantScales[QPRem] << QPPer;

  for(int32 y=0; y<Size; y++)
  {    
    for(int32 x=0; x<Size; x++)
    {
      int32 Coeff = Src[x];
      Coeff = ((Coeff * Scale + Add ) >> Shift);      
      Dst[x] = Coeff;   
    }
    Src += SrcStride;
    Dst += DstStride;
  }
}
void xQuant::xDeQuant_SSE(int16* restrict Src, int16* restrict Dst, uint32 SrcStride, uint32 DstStride, uint32 Log2TrSize, uint32 QP, uint32 BitDepth)
{
  const int32 Size  = 1<<Log2TrSize;
  const int32 QPPer = QP/6;
  const int32 QPRem = QP%6;
  const int32 Shift = BitDepth + Log2TrSize - 9;
  const int32 Add   = 1<<(Shift-1);
  const int32 Scale = m_DeQuantScales[QPRem] << QPPer;

  const __m128i Add_V   = _mm_set1_epi32(Add);
  const __m128i Scale_V = _mm_set1_epi32(Scale);

  if(Log2TrSize>=3) //>=8x8
  {
    for(int32 y=0; y<Size; y++)
    {    
      for(int32 x=0; x<Size; x+=8)
      {
        //load
        __m128i Line  = _mm_loadu_si128((__m128i*)&(Src[x]));
        __m128i Line_0  = _mm_cvtepi16_epi32(Line);
        __m128i Line_1  = _mm_cvtepi16_epi32(_mm_srli_si128(Line,8));
        //dequant
        Line_0 = _mm_mullo_epi32(Line_0, Scale_V);
        Line_1 = _mm_mullo_epi32(Line_1, Scale_V);
        Line_0 = _mm_add_epi32(Line_0, Add_V);
        Line_1 = _mm_add_epi32(Line_1, Add_V);
        Line_0 = _mm_srai_epi32(Line_0, Shift);
        Line_1 = _mm_srai_epi32(Line_1, Shift);
        Line = _mm_packs_epi32(Line_0, Line_1);
        //store
        _mm_storeu_si128((__m128i*)&(Dst[x]), Line); 
      }
      Src += SrcStride;
      Dst += DstStride;
    }  
  }
  else if(Log2TrSize==2) //>=4x4
  {
    for(int32 y=0; y<4; y++)
    { 
      __m128i Line  = _mm_loadl_epi64((__m128i*)Src);
      Line  = _mm_cvtepi16_epi32(Line);
      Line = _mm_mullo_epi32(Line, Scale_V);
      Line = _mm_add_epi32(Line, Add_V);
      Line = _mm_srai_epi32(Line, Shift);
      Line = _mm_packs_epi32(Line, Line);
      _mm_storel_epi64((__m128i*)Dst, Line);  
      Src += SrcStride;
      Dst += DstStride;
    }
  }
}

} //end of namespace AVLib
