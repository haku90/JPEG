#include "xSys.h"
#include "xConsole.h"

//================================================================================================
// Compiler
//================================================================================================
void x_sys_print_compiler()
{
  x_fprintf(stdout, TEXT("Compiler: %s  version: %d\n"), X_COMPILER_NAME, X_COMPILER_VER);
}

//================================================================================================
// CPU
//================================================================================================
#define regEAX 0
#define regEBX 1
#define regECX 2
#define regEDX 3

void xCpuInfo::checkCPIUD()
{
  // http://www.sandpile.org/x86/cpuid.htm
  int32 MaxSupportedStandardLevel;
  int32 CPUInfo[4]; //[0] =EAX, [1]=EBX, [2]=ECX, [3]=EDX  
  memset(this, 0, sizeof(xCpuInfo));

  //StandardLevel = 0
  __cpuid(CPUInfo, 0);
  MaxSupportedStandardLevel = CPUInfo[0];
  memcpy(m_VendorID, &CPUInfo[1], 3*sizeof(int32));

  //StandardLevel = 1
  if(MaxSupportedStandardLevel>=1)
  {
    __cpuid(CPUInfo, 1);
    m_FPU      = (CPUInfo[regEDX] & (1<<0 )) != 0;
    m_MMX      = (CPUInfo[regEDX] & (1<<23)) != 0;
    m_SSE1     = (CPUInfo[regEDX] & (1<<25)) != 0;
    m_SSE2     = (CPUInfo[regEDX] & (1<<26)) != 0;
    m_SSE3     = (CPUInfo[regECX] & (1<<0 )) != 0;
    m_SSSE3    = (CPUInfo[regECX] & (1<<9 )) != 0;
    m_SSE4_1   = (CPUInfo[regECX] & (1<<19)) != 0;
    m_SSE4_2   = (CPUInfo[regECX] & (1<<20)) != 0;
    m_AVX1     = (CPUInfo[regECX] & (1<<28)) != 0;
    m_CMOV     = (CPUInfo[regEDX] & (1<<15)) != 0;
    m_POPCNT   = (CPUInfo[regECX] & (1<<23)) != 0;
    m_LZCNT    = (CPUInfo[regECX] & (1<< 5)) != 0;
    m_AES      = (CPUInfo[regECX] & (1<<25)) != 0;
    m_FP16C    = (CPUInfo[regECX] & (1<<29)) != 0;
    m_FMA      = (CPUInfo[regECX] & (1<<12)) != 0;
    m_HT       = (CPUInfo[regEDX] & (1<<28)) != 0;
    m_RDRAND   = (CPUInfo[regECX] & (1<<30)) != 0;
  }

  //StandardLevel = 7
  if(MaxSupportedStandardLevel>=7)
  {
    __cpuid(CPUInfo, 7);
    m_AVX2     = (CPUInfo[regEBX] & (1<<5 )) != 0;
    m_AVX512CD = (CPUInfo[regEBX] & (1<<28)) != 0;
    m_AVX512ER = (CPUInfo[regEBX] & (1<<27)) != 0;
    m_AVX512PF = (CPUInfo[regEBX] & (1<<26)) != 0;
    m_BMI1     = (CPUInfo[regEBX] & (1<<3 )) != 0;
    m_BMI2     = (CPUInfo[regEBX] & (1<<8 )) != 0;
    m_SHA      = (CPUInfo[regEBX] & (1<<29)) != 0;
    m_RDSEED   = (CPUInfo[regEBX] & (1<<18)) != 0;
    m_ADX      = (CPUInfo[regEBX] & (1<<19)) != 0;
  }
}

void xCpuInfo::printCpuInfo()
{
  //available
  x_fprintf(stdout, TEXT("Detected CPU feature list:\n"));
  x_fprintf(stdout, TEXT("Available extensions:     "));
  if(m_FPU     ) x_fprintf(stdout, TEXT("FPU "));
  if(m_MMX     ) x_fprintf(stdout, TEXT("MMX "));
  if(m_SSE1    ) x_fprintf(stdout, TEXT("SSE1 "));
  if(m_SSE2    ) x_fprintf(stdout, TEXT("SSE2 "));
  if(m_SSE3    ) x_fprintf(stdout, TEXT("SSE3 "));
  if(m_SSSE3   ) x_fprintf(stdout, TEXT("SSSE3 "));
  if(m_SSE4_1  ) x_fprintf(stdout, TEXT("SSE4.1 "));
  if(m_SSE4_2  ) x_fprintf(stdout, TEXT("SSE4.2 "));
  if(m_AVX1    ) x_fprintf(stdout, TEXT("AVX1 "));
  if(m_AVX2    ) x_fprintf(stdout, TEXT("AVX2 "));
  if(m_AVX512CD) x_fprintf(stdout, TEXT("AVX512CD "));
  if(m_AVX512ER) x_fprintf(stdout, TEXT("AVX512ER "));
  if(m_AVX512PF) x_fprintf(stdout, TEXT("AVX512PF "));
  if(m_FMA     ) x_fprintf(stdout, TEXT("FMA "));
  if(m_BMI1    ) x_fprintf(stdout, TEXT("BMI1 "));
  if(m_BMI2    ) x_fprintf(stdout, TEXT("BMI2 "));
  if(m_CMOV    ) x_fprintf(stdout, TEXT("CMOV "));
  if(m_POPCNT  ) x_fprintf(stdout, TEXT("POPCNT "));
  if(m_LZCNT   ) x_fprintf(stdout, TEXT("LZCNT "));
  if(m_AES     ) x_fprintf(stdout, TEXT("AES "));
  if(m_SHA     ) x_fprintf(stdout, TEXT("SHA "));
  if(m_FP16C   ) x_fprintf(stdout, TEXT("FP16C "));  
  if(m_RDRAND  ) x_fprintf(stdout, TEXT("RDRAND "));
  if(m_RDSEED  ) x_fprintf(stdout, TEXT("RDSEED "));  
  if(m_HT      ) x_fprintf(stdout, TEXT("HyperThreading ")); 
  x_fprintf(stdout, TEXT("\n"));

  //not available
  x_fprintf(stdout, TEXT("Not available extensions: "));
  if(!m_FPU     ) x_fprintf(stdout, TEXT("FPU "));
  if(!m_MMX     ) x_fprintf(stdout, TEXT("MMX "));
  if(!m_SSE1    ) x_fprintf(stdout, TEXT("SSE1 "));
  if(!m_SSE2    ) x_fprintf(stdout, TEXT("SSE2 "));
  if(!m_SSE3    ) x_fprintf(stdout, TEXT("SSE3 "));
  if(!m_SSSE3   ) x_fprintf(stdout, TEXT("SSSE3 "));
  if(!m_SSE4_1  ) x_fprintf(stdout, TEXT("SSE4.1 "));
  if(!m_SSE4_2  ) x_fprintf(stdout, TEXT("SSE4.2 "));
  if(!m_AVX1    ) x_fprintf(stdout, TEXT("AVX1 "));
  if(!m_AVX2    ) x_fprintf(stdout, TEXT("AVX2 "));
  if(!m_AVX512CD) x_fprintf(stdout, TEXT("AVX512CD "));
  if(!m_AVX512ER) x_fprintf(stdout, TEXT("AVX512ER "));
  if(!m_AVX512PF) x_fprintf(stdout, TEXT("AVX512PF "));
  if(!m_FMA     ) x_fprintf(stdout, TEXT("FMA "));
  if(!m_BMI1    ) x_fprintf(stdout, TEXT("BMI1 "));
  if(!m_BMI2    ) x_fprintf(stdout, TEXT("BMI2 "));
  if(!m_CMOV    ) x_fprintf(stdout, TEXT("CMOV "));
  if(!m_POPCNT  ) x_fprintf(stdout, TEXT("POPCNT "));
  if(!m_LZCNT   ) x_fprintf(stdout, TEXT("LZCNT "));
  if(!m_AES     ) x_fprintf(stdout, TEXT("AES "));
  if(!m_SHA     ) x_fprintf(stdout, TEXT("SHA "));
  if(!m_FP16C   ) x_fprintf(stdout, TEXT("FP16C "));  
  if(!m_RDRAND  ) x_fprintf(stdout, TEXT("RDRAND "));
  if(!m_RDSEED  ) x_fprintf(stdout, TEXT("RDSEED "));  
  if(!m_HT      ) x_fprintf(stdout, TEXT("HyperThreading ")); 
  x_fprintf(stdout, TEXT("\n"));
}

int xCpuInfo::checkCompatibility()
{
  int compatible = 0;
#if X_SSE1
  if(!m_SSE1)
  {
    compatible -= 1;
    x_print_err(TEXT("x_sys_cpu: your CPU is incompatible with SSE instruction set\n") );
  }
#endif
#if X_SSE2
  if(!m_SSE2)
  {
    compatible -= 1;
    x_print_err(TEXT("x_sys_cpu: your CPU is incompatible with SSE2 instruction set\n") );
  }
#endif
#if X_SSE3
  if(!m_SSE3)
  {
    compatible -= 1;
    x_print_err(TEXT("x_sys_cpu: your CPU is incompatible with SSE3 instruction set\n") );
  }
#endif
#if X_SSSE3
  if(!m_SSSE3)
  {
    compatible -= 1;
    x_print_err(TEXT("x_sys_cpu: your CPU is incompatible with SSSE3 instruction set\n") );
  }
#endif
#if X_SSE4_1
  if(!m_SSE4_1)
  {
    compatible -= 1;
    x_print_err(TEXT("x_sys_cpu: your CPU is incompatible with SSE4.1 instruction set\n") );
  }
#endif
#if X_SSE4_2
  if(!m_SSE4_2)
  {
    compatible -= 1;
    x_print_err(TEXT("x_sys_cpu: your CPU is incompatible with SSE4.2 instruction set\n") );
  }
#endif
#if X_AVX1
  if(!m_AVX1)
  {
    compatible -= 1;
    x_print_err(TEXT("x_sys_cpu: your CPU is incompatible with AVX instruction set\n") );
  }
#endif
#if X_AVX2
  if(!m_AVX2)
  {
    compatible -= 1;
    x_print_err(TEXT("x_sys_cpu: your CPU is incompatible with AVX2 instruction set\n") );
  }
#endif

  return compatible;
}

//==========================================================================
// File I/O
//==========================================================================
FILE_HANDLE x_fopen(xchar* in_name, xchar* attr)
{
  HANDLE     file_handle;
  uint32     fattr;
  uint32     fcreate;
  uint32     fsk;
  xchar*   a;

  if (in_name==NULL || attr==NULL) return NULL;
  fattr   = 0;
  fcreate = 0;
  fsk = 0;
  for(a=attr;*a;++a)
  {
    if(*a==TEXT('r')) { fattr |= GENERIC_READ;  fcreate = OPEN_EXISTING; };
    if(*a==TEXT('w')) { fattr |= GENERIC_WRITE; fcreate = CREATE_ALWAYS; };
    if(*a==TEXT('a')) { fattr |= GENERIC_WRITE; fcreate = OPEN_ALWAYS;   fsk = 1; };
  }
  
  file_handle = CreateFile(
    in_name,
    fattr,
    FILE_SHARE_READ|FILE_SHARE_WRITE,
    NULL,
    fcreate,
    FILE_ATTRIBUTE_NORMAL,
    NULL);

  if(file_handle!=INVALID_HANDLE_VALUE && fsk) 
  SetFilePointer(file_handle,0,NULL,FILE_END );  

  if(file_handle==INVALID_HANDLE_VALUE)
  {
    x_fprinterror();
    x_print_err(TEXT("(%s)\n"),in_name);
    return(NULL);
  }
  return(file_handle);
}

FILE_HANDLE x_fclose(FILE_HANDLE file_handle)
{
  if(file_handle!=NULL)
  {
    if(file_handle!=INVALID_HANDLE_VALUE) CloseHandle(file_handle);
    file_handle = NULL;
  }
  return(NULL);
}

uint32 x_fread(FILE_HANDLE file_handle, void* buf, uint32 len)
{
  int32 retrd;
  ReadFile(file_handle,buf,len,(LPDWORD)&retrd,NULL);
  return(retrd);
}

uint32 x_fwrite(FILE_HANDLE file_handle, void* buf, uint32 len)
{
  int32 retwr;
  WriteFile(file_handle,buf,len,(LPDWORD)&retwr,NULL);
  return(retwr);
}

uint64 x_ftell(FILE_HANDLE file_handle)
{
  LARGE_INTEGER zero,pos;
  memset(&zero,0, sizeof(LARGE_INTEGER));
  SetFilePointerEx(file_handle, zero, &pos, FILE_CURRENT);    
  return pos.QuadPart;
}

uint64 x_fsize(FILE_HANDLE file_handle)
{
  LARGE_INTEGER size;
  GetFileSizeEx(file_handle, &size);
  return size.QuadPart;
}

uint32 x_eof(FILE_HANDLE file_handle)
{
  return x_ftell(file_handle)==x_fsize(file_handle);
}

uint32 x_fseek(FILE_HANDLE file_handle, int64 pos, uint32 origin)
{
  LARGE_INTEGER i;
  uint32 er;
  uint32 method = 
    (origin==X_SEEK_CUR) ? FILE_CURRENT : 
    (origin==X_SEEK_END) ? FILE_END : 
    FILE_BEGIN;
  i.QuadPart = pos;
  er = SetFilePointerEx(file_handle, i, NULL, method);

  if(er == INVALID_SET_FILE_POINTER) // Test for failure
  { 
    x_fprinterror();
    return(0);
  }
  return(1);
}

void x_fflush(FILE_HANDLE file_handle)
{
  FlushFileBuffers(file_handle);
}

uint32 x_fskip(FILE_HANDLE file_handle, int64 pos)
{
  return x_fseek(file_handle, pos, SEEK_CUR);
}

void x_fprinterror()
{
  xchar* lpMsgBuf;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (xchar*) &lpMsgBuf,
    0,
    NULL 
  );

  x_print_err((TEXT("%s")),lpMsgBuf);
  LocalFree( lpMsgBuf );
}

//================================================================================================
// Simple commandline
//================================================================================================
#ifdef UNICODE

int argi(int argc, wchar_t *argv[], wchar_t* header, int deft)
{
  int i;
  for(i=1;i<argc;i++)
    if(!wcscmp(argv[i],header)) 
      if(i<(argc-1)) return( _wtoi(argv[i+1]) );

  return(deft);
}

float argf(int argc, wchar_t *argv[], wchar_t* header, float deft)
{
  int i;
  for(i=1;i<argc;i++)
    if(!wcscmp(argv[i],header)) 
      if(i<(argc-1)) return( (float)_wtof(argv[i+1]) );

  return(deft);
};

wchar_t*  args(int argc, wchar_t *argv[], wchar_t* header, wchar_t* deft)
{
  int i;
  for(i=1;i<argc;i++)
    if(!wcscmp(argv[i],header)) 
      if(i<(argc-1)) return(argv[i+1]);

  return(deft);
}

int argt(int argc, wchar_t *argv[], wchar_t* header)
{
  int i;
  for(i=1;i<argc;i++)
    if(!wcscmp(argv[i],header)) return(1);

  return(0);
}

int argl(int argc, wchar_t *argv[], wchar_t* header, wchar_t* label)
{
  int i;
  for(i=1;i<argc;i++)
    if(!wcscmp(argv[i],header)) 
      if(i<(argc-1)) 
        if(!wcscmp(argv[i+1],label)) return(1);

  return(0);
}

#else //no UNICODE

int argi(int argc, char *argv[], char* header, int deft)
{
  int i;
  for(i=1;i<argc;i++)
    if(!strcmp(argv[i],header)) 
      if(i<(argc-1)) return( atoi(argv[i+1]) );

  return(deft);
}

float argf(int argc, char *argv[], char* header, float deft)
{
  int i;
  for(i=1;i<argc;i++)
    if(!strcmp(argv[i],header)) 
      if(i<(argc-1)) return( (float)atof(argv[i+1]) );

  return(deft);
}

char*  args(int argc, char *argv[], char* header, char* deft)
{
  int i;
  for(i=1;i<argc;i++)
    if(!strcmp(argv[i],header)) 
      if(i<(argc-1)) return(argv[i+1]);

  return(deft);
}

int argt(int argc, char *argv[], char* header)
{
  int i;
  for(i=1;i<argc;i++)
    if(!strcmp(argv[i],header)) return(1);

  return(0);
}

int argl(int argc, char *argv[], char* header, char* label)
{
  int i;
  for(i=1;i<argc;i++)
    if(!strcmp(argv[i],header)) 
      if(i<(argc-1)) 
        if(!strcmp(argv[i+1],label)) return(1);

  return(0);
}
#endif