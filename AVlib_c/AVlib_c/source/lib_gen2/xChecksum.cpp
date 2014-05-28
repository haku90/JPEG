#include "xChecksum.h"

namespace AVlib {

uint32 xChecksum::calcCRC32C(const uint8* Src, uint32 Length)
{
#if (X_USE_SSE && X_SSE4_2)
  uint32 CRC32C = 0;
  uint32 Length64b = Length & (uint32)0xFFFFFFF8;
  for (uint32 i=0;         i<Length64b; i+=8) { CRC32C = (uint32)_mm_crc32_u64(CRC32C, *((uint64*)(Src+i))); }
  for (uint32 i=Length64b; i<Length;    i+=1) { CRC32C = (uint32)_mm_crc32_u8 (CRC32C, *((uint8 *)(Src+i))); }
  return CRC32C;
#else
  uint32 CRC32C = 0;
  for(int32 i=0; i<Length; i++)
  {
    CRC32C ^= Src[i];
    for (int32 i=0; i<8; i++)
    {
      CRC32C = (CRC32C >> 1) ^ ((CRC32C & 1) ? 0x82F63B78 : 0);
    }
  }
  return CRC32C;
#endif
}

} //end of namespace AVLib