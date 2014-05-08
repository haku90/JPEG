#include "xSequence.h"

#include "..\external\lz4.h"
#include "..\external\lz4hc.h"
#define MINIZ_NO_STDIO
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_TIME
#define MINIZ_NO_ZLIB_APIS
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#define MINIZ_NO_MALLOC
#include "..\external\miniz.c"

namespace AVlib {

void xSeq::createYUVWrite(int32 SizeX, int32 SizeY, int32 BitDepth, eCrF ChromaFormat, eClrSpc ColorSpace, xchar* FileName, int32 BufferSize)
{
  m_SizeX = SizeX;  m_SizeY = SizeY;  m_BitDepth = BitDepth;  m_ChromaFormat = ChromaFormat;  m_ColorSpace = ColorSpace;
  m_SequenceType = SeqTp_yuv; m_SequenceCompression = SeqCmpr_None; m_SequencePrediction = SeqPred_None; m_SequenceMode = Seq::SeqMode_Write;
  m_PackedFrameSize = xImg::calcSizeRAW(m_SizeX, m_SizeY, m_BitDepth, m_ChromaFormat);
  m_BufferSize = BufferSize;  
  m_FileName = xstrdup(FileName);
  m_File = m_SequenceMode==Seq::SeqMode_Read ? x_fopen(m_FileName, TEXT("r")) : x_fopen(m_FileName, TEXT("w"));
  m_NumOfFrames = 0;
  xCreate();
}

void xSeq::createYUVRead(int32 SizeX, int32 SizeY, int32 BitDepth, eCrF ChromaFormat, eClrSpc ColorSpace, xchar* FileName, int32 BufferSize)
{
  m_SizeX = SizeX;  m_SizeY = SizeY;  m_BitDepth = BitDepth;  m_ChromaFormat = ChromaFormat;  m_ColorSpace = ColorSpace;
  m_SequenceType = SeqTp_yuv; m_SequenceCompression = SeqCmpr_None; m_SequencePrediction = SeqPred_None; m_SequenceMode = Seq::SeqMode_Read;
  m_PackedFrameSize = xImg::calcSizeRAW(m_SizeX, m_SizeY, m_BitDepth, m_ChromaFormat);
  m_BufferSize = BufferSize; 
  m_FileName = xstrdup(FileName);
  m_File = m_SequenceMode==Seq::SeqMode_Read ? x_fopen(m_FileName, TEXT("r")) : x_fopen(m_FileName, TEXT("w"));
  m_NumOfFrames = (uint32)(x_fsize(m_File) / m_PackedFrameSize);
  xCreate();
}

void xSeq::createXSEQWrite(int32 SizeX, int32 SizeY, int32 BitDepth, eCrF ChromaFormat, eClrSpc ColorSpace, xchar* FileName, eSeqCmpr SeqCmpr, eSeqPred SeqPred, int32 BufferSize)
{
  m_SizeX = SizeX;  m_SizeY = SizeY;  m_BitDepth = BitDepth;  m_ChromaFormat = ChromaFormat;  m_ColorSpace = ColorSpace;
  m_SequenceType = SeqTp_xseq; m_SequenceCompression = SeqCmpr; m_SequencePrediction = SeqPred; m_SequenceMode = Seq::SeqMode_Write;
  m_PackedFrameSize = xImg::calcSizeRAW(m_SizeX, m_SizeY, m_BitDepth, m_ChromaFormat);
  m_BufferSize = BufferSize;  
  m_FileName = xstrdup(FileName);
  m_File = m_SequenceMode==Seq::SeqMode_Read ? x_fopen(m_FileName, TEXT("r")) : x_fopen(m_FileName, TEXT("w"));
  m_NumOfFrames = 0;
  xWriteHeader();
  xCreate();
}

void xSeq::createXSEQRead(xchar* FileName, int32 BufferSize)
{
  m_SequenceType = SeqTp_xseq; m_SequenceMode = Seq::SeqMode_Read;
  m_BufferSize = BufferSize;  
  m_FileName = xstrdup(FileName);
  m_File = m_SequenceMode==Seq::SeqMode_Read ? x_fopen(m_FileName, TEXT("r")) : x_fopen(m_FileName, TEXT("w"));
  xReadHeader();
  m_PackedFrameSize = xImg::calcSizeRAW(m_SizeX, m_SizeY, m_BitDepth, m_ChromaFormat);
  xCreate();
}

void xSeq::xCreate()
{    
  BY_HANDLE_FILE_INFORMATION FileInformation;
  GetFileInformationByHandle(m_File, &FileInformation);
  m_VolumeSerialNumber = FileInformation.dwVolumeSerialNumber;
  m_UserFrame = 0;
  m_IOFrame = 0;

  m_PackedBuffer     = (byte*)xAlignedMalloc(m_PackedFrameSize+64, xDefAlign);
  m_CompressedBuffer = (byte*)xAlignedMalloc(m_PackedFrameSize+64, xDefAlign);
  m_Img = new xImg;
  m_Img->create(m_SizeX, m_SizeY, 0, m_BitDepth, ImgTp_YUV, m_ChromaFormat, m_ColorSpace, 0);
  m_Img444 = new xImg;
  m_Img444->create(m_SizeX, m_SizeY, 0, m_BitDepth, ImgTp_YUV, CrF_444, m_ColorSpace, 0);
  
  if(m_SequenceCompression>=SeqCmpr_DEFLATE_CL1 && m_SequenceCompression<= SeqCmpr_DEFLATE_CL10)
  {
    m_MiniZ = m_SequenceMode==Seq::SeqMode_Read ? (void*)xMalloc(sizeof(tinfl_decompressor)) : (void*)xMalloc(sizeof(tdefl_compressor  ));
  }

  if(m_BufferSize>0)
  {
    m_FullRing = new xRing;
    m_FullRing->create(m_BufferSize);
    m_EmptyRing = new xRing;
    m_EmptyRing->create(m_BufferSize);
    for(int32 i=0; i<m_BufferSize; i++)
    {
      byte* Buffer = (byte*)xAlignedMalloc(m_PackedFrameSize+64, xDefAlign);
      m_EmptyRing->put(Buffer);
    }

    for(int32 i=0; i<Seq::EventIdx_NumberOf; i++) { m_Events[i] = CreateEvent(NULL, false, false, NULL); }

    m_IOThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)xThreadFunction, (LPVOID)this, 0, 0);
  }
}

void xSeq::destroy()
{
  if(m_BufferSize>0)
  {
    SetEvent(m_Events[Seq::EventIdx_Terminate]);
    WaitForSingleObject(m_Events[Seq::EventIdx_Ack], INFINITE);

    while(!m_FullRing->isEmpty())  { xAlignedFree(m_FullRing->get());  }
    while(!m_EmptyRing->isEmpty()) { xAlignedFree(m_EmptyRing->get()); }

    m_FullRing->destroy();  delete m_FullRing;
    m_EmptyRing->destroy(); delete m_EmptyRing;
  }

  xAlignedFree(m_PackedBuffer);
  xAlignedFree(m_CompressedBuffer);

  m_Img->destroy(); delete m_Img;
  m_Img444->destroy(); delete m_Img444;

  x_fseek(m_File, 0, X_SEEK_BEGIN);

  xWriteHeader();

  free(m_FileName);
  x_fclose(m_File);
}

void xSeq::getFrame()
{
  xAssert(m_SequenceMode==Seq::SeqMode_Read);
  if(m_BufferSize>0)
  {
    byte* Buffer = (byte*)m_FullRing->get();
    m_Img->unpackRAW(Buffer);
    m_EmptyRing->put(Buffer);
    SetEvent(m_Events[Seq::EventIdx_Update]);    
  }
  else
  {
    xReadFrame(m_PackedBuffer);
    m_Img->unpackRAW(m_PackedBuffer);
    m_IOFrame++;
  }
  m_UserFrame++;
}

void xSeq::putFrame()
{
  xAssert(m_SequenceMode==Seq::SeqMode_Write);
  if(m_BufferSize>0)
  {
    byte* Buffer = (byte*)m_EmptyRing->get();
    m_Img->packRAW(Buffer);
    m_FullRing->put(Buffer);
    SetEvent(m_Events[Seq::EventIdx_Update]);    
  }
  else
  {
    m_Img->packRAW(m_PackedBuffer);
    xWriteFrame(m_PackedBuffer);
    m_IOFrame++;
  }
  m_UserFrame++;
  m_NumOfFrames++;
}

void xSeq::seek(int32 SeekFrames, uint32 Origin)
{
  if(m_BufferSize>0)
  {
    switch(Origin)
    {
      case X_SEEK_CUR:
        m_UserFrame += SeekFrames;
        break;
      case X_SEEK_END:
        m_UserFrame = m_NumOfFrames+SeekFrames;
        break;
      default:
        m_UserFrame = m_NumOfFrames;
        break;
    }

    if(m_UserFrame<0) m_UserFrame=0;

    SetEvent(m_Events[Seq::EventIdx_Seek]);
    WaitForSingleObject(m_Events[Seq::EventIdx_Ack], INFINITE);
  }
  else
  {
    x_fseek(m_File, SeekFrames*m_PackedFrameSize, Origin);
  }
}

void xSeq::xWriteHeader()
{
  if(m_SequenceType==SeqTp_xseq)
  {
    Seq::xSeqHeader SeqHeader;
    ::memcpy(SeqHeader.m_FourCC, "xSeq", 4);
    SeqHeader.m_Version = 0;
    SeqHeader.m_SizeX = m_SizeX;
    SeqHeader.m_SizeY = m_SizeY;
    SeqHeader.m_NumComponents = 3;
    SeqHeader.m_ImageType = ImgTp_YUV;
    SeqHeader.m_ColorSpace = m_ColorSpace;
    SeqHeader.m_ChromaFormat = m_ChromaFormat-400;
    SeqHeader.m_PelType = m_BitDepth<=8 ? PelType_uint8 : PelType_uint16;
    SeqHeader.m_BitDepth = m_BitDepth;
    SeqHeader.m_Compression = m_SequenceCompression;
    SeqHeader.m_Prediction = m_SequencePrediction;
    SeqHeader.m_Reserved0 = 0;
    SeqHeader.m_Reserved1 = 0;
    SeqHeader.m_Reserved2 = 0;
    SeqHeader.m_Reserved3 = 0;
    SeqHeader.m_NumImages = m_NumOfFrames;
    SeqHeader.m_HeaderCRC32C = xChecksum::calcCRC32C((const uint8*)&SeqHeader, sizeof(SeqHeader) - sizeof(SeqHeader.m_HeaderCRC32C));

    x_fwrite(m_File, &SeqHeader, sizeof(SeqHeader));
  }
}

void xSeq::xReadHeader()
{
  if(m_SequenceType==SeqTp_xseq)
  {
    Seq::xSeqHeader SeqHeader;
    x_fread(m_File, &SeqHeader, sizeof(SeqHeader));
    uint32 CRC32C = xChecksum::calcCRC32C((const uint8*)&SeqHeader, sizeof(SeqHeader) - sizeof(SeqHeader.m_HeaderCRC32C));
    xAssert(SeqHeader.m_HeaderCRC32C == CRC32C);

    m_SizeX = SeqHeader.m_SizeX;
    m_SizeY = SeqHeader.m_SizeY;
    m_ColorSpace = (eClrSpc)SeqHeader.m_ColorSpace;
    m_ChromaFormat = (eCrF)(SeqHeader.m_ChromaFormat+400);
    m_BitDepth = SeqHeader.m_BitDepth;
    m_SequenceCompression = (eSeqCmpr)SeqHeader.m_Compression;
    m_SequencePrediction = (eSeqPred)SeqHeader.m_Prediction;
    m_NumOfFrames = SeqHeader.m_NumImages;

    //printf("m_SizeX = %d\n", SeqHeader.m_SizeX);
    //printf("m_SizeY = %d\n", SeqHeader.m_SizeY);
    //printf("m_NumComponents = %d\n", SeqHeader.m_NumComponents);
    //printf("m_ImageType = %d\n", SeqHeader.m_ImageType);
    //printf("m_ColorSpace = %d\n", SeqHeader.m_ColorSpace);
    //printf("m_ChromaFormat = %d\n", SeqHeader.m_ChromaFormat);
    //printf("m_PelType = %d\n", SeqHeader.m_PelType);
    //printf("m_BitDepth = %d\n", SeqHeader.m_BitDepth);
    //printf("m_Compression = %d\n", SeqHeader.m_Compression);
    //printf("m_Prediction = %d\n", SeqHeader.m_Prediction);
    //printf("m_Reserved = %d\n", SeqHeader.m_Reserved0);
    //printf("m_Reserved = %d\n", SeqHeader.m_Reserved1);
    //printf("m_Reserved = %d\n", SeqHeader.m_Reserved2);
    //printf("m_Reserved = %d\n", SeqHeader.m_Reserved3);
    //printf("m_NumImages = %d\n", SeqHeader.m_NumImages);
    //printf("m_HeaderCRC32C = %d\n", SeqHeader.m_HeaderCRC32C);
  }
}

void xSeq::xWriteFrame(byte* PackedFrameBuffer)
{
  if(m_SequenceType==SeqTp_yuv)
  {
    uint32 Writen = x_fwrite(m_File, PackedFrameBuffer, m_PackedFrameSize);
    xAssert(Writen==m_PackedFrameSize);
  }
  else if(m_SequenceType==SeqTp_xseq)
  {
    if(m_SequencePrediction==SeqPred_Linear)
    {
      uint8 PrevByte = PackedFrameBuffer[0];
      for(uint32 i=1; i<m_PackedFrameSize; i++)
      {
        uint8 Delta = PackedFrameBuffer[i] - PrevByte + (uint8)128;
        PrevByte = PackedFrameBuffer[i];
        PackedFrameBuffer[i] = Delta;
      }
    }

    if(m_SequenceCompression==SeqCmpr_None)
    {
      uint32 CRC32C = xChecksum::calcCRC32C(PackedFrameBuffer, m_PackedFrameSize);
      *((uint32*)PackedFrameBuffer) = CRC32C;
      uint32 Writen = x_fwrite(m_File, PackedFrameBuffer, m_PackedFrameSize+4);
      xAssert(Writen==m_PackedFrameSize+4);
    }
    else
    {
      int32 CompressedFrameSize = 0;

      if(m_SequenceCompression==SeqCmpr_LZ4)
      {
        CompressedFrameSize = LZ4_compress_limitedOutput  ((char*)PackedFrameBuffer, (char*)m_CompressedBuffer+4, m_PackedFrameSize, m_PackedFrameSize+32);                  
      }
      else if(m_SequenceCompression==SeqCmpr_LZ4HC)
      {
        CompressedFrameSize = LZ4_compressHC_limitedOutput((char*)PackedFrameBuffer, (char*)m_CompressedBuffer+4, m_PackedFrameSize, m_PackedFrameSize+32);
      }
      else if(m_SequenceCompression>=SeqCmpr_DEFLATE_CL1 && m_SequenceCompression<=SeqCmpr_DEFLATE_CL10)
      {
        int32  DelfateCompresionLevel = m_SequenceCompression-SeqCmpr_DEFLATE_CL1+1;
        int32  DelfateNumProbesMapping[11] = { 0, 1, 6, 32,  16, 32, 128, 256,  512, 768, 1500 };
        int32  DelfateFlags = ((DelfateCompresionLevel <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0) 
                            | DelfateNumProbesMapping[xMin(10, DelfateCompresionLevel)];
        size_t DelfatePackedBytes = m_PackedFrameSize;
        size_t DeflateCompressedBytes = m_PackedFrameSize+32;
        int32 DeflateStatus = tdefl_init((tdefl_compressor*)m_MiniZ, NULL, NULL, DelfateFlags);
        DeflateStatus = tdefl_compress((tdefl_compressor*)m_MiniZ, PackedFrameBuffer, &DelfatePackedBytes, m_CompressedBuffer+4, &DeflateCompressedBytes, TDEFL_FINISH);
        CompressedFrameSize = (int32)DeflateCompressedBytes;
      }

      *((int32*)m_CompressedBuffer) = CompressedFrameSize;
      uint32 CRC32C = xChecksum::calcCRC32C(m_CompressedBuffer, CompressedFrameSize+4);
      *((int32*)(m_CompressedBuffer+CompressedFrameSize+4)) = CRC32C;
      uint32 Writen = x_fwrite(m_File, m_CompressedBuffer, CompressedFrameSize+8);
      xAssert(Writen==CompressedFrameSize+8);
    }
  }
}

void xSeq::xReadFrame(byte* PackedFrameBuffer)
{
  if(m_SequenceType==SeqTp_yuv)
  {
    uint32 Read = x_fread(m_File, PackedFrameBuffer, m_PackedFrameSize);
  }
  else if(m_SequenceType==SeqTp_xseq)
  {
    if(m_SequenceCompression==SeqCmpr_None)
    {
      uint32 Read = x_fread(m_File, PackedFrameBuffer, m_PackedFrameSize+4);
      uint32 CRC32C = xChecksum::calcCRC32C(PackedFrameBuffer, m_PackedFrameSize);
      xAssert(*((uint32*)PackedFrameBuffer) == CRC32C);
    }
    else
    {
      uint32 CompressedFrameSize;
      uint32 Read = x_fread(m_File, &CompressedFrameSize, 4);
      *((int32*)m_CompressedBuffer) = CompressedFrameSize;
      Read = x_fread(m_File, m_CompressedBuffer+4, CompressedFrameSize+4);
      uint32 CRC32C = xChecksum::calcCRC32C(m_CompressedBuffer, CompressedFrameSize+4);
      xAssert(*((int32*)(m_CompressedBuffer+CompressedFrameSize+4)) == CRC32C);

      if(m_SequenceCompression==SeqCmpr_LZ4 || m_SequenceCompression==SeqCmpr_LZ4HC)
      {
        LZ4_decompress_safe((char*)(m_CompressedBuffer+4), (char*)(PackedFrameBuffer), CompressedFrameSize, m_PackedFrameSize);        
      }
      else if(m_SequenceCompression>=SeqCmpr_DEFLATE_CL1 && m_SequenceCompression<=SeqCmpr_DEFLATE_CL10)
      {
        size_t InflatorPackedBytes = m_PackedFrameSize;
        size_t InflatorCompressedBytes = CompressedFrameSize;
        tinfl_init((tinfl_decompressor*)m_MiniZ);
        int32 InflatorStatus = tinfl_decompress((tinfl_decompressor*)m_MiniZ, m_CompressedBuffer+4, &InflatorCompressedBytes, PackedFrameBuffer, PackedFrameBuffer, &InflatorPackedBytes, TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
      }      
    }

    if(m_SequencePrediction==SeqPred_Linear)
    {      
      for(uint32 i=1; i<m_PackedFrameSize; i+=8)
      {
        uint8 A = PackedFrameBuffer[i-1] - (uint8)128;

        uint8 d0 = PackedFrameBuffer[i];
        uint8 d1 = PackedFrameBuffer[i+1];
        uint8 d2 = PackedFrameBuffer[i+2];
        uint8 d3 = PackedFrameBuffer[i+3];
        uint8 d4 = PackedFrameBuffer[i+4];
        uint8 d5 = PackedFrameBuffer[i+5];
        uint8 d6 = PackedFrameBuffer[i+6];
        uint8 d7 = PackedFrameBuffer[i+7];

        PackedFrameBuffer[i  ] =  A + d0;
        PackedFrameBuffer[i+1] =  A + d0 + d1;
        PackedFrameBuffer[i+2] =  A + d0 + d1 + d2;
        PackedFrameBuffer[i+3] =  A + d0 + d1 + d2 + d3;       
        PackedFrameBuffer[i+3] =  A + d0 + d1 + d2 + d3 + d4;
        PackedFrameBuffer[i+3] =  A + d0 + d1 + d2 + d3 + d4 + d5;
        PackedFrameBuffer[i+3] =  A + d0 + d1 + d2 + d3 + d4 + d5 + d6;
        PackedFrameBuffer[i+3] =  A + d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7; 
        //looks strange but allows to avoid performance penalty caused by uop execution and bypass network latency
      }
    }
  }
}

DWORD xSeq::xThreadFunction(xSeq* Sequence)
{
  if(Sequence->m_SequenceMode==Seq::SeqMode_Read)
  {
    return Sequence->xThreadReaderFunction();
  }
  else if(Sequence->m_SequenceMode==Seq::SeqMode_Write)
  {
    return Sequence->xThreadWriterFunction();
  }
  else
  {
    xAssert(0);
    return EXIT_FAILURE;
  }
}

uint32 xSeq::xThreadReaderFunction()
{
  //fill buffer
  while((!m_EmptyRing->isEmpty()) && m_IOFrame<m_NumOfFrames)
  {
    byte* Buffer = (byte*)m_EmptyRing->get();
    xReadFrame(Buffer);
    m_FullRing->put(Buffer);
    m_IOFrame++;
  }

  while(1)
  {
    int32 EventIdx = WaitForMultipleObjects(3, m_Events, false, INFINITE);
    if(EventIdx==Seq::EventIdx_Update) //update
    {
      while((!m_EmptyRing->isEmpty()) && m_IOFrame<m_NumOfFrames)
      {
        byte* Buffer = (byte*)m_EmptyRing->get();
        xReadFrame(Buffer);
        m_FullRing->put(Buffer);
        m_IOFrame++;
      }
    }
    else if(EventIdx==Seq::EventIdx_Seek) //seek
    {
      while(!m_FullRing->isEmpty()) { m_EmptyRing->put(m_FullRing->get()); } //clean buffer
      m_IOFrame = m_UserFrame;
      x_fseek(m_File, m_IOFrame, X_SEEK_BEGIN);
      while((!m_EmptyRing->isEmpty()) && m_IOFrame<m_NumOfFrames)
      {
        byte* Buffer = (byte*)m_EmptyRing->get();
        xReadFrame(Buffer);
        m_FullRing->put(Buffer);
        m_IOFrame++;
      }
      SetEvent(m_Events[Seq::EventIdx_Ack]);
    }
    else if(EventIdx==Seq::EventIdx_Terminate) //terminate
    {
      SetEvent(m_Events[Seq::EventIdx_Ack]);
      return EXIT_SUCCESS;
    }
  }  
}

uint32 xSeq::xThreadWriterFunction()
{
  while(1)
  {
    int32 EventIdx = WaitForMultipleObjects(3, m_Events, false, INFINITE);
    if(EventIdx==Seq::EventIdx_Update) //update
    {
      while(!m_FullRing->isEmpty())
      {
        byte* Buffer = (byte*)m_FullRing->get();
        xWriteFrame(Buffer);
        m_EmptyRing->put(Buffer);
        m_IOFrame++;
      }
    }
    else if(EventIdx==Seq::EventIdx_Seek) //seek
    {
      //store buffered data
      while(!m_FullRing->isEmpty())
      {
        byte* Buffer = (byte*)m_FullRing->get();
        xWriteFrame(Buffer);
        m_EmptyRing->put(Buffer);
        m_IOFrame++;
      }
      m_IOFrame = m_UserFrame;
      x_fseek(m_File, m_IOFrame, X_SEEK_BEGIN);
      SetEvent(m_Events[Seq::EventIdx_Ack]);
    }
    else if(EventIdx==Seq::EventIdx_Terminate) //terminate
    {
      while(!m_FullRing->isEmpty())
      {
        byte* Buffer = (byte*)m_FullRing->get();
        xWriteFrame(Buffer);
        m_EmptyRing->put(Buffer);
        m_IOFrame++;
      }
      SetEvent(m_Events[Seq::EventIdx_Ack]);
      return EXIT_SUCCESS;
    }
  }
}

} //end of namespace AVLib




