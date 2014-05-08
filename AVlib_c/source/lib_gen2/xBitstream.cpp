#include "xBitstream.h"

namespace AVlib {

// ====================================================================================================================
// xBitstream
// ====================================================================================================================
xBitstream::xBitstream()
{
  m_BstrBuff = nullptr;
  m_BuffSize = 0;
  m_BuffPos  = 0;
  m_TmpBuff  = 0;  
  m_ByteAligned = true;
}

void xBitstream::create(uint32 Size)
{
  m_BuffSize = Size;
  m_BstrBuff = (uint8*)xAlignedMalloc(Size, xDefAlign);
}

void xBitstream::resize(uint32 NewSize)
{
  m_BuffSize = NewSize;
  xAlignedFree(m_BstrBuff);
  m_BstrBuff = (uint8*)xAlignedMalloc(NewSize, xDefAlign);
}

void xBitstream::destroy()
{
  m_BuffSize = 0;
  xAlignedFree(m_BstrBuff);
  m_BstrBuff = nullptr;
}

// ====================================================================================================================
// xBitstreamWriter
// ====================================================================================================================
void xBitstreamWriter::writeBits(uint32 Bits, uint32 NumberOfBitsToWrite)
{
  xAssert( NumberOfBitsToWrite <= 32 );
  uint32 Mask = (0xFFFFFFFF >> (32 - NumberOfBitsToWrite));
  xAssert( (Bits & Mask) == Bits);
  Bits &= Mask;

  if(NumberOfBitsToWrite & 0x07) m_ByteAligned = false;

  if(m_RemainigTmpBufferBits > NumberOfBitsToWrite)
  {
    m_TmpBuff = (m_TmpBuff<<NumberOfBitsToWrite) | Bits;
    m_RemainigTmpBufferBits -= NumberOfBitsToWrite;
  }
  else
  {
    uint32 Delta = NumberOfBitsToWrite - m_RemainigTmpBufferBits;
    m_TmpBuff = (m_TmpBuff<<m_RemainigTmpBufferBits) | (Bits >> Delta);
    *(uint32*)(m_BstrBuff + m_BuffPos) = xSwapBytes32(m_TmpBuff);  
    m_BuffPos += 4;
    m_TmpBuff = 0;

    if(NumberOfBitsToWrite - m_RemainigTmpBufferBits)
    {
      Mask >>= m_RemainigTmpBufferBits;
      m_TmpBuff = (Bits & Mask);
      m_RemainigTmpBufferBits = 32 - Delta;
    }
    else
    {      
      m_RemainigTmpBufferBits = 32;
    }
  }  
}

void xBitstreamWriter::writeByte(uint32 Byte)
{
  uint32 Mask = 0xFF;
  Byte &= Mask;

  if(m_RemainigTmpBufferBits > 8)
  {
    m_TmpBuff = (m_TmpBuff<<8) | Byte;
    m_RemainigTmpBufferBits -= 8;
  }
  else
  {
    uint32 Delta = 8 - m_RemainigTmpBufferBits;
    m_TmpBuff = (m_TmpBuff<<m_RemainigTmpBufferBits) | (Byte >> Delta);
    *(uint32*)(m_BstrBuff + m_BuffPos) = xSwapBytes32(m_TmpBuff);  
    m_BuffPos += 4;
    m_TmpBuff = 0;

    if(8 - m_RemainigTmpBufferBits)
    {
      Mask >>= m_RemainigTmpBufferBits;
      m_TmpBuff = (Byte & Mask);
      m_RemainigTmpBufferBits = 32 - Delta;
    }
    else
    {      
      m_RemainigTmpBufferBits = 32;
    }
  }  
}

void xBitstreamWriter::xWriteAlign(uint32 AlignBit)
{
  xAssert(AlignBit==0 || AlignBit==1);

  writeBits(AlignBit, 1);
  if(m_RemainigTmpBufferBits)
  {
    writeBits(0, m_RemainigTmpBufferBits & 0x07);
  }
  m_ByteAligned = true;
}

void xBitstreamWriter::flushToBuffer()
{
  xCheckAlignment();
  xAssert(m_ByteAligned);

  if(m_RemainigTmpBufferBits)
  { 
    m_TmpBuff <<= m_RemainigTmpBufferBits;
    *(uint32*)(m_BstrBuff + m_BuffPos) = xSwapBytes32(m_TmpBuff);
    m_BuffPos += (4 - (m_RemainigTmpBufferBits >> 3));
    m_RemainigTmpBufferBits = 32;      
  }
}

void xBitstreamWriter::flushToStream(std::ostream* OutStream)
{
  flushToBuffer();
  xAssert(OutStream && OutStream->good());
  OutStream->write((const char*)m_BstrBuff, m_BuffPos);  
}

void xBitstreamWriter::init()
{
  m_BuffPos = 0;
  m_TmpBuff = 0;
  m_RemainigTmpBufferBits = 32;
  m_ByteAligned = true;
}

// ====================================================================================================================
// xBitstreamReader
// ====================================================================================================================
uint32 xBitstreamReader::readBits(uint32 NumberOfBitsToRead)
{
  xAssert( NumberOfBitsToRead <= 32 );  
  uint32 Bits = 0;

  if(NumberOfBitsToRead & 0x07) m_ByteAligned = false;

  if(NumberOfBitsToRead <= m_RemainigTmpBufferBits) //reading within a buffer
  {
    Bits = m_TmpBuff>>(32-NumberOfBitsToRead);
    m_RemainigTmpBufferBits -= NumberOfBitsToRead;
    m_TmpBuff <<= NumberOfBitsToRead;
    xAssert( getBitsLeft() >= 0 );
  }
  else //reading across buffer boundary
  {
    xAssert( m_BuffPos < m_BuffSize );
    
    //read remaining bits
    uint32 Delta = NumberOfBitsToRead - m_RemainigTmpBufferBits;
    Bits = m_TmpBuff>>(32-m_RemainigTmpBufferBits);
    Bits <<= Delta;
    
    //load new from bitstream
    m_TmpBuff = xSwapBytes32(*(uint32*)(m_BstrBuff + m_BuffPos));
    m_BuffPos += 4;

    //load bits
    Bits |= m_TmpBuff>>(32-Delta);
    m_RemainigTmpBufferBits = 32 - Delta;
    m_TmpBuff <<= Delta;
  }

  return Bits;
}

uint32 xBitstreamReader::readByte()
{
  return readBits(8); //_TODO_
}

uint32 xBitstreamReader::xReadAlign()
{
  uint32 Bits = 0;
  
  uint32 RemainingBits = m_RemainigTmpBufferBits & 0x07; 
  if(RemainingBits) Bits = readBits(RemainingBits);

  m_ByteAligned = true;  
  return Bits; 
}

void xBitstreamReader::fetchFromStream(std::istream* InStream)
{
  xAssert(InStream && InStream->good());
  xAssert(m_BuffPos==0 && m_TmpBuff==0 && m_RemainigTmpBufferBits==0 && m_ByteAligned);

  InStream->seekg(0, std::istream::end);
  uint32 StreamLength = (uint32)InStream->tellg();
  InStream->seekg(0, std::istream::beg);

  InStream->read((char*)m_BstrBuff, xMin(StreamLength, m_BuffSize));
}

void xBitstreamReader::init()
{
  m_BuffPos = 0;
  m_TmpBuff = 0;
  m_RemainigTmpBufferBits = 0;
  m_ByteAligned = true;
}

} //end of namespace AVLib