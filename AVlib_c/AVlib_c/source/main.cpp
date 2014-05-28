#include "xBitstream.h"
#include "xEntropy.h"
#include "xHuffman.h"
#include "xSequence.h"
#include "xCfg.h"
#include "xCmp.h"
#include "xTransform.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>


using namespace std;
using namespace AVlib;

//Bossy.
void dispMacroblock(xCmp* macroblock)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			cout << macroblock->m_Pel[i][j] << "\t";
		}
		cout << endl;
	}
}
float C(int u)
{
	if (u == 0)
		return (1.0 / sqrt(2.0));
	else
		return 1.0;
}
void DCT(xCmp* makroblock, xCmp* makroblockDCT)
{
	double tempDCT = 0.0;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			tempDCT = 0.0;
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					tempDCT += double(makroblock->m_Pel[x][y]) * cos((3.14*double(i)*(2 * double(x) + 1)) / 16.0) * cos((3.14*double(j)*(2 * double(y) + 1)) / 16.0);
				}
			}
			makroblockDCT->m_Pel[i][j] = 0.25*C(i)*C(j)*tempDCT;
		}
	}
}
void IDCT(xCmp* makroblock, xCmp* makroblockDCT)
{
	double tempIDCT;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			tempIDCT = 0.0;
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					tempIDCT += double(makroblockDCT->m_Pel[x][y]) *C(x)*C(y) * cos((3.14*double(i)*(2 * double(x) + 1)) / 16.0) * cos((3.14*double(j)*(2 * double(y) + 1)) / 16.0);
				}
			}
			makroblock->m_Pel[i][j] = 0.25*tempIDCT;
		}
	}

}
void Quant(xCmp* macroblockDCT, xCmp* macroblockQuant, xCmp* Quant)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			macroblockQuant->m_Pel[i][j] = int(round(macroblockDCT->m_Pel[i][j] / Quant->m_Pel[i][j]));
		}
	}
}
void ZygZak2RL(xCmp* macroblockQuant, int tabRL[])
{
	int x = 0, y = 0;
	int end = 0;
	int index = 0;
	//Pierwsza polowa Zygzak'a.
	for (int i = 0; i < 7; i++)
	{
		end++;
		if (end % 2 == 0)
		{
			x = end-1;
			y = 0;
		}
		else
		{
			x = 0;
			y = end-1;
		}
		for (int j = 0; j < end; j++)
		{
			tabRL[index] = macroblockQuant->m_Pel[y][x];
			//cout << x << "   " << y << endl;
			if (end % 2 == 0)
			{
				x--;
				y++;
			}
			else
			{
				x++;
				y--;
			}
			index++;
		}
	}
	//Glowna przekatna.
	x = 7;
	y = 0;
	for (int i = 0; i < 8; i++)
	{
		tabRL[index] = macroblockQuant->m_Pel[y][x];
		x--;
		y++;
		index++;
	}
	//Druga polowa ZygZak'a.
	end = 0;
	for (int i = 0; i < 7; i++)
	{
		end++;
		if ((8 - end) % 2 == 0)
		{
			x = 7;
			y = end;
		}
		else
		{
			x = end;
			y = 7;
		}
		for (int j = 0; j < (8-end); j++)
		{
			tabRL[index] = macroblockQuant->m_Pel[y][x];
			if ((8 - end) % 2 == 0)
			{
				x--;
				y++;
			}
			else
			{
				x++;
				y--;
			}
			index++;
		}
	}
}
//Bossy.

//Haku

class Huffman
{
public:

	// S³owa kodowe dla b³edu predykcji w zale¿noœci od kategori, DC 
	const static string CW_Table_DC[12];
	// D³ugoœæ s³ów kodowych, DC
	const static int  CW_Len[12];
	// Liczba kodow o danej d³ugoœci
	const static int CW_Table_DC_numOfCode[12];
	//S³owa kodowe dla AC
	const static string CW_Table_AC[16][11];
	//D³ugoœæ s³ów kodowych, AC
	const static int CW_Len_AC[16][11];
	//Liczba s³ów kodowych o danej d³ugoœci
	const static int CW_Table_AC_numOfCode[16][17];
	// kategoryzacja kodowanych wartoœci
	int categoryHuffman(int value)
	{
		int absValue = abs(value);
		if (absValue == 0)		return 0;
		if (absValue == 1)		return 1;
		if (absValue <= 3)		return 2;
		if (absValue <= 7)		return 3;
		if (absValue <= 15)		return 4;
		if (absValue <= 31)		return 5;
		if (absValue <= 63)		return 6;
		if (absValue <= 127)	return 7;
		if (absValue <= 255)	return 8;
		if (absValue <= 511)	return 9;
		if (absValue <= 1023)	return 10;
		return 11;

	}


	
	void encodeDC(int DC, string* result, int &bitLen)
	{
		int category = categoryHuffman(DC);
		bitLen = CW_Len[category];

		//strcpy(result,&CW_Table_DC[category]);
		string *temp= new string();
		*result += CW_Table_DC[category];
		int value = DC;
 		if (DC < 0)
			value += (int)pow(2, category) - 1;
		for (int i = category - 1; i >= 0; i--)
		{
			if (value % 2 == 1)
				temp->insert(temp->begin(), '1');
			else
				temp->insert(temp->begin(), '0');
			value /= 2;
		}
		*result += *temp;

}
void encodeAC(int R, int L, string* result, int &bitLen)
{
	
	int category = categoryHuffman(L);
	bitLen = CW_Len_AC[R][category];
	*result += CW_Table_AC[R][category];
	string *temp = new string();
	int value = L;
	if (L < 0)
		value += (int)pow(2, category) - 1;
	for (int i = category - 1; i >= 0; i--)
	{
		if (value % 2 == 1)
			temp->insert(temp->begin(),'1');
		else
			temp->insert(temp->begin(),'0');
		value /= 2;
	}
	*result += *temp;
}
void decodeDC(string &value,int decodeTabRL[])
{
	bool stop = true;
	string temp = "";
	int SearchCodeWord = 0;
	int j = 0;
	int NumOfCodeWord = 0;
	string DecodeCodeWord;
	int category = 0;
	int bitLen = 0;
	while (stop)
	{
		if (CW_Table_DC_numOfCode[j] != 0)
		{
			for (int i = 0; i < j - NumOfCodeWord; i++)
			{
				temp += value[i + NumOfCodeWord];

			}
			NumOfCodeWord = temp.length();
			//cout << "S³owo do zdekodowania" << endl;
			//cout << temp << endl;
		}
		for (int i = 0; i < CW_Table_DC_numOfCode[j]; i++)
		{
			if (temp == CW_Table_DC[i + SearchCodeWord])
			{
				//cout << "Found" << endl;
				//cout << CW_Table_DC[i + SearchCodeWord] << endl;
				stop = false;
				DecodeCodeWord = CW_Table_DC[i + SearchCodeWord];
				category = i + SearchCodeWord;
			}
		}
		SearchCodeWord += CW_Table_DC_numOfCode[j];
		j++;
	}
	bitLen = CW_Len[category];
	int powOf2 = 1;
	string decodeValue;
	int iDecodeValue = 0;
	int power = category - 1;
	for (int i = 1; i <= category; i++)
	{

		powOf2 = pow(2, power--);
		if (value[i + temp.length() - 1] == '1')
		{
			iDecodeValue += powOf2;

		}

	}
	
	if (value[temp.length()] == '0')
	{
		iDecodeValue -= (int)pow(2, category) - 1;
	}
	
	//cout << "wartostsc zdekodowana" << endl;
	//cout << iDecodeValue << endl;
	value.erase(value.begin(), value.begin() + category+temp.length());
	//cout << "new value" << endl;
	//cout << value << endl;
	decodeTabRL[0] = iDecodeValue;
}
void decodeAC(string &value,int decodeTabRL[])
{
	int numOfIter = 1;
	bool EOB = true;
	//	for (int k = 0; k < 2; k++)
	//{
	
	while (EOB)
	{
		bool stop = true;
		string temp = "";
		int SearchCodeWord = 1;
		int j = 0;
		int k = 0;
		int NumOfCodeWord = 0;
		string DecodeCodeWord;
		int category = 0;
		int bitLen = 0;
		bool notToWrite = true;
		while (stop)
		{
			while (k < 17 && stop)
			{
				while (j < 16 && stop)
				{

					if (CW_Table_AC_numOfCode[j][k] != 0)
					{
						for (int i = 0; i < k - NumOfCodeWord; i++)
						{
							temp += value[i + NumOfCodeWord];
						}
						NumOfCodeWord = temp.length();
						

						for (int i = 0; i < k; i++)
						{
							SearchCodeWord += CW_Table_AC_numOfCode[j][i];

						}
						if (j == 0 && k == 4 && temp == "1010")
						{
							stop = false;
							EOB = false;
							decodeTabRL[numOfIter++] = 0;
							decodeTabRL[numOfIter++] = 0;
							value.erase(value.begin(), value.begin() + category + temp.size());
							return;
						}
						
					
						for (int i = 0; i < CW_Table_AC_numOfCode[j][k]; i++)
						{
							/*
							cout << "wartosc do porownania" << endl;
							cout << temp << endl;
							cout << "wartosc z tablicy huffmana" << endl;
							cout << CW_Table_AC[j][i + SearchCodeWord] << endl;
							*/
							if (temp == CW_Table_AC[j][i + SearchCodeWord])
							{
								DecodeCodeWord = CW_Table_AC[j][i + SearchCodeWord];
								category = i + SearchCodeWord;
								decodeTabRL[numOfIter++] = j;
								stop = false;
								
							}
						}
						if (temp.size() == 11 && temp == "11111111001")
						{
							//cout << "F/0 ZRL" << endl;
							decodeTabRL[numOfIter++] = 15;
							decodeTabRL[numOfIter++] = 0;
							value.erase(value.begin(), value.begin() + category + temp.size());
							stop = false;
							notToWrite = false;
						}
					}
					j++;
					SearchCodeWord = 1;
				
				}
				
				j = 0;
				k++;
			}


		}
		if (notToWrite)
		{
			bitLen = CW_Len[category];
			int powOf2 = 1;
			string decodeValue;
			int iDecodeValue = 0;
			/*
			for (int i = 0; i < category; i++)
			{
			decodeValue += value[NumOfCodeWord + i];
			}
			cout << "wartosc do zdekodownaia" << endl;
			cout << decodeValue << endl;
			*/
			int power = category - 1;
			for (int i = 1; i <= category; i++)
			{

				powOf2 = pow(2, power--);
				if (value[i + temp.length() - 1] == '1')
				{
					iDecodeValue += powOf2;

				}

			}

			if (value[temp.length()] == '0')
			{
				iDecodeValue -= (int)pow(2, category) - 1;
			}

			//cout << "wartostsc zdekodowana" << endl;
			//cout << iDecodeValue << endl;
			value.erase(value.begin(), value.begin() + category + temp.length());
			//cout << "new value" << endl;
			//cout << value << endl;

			decodeTabRL[numOfIter++] = iDecodeValue;
		}
		}
		//}
	}

};
const string Huffman::CW_Table_DC[12] = { "00", "010", "011", "100", "101", "110", "1110", "11110", "111110",
"1111110", "11111110", "111111110" };
const int Huffman::CW_Len[12] = { 2, 4, 5, 6, 7, 8, 10, 12, 14, 16, 18, 20 };
const int Huffman::CW_Table_DC_numOfCode[12] = { 0, 0, 1, 5, 1, 1, 1, 1, 1, 1 };
const string Huffman::CW_Table_AC[16][11] = {
	"1010", "00", "01", "100", "1011", "11010", "1111000", "11111000", "1111110110", "1111111110000010", "1111111110000011",
	"", "1100", "11011", "1111001", "111110110", "11111110110", "1111111110000100", "1111111110000101", "1111111110000110", "1111111110000111", "1111111110001000",
	"", "11100", "11111001", "1111110111", "111111110100", "1111111110001001", "1111111110001010", "1111111110001011", "1111111110001100", "1111111110001101", "1111111110001110",
	"", "111010", "111110111", "111111110101", "1111111110001111", "1111111110010000", "1111111110010001", "1111111110010010", "1111111110010011", "1111111110010100", "1111111110010101",
	"", "111011", "1111111000", "1111111110010110", "1111111110010111", "1111111110011000", "1111111110011001", "1111111110011010", "1111111110011011", "1111111110011100", "1111111110011101",
	"", "1111010", "11111110111", "1111111110011110", "1111111110011111", "1111111110100000", "1111111110100001", "1111111110100010", "1111111110100011", "1111111110100100", "1111111110100101",
	"", "1111011", "111111110110", "1111111110100110", "1111111110100111", "1111111110101000", "1111111110101001", "1111111110101010", "1111111110101011", "1111111110101100", "1111111110101101",
	"", "11111010", "111111110111", "1111111110101110", "1111111110101111", "1111111110110000", "1111111110110001", "1111111110110010", "1111111110110011", "1111111110110100", "1111111110110101",
	"", "111111000", "111111111000000", "1111111110110110", "1111111110110111", "1111111110111000", "1111111110111001", "1111111110111010", "1111111110111011", "1111111110111100", "1111111110111101",
	"", "111111001", "1111111110111110", "1111111110111111", "1111111111000000", "1111111111000001", "1111111111000010", "1111111111000011", "1111111111000100", "1111111111000101", "1111111111000110",
	"", "111111010", "1111111111000111", "1111111111001000", "1111111111001001", "1111111111001010", "1111111111001011", "1111111111001100", "1111111111001101", "1111111111001110", "1111111111001111",
	"", "1111111001", "1111111111010000", "1111111111010001", "1111111111010010", "1111111111010011", "1111111111010100", "1111111111010101", "1111111111010110", "1111111111010111", "1111111111011000",
	"", "1111111010", "1111111111011001", "1111111111011010", "1111111111011011", "1111111111011100", "1111111111011101", "1111111111011110", "1111111111011111", "1111111111100000", "1111111111100001",
	"", "11111111000", "1111111111100010", "1111111111100011", "1111111111100100", "1111111111100101", "1111111111100110", "1111111111100111", "1111111111101000", "1111111111101001", "1111111111101010",
	"", "1111111111101011", "1111111111101100", "1111111111101101", "1111111111101110", "1111111111101111", "1111111111110000", "1111111111110001", "1111111111110010", "1111111111110011", "1111111111110100",
	"11111111001", "1111111111110101", "1111111111110110", "1111111111110111", "1111111111111000", "1111111111111001", "1111111111111010", "1111111111111011", "1111111111111100", "1111111111111101", "1111111111111110"
};
const int Huffman::CW_Len_AC[16][11] = {
	4, 3, 4, 6, 8, 10, 13, 15, 18, 25, 26,
	0, 5, 7, 10, 13, 16, 22, 23, 24, 25, 26,
	0, 6, 10, 13, 16, 21, 22, 23, 24, 25, 26,
	0, 7, 11, 15, 20, 21, 22, 23, 24, 25, 26,
	0, 7, 12, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 8, 13, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 8, 14, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 9, 14, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 10, 17, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 10, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 10, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 11, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 11, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 12, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	0, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	11, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26
};
	const int Huffman:: CW_Table_AC_numOfCode[16][17] = {
	
		0, 0, 2, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 2,
		0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 5,
		0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 6,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 7,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 8,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 8,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10

	};

int RL(int ZygZak[], int RL[])
{
	RL[0] = ZygZak[0]; // DC
	int numOfZero = 0;
	int numOfData = 1; // 1 ze wzgledu na wspolczynnik DC
	int i = 1;
	while(i<64)
	{
		numOfZero = 0;
		// numOfZero<15 poniewaz R zapis na 4 bitach 
		while ((i < 64) && (ZygZak[i] == 0) && (numOfZero<15)) 
		{
			i++;
			numOfZero++;
		}
		//EOB
		if (i == 64)
		{
			RL[numOfData++] = 0;
			RL[numOfData++] = 0;
		}
		//Zapis R L
		else
		{
			RL[numOfData++] = numOfZero;
			RL[numOfData++] = ZygZak[i++];
		}
	}
	//EOB
	if (!(RL[numOfData - 1] == 0 && RL[numOfData - 2] == 0))
	{
		RL[numOfData++] = 0;
		RL[numOfData++] = 0;
	}
	if ((RL[numOfData - 4] == 15) && (RL[numOfData - 3] == 0))
	{
		RL[numOfData - 4] = 0;
		numOfData -= 2;
	}

	return numOfData;
}
//Haku



int wmain( int argc, wchar_t *argv[ ], wchar_t *envp[ ])
{
  printf("Start....\n");
  x_sys_print_compiler();
  xCpuInfo CpuInfo;
  CpuInfo.checkCPIUD();
  CpuInfo.printCpuInfo();
  CpuInfo.checkCompatibility();

  /*
  Bossy.
  Wejscie:
  Obraz w formacie .bmp o rozmiarach podzielnych przez 8.
  */

  //Wczytywanie pliku i sprawdzenie wymiarow. 
  FILE_HANDLE InputFile = x_fopen(TEXT("samochody_1920x1080_0538.bmp"), TEXT("r"));
  xImg* InputImage = new xImg;
  InputImage->getBMP(InputFile);

  int SizeX = InputImage->m_Cmp[0]->m_SizeX;
  int SizeY = InputImage->m_Cmp[0]->m_SizeY;

  if (SizeX % 8 || SizeY % 8)
  {
	  cout << "Error";
	  return 0;
  }

  //Konwersja z RGB na YUV.
  xImg* ImageYUV = new xImg;
  ImageYUV->create(SizeX, SizeY, 0, 8, ImgTp_YUV, CrF_444, ClrSpc_BT709, 0);
  ImageYUV->convertRGB2YUV(InputImage);

  //Pozbywanie sie wartosci sredniej.
  for (int i = 0; i < SizeX; i++)
  {
	  for (int j = 0; j < SizeY; j++)
	  {
		  ImageYUV->m_Cmp[0]->m_Pel[j][i] -= 128;
		  ImageYUV->m_Cmp[1]->m_Pel[j][i] -= 128;
		  ImageYUV->m_Cmp[2]->m_Pel[j][i] -= 128;
	  }
  }

  //Tworzenie makroblokow 8x8: obraz, po DCT, po kwantyzacji.
  //Luma
  xCmp* macroblock = new xCmp;
  xCmp* macroblockDCT = new xCmp;
  xCmp* macroblockQuant = new xCmp;
  macroblock->create(8, 8, 0, 8);
  macroblockDCT->create(8, 8, 0, 11);
  macroblockQuant->create(8, 8, 0, 11);

  //cb
  xCmp* cb_macroblock = new xCmp;
  xCmp* cb_macroblockDCT = new xCmp;
  xCmp* cb_macroblockQuant = new xCmp;
  cb_macroblock->create(8, 8, 0, 8);
  cb_macroblockDCT->create(8, 8, 0, 11);
  cb_macroblockQuant->create(8, 8, 0, 11);

  //cr
  xCmp* cr_macroblock = new xCmp;
  xCmp* cr_macroblockDCT = new xCmp;
  xCmp* cr_macroblockQuant = new xCmp;
  cr_macroblock->create(8, 8, 0, 8);
  cr_macroblockDCT->create(8, 8, 0, 11);
  cr_macroblockQuant->create(8, 8, 0, 11);

  //Tablice kwantyzacji, pobieranie z pliku.
  xCmp* quantLuma = new xCmp();
  xCmp* quantChroma = new xCmp();

  quantLuma->create(8, 8, 0, 8);
  quantChroma->create(8, 8, 0, 8);

  ifstream fileQuantLuma;
  ifstream fileQuantChroma;
  fileQuantLuma.open("QuantLuma.txt", ios::binary);
  fileQuantChroma.open("QuantChroma.txt", ios::binary);

  int tempQuant;
  for (int i = 0; i < 8; i++)
  {
	  for (int j = 0; j < 8; j++)
	  {
		  fileQuantLuma >> tempQuant;
		  quantLuma->m_Pel[i][j] = tempQuant;
		  fileQuantChroma >> tempQuant;
		  quantChroma->m_Pel[i][j] = tempQuant;
	  }
  }
  
  int prevDC = 0;
  int prevDC_cb = 0;
  int prevDC_cr = 0;

  int tempPrevDC = 0;
  int tempPrevDC_cb = 0;
  int tempPrevDC_cr = 0;
  Huffman* huff = new Huffman();
  //Podzial obrazu na makrobloki 8x8.
  for (int y = 0; y < SizeY/8; y++)
  {
	  for (int x = 0; x < SizeX/8; x++)
	  {
		  for (int i = 8*y; i < (8*y)+8; i++)
		  {
			  for (int j = 8*x; j < (8*x)+8; j++)
			  {
				  macroblock->m_Pel[i % 8][j % 8] = ImageYUV->m_Cmp[0]->m_Pel[i][j];
				  cb_macroblock->m_Pel[i % 8][j % 8] = ImageYUV->m_Cmp[1]->m_Pel[i][j];
				  cr_macroblock->m_Pel[i % 8][j % 8] = ImageYUV->m_Cmp[2]->m_Pel[i][j];
			  }
		  }
		  //Wyliczanie DCT dla kazdego makrobloku, dla kazdej skladowej.
		  DCT(macroblock, macroblockDCT);
		  DCT(cb_macroblock, cb_macroblockDCT);
		  DCT(cr_macroblock, cr_macroblockDCT);

		  //Kwantowanie.
		  Quant(macroblockDCT, macroblockQuant, quantLuma);
		  Quant(cb_macroblockDCT, cb_macroblockQuant, quantChroma);
		  Quant(cr_macroblockDCT, cr_macroblockQuant, quantChroma);
			
		  //ZigZag.
		  int tabRL[64];
		  int cb_tabRL[64];
		  int cr_tabRL[64];
		  ZygZak2RL(macroblockQuant, tabRL);
		  ZygZak2RL(cb_macroblockQuant, cb_tabRL);
		  ZygZak2RL(cr_macroblockQuant, cr_tabRL);
		  /*
		  Bossy.
		  Wyjscie:
		  Tablcia int 64 - elementowa dla kazdego makrobloku i dla kazdej skladowej.
		  Uporzadowanie zygzakowate zgodne z rys. 7.3. na stronie 356 w ksiazce "Obraz cyfrowy"
		  */

		  //RL
		  int tabRL2[64 * 2] = { 0 };
		  int tabRL2_cb[64 * 2] = { 0 };
		  int tabRL2_cr[64 * 2] = { 0 };
		  int numOfData, numOfDataCB, numOfDataCR;

		  numOfData = RL(tabRL, tabRL2);
		  numOfDataCB = RL(cb_tabRL, tabRL2_cb);
		  numOfDataCR = RL(cr_tabRL, tabRL2_cr);

		  //Kodowanie DC ró¿nicowe
		  tempPrevDC = tabRL2[0];
		  tabRL2[0] =tabRL2[0] - prevDC;
		  prevDC = tempPrevDC;

		  tempPrevDC_cb = tabRL2_cb[0];
		  tabRL2_cb[0] = tabRL2_cb[0] - prevDC_cb;
		  prevDC_cb = tempPrevDC_cb;

		  tempPrevDC_cr = tabRL2_cr[0];
		  tabRL2_cr[0] = tabRL2_cr[0] - prevDC_cr;
		  prevDC_cr = tempPrevDC_cr;

		  //Kodowanie Huffman 
		  //DC luma
		  int bitLen = 0;
		  string* encodeData=new string();
		  huff->encodeDC(tabRL2[0], encodeData,bitLen);
		  //AC luma
		  for (int i = 1; i < numOfData; i += 2)
		  {
 			  huff->encodeAC(tabRL2[i], tabRL2[i + 1], encodeData, bitLen);
		  }
		 
		  //DC CB
		  huff->encodeDC(tabRL2_cb[0], encodeData, bitLen);
		  //AC CB
		  for (int i = 1; i < numOfDataCB; i += 2)
		  {
			  huff->encodeAC(tabRL2_cb[i], tabRL2_cb[i + 1], encodeData, bitLen);
		  }
		 
		  //DC CR
		  huff->encodeDC(tabRL2_cr[0], encodeData, bitLen);
		  //AC CR
		  for (int i = 1; i < numOfDataCR; i += 2)
		  {
			  huff->encodeAC(tabRL2_cr[i], tabRL2_cr[i + 1], encodeData, bitLen);
		  }
		//Dekodowanie Huffman
		  int DecodetabRL2[64 * 2] = { 0 };
		  int DecodetabRL2_cb[64 * 2] = { 0 };
		  int DecodetabRL2_cr[64 * 2] = { 0 };
		//DC Luma
		  if (encodeData->size() != 0)
		  huff->decodeDC(*encodeData,DecodetabRL2);
		//AC Luma
		  if (encodeData->size() != 0)
		  huff->decodeAC(*encodeData, DecodetabRL2);
		//DC CB
		  if (encodeData->size()!=0)
		  huff->decodeDC(*encodeData, DecodetabRL2_cb);
		//AC CB
		  if (encodeData->size() != 0)
		  huff->decodeAC(*encodeData, DecodetabRL2_cb);
		//DC CR
		  if (encodeData->size() != 0)
		  huff->decodeDC(*encodeData, DecodetabRL2_cr);
		//AC CR
		  if (encodeData->size() != 0)
		  huff->decodeAC(*encodeData, DecodetabRL2_cr);
		 //D_RL
		  
	  }
  }



  system("pause");
  return EXIT_SUCCESS;
}

