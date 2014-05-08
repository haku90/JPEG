#pragma once
#ifndef _xBitstreamH_
#define _xBitstreamH_

#include "xLicense.h"
#include "xCommon.h"
#include <ostream>
#include <istream>

namespace AVlib {

class xBitstream
{
protected:
  uint8* m_BstrBuff; //main buffer
  uint32 m_BuffSize; //size of main buffer
  uint32 m_BuffPos;  //current position in main buffer

  uint32 m_TmpBuff;
  uint32 m_RemainigTmpBufferBits;

  uint32 m_ByteAligned;

  void  xCheckAlignment() { m_ByteAligned = ((m_RemainigTmpBufferBits & 0x07) == 0); }

public:
  xBitstream();
  //~xBitstream() {};

  void  create(uint32 BufferSize);
  void  resize(uint32 BufferSize);
  void  destroy();

  virtual void init() = 0;

  uint32 getBuffSize()    { return m_BuffSize; }
  uint32 getByteAligned() { return m_ByteAligned; }  
};

// ====================================================================================================================

class xBitstreamWriter : public xBitstream
{
protected:
  void  xWriteAlign(uint32 AlignBit);

public:
  xBitstreamWriter() : xBitstream() { m_RemainigTmpBufferBits = 32; };
  
  void   init();
  void   writeBits(uint32 Bits, uint32 NumberOfBits);
  void   writeByte(uint32 Byte);  
  void   writeAlignZero() { xWriteAlign(0); }
  void   writeAlignOne () { xWriteAlign(1); }  
  void   flushToBuffer();
  void   flushToStream(std::ostream* OutStream);
  void   flushToFile();
  uint32 getWrittenBits() { return ((m_BuffPos-1)<<3) + m_RemainigTmpBufferBits; } 
};

// ====================================================================================================================

class xBitstreamReader : public xBitstream
{
protected:
  uint32 xReadAlign();

public:
  xBitstreamReader() : xBitstream() { m_RemainigTmpBufferBits = 0;  };

  void   init();
  uint32 readBits(uint32 Length);
  uint32 readByte();
  uint32 readAlignZero() { uint32 AlignedBit = xReadAlign(); xAssert(AlignedBit==0); return AlignedBit; }
  uint32 readAlignOne () { uint32 AlignedBit = xReadAlign(); xAssert(AlignedBit==1); return AlignedBit; }
  void   fetchFromStream(std::istream* InStream);
  void   fetchFromFile();
  int32  getBitsLeft()   { return ((m_BuffSize-(m_BuffPos-4))<<3)-(32-m_RemainigTmpBufferBits); }  
};

} //end of namespace AVLib

#endif