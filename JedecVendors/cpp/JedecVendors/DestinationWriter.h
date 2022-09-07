#pragma once
#ifndef DESTINATIONWRITER_H
#define DESTINATIONWRITER_H
#include "HeadBlock.h"

bool DestinationWriter(char*& outPtr, int& outSize, std::vector<std::vector<LOCATOR>>& locators);

#endif  // DESTINATIONWRITER_H

