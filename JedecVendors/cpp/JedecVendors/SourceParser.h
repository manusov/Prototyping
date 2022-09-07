#pragma once
#ifndef SOURCEPARSER_H
#define SOURCEPARSER_H
#include "HeadBlock.h"

struct SUBSTRING
{
	char* ptr;
	int length;
};

bool SourceParser(char* inPtr, char*& outPtr, int inSize, int& outSize, std::vector<std::vector<LOCATOR>>& locators);

#endif  // SOURCEPARSER_H

