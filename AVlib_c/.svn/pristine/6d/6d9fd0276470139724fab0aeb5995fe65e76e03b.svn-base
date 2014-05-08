#pragma once
#ifndef _xConsoleH_
#define _xConsoleH_

#include "xLicense.h"
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>

#define CONSOLE_NAME "x - console"

void x_console_set_stdio_to_UNICODE(void);  //enable UNICODE in stduot, stderr and stdid, breaks printf and fprinf compatibility

int  x_console_create(void);
int  x_console_delete(void);
int  x_cfwprintf(FILE* stream, const wchar_t* format, ... );
int  x_cfcprintf(FILE* stream, const char*    format, ... );

#ifdef UNICODE
#define x_printf(_Format, ...)              wprintf(_Format, __VA_ARGS__)
#define x_fprintf(_File, _Format, ...)     fwprintf(_File, _Format, __VA_ARGS__)
#define x_cfprintf(_File, _Format, ...) x_cfwprintf(_File, _Format, __VA_ARGS__)
#else
#define x_printf(_Format, ...)               printf(_Format, __VA_ARGS__)
#define x_fprintf(_File, _Format, ...)      fprintf(_File, _Format, __VA_ARGS__)
#define x_cfprintf(_File, _Format, ...) x_cfcprintf(_File, _Format, __VA_ARGS__)
#endif

#define x_print_err(_Format, ...)        x_cfprintf(stderr, _Format, __VA_ARGS__)

#endif //_xConsoleH_