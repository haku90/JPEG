#pragma once
#ifndef _xEntropyH_
#define _xEntropyH_

#include "xLicense.h"
#include "xCommon.h"
#include "xBitstream.h"

namespace AVlib {

class xEntropy
{
public:
  //Coding
  static uint32 WriteExpGolomb          (xBitstreamWriter* BitWriter, uint32 Symbol, uint32 Order);
  static uint32 WriteGolombRice         (xBitstreamWriter* BitWriter, uint32 Symbol, uint32 Log2RiceParam); //Rice parameter is Golomb code whith order (M) is equal to 2^n, n=Log2RiceParam; for n=0 same as unary coding
  static uint32 WriteCoeffRemainingHEVC (xBitstreamWriter* BitWriter, uint32 Symbol, uint32 Log2RiceParam); //Binarization of HEVC CoeffRemaining value
  static uint32 WriteUnary              (xBitstreamWriter* BitWriter, uint32 Symbol);
  static uint32 WriteTruncatedUnary     (xBitstreamWriter* BitWriter, uint32 Symbol, uint32 MaxSymbol);
  static uint32 WriteBinary             (xBitstreamWriter* BitWriter, uint32 Symbol, uint32 Length);
  static uint32 WriteFlag               (xBitstreamWriter* BitWriter, uint32 Symbol);

  //Decoding
  static uint32 ReadExpGolomb          (xBitstreamReader* BitReader, uint32 Order);
  static uint32 ReadGolombRice         (xBitstreamReader* BitReader, uint32 Log2RiceParam);
  static uint32 ReadCoeffRemainingHEVC (xBitstreamReader* BitReader, uint32 Log2RiceParam);
  static uint32 ReadUnary              (xBitstreamReader* BitReader);
  static uint32 ReadTruncatedUnary     (xBitstreamReader* BitReader, uint32 MaxSymbol);
  static uint32 ReadBinary             (xBitstreamReader* BitReader, uint32 Symbol, uint32 Length);
  static uint32 ReadFlag               (xBitstreamReader* BitReader, uint32 Symbol);

  //Estimation - fast and precise code length estimation (for RDopt)
  static uint32 EstimateExpGolomb          (uint32 Symbol, uint32 Order)         { return (xLog2((Symbol>>Order) + 1)<<1) + 1 + Order; } //or 2*floor(log2(i+1+Order))
  static uint32 EstimateGolombRice         (uint32 Symbol, uint32 Log2RiceParam) { return (Symbol>>Log2RiceParam)+1+Log2RiceParam; }
  static uint32 EstimateCoeffRemainingHEVC (uint32 Symbol, uint32 Log2RiceParam);
  static uint32 EstimateUnary              (uint32 Symbol)                       { return Symbol+1; }
  static uint32 EstimateTruncatedUnary     (uint32 Symbol, uint32 MaxSymbol)     { return Symbol==MaxSymbol ? Symbol : Symbol+1; }

  //Helpers
  static uint32 MapSignedToUnsigned (int32  Symbol) { return (Symbol <= 0)    ? (uint32)(-Symbol<<1)    : (uint32)((Symbol<<1)-1); }
  static int32  MapUnsignedToSigned (uint32 Symbol) { return ((Symbol+1) & 1) ? -(int32)((Symbol+1)>>1) : (int32)((Symbol+1)>>1);  }
};

} //end of namespace AVLib

#endif