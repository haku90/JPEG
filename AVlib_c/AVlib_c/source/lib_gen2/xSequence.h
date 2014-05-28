#pragma once
#ifndef _xSequenceH_
#define _xSequenceH_

#include "xLicense.h"
#include "xCommon.h"
#include "xImage.h"
#include "xRing.h"
#include "xChecksum.h"

namespace AVlib {
namespace Seq {
enum eEventIdx
{
  EventIdx_Update = 0,
  EventIdx_Seek,
  EventIdx_Terminate,
  EventIdx_Ack,
  EventIdx_NumberOf,  
};

enum eSeqMode : int32
{
  SeqMode_Unknown,
  SeqMode_Read,
  SeqMode_Write,
};


#pragma pack (1)
__declspec(align(1)) struct xSeqHeader
{
  char    m_FourCC[4];
  uint32  m_Version;
  uint32  m_SizeX;
  uint32  m_SizeY;

  uint8   m_NumComponents;
  uint8   m_ImageType;
  uint8   m_ColorSpace;
  uint8   m_ChromaFormat;

  uint8   m_PelType;
  uint8   m_BitDepth;
  uint8   m_Compression;
  uint8   m_Prediction;

  uint32  m_Reserved0;
  uint32  m_Reserved1;
  uint32  m_Reserved2;
  uint32  m_Reserved3;

  uint32  m_NumImages;
  uint32  m_HeaderCRC32C;
};

} // end of namespace xSeq

class xSeq
{
public:
  int32          m_SizeX;
  int32          m_SizeY;
  int32          m_BitDepth;      //bits per sample for int cmp
  eCrF           m_ChromaFormat;
  eClrSpc        m_ColorSpace;  
  eSeqTp         m_SequenceType;
  eSeqCmpr       m_SequenceCompression;
  eSeqPred       m_SequencePrediction;
  uint32         m_PackedFrameSize;
  int32          m_BufferSize;

  Seq::eSeqMode  m_SequenceMode;
  xchar*         m_FileName;
  FILE_HANDLE    m_File;
  uint32         m_VolumeSerialNumber;
  int32          m_NumOfFrames;
  int32          m_UserFrame;
  int32          m_IOFrame;  

  byte*          m_PackedBuffer;
  byte*          m_CompressedBuffer;
  xImg*          m_Img;
  xImg*          m_Img444;

  xRing*         m_FullRing;
  xRing*         m_EmptyRing;

  void*          m_MiniZ;

  EVENT_HANDLE   m_Events[Seq::EventIdx_NumberOf];

  THREAD_HANDLE  m_IOThread;


public:
  //base functions
  xSeq                      (){}
  //yuv sequence
  void     createYUVWrite   (int32 SizeX, int32 SizeY, int32 BitDepth, eCrF ChromaFormat, eClrSpc ColorSpace, xchar* FileName, int32 BufferSize);
  void     createYUVRead    (int32 SizeX, int32 SizeY, int32 BitDepth, eCrF ChromaFormat, eClrSpc ColorSpace, xchar* FileName, int32 BufferSize);
  //xseq sequence
  void     createXSEQWrite  (int32 SizeX, int32 SizeY, int32 BitDepth, eCrF ChromaFormat, eClrSpc ColorSpace, xchar* FileName, eSeqCmpr SeqCmpr, eSeqPred SeqPred, int32 BufferSize);
  void     createXSEQRead   (xchar* FileName, int32 BufferSize);

  void     destroy          ();

  void     seek             (int32 SeekFrames, uint32 Origin);
  void     getFrame         ();
  void     putFrame         ();

protected:
  void     xCreate();
  void     xWriteHeader ();
  void     xReadHeader  ();
  void     xWriteFrame  (byte* PackedFrameBuffer);
  void     xReadFrame   (byte* PackedFrameBuffer);

protected:
  static DWORD xThreadFunction(xSeq* Sequence);
  uint32       xThreadReaderFunction();
  uint32       xThreadWriterFunction();
};   


/*
header "xseq"   4B
version/reserv  4B
size_x          4B
size_y          4B
num_components  1B
img_type        1B (yuv, rgb...)
chroma_format   1B (400...)
color_space     1B (BT....)
pel_type        1B (8,16,32,64 int/uint, hfloat, float, double)
bit_depth       1B
compresion      1B (none, deflate, LZ4)
xxx             1B
num_images      4B (0=unknown)
header crc-32c  4B

image_len       4B (only for compressed image)
image_payload   var
image crc/adler 4B
*/

} //end of namespace AVLib

#endif //_xSequenceH_