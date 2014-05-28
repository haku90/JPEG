#include "xCmp.h"
#include "xDistortion.h"
#include "xQuant.h"
#include "xTransform.h"
#include "xPixel.h"

namespace AVlib {

xCmp::xCmp()
{
  m_SizeX        = -1;    
  m_SizeY        = -1;  
  m_Margin       = -1;
  m_BitDepth     = -1;

  m_PelBuffer    = nullptr;
  m_PelPtrBuffer = nullptr;
  m_BufferSize   = -1;

  m_Pel          = nullptr;
  m_PelOrg       = nullptr;
  m_PelStride    = -1;
  
  m_Type         = -1;
  m_Id           = -1;
}
void xCmp::create(int32 SizeX, int32 SizeY, int32 Margin, int32 BitDepth)
{
  xAssert(SizeX>=0 && SizeY>=0 && Margin>=0 && BitDepth>0);

  m_SizeX        = SizeX;    
  m_SizeY        = SizeY;  
  m_Margin       = Margin;
  m_BitDepth     = BitDepth;

  m_PelBuffer    = nullptr;
  m_PelPtrBuffer = nullptr;
  m_BufferSize   = (SizeX + 2*Margin)*(SizeY + 2*Margin);

  m_Pel          = nullptr;
  m_PelOrg       = nullptr;
  m_PelStride    = SizeX + 2*Margin;
  
  m_Type         = NULL;
  m_Id           = NULL;

  ::memset(m_Name, 0, sizeof(m_Name));

  if(SizeX==0 && SizeY==0) return;

  m_PelBuffer    = (int16*)xAlignedCalloc(m_BufferSize+xRegAlign, sizeof(int16), xPelAlign);
  m_PelOrg       = m_PelBuffer + (Margin*(SizeX + 2*Margin) + Margin);

  m_PelPtrBuffer = (int16**)xCalloc((SizeY + 2*Margin), sizeof(int16*));  
  m_Pel          = m_PelPtrBuffer + Margin;

  for(int32 y=0,off=0; y<(SizeY + 2*Margin); y++,off+=m_PelStride)
  {
    m_PelPtrBuffer[y] = (int16*)&m_PelBuffer[off];
  }
}
void xCmp::destroy()
{
  if(m_PelBuffer   !=nullptr) { xAlignedFree(m_PelBuffer);    m_PelBuffer=nullptr;    }
  if(m_PelPtrBuffer!=nullptr) { xAlignedFree(m_PelPtrBuffer); m_PelPtrBuffer=nullptr; }
  m_Pel          = nullptr;
  m_PelOrg       = nullptr;
}
void xCmp::createCompatible(xCmp* ReferenceCmp)
{
  create(ReferenceCmp->m_SizeX, ReferenceCmp->m_SizeY, ReferenceCmp->m_Margin, ReferenceCmp->m_BitDepth);
}
void xCmp::duplicate(xCmp* ReferenceCmp)
{
  createCompatible(ReferenceCmp);

  m_Type = ReferenceCmp->m_Type;
  m_Id   = ReferenceCmp->m_Id;

  ::memcpy(m_Name, ReferenceCmp->m_Name, sizeof(m_Name)); 
  copy(ReferenceCmp);
}
void xCmp::clear()
{
  ::memset(m_Name, 0, sizeof(m_Name));
  ::memset((void*)m_PelBuffer, 0, m_BufferSize * sizeof(int16));
}
void xCmp::resize(int32 SizeX, int32 SizeY, int32 Margin, int32 BitDepth)
{
  xAssert(SizeX>=0 && SizeY>=0 && Margin>=0 && BitDepth>0);

  if(m_PelBuffer   !=nullptr) { xAlignedFree(m_PelBuffer);    }
  if(m_PelPtrBuffer!=nullptr) { xAlignedFree(m_PelPtrBuffer); }

  m_SizeX        = SizeX;    
  m_SizeY        = SizeY;  
  m_Margin       = Margin;
  m_BitDepth     = BitDepth;

  m_PelBuffer    = nullptr;
  m_PelPtrBuffer = nullptr;
  m_BufferSize   = (SizeX + 2*Margin)*(SizeY + 2*Margin);

  m_Pel          = nullptr;
  m_PelOrg       = nullptr;
  m_PelStride    = SizeX + 2*Margin;

  if(SizeX==0 && SizeY==0) return;

  m_PelBuffer    = (int16*)xAlignedCalloc(m_BufferSize+xRegAlign, sizeof(int16), xPelAlign);
  m_PelOrg       = m_PelBuffer + (Margin*(SizeX + 2*Margin) + Margin);

  m_PelPtrBuffer = (int16**)xCalloc((SizeY + 2*Margin), sizeof(int16*));  
  m_Pel          = m_PelPtrBuffer + Margin;

  for(int32 y=0,off=0; y<(SizeY + 2*Margin); y++,off+=m_PelStride)
  {
    m_PelPtrBuffer[y] = (int16*)&m_PelBuffer[off];
  }
}
void xCmp::copy(xCmp* Src)
{
  xAssert(Src!=nullptr && Src->m_PelBuffer!=nullptr && m_PelBuffer!=nullptr && Src->m_SizeX==m_SizeX && Src->m_SizeY==m_SizeY);
  ::memcpy(m_PelBuffer, Src->m_PelBuffer, m_BufferSize * sizeof(int16));
}
void xCmp::copyEx(xCmp* Src, int32 DstX, int32 DstY, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY)
{
  xAssert(Src!=nullptr && Src->m_PelBuffer!=nullptr && m_PelBuffer!=nullptr);

  int32 dx = SrcEX-SrcSX+1;
  int32 dy = SrcEY-SrcSY;

#if X_OMP_CMP
  #pragma omp parallel for private(y)
#endif  
  for(int32 y=0; y<=dy; y++)
  {
    ::memcpy(&(m_Pel[y+DstY][DstX]), &(Src->m_Pel[y+SrcSY][SrcSX]), dx*sizeof(int16));
  }
}
void xCmp::set(int16 Value)
{
  xMemsetX(m_PelBuffer, (int16)0, m_BufferSize * sizeof(int16));
}
void xCmp::setName(xchar* Name)
{
  xAssert(Name!=nullptr);
  int32 Len = xClipU((int32)xstrlen(Name), CmpNameLen-1);
  ::memcpy(m_Name, Name, Len*sizeof(xchar));
}

//================================================================================================
// resize and rescale
//================================================================================================
void xCmp::rescaleFast(xCmp* Src)
{
  //same size of destination and source component
  if((m_SizeX==Src->m_SizeX)&&(m_SizeY==Src->m_SizeY)) copy(Src);

  //2x downsampling
  else if((2*m_SizeX==Src->m_SizeX)&&(2*m_SizeY==Src->m_SizeY)) rescaleD2Avg(Src);

  //2x upsampling
  else if((m_SizeX==2*Src->m_SizeX)&&(m_SizeY==2*Src->m_SizeY)) rescaleU2Bilinear(Src);

  //more than 2x downsampling
  else if((2*m_SizeX<=Src->m_SizeX)&&(2*m_SizeY<=Src->m_SizeY))
  {
    xCmp* TmpCmp = new xCmp;
    TmpCmp->create(Src->m_SizeX>>1, Src->m_SizeY>>1, m_Margin, m_BitDepth);
    TmpCmp->rescaleD2Avg(Src);
    rescaleFast(TmpCmp);
    TmpCmp->destroy();
    delete TmpCmp;
  }

  //more than 2x upsampling
  else if((m_SizeX>2*Src->m_SizeX)&&(m_SizeY>2*Src->m_SizeY))
  {
    xCmp* TmpCmp = new xCmp;
    TmpCmp->create(Src->m_SizeX>>1, Src->m_SizeY>>1, m_Margin, m_BitDepth);
    TmpCmp->rescaleFast(Src);
    rescaleU2Bilinear(TmpCmp);
    TmpCmp->destroy();
    delete TmpCmp;
  }

  //no diadic resampling
  else rescaleBilinear(Src);
}
void xCmp::rescaleChroma(xCmp* Src)
{
  //same size of destination and source component
  if((m_SizeX==Src->m_SizeX)&&(m_SizeY==Src->m_SizeY)) copy(Src);

  //2x downsampling
  else if((2*m_SizeX==Src->m_SizeX)&&(2*m_SizeY==Src->m_SizeY)) rescaleD2Avg(Src);

  //2x upsampling
  else if((m_SizeX==2*Src->m_SizeX)&&(m_SizeY==2*Src->m_SizeY)) rescaleU2BilinearChroma(Src);

  else xAssert(0);
}
void xCmp::rescaleBilinear(xCmp* Src)
{
  int64 y,x;
  int64 dst_dx = m_SizeX, dst_dy = m_SizeY;
  int64 src_dx = Src->m_SizeX, src_dy = Src->m_SizeY;
  int64 scale_x = ((src_dx-1)<<32)/(dst_dx-1);
  int64 scale_y = ((src_dy-1)<<32)/(dst_dy-1);
  int16 *restrict D, *restrict S1, *restrict S2;

#if X_OMP_CMP
  #pragma omp parallel for private(y,x,D,S1,S2) if(dst_dx*dst_dy >= X_OMP_THRESHOLD)
#endif
  for(y=0; y<dst_dy; y++)
  {
    int64 y1 = y*scale_y;
    int64 i=y1>>32;
    int64 distY = (y1-(i<<32)+(1<<23))>>24;
    if(distY==256) {i++; distY=0;}

    //line
    D  = m_Pel[y];
    S1 = Src->m_Pel[i];
    if(distY != 0)
    S2 = Src->m_Pel[i+1];

    for(x=0; x<dst_dx; x++)
    {  
      int64 x1 = x*scale_x;      
      int64 j=x1>>32;
      int64 distX = (x1-(j<<32)+(1<<23))>>24;
      if(distX==256) {j++; distX=0;}

      if(distX!=0 && distY!=0)
      {
        int32 c1 = S1[j];
        int32 c2 = S1[j+1];
        int32 c3 = S2[j];
        int32 c4 = S2[j+1];
        D[0] = (int16)(((c1*(256 - distX) + c2*(distX))*(256 - distY)
                       +(c3*(256 - distX) + c4*(distX))*(distY)+ 32768)>>16);        
      }
      else if(distY!=0)
      {
        int32 c1 = S1[j];
        int32 c3 = S2[j];
        D[0] = (int16)((c1*(256-distY) + c3*distY + 128) >> 8);        
      }
      else if(distX!=0)
      {
        int32 c1 = S1[j];
        int32 c2 = S1[j+1];
        D[0] = (int16)((c1*(256-distX) + c2*distX + 128) >> 8);        
      }
      else
      {
        D[0] = S1[j];
      } 

      D++;
    }
  }
}
void xCmp::rescaleU2Nearest(xCmp* Src)
{
#if (X_USE_SSE && X_SSE2)
  int32 dy,dx;
  int32 yd,ys,xs;
  int16 *restrict S;
  int16 *restrict D1;
  int16 *restrict D2;

  dy = m_SizeY/2; if(dy > Src->m_SizeY) dy = Src->m_SizeY; 
  dx = m_SizeX/2; if(dx > Src->m_SizeX) dx = Src->m_SizeX;

  if((Src->m_SizeX%xSSEAlign == 0) && (m_SizeX%xSSEAlign == 0)) //all lines aligned 
  {
    for(ys=0,yd=0; ys<dy; ys++,yd+=2)
    {    
      S =Src->m_Pel[ys];
      D1=m_Pel[yd];
      D2=m_Pel[yd+1];
      for(xs=0; xs<dx; xs+=8)
      {
        __m128i in = _mm_load_si128((__m128i*)(S));
        __m128i left_out = _mm_unpacklo_epi16(in, in);
        __m128i right_out = _mm_unpackhi_epi16(in, in);
        _mm_store_si128((__m128i*)(D1),left_out);
        _mm_store_si128((__m128i*)(D1+8),right_out);
        _mm_store_si128((__m128i*)(D2),left_out);
        _mm_store_si128((__m128i*)(D2+8),right_out);
        S+=8;
        D1+=16;
        D2+=16;
      }   
    }  
  }
  else //some lines non aligned
  {
    for(ys=0,yd=0; ys<dy; ys++,yd+=2)
    {    
      S =Src->m_Pel[ys];
      D1=m_Pel[yd];
      D2=m_Pel[yd+1];
      for(xs=0; xs<dx; xs+=8)
      {
        __m128i in = _mm_loadu_si128((__m128i*)(S));
        __m128i left_out = _mm_unpacklo_epi16(in, in);
        __m128i right_out = _mm_unpackhi_epi16(in, in);
        _mm_storeu_si128((__m128i*)(D1),left_out);
        _mm_storeu_si128((__m128i*)(D1+8),right_out);
        _mm_storeu_si128((__m128i*)(D2),left_out);
        _mm_storeu_si128((__m128i*)(D2+8),right_out);
        S+=8;
        D1+=16;
        D2+=16;
      }   
    }  
  }
#else
  int32 dy,dx;
  int32 yd,ys,xs;
  int16 *restrict S;
  int16 *restrict D1;
  int16 *restrict D2;

  dy = m_SizeY/2; if(dy > Src->m_SizeY) dy = Src->m_SizeY; 
  dx = m_SizeX/2; if(dx > Src->m_SizeX) dx = Src->m_SizeX;

  for(ys=0,yd=0; ys<dy; ys++,yd+=2)
  {    
    S =Src->m_Pel[ys];
    D1=m_Pel[yd];
    D2=m_Pel[yd+1];
    for(xs=0; xs<dx; xs++)
    {
      D1[0] = D1[1] = D2[0] = D2[1] = S[0];
      S++;
      D1+=2;
      D2+=2;
    }   
  }
#endif
}
void xCmp::rescaleU2BilinearChroma(xCmp* Src)
{
  int x,y;
  int dst_dx = m_SizeX, dst_dy = m_SizeY;
  int src_dx = Src->m_SizeX, src_dy = Src->m_SizeY;

  m_Pel[0][0] = Src->m_Pel[0][0]; //left-top sample

  for(x=1; x<dst_dx-1; x+=2) //top line
  {
    int32 a = Src->m_Pel[0][x/2];
    int32 b = Src->m_Pel[0][x/2+1];
    m_Pel[0][x]     = (int16)(((177*a+79*b)+128)>>8);
    m_Pel[0][x+1]   = (int16)(((177*b+79*a)+128)>>8);
  }

  m_Pel[0][dst_dx-1] = Src->m_Pel[0][src_dx-1]; //right-top sample

  for(y=1; y<dst_dy-1; y+=2)
  {
    int y_src = y>>1; //left collumn
    int32 a = Src->m_Pel[y_src][0];
    int32 b = Src->m_Pel[y_src+1][0];
    m_Pel[y][0]   = (int16)(((177*a+79*b)+128)>>8);
    m_Pel[y+1][0] = (int16)(((177*b+79*a)+128)>>8);

    for(x=1; x<dst_dx-1; x+=2)
    {
      int32 x_src = x>>1; //almost all
      int32 a = Src->m_Pel[y_src][x_src];
      int32 b = Src->m_Pel[y_src][x_src+1];
      int32 c = Src->m_Pel[y_src+1][x_src];
      int32 d = Src->m_Pel[y_src+1][x_src+1];
      m_Pel[y][x]     = (int16)(((116*a+51*(b+c)+38*d)+128)>>8);
      m_Pel[y][x+1]   = (int16)(((116*b+51*(a+d)+38*c)+128)>>8);
      m_Pel[y+1][x]   = (int16)(((116*c+51*(a+d)+38*b)+128)>>8);
      m_Pel[y+1][x+1] = (int16)(((116*d+51*(b+c)+38*a)+128)>>8);
    }

    int32 c = Src->m_Pel[y/2][src_dx-1]; //right collumn
    int32 d = Src->m_Pel[y/2+1][src_dx-1];
    m_Pel[y][dst_dx-1]   = (int16)(((177*c+79*d)+128)>>8);
    m_Pel[y+1][dst_dx-1] = (int16)(((177*d+79*c)+128)>>8);
  }

  m_Pel[dst_dy-1][0] = Src->m_Pel[src_dy-1][0]; //left-bottom sample

  for(x=1; x<dst_dx-1; x+=2) //bottom line
  {
    int32 a = Src->m_Pel[src_dy-1][x/2];
    int32 b = Src->m_Pel[src_dy-1][x/2+1];
    m_Pel[dst_dy-1][x]     = (int16)(((177*a+79*b)+128)>>8);
    m_Pel[dst_dy-1][x+1]   = (int16)(((177*b+79*a)+128)>>8);
  }

  m_Pel[dst_dy-1][dst_dx-1] = Src->m_Pel[src_dy-1][src_dx-1]; //right-bottom sample
}
void xCmp::rescaleU2Bilinear(xCmp* Src)
{
  int32 x,y, y2;
  int32 dx1,dy1; 
  int16 *srx0, *srx1, *dsx0, *dsx1;
  int16 sr00, sr01, sr10, sr11;

  dy1 = Src->m_SizeY-1;
  dx1 = Src->m_SizeX-1;
  for(y=0,y2=0 ; y<dy1 ; y++,y2+=2)
  {
    srx0 = Src->m_Pel[y];
    srx1 = Src->m_Pel[y+1];

    dsx0 = m_Pel[y2];
    dsx1 = m_Pel[y2+1];   

    sr00  = srx0[0];    
    sr01  = srx1[0];
    srx0 ++;
    srx1 ++;

    for(x=0 ; x<dx1 ; x++)
    {     
      sr10  = *srx0;
      sr11  = *srx1;
      dsx0[0] = sr00;
      dsx0[1] = (sr10+sr00)>>1;
      dsx1[0] = (sr01+sr00)>>1;
      dsx1[1] = (sr11+sr00+sr10+sr11+1)>>2;   
      sr00 = sr10;
      sr01 = sr11;
      dsx0 += 2;
      dsx1 += 2;
      srx0 ++;
      srx1 ++;
    }
    sr01 = (sr01+sr00+1)>>1;
    for (x*=2 ; x<m_SizeX ; ++x) 
    {
      *dsx0 = sr00;
      *dsx1 = sr01;
      dsx0 ++;
      dsx1 ++;
    }
  }

  for ( ; y2<m_SizeY ; ++y2) 
  {
    srx0 = Src->m_Pel[dy1];   
    dsx0 = m_Pel[y2];

    sr00  = srx0[0];    

    srx0 ++;

    for(x=0;x<dx1;x++)
    {     
      sr10  = *srx0;
      dsx0[0] = sr00;
      dsx0[1] = (sr10+sr00+1)>>1;
      sr00 = sr10;      
      dsx0 += 2;     
      srx0 ++;     
    }
    for (x*=2 ; x<m_SizeX ; ++x) 
    {
      *dsx0 = sr00;
      dsx0 ++;     
    }
  }
}
void xCmp::rescaleD2Avg(xCmp* Src)
{
#if (X_USE_SSE && X_SSE2)
  int32 dy,dx;
  int32 yd,ys,xd;
  int16 *restrict D;
  int16 *restrict S1;
  int16 *restrict S2;
  __m128i two = _mm_set1_epi16((int16)2);

  dy = Src->m_SizeY/2; if(dy > m_SizeY) dy = m_SizeY;
  dx = Src->m_SizeX/2; if(dx > m_SizeX) dx = m_SizeX;

  if((Src->m_SizeX%xSSEAlign == 0) && (m_SizeX%xSSEAlign == 0)) //all lines aligned 
  {
    for(yd=0,ys=0; yd<dy; yd++,ys+=2)
    {
      ys=yd<<1;
      S1=Src->m_Pel[ys  ];
      S2=Src->m_Pel[ys+1];
      D =m_Pel[yd  ];
      for(xd=0; xd<dx; xd+=8)
      {
        __m128i top_left  = _mm_load_si128((__m128i*)(S1));  //aligned load - faster
        __m128i top_right = _mm_load_si128((__m128i*)(S1+8));//aligned load - faster
        __m128i top = _mm_hadd_epi16(top_left, top_right);
        __m128i bottom_left  = _mm_load_si128((__m128i*)(S2));
        __m128i bottom_right = _mm_load_si128((__m128i*)(S2+8));
        __m128i bottom = _mm_hadd_epi16(bottom_left, bottom_right);
        __m128i out = _mm_add_epi16(top, bottom);
        out = _mm_add_epi16(out, two);
        out = _mm_srai_epi16(out, 2);
        _mm_store_si128 ((__m128i*)(D),out); //aligned store - faster
        D+=8;
        S1+=16;
        S2+=16;
      }      
    }
  }
  else //some lines non aligned
  {
    for(ys=0,yd=0; yd<dy; ys+=2,yd++)
    {
      S1=Src->m_Pel[ys  ];
      S2=Src->m_Pel[ys+1];
      D =m_Pel[yd  ];
      for(xd=0; xd<dx; xd+=8)
      {
        __m128i top_left  = _mm_loadu_si128((__m128i*)(S1));
        __m128i top_right = _mm_loadu_si128((__m128i*)(S1+8));
        __m128i top = _mm_hadd_epi16(top_left, top_right);
        __m128i bottom_left  = _mm_loadu_si128((__m128i*)(S2));
        __m128i bottom_right = _mm_loadu_si128((__m128i*)(S2+8));
        __m128i bottom = _mm_hadd_epi16(bottom_left, bottom_right);
        __m128i out = _mm_add_epi16(top, bottom);
        out = _mm_add_epi16(out, two);
        out = _mm_srai_epi16(out, 2);
        _mm_storeu_si128 ((__m128i*)(D),out);
        D+=8;
        S1+=16;
        S2+=16;
      }     
    }
  }
#else
  int32 dy,dx;
  int32 yd,ys,xd; 
  int16 *restrict D;
  int16 *restrict S1;
  int16 *restrict S2;

  dy = Src->m_SizeY/2; if(dy > m_SizeY) dy = m_SizeY;
  dx = Src->m_SizeX/2; if(dx > m_SizeX) dx = m_SizeX;

  for(ys=0,yd=0; yd<dy; ys+=2,yd++)
  {    
    S1=Src->m_Pel[ys  ];
    S2=Src->m_Pel[ys+1];
    D =m_Pel[yd  ];
    for(xd=0; xd<dx; xd++)
    {
      D[xd] = (S1[0] + S1[1] + S2[0] + S2[1] + 2)>>2;
      S1+=2;
      S2+=2;
    }   
  } 
#endif
}

//================================================================================================
// precision
//================================================================================================
void xCmp::bitDepthDecrease(xCmp* Src, int32 DeltaBits)
{
  xAssert(isCompatible(Src));
  xAssert(isSameSize  (Src));

  uint32 Offset = 1<<(DeltaBits-1);
  int16* restrict d = m_PelBuffer;
  int16* restrict s = Src->m_PelBuffer;  

  for(int32 i=0; i<m_BufferSize; i++)
  {
    d[i] = (s[i] + Offset) >> DeltaBits;
  }
}
void xCmp::bitDepthIncrease(xCmp* Src, int32 DeltaBits)
{
  xAssert(isCompatible(Src));
  xAssert(isSameSize  (Src));

  int16* restrict d = m_PelBuffer;
  int16* restrict s = Src->m_PelBuffer;  

  for(int32 i=0; i<m_BufferSize; i++)
  {
    d[i] = s[i] << DeltaBits;
  }
}

//================================================================================================
// crop & extend
//================================================================================================
void xCmp::crop(xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY)
{
  copyEx(Src, 0, 0, SrcSX, SrcSY, SrcEX, SrcEY);
}
void xCmp::extend(xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY, eCmpExtMode Mode)
{
  switch(Mode)
  {
    case CmpExtMode_FillGrey:
      extendGrey(Src, SrcSX, SrcSY, SrcEX, SrcEY);
      break;
    case CmpExtMode_EdgePixel:
      extendEdge(Src, SrcSX, SrcSY, SrcEX, SrcEY);
      break;
    case CmpExtMode_Mirror:
      extendMirror(Src, SrcSX, SrcSY, SrcEX, SrcEY);
      break;
    default:
      xAssert(0);
      break;
  }
}
void xCmp::extendGrey(xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY)
{
  int32 SrcSizeX = Src->m_SizeX;
  int32 SrcSizeY = Src->m_SizeY;

  //copy source
  copyEx(Src, -SrcSX, -SrcSY, 0, 0, SrcSizeX-1, SrcSizeY-1);

  //fill grey
  int16 Grey = 1<<(m_BitDepth-1);  
  //top
  #if X_OMP_CMP
  #pragma omp parallel
  {
  #pragma omp for
  #endif
  for(int32 y=0; y<-SrcSX; y++)
  {
    xMemsetX(m_Pel[y], Grey, m_SizeX);
  } 

  //left and right
  #if X_OMP_CMP
  #pragma omp for
  #endif
  for (int32 y=-SrcSY; y<SrcSizeY-SrcSY; y++)
  {
    int16* Line = m_Pel[y];
    for (int32 x=0; x<-SrcSX; x++) { Line[x] = Grey; }
    for (int32 x=SrcSizeX-SrcSX; x<m_SizeX; x++) { Line[x] = Grey; }
  }

  //bottom
  #if X_OMP_CMP
  #pragma omp for
  #endif
  for (int32 y=SrcSizeY-SrcSY; y<m_SizeY; y++)
  {
    xMemsetX(m_Pel[y], Grey, m_SizeX);
  }
  #if X_OMP_CMP
  } //end of porallel region
  #endif
}
void xCmp::extendEdge(xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY)
{
  int32 SrcSizeX = Src->m_SizeX;
  int32 SrcSizeY = Src->m_SizeY;

  //copy source
  copyEx(Src, -SrcSX, -SrcSY, 0, 0, SrcSizeX-1, SrcSizeY-1);

  //fill edge
  //left and right
#if X_OMP_CMP
  #pragma omp parallel for private(y,x)
#endif
  for(int32 y=-SrcSY; y<SrcSizeY-SrcSY; y++)
  {
    int16* Line = m_Pel[y];
    int16  LeftSample  = Line[-SrcSX];
    int16  RightSample = Line[SrcSizeX-SrcSX-1];
    for(int32 x=0; x<-SrcSX; x++) { Line[x] = LeftSample; }
    for(int32 x=SrcSizeX-SrcSX; x<m_SizeX; x++) { Line[x] = RightSample; }
  }

  //top
  for(int32 y=0; y<-SrcSY; y++) { memcpy(m_Pel[y], m_Pel[-SrcSY], m_SizeX*sizeof(int16)); }

  //bottom
  for(int32 y=SrcSizeY-SrcSY; y<SrcSizeY; y++) { memcpy(m_Pel[y], m_Pel[SrcSizeY-SrcSY-1], m_SizeX*sizeof(int16)); }
}
void xCmp::extendMirror(xCmp* Src, int32 SrcSX, int32 SrcSY, int32 SrcEX, int32 SrcEY)
{
  int32 SrcSizeX = Src->m_SizeX;
  int32 SrcSizeY = Src->m_SizeY;

  //copy source
  copyEx(Src, -SrcSX, -SrcSY, 0, 0, SrcSizeX-1, SrcSizeY-1);

  //fill mirror
  //left and right
#if X_OMP_CMP
  #pragma omp parallel for private(y,x)
#endif
  for(int32 y=-SrcSY; y<SrcSizeY-SrcSY; y++)
  {
    int16* Line = m_Pel[y];
    for(int32 x=0; x<-SrcSX; x++) { Line[x] = Line[-2*SrcSX-x-1]; }
    for(int32 x=SrcSizeX-SrcSX; x<m_SizeX; x++) { Line[x] = Line[2*(SrcSizeX-SrcSX)-1-x]; }
  }

  //top
  for(int32 y=0; y<-SrcSY; y++) { memcpy(m_Pel[y], m_Pel[-2*SrcSY-y], m_SizeX*sizeof(int16)); }

  //bottom
  for(int32 y=SrcSizeY-SrcSY; y<SrcSizeY; y++) { memcpy(m_Pel[y], m_Pel[2*(SrcSizeY-SrcSY)-1-y], m_SizeX*sizeof(int16)); }
}

//================================================================================================
// arithmetic operations
//================================================================================================
void xCmp::add(xCmp* Src0, xCmp* Src1)
{
  xAssert(isCompatible(Src0)); xAssert(isCompatible(Src1));
  if(m_Margin) { xPixel::Add(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_PelStride, Src0->m_PelStride, Src1->m_PelStride, m_SizeX, m_SizeY); }
  else         { xPixel::AddContinuous(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_BufferSize); }
}
void xCmp::addAndClip(xCmp* Src0, xCmp* Src1)
{
  xAssert(isCompatible(Src0)); xAssert(isCompatible(Src1));
  int16 ClippingRange = (int16)(1<<m_BitDepth)-1;
  if(m_Margin) { xPixel::AddAndClip(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_PelStride, Src0->m_PelStride, Src1->m_PelStride, m_SizeX, m_SizeY, ClippingRange); }
  else         { xPixel::AddAndClipContinuous(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_BufferSize, ClippingRange); }

}
void xCmp::sub(xCmp* Src0, xCmp* Src1)
{
  xAssert(isCompatible(Src0)); xAssert(isCompatible(Src1));
  if(m_Margin) { xPixel::Sub(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_PelStride, Src0->m_PelStride, Src1->m_PelStride, m_SizeX, m_SizeY); }
  else         { xPixel::SubContinuous(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_BufferSize); }
}
void xCmp::average(xCmp* Src0, xCmp* Src1)
  {
  xAssert(isCompatible(Src0)); xAssert(isCompatible(Src1));
  if(m_Margin) { xPixel::Avg(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_PelStride, Src0->m_PelStride, Src1->m_PelStride, m_SizeX, m_SizeY); }
  else         { xPixel::AvgContinuous(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_BufferSize); }
  }
void xCmp::averageAndClip(xCmp* Src0, xCmp* Src1)
  {
  xAssert(isCompatible(Src0)); xAssert(isCompatible(Src1));
  int16 ClippingRange = (int16)(1<<m_BitDepth)-1;
  if(m_Margin) { xPixel::AvgAndClip(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_PelStride, Src0->m_PelStride, Src1->m_PelStride, m_SizeX, m_SizeY, ClippingRange); }
  else         { xPixel::AvgAndClipContinuous(m_PelOrg, Src0->m_PelOrg, Src1->m_PelOrg, m_BufferSize, ClippingRange); }
  }
void xCmp::clip(xCmp* Src, int16 ClippingRangeLow, int16 ClippingRangeHigh)
{
  xAssert(isCompatible(Src));
  if(m_Margin) { xPixel::Clip(m_PelOrg, Src->m_PelOrg, m_PelStride, Src->m_PelStride, m_SizeX, m_SizeY, ClippingRangeLow, ClippingRangeHigh); }
  else         { xPixel::ClipContinuous(m_PelOrg, Src->m_PelOrg, m_BufferSize, ClippingRangeLow, ClippingRangeHigh); }
}

//================================================================================================
// distortion & PSNR
//================================================================================================
int64 xCmp::calcSAD(xCmp* ReferenceCmp)
{
  xAssert(isCompatible(ReferenceCmp));
  if(m_Margin) { return xDistortion::calcSAD(ReferenceCmp->m_PelOrg, ReferenceCmp->m_PelStride, m_PelOrg, m_PelStride, m_SizeX, m_SizeY); }
  else         { return xDistortion::calcSADContinuous(ReferenceCmp->m_PelOrg, m_PelOrg, m_BufferSize); }
}
int64 xCmp::calcSATD(xCmp* ReferenceCmp)
{
  xAssert(isCompatible(ReferenceCmp));
  return xDistortion::calcSATD(ReferenceCmp->m_PelOrg, ReferenceCmp->m_PelStride, m_PelOrg, m_PelStride, m_SizeX, m_SizeY, false);
}
int64 xCmp::calcSSD(xCmp* ReferenceCmp)
{
  xAssert(isCompatible(ReferenceCmp));
  if(m_Margin) { return xDistortion::calcSSD(ReferenceCmp->m_PelOrg, ReferenceCmp->m_PelStride, m_PelOrg, m_PelStride, m_SizeX, m_SizeY); }
  else         { return xDistortion::calcSSDContinuous(ReferenceCmp->m_PelOrg, m_PelOrg, m_BufferSize); }
  }
double xCmp::calcPSNR(xCmp* ReferenceCmp)
{
  xAssert(ReferenceCmp!=nullptr && ReferenceCmp->m_BitDepth==m_BitDepth);
  
  int32 NumPoints = m_Margin ? m_SizeX*m_SizeY : m_BufferSize;
  int32 MaxValue =(1<<m_BitDepth) - 1;
  int64 MAX = (int64)(NumPoints)*(int64)(MaxValue*MaxValue);
  int64 SSE = calcSSD(ReferenceCmp);

  double PSNR = SSE>0 ? 10.0*log10((double)MAX / (double)SSE) : double_max;
  return PSNR; 
}
void xCmp::calcHistogram(int32* DstVector)
{
  int32  Intervals = 1<<m_BitDepth; //2^BitDepth
  memset(DstVector, 0, Intervals*sizeof(int32));

  for(int32 i=0; i<m_BufferSize; i++)
  {
    DstVector[m_PelOrg[i]]++;
  }
}

//================================================================================================
// Transform
//================================================================================================
uint64 xCmp::transform_4x4(xCmp* Src)
{
  _declspec(align(32)) int16 S[4*4];
  _declspec(align(32)) int16 T[4*4];
  _declspec(align(32)) int16 Q[4*4];
  _declspec(align(32)) int16 D[4*4];

  uint64 TotalTicks = 0;

  int16* SrcPtr = Src->m_PelOrg;
  int16* DstPtr = m_PelOrg;
  uint32 SrcStride = Src->m_PelStride;
  uint32 DstStride = m_PelStride;
  
  for(int32 y=0; y<m_SizeY; y+=4)
  {
    for(int32 x=0; x<m_SizeX; x+=4)
    {
      for(int32 l=0; l<4; l++) memcpy(&S[4*l], SrcPtr+x+l*SrcStride, 4*sizeof(int16));

      uint64 Ticks = __rdtsc();
      xTransform::TransformDCT_4x4_SSE(S, T, 8);
      xQuant::xQuant_SSE(T, Q, 2, 20, 8);
      xQuant::xDeQuant_SSE(Q, T, 2, 20, 8);      
      xTransform::InvTransformDCT_4x4_SSE(T, D, 4, 8);
      TotalTicks += (__rdtsc() - Ticks);    

      for(int32 l=0; l<4; l++) memcpy(DstPtr+x+l*DstStride, &D[4*l], 4*sizeof(int16));
    }
    SrcPtr += (4*SrcStride);
    DstPtr += (4*DstStride);
  }

  return TotalTicks;
}
uint64 xCmp::transform_8x8(xCmp* Src)
{
  _declspec(align(32)) int16 S[8*8];
  _declspec(align(32)) int16 T[8*8];
  //_declspec(align(32)) int16 Q[8*8];
  _declspec(align(32)) int16 D[8*8];

  uint64 TotalTicks = 0;

  int16* SrcPtr = Src->m_PelOrg;
  int16* DstPtr = m_PelOrg;
  uint32 SrcStride = Src->m_PelStride;
  uint32 DstStride = m_PelStride;
  
  for(int32 y=0; y<m_SizeY; y+=8)
  {
    for(int32 x=0; x<m_SizeX; x+=8)
    {
      for(int32 l=0; l<8; l++) memcpy(&S[8*l], SrcPtr+x+l*SrcStride, 8*sizeof(int16));

      uint64 Ticks = __rdtsc();
      //xTransform::TransformDCT_4x4_STD_C(S, T, 4, 8);
      //xTransform::InvTransformDCT_4x4_STD_C(T, D, 4, 8);
      xTransform::TransformDCT_8x8_SSE(S, T, 8, 8);
      xTransform::InvTransformDCT_8x8_SSE(T, D, 8, 8);
      TotalTicks += (__rdtsc() - Ticks);    

      for(int32 l=0; l<8; l++) memcpy(DstPtr+x+l*DstStride, &D[8*l], 8*sizeof(int16));
    }
    SrcPtr += (8*SrcStride);
    DstPtr += (8*DstStride);
  }

  return TotalTicks;
}
uint64 xCmp::transform_16x16(xCmp* Src)
{
  _declspec(align(32)) int16 S[16*16];
  _declspec(align(32)) int16 T[16*16];
  //_declspec(align(32)) int16 Q[16*16];
  _declspec(align(32)) int16 D[16*16];

  uint64 TotalTicks = 0;

  int16* SrcPtr = Src->m_PelOrg;
  int16* DstPtr = m_PelOrg;
  uint32 SrcStride = Src->m_PelStride;
  uint32 DstStride = m_PelStride;
  
  for(int32 y=0; y<m_SizeY; y+=16)
  {
    for(int32 x=0; x<m_SizeX; x+=16)
    {
      for(int32 l=0; l<16; l++) memcpy(&S[16*l], SrcPtr+x+l*SrcStride, 16*sizeof(int16));

      uint64 Ticks = __rdtsc();
      //xTransform::TransformDCT_4x4_STD_C(S, T, 4, 8);
      //xTransform::InvTransformDCT_4x4_STD_C(T, D, 4, 8);
      xTransform::TransformDCT_16x16_SSE(S, T, 16, 8);
      xTransform::InvTransformDCT_16x16_SSE(T, D, 16, 8);
      TotalTicks += (__rdtsc() - Ticks);    

      for(int32 l=0; l<16; l++) memcpy(DstPtr+x+l*DstStride, &D[16*l], 16*sizeof(int16));
    }
    SrcPtr += (16*SrcStride);
    DstPtr += (16*DstStride);
  }

  return TotalTicks;
}
uint64 xCmp::transform_32x32(xCmp* Src)
{
  _declspec(align(32)) int16 S[32*32];
  _declspec(align(32)) int16 T[32*32];
  //_declspec(align(32)) int16 Q[32*32];
  _declspec(align(32)) int16 D[32*32];

  uint64 TotalTicks = 0;

  int16* SrcPtr = Src->m_PelOrg;
  int16* DstPtr = m_PelOrg;
  uint32 SrcStride = Src->m_PelStride;
  uint32 DstStride = m_PelStride;
  
  for(int32 y=0; y<m_SizeY; y+=32)
  {
    for(int32 x=0; x<m_SizeX; x+=32)
    {
      for(int32 l=0; l<32; l++) memcpy(&S[32*l], SrcPtr+x+l*SrcStride, 32*sizeof(int16));

      uint64 Ticks = __rdtsc();
      xTransform::TransformDCT_32x32_SSE(S, T, 32, 8);
      xTransform::InvTransformDCT_32x32_SSE(T, D, 32, 8);
      TotalTicks += (__rdtsc() - Ticks);    

      for(int32 l=0; l<32; l++) memcpy(DstPtr+x+l*DstStride, &D[32*l], 32*sizeof(int16));
    }
    SrcPtr += (32*SrcStride);
    DstPtr += (32*DstStride);
  }

  return TotalTicks;
}
uint64 xCmp::transform_64x64(xCmp* Src)
{
  _declspec(align(64)) int16 S[64*64];
  _declspec(align(64)) int16 T[64*64];
  _declspec(align(64)) int16 Q[64*64];
  _declspec(align(64)) int16 D[64*64];

  uint64 TotalTicks = 0;

  int16* SrcPtr = Src->m_PelOrg;
  int16* DstPtr = m_PelOrg;
  uint32 SrcStride = Src->m_PelStride;
  uint32 DstStride = m_PelStride;
  
  for(int32 y=0; y<m_SizeY; y+=64)
  {
    for(int32 x=0; x<m_SizeX; x+=64)
    {
      for(int32 l=0; l<64; l++) memcpy(&S[64*l], SrcPtr+x+l*SrcStride, 64*sizeof(int16));

      uint64 Ticks = __rdtsc();
      xTransform::TransformDCT_64x64_STD_M(S, T, 64, 8);
      xQuant::xQuant_STD(T, Q, 6, 32, 8);
      xQuant::xDeQuant_STD(Q, T, 6, 32, 8);
      xTransform::InvTransformDCT_64x64_STD_M(T, D, 64, 8);
      TotalTicks += (__rdtsc() - Ticks);    

      for(int32 l=0; l<64; l++) memcpy(DstPtr+x+l*DstStride, &D[64*l], 64*sizeof(int16));
    }
    SrcPtr += (64*SrcStride);
    DstPtr += (64*DstStride);
  }

  return TotalTicks;
}

} //end of namespace AVLib