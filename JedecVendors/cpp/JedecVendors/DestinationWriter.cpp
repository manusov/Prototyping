#include "HeadBlock.h"
#include "DestinationWriter.h"
#include "CharCorrector.h"
using namespace std;

bool DestinationWriter(char*& outPtr, int& outSize, std::vector<std::vector<LOCATOR>>& locators)
{
	char msg[MAX_MSG];
	int addend = 0;

	int limit = MAX_OUT - outSize;
	addend = snprintf(outPtr, limit, "\r\n\r\n/* Start of CPP source block. */\r\n\r\n");
	outPtr += addend;
	outSize += addend;

	int groupCount = (int)locators.size();

	for (int i = 0; i < groupCount; i++)
	{
		vector<LOCATOR> v = locators[i];
		int nameCount = (int)v.size();
		limit = MAX_OUT - outSize;
		addend = snprintf(outPtr, limit, "const char* JedecGroup%d[] = \r\n{\r\n", i + 1);
		outPtr += addend;
		outSize += addend;

		for (int j = 0; j < nameCount; j++)
		{
			char* pmsg = msg;
			int maxmsg = MAX_MSG;
			addend = snprintf(pmsg, maxmsg, "\"");
			pmsg += addend;
			maxmsg -= addend;
			char* p1 = v[j].p1;
			char* p2 = v[j].p2;
			CorrectorReset();
			while (p1 < p2)
			{
				CorrectorChar(pmsg, p1);
				maxmsg--;
			}
			addend = snprintf(pmsg, limit, "\"");
			pmsg += addend;
			maxmsg -= addend;
			limit = MAX_OUT - outSize;
			if (j == (nameCount - 1))
			{
				addend = snprintf(outPtr, limit, "   %-50s   // %d/%d\r\n", msg, i + 1, j + 1);
			}
			else
			{
				addend = snprintf(outPtr, limit, "   %-50s , // %d/%d\r\n", msg, i + 1, j + 1);
			}
			outPtr += addend;
			outSize += addend;
		}

		limit = MAX_OUT - outSize;
		addend = snprintf(outPtr, limit, "};\r\n\r\n");
		outPtr += addend;
		outSize += addend;
	}

	limit = MAX_OUT - outSize;
	addend = snprintf(outPtr, limit, "\r\n/* End of CPP source block. */\r\n\r\n");
	outPtr += addend;
	outSize += addend;
	return TRUE;
}
