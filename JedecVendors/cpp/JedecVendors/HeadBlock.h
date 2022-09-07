#pragma once
#ifndef HEADBLOCK_H
#define HEADBLOCK_H

#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

struct LOCATOR
{
	char* p1;
	char* p2;
};

#define MIN_IN  40
#define MAX_IN  2 * 1024 * 1024
#define MAX_OUT 4 * 1024 * 1024
#define MAX_SUB 20
#define MAX_MSG 256

#define SPACE_CHAR 32
#define TAB_CHAR   9
#define CR_CHAR    13
#define LF_CHAR    10

#define MIN_LEXEM_COUNT 11
#define MAX_ENTRY       128

#endif  // HEADBLOCK_H

