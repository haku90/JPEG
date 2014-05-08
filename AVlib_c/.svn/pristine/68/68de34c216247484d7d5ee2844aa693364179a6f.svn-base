#include "xHuffman.h"

namespace AVlib {

void xHuffman::BuildCodeTable(xCmp* Cmp)
{
	//count probabilities
	uint32 Count[65536];
	::memset(Count, 0, sizeof(Count));
	int16* Ptr = Cmp->m_PelOrg;
	for(int32 y=0; y<Cmp->m_SizeY; y++)
	{
		for(int32 x=0; x<Cmp->m_SizeX; x++)
		{
			int32 Addres = (int32)Ptr[x] - (int32)int16_min;
			Count[Addres]++;
		}
		Ptr += Cmp->m_PelStride;
	}

	//insert values
	for(int32 i=0; i<65536; i++)
	{
		if(Count[i])
		{
			m_HuffmanTree.push(new xHuffTree((int16)(i+(int32)int16_min), Count[i]));
		}
	}

	xBuildHuffmanTree();

	//generate codes
	m_AvgLength = 0;
	m_Entropy = 0;
	xGenerateCode(m_HuffmanTree.top(), 0, 0);
}

void xHuffman::xBuildHuffmanTree()
{
	while(m_HuffmanTree.size() > 1)
	{
		xHuffTree* L = m_HuffmanTree.top();
		m_HuffmanTree.pop();
		xHuffTree* R = m_HuffmanTree.top();
		m_HuffmanTree.pop();

		m_HuffmanTree.push(new xHuffTree(L, R));
	}
}

void xHuffman::xGenerateCode(xHuffTree* Node, uint32 Code, int32 Length)
{
	if(Node->m_Left==nullptr && Node->m_Right==nullptr)
	{
		m_CodeTable.insert(std::pair<int16, xHuffCode>(Node->m_Value, xHuffCode(Code, Length)));

		int32 TotalCount = m_HuffmanTree.top()->m_Count;
		double Probability = (double)Node->m_Count / (double)TotalCount;
		m_AvgLength += Probability*(double)Length;
		m_Entropy -= Probability*(log(Probability)/log(2));
	}
	else
	{
		xGenerateCode(Node->m_Left,  Code<<1,     Length+1);
		xGenerateCode(Node->m_Right, (Code<<1)+1, Length+1);
	}
}




} //end of namespace AVLib