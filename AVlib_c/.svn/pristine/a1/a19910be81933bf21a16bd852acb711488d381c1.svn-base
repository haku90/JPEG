#define _CRT_SECURE_NO_WARNINGS 1

#include "xConsole.h"

void  x_console_set_stdio_to_UNICODE(void)
{
  fflush(stdout);
  fflush(stderr);
  fflush(stdin);
  _setmode(_fileno(stdout), _O_U16TEXT);
  _setmode(_fileno(stderr), _O_U16TEXT);
  _setmode(_fileno(stdin),  _O_U16TEXT);
}


int x_console_create(void)
{
  AllocConsole();
  SetConsoleTitle(TEXT(CONSOLE_NAME));
#ifdef UNICODE
  _wfreopen(TEXT("CONOUT$"),TEXT("w"),stdout); // reopen stout handle as console window output
  _wfreopen(TEXT("CONOUT$"),TEXT("w"),stderr); // reopen stderr handle as console window output
  _wfreopen(TEXT("CONIN$" ),TEXT("r"),stdin ); // reopen stdin handle as console window input
#else
  freopen(TEXT("CONOUT$"),TEXT("w"),stdout);   // reopen stout handle as console window output
  freopen(TEXT("CONOUT$"),TEXT("w"),stderr);   // reopen stderr handle as console window output
  freopen(TEXT("CONIN$" ),TEXT("r"),stdin );   // reopen stdin handle as console window input
#endif
#if X_DEBUG
  x_fprintf(stdout, TEXT("V Console created\n"));
#endif
  return 0;
}

int x_console_delete(void)
{
  HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if(std_output_handle != NULL)  
    FreeConsole();
  return 0;
}

int x_cfwprintf(FILE* stream, const wchar_t* format, ... )
{
  static wchar_t buffer[1024];
  va_list args;
  int retval;
  HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if(std_output_handle == NULL)
  {
    x_console_create();    
  }

  if(stream == stderr)
    SetConsoleTextAttribute( std_output_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);  

  va_start(args, format);
  retval = vswprintf(buffer, 1024, format, args);
  va_end(args);
  WriteConsole(std_output_handle,buffer, retval,NULL,NULL);

  if(stream == stderr)
    SetConsoleTextAttribute( std_output_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  
  return(retval);
}

int x_cfprintf(FILE* stream, const char* format, ... )
{
  static char buffer[1024];
  static wchar_t wbuffer[1024];
  va_list args;
  int retval;
  HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if(std_output_handle == NULL)
  {
    x_console_create();    
  }

  if(stream == stderr)
    SetConsoleTextAttribute( std_output_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);  

  va_start(args, format);
  retval = vsnprintf(buffer, 1024, format, args);
  va_end(args);
  mbstowcs(wbuffer, buffer, 1024);
  WriteConsole(std_output_handle,wbuffer,retval,NULL,NULL);

  if(stream == stderr)
    SetConsoleTextAttribute( std_output_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  
  return(retval);
}





