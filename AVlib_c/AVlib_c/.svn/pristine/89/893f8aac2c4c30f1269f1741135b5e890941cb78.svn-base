#pragma once
#ifndef _xHuffmanH_
#define _xHuffmanH_

#include "xLicense.h"
#include "xCommon.h"
#include "xBitstream.h"
#include "xCmp.h"
#include <map>
#include <queue>
#include <functional>

namespace AVlib {

class xHuffTree
{
public:
  int16 m_Value;
  int32 m_Count;

  xHuffTree* m_Left;
  xHuffTree* m_Right;

  xHuffTree(int16 Value, int32 Count) { m_Value = Value; m_Count = Count; m_Left = nullptr; m_Right = nullptr; }
  xHuffTree(xHuffTree* L, xHuffTree* R) { m_Value = 0; m_Count = L->m_Count + R->m_Count; m_Left = L; m_Right = R; }
};

class xHuffCode
{
public:
  uint32 m_Code;
  int32  m_Length;

  xHuffCode(uint32 Code, int32 Length) { m_Code = Code; m_Length = Length; }
};

struct Comparator { bool operator()(const xHuffTree* L, const xHuffTree* R) const { return L->m_Count > R->m_Count; } };

class xHuffman
{
  std::map<int16, xHuffCode> m_CodeTable;
  std::priority_queue<xHuffTree*, std::vector<xHuffTree*>, Comparator > m_HuffmanTree;

  double m_AvgLength;
  double m_Entropy;

public:
  void BuildCodeTable(xCmp* Cmp);

protected:
  void xBuildHuffmanTree();
  void xGenerateCode(xHuffTree* Node, uint32 Code, int32 Length);  
};


} //end of namespace AVLib

#endif