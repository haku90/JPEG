#include "xEntropy.h"

namespace AVlib {

uint32 xEntropy::WriteExpGolomb(xBitstreamWriter* BitWriter, uint32 Symbol, uint32 Order)
{
  uint32 Code = 0;
  uint32 Length = 0;
  uint32 Count = Order;

  while(Symbol >= (uint32)(1<<Count))
  {
    Code = (Code<<1) + 1;
    Length++;
    Symbol -= 1<<Count;
    Count++;
  }

  Code = (Code<<1);
  Length++;

  Code = (Code<<Count) | Symbol;
  Length += Count;

  BitWriter->writeBits(Code, Length);

  return Length;
}
uint32 xEntropy::WriteGolombRice(xBitstreamWriter* BitWriter, uint32 Symbol, uint32 Log2RiceParam) 
{
  //unary code
  uint32 UnaryLength = (Symbol>>Log2RiceParam)+1;
  uint32 Length = UnaryLength;
  uint32 Code   = (1<<UnaryLength)-2;

  //truncated binary code
  Length += Log2RiceParam;
  Code <<= Log2RiceParam;
  Code += Symbol - ((Symbol>>Log2RiceParam)<<Log2RiceParam); 

  BitWriter->writeBits(Code, Length);

  return Length;
}
uint32 xEntropy::WriteCoeffRemainingHEVC(xBitstreamWriter* BitWriter, uint32 Symbol, uint32 Log2RiceParam)
{
  const uint32 CoeffRemainingBinReduction = 3;
  uint32 CodeLength;
  if (Symbol < (CoeffRemainingBinReduction << Log2RiceParam)) //Golomb-Rice
  {
    CodeLength = Symbol>>Log2RiceParam;
    BitWriter->writeBits( (1<<(CodeLength+1))-2 , CodeLength+1);
    BitWriter->writeBits(Symbol & ((1<<Log2RiceParam)-1), Log2RiceParam);
    CodeLength = CodeLength + 1 + Log2RiceParam;
  }
  else //Truncated Unary + Exp Golomb
  {
    CodeLength = Log2RiceParam;
    Symbol  = Symbol - ( CoeffRemainingBinReduction << Log2RiceParam);    
    while (Symbol >= (uint32)(1<<CodeLength))
    {
      Symbol -=  (1<<(CodeLength++));    
    }
    BitWriter->writeBits((1<<(CoeffRemainingBinReduction+CodeLength+1-Log2RiceParam))-2,CoeffRemainingBinReduction+CodeLength+1-Log2RiceParam);
    BitWriter->writeBits(Symbol,CodeLength);
    CodeLength = CoeffRemainingBinReduction+(CodeLength<<1)+1-Log2RiceParam;
  }
  return CodeLength;
}
uint32 xEntropy::WriteUnary(xBitstreamWriter* BitWriter, uint32 Symbol)
{
  uint32 Length = Symbol+1;
  uint32 Code = (1<<Length)-2;

  BitWriter->writeBits(Code, Length);

  return Length;
}
uint32 xEntropy::WriteTruncatedUnary(xBitstreamWriter* BitWriter, uint32 Symbol, uint32 MaxSymbol)
{
  uint32 Length = Symbol+1;
  uint32 Code = (1<<Length)-2;

  if(Symbol==MaxSymbol)
  {
    Length -=1;
    Code >>= 1;
  }

  BitWriter->writeBits(Code, Length);

  return Length;
}
uint32 xEntropy::WriteBinary(xBitstreamWriter* BitWriter, uint32 Symbol, uint32 Length)
{
  xAssert(Length > 0);
  BitWriter->writeBits(Symbol, Length);
  return Length;
}
uint32 xEntropy::WriteFlag(xBitstreamWriter* BitWriter, uint32 Symbol)
{
  xAssert (Symbol==0 || Symbol==1);
  BitWriter->writeBits(Symbol, 1);
  return 1;
}

//================================================================================================

uint32 xEntropy::ReadExpGolomb(xBitstreamReader* BitReader, uint32 Order)
{
  uint32 Symbol = 0;
  uint32 ReadBit = 1;
  uint32 Count = Order;
  
  while(ReadBit)
  {
    ReadBit = BitReader->readBits(1);    
    Symbol += ReadBit << Count;
    Count++;
  }

  Count--;

  if(Count)
  {
    ReadBit = BitReader->readBits(Count);
    Symbol += ReadBit;
  }
  
  return Symbol;
}
uint32 xEntropy::ReadGolombRice(xBitstreamReader* BitReader, uint32 Log2RiceParam)
{
  uint32 Prefix = 0;
  uint32 CodeWord = 0;
  do
  {
    Prefix++;
    CodeWord = BitReader->readBits(1);
  }
  while(CodeWord);
  CodeWord  = 1 - CodeWord;
  Prefix -= CodeWord;
  CodeWord = BitReader->readBits(Log2RiceParam);
  return (Prefix<<Log2RiceParam) + CodeWord;
}
uint32 xEntropy::ReadCoeffRemainingHEVC(xBitstreamReader* BitReader, uint32 Log2RiceParam)
{
  const uint32 CoeffRemainingBinReduction = 3;
  uint32 Prefix = 0;
  uint32 codeWord = 0;
  do
  {
    Prefix++;
    codeWord = BitReader->readBits(1);
  }
  while(codeWord);
  codeWord  = 1 - codeWord;
  Prefix -= codeWord;
  codeWord=0;
  if (Prefix < CoeffRemainingBinReduction) //Golomb-Rice path
  {
    codeWord = BitReader->readBits(Log2RiceParam);
    return (Prefix<<Log2RiceParam) + codeWord;
  }
  else //Truncated Unary + Exp Golomb path
  {
    codeWord = BitReader->readBits(Prefix-CoeffRemainingBinReduction+Log2RiceParam);
    return (((1<<(Prefix-CoeffRemainingBinReduction))+CoeffRemainingBinReduction-1)<<Log2RiceParam)+codeWord;
  }
}
uint32 xEntropy::ReadUnary(xBitstreamReader* BitReader)
{
  uint32 Symbol=0;
  uint32 ReadBit;
  do
  {
    ReadBit = BitReader->readBits(1);
    Symbol++;
  }
  while(ReadBit);

  return Symbol-1;
}
uint32 xEntropy::ReadTruncatedUnary(xBitstreamReader* BitReader, uint32 MaxSymbol)
{
  if(MaxSymbol==0) { return 0; }

  uint32 Symbol = 0;
  uint32 ReadBit;
  do
  {
    ReadBit = BitReader->readBits(1);
    Symbol++;
  }
  while(ReadBit && (Symbol < MaxSymbol - 1));
  
  if(ReadBit && (Symbol == MaxSymbol - 1)) { Symbol++; }
  
  return Symbol;
}
uint32 xEntropy::ReadBinary(xBitstreamReader* BitReader, uint32 Symbol, uint32 Length)
{
  xAssert(Length > 0);
  return BitReader->readBits(Length);
}
uint32 xEntropy::ReadFlag(xBitstreamReader* BitReader, uint32 Symbol)
{
  return BitReader->readBits(1);
}

//================================================================================================

uint32 xEntropy::EstimateCoeffRemainingHEVC(uint32 Symbol, uint32 Log2RiceParam)
{
  const uint32 CoeffRemainingBinReduction = 3;
  int32 CodeLength;
  if (Symbol < (CoeffRemainingBinReduction << Log2RiceParam)) //Golomb-Rice
  {
    CodeLength = (Symbol>>Log2RiceParam)+1+Log2RiceParam;
  }
  else //Truncated Unary + Exp Golomb
  {
    CodeLength = 4+Log2RiceParam+((xLog2((Symbol>>Log2RiceParam)-2))<<1);   
  }
  return CodeLength;
}

} //end of namespace AVLib