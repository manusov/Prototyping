#pragma once
#ifndef SPDINFO_H
#define SPDINFO_H

#include <windows.h>

#if _WIN64
#define BUILD_STRING "SPDID v0.00.04 (x64)."
#define NATIVE_WIDTH 64
#elif _WIN32
#define BUILD_STRING "SPDID v0.00.04 (ia32)."
#define NATIVE_WIDTH 32
#else
#define BUILD_STRING "UNSUPPORTED PLATFORM."
#endif
#define USAGE_STRING "Usage: spdinfo filename.bin, required binary data file."

#define PARM_COLOR     FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define VALUE_COLOR    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define GROUP_COLOR    FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define ERROR_COLOR    FOREGROUND_RED | FOREGROUND_INTENSITY
#define SUMMARY_COLOR  FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY

#define DUMP_ADDRESS_COLOR  FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define DUMP_DATA_COLOR     FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define DUMP_TEXT_COLOR     FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY

#define PARM_WIDTH          44
#define VALUE_WIDTH         80

#define MAX_TEXT_STRING      128
#define DUMP_BYTES_PER_LINE  16
#define MAX_SPD_BUFFER       4096

BOOL parmInit();
void parmValue(const char s1[], const char s2[]);
void parmGroup(const char s[]);
void parmError(const char s[]);
void parmSummary(const char s[]);
void parmDump(byte* ptr, int size);
void printSize(char* buf, ULONGLONG bytes);

#endif  // SPDINFO_H
