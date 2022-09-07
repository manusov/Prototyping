#include "HeadBlock.h"
#include "SourceParser.h"
#include "CharCorrector.h"
using namespace std;

bool helperDecimal(SUBSTRING subs[], int index, int& value)
{
	value = 0;
	bool b = false;
	char* p = subs[index].ptr;
	int n = subs[index].length;
	for (int i = 0; i < n; i++)
	{
		char c = *p++;
		if ((c < '0') || (c > '9'))
		{
			b = false;
			break;
		}
		else
		{
			b = true;
			value = value * 10 + (c - '0');
		}
	}
	return b;
}

bool helperHex(SUBSTRING subs[], int index, int& value)
{
	value = 0;
	bool b = false;
	char* p = subs[index].ptr;
	int n = subs[index].length;
	for (int i = 0; i < n; i++)
	{
		char c = *p++;
		if ((c >= '0') && (c <= '9'))
		{
			b = true;
			value = value * 16 + (c - '0');
		}
		else if ((c >= 'A') && (c <= 'F'))
		{
			b = true;
			value = value * 16 + (c - 'A' + 10);
		}
		else if ((c >= 'a') && (c <= 'f'))
		{
			b = true;
			value = value * 16 + (c - 'a' + 10);
		}
		else
		{
			b = false;
			break;
		}
	}
	return b;
}

bool helperBinaryOneDigit(SUBSTRING subs[], int index, int& value)
{
	value = 0;
	bool b = false;
	char* p = subs[index].ptr;
	int n = subs[index].length;
	if (n == 1)
	{
		char c = *p;
		if ((c == '0') || (c == '1'))
		{
			b = true;
			value = c - '0';
		}
	}
	return b;
}

bool SourceParser(char* inPtr, char*& outPtr, int inSize, int& outSize, vector<vector<LOCATOR>>& locators)
{
	int previousNumber = 0x7FFFFFFF;
	int jedecGroup = 0;
	while ((inSize > 0) && (outSize < MAX_OUT))   // cycle for strings in the parsed file
	{
		SUBSTRING subs[MAX_SUB];
		memset(subs, 0, sizeof(subs));
		int index = 0;
		char c = 0;
		while ((inSize > 0) && (c != CR_CHAR) && (c != LF_CHAR) && (index < MAX_SUB))  // cycle for words in the parsed string
		{
			c = *inPtr;
			switch (c)
			{
			case CR_CHAR:
			case LF_CHAR:
				index = 0;
				while ((inSize > 0) && ((c == CR_CHAR) || (c == LF_CHAR)))
				{
					c = *inPtr;
					if ((c == CR_CHAR) || (c == LF_CHAR))
					{
						inPtr++;
						inSize--;
					}
				}
				break;
			case SPACE_CHAR:
			case TAB_CHAR:
				while ((inSize > 0) && ((c == SPACE_CHAR) || (c == TAB_CHAR)))
				{
					c = *inPtr;
					if ((c == SPACE_CHAR) || (c == TAB_CHAR))
					{
						inPtr++;
						inSize--;
					}
				}
				break;
			default:
				subs[index].ptr = inPtr;
				int length = 0;
				while ((inSize > 0) && (c != SPACE_CHAR) && (c != TAB_CHAR) && (c != CR_CHAR) && (c != LF_CHAR))
				{
					c = *inPtr;
					if ((c != SPACE_CHAR) && (c != TAB_CHAR) && (c != CR_CHAR) && (c != LF_CHAR))
					{
						length++;
						inPtr++;
						inSize--;
					}
				}
				subs[index++].length = length;
				break;
			}
		}
		if (index >= MIN_LEXEM_COUNT)  // end parse one string, start verify parsed string is JEDEC ID entry
		{
			int n = 0;
			if (helperDecimal(subs, 0, n))
			{
				if ((n > 0) && (n < 127))
				{
					int m = 0;
					if (helperHex(subs, index - 1, m))
					{
						if (((m > 0) && (m < 256)) && ((m & 0x7F) == n))
						{
							bool b = true;
							int k = 0;
							for (int i = index - 2; i >= index - 9; i--)
							{
								b = b && helperBinaryOneDigit(subs, i, k);
							}
							if (b)
							{
								// start copy parsed string to output buffer, if JEDEC ID entry, check ASCII range for copied chars
								char name[MAX_ENTRY];
								int cntName = 0;
								char* srcName = NULL;
								char* dstName = name;
								char* p1 = subs[1].ptr;
								for (int i = 1; i < index - 9; i++)
								{
									srcName = subs[i].ptr;
									cntName = subs[i].length;
									CorrectorReset();
									for (int j = 0; j < cntName; j++)
									{
										if (j == 0)
										{
											*dstName++ = ' ';
										}
										CorrectorChar(dstName, srcName);
									}
								}
								char* p2 = srcName;
								*dstName = 0;
								LOCATOR locator = { p1, p2 };
								int limit = MAX_OUT - outSize;
								if (limit > 0)
								{
									int addend;
									if (previousNumber > n)
									{
										addend = snprintf(outPtr, limit, "\r\nGROUP %d\r\n\r\n%3d, %s\r\n", ++jedecGroup, n, name);
										vector<LOCATOR> v;
										v.push_back(locator);
										locators.push_back(v);
									}
									else
									{
										addend = snprintf(outPtr, limit, "%3d, %s\r\n", n, name);
										locators[jedecGroup - 1].push_back(locator);
									}
									previousNumber = n;
									cout << outPtr;
									outPtr += addend;
									outSize += addend;
								}
							}
						}
					}
				}
			}
		}
	}
	return TRUE;
}
