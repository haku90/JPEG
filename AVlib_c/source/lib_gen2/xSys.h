#pragma once
#ifndef _xSysH_
#define _xSysH_

#include "xLicense.h"
#include "xType.h"
#include "xLanguage.h"
#include "xDefines.h"
#include <windows.h>
#include <stdio.h>

//================================================================================================
//Clasic main():
//main( int argc, char *argv[ ], char *envp[ ] )
//
//UNICODE main():
//wmain( int argc, wchar_t *argv[ ], wchar_t *envp[ ] )
//================================================================================================

//================================================================================================
// SYS
//================================================================================================
#define X_SEEK_CUR    1
#define X_SEEK_END    2
#define X_SEEK_BEGIN  0
#define X_SEEK_SET    X_SEEK_BEGIN

FILE_HANDLE    x_fopen       (xchar* name, xchar* attr);
uint32         x_fwrite      (FILE_HANDLE hfile, void* buff, uint32 len);
uint32         x_fread       (FILE_HANDLE hfile, void* buff, uint32 len);
uint32         x_fwrite      (FILE_HANDLE file_handle, void* buf, uint32 len);
uint32         x_fseek       (FILE_HANDLE file_handle, int64 pos, uint32 origin);
uint64         x_fsize       (FILE_HANDLE file_handle);
uint32         x_eof         (FILE_HANDLE file_handle);
uint64         x_ftell       (FILE_HANDLE hfile);
uint32         x_fskip       (FILE_HANDLE hfile, int64 pos);
FILE_HANDLE    x_fclose      (FILE_HANDLE hfile);

int32          argi          (int argc, xchar *argv[], xchar* header, int deft);
float          argf          (int argc, xchar *argv[], xchar* header, float deft);
xchar*         args          (int argc, xchar *argv[], xchar* header, xchar* deft);
int32          argt          (int argc, xchar *argv[], xchar* header);
int            argl          (int argc, xchar *argv[], xchar* header, xchar* label);

#define _ARGI(h,d)    argi(argc,argv,(h),(d)) 
#define _ARGF(h,d)    argf(argc,argv,(h),(d)) 
#define _ARGS(h,d)    args(argc,argv,(h),(d)) 
#define _ARGT(h)      argt(argc,argv,(h))
#define _ARGL(h,d)    argl(argc,argv,(h),(d))

void x_fprinterror();
void x_sys_print_compiler();

//================================================================================================
// CPU
//================================================================================================

class xCpuInfo
{
protected:
  char  m_VendorID[13];

  uint8 m_FPU;     //Floating Point Unit (integrated since 80486)
  uint8 m_MMX;
  uint8 m_SSE1;
  uint8 m_SSE2;
  uint8 m_SSE3;
  uint8 m_SSSE3;   //SupplementalSSE3
  uint8 m_SSE4_1;
  uint8 m_SSE4_2;
  uint8 m_AVX1;
  uint8 m_AVX2;
  uint8 m_AVX512CD;
  uint8 m_AVX512ER;
  uint8 m_AVX512PF;
  uint8 m_FMA;     //Fused multiple-add
  uint8 m_BMI1;    //Bit Manipulation Instructions 1
  uint8 m_BMI2;    //Bit Manipulation Instructions 2
  uint8 m_CMOV;    //Conditional move
  uint8 m_POPCNT;  //Population count instruction
  uint8 m_LZCNT;   //Leading zero count instruction
  uint8 m_AES;     //EAS encryption accelerator
  uint8 m_SHA;
  uint8 m_FP16C;   //Half float convertion
  uint8 m_RDRAND;  //SP 800-90A - Cryptographically secure pseudorandom number generator
  uint8 m_RDSEED;  //SP 800-90B & C - Non-deterministic random bit generator
  uint8 m_TSX;     //TSX=RTM+HLE
  uint8 m_MOVBE;   //Load and Store of Big Endian forms
  uint8 m_INVPCID; //Invalidate processor context ID  
  uint8 m_ADX;     //ADOX, ADCX, MULX arbitrary-precision integer operations
  uint8 m_HT;      //Hyperthreading;

public:
  void  checkCPIUD();
  void  printCpuInfo();
  int32 checkCompatibility();
};


 
#endif //_xSysH_