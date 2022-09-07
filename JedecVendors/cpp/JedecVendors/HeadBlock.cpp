/*
Data extractor for JEDEC ID text,

RUN EXAMPLE:
name.exe E:\JedecVendors\data\input.txt E:\JedecVendors\data\output.txt

input file must be text, extracted from document JEP106BE.PDF.
TODO.
1)  Make parsing and compilation as separate phases.
	Pass 1 = Parse, save text and build vectors.
	Pass 2 = Build C-source from vectors, save it.
2)  Bug if >1 lines per one vendor string.
    Yet required manually patch input text file if this situation.
3)  Check for acceptable chars when copy.
	Tabulation wrong if "'" char in the name string.
4)  Intervals between strings.
5)  Format numbers.
6)  JEDEC groups numering.
7)  CR and LF, \r\n correct sequence.
8)  Check names per group, 125/126.
	126 normal, but exist group with 125 names.
	Caused by split string with long vendor name.
	Temporary solution by patched text file.
9)  C++/ASM requirements for output text.
10) Compare decimal, binary and hex IDs, check parity requirements.
11) Write string number if error.
12) Verify common. Include JEDEC Decoder write.
13) Comments.

*/

#include "HeadBlock.h"
#include "SourceParser.h"
#include "DestinationWriter.h"
using namespace std;

vector<vector<LOCATOR>> locators;

int main(int argc, char** argv)
{
	char msg[MAX_MSG];
	int status = 0;
	if (argc > 2)
	{
		char* inName = argv[1];
		char* outName = argv[2];
		if ((inName) && (outName))
		{
			ifstream in;  // input file
			in.open(inName, ios::binary | ios::in);
			if (in.is_open())
			{
				in.seekg(0, ios_base::end);
				streamoff s = in.tellg();
				if (s < MIN_IN)
				{
					snprintf(msg, MAX_MSG, "Input file size too small, expected from %d to %d bytes.", MIN_IN, MAX_IN);
					cout << msg << endl;
					status = 4;
				}
				else if (s > MAX_IN)
				{
					snprintf(msg, MAX_MSG, "Input file size too big, expected from %d to %d bytes.", MIN_IN, MAX_IN);
					cout << msg << endl;
					status = 5;
				}
				else
				{
					int inSize = (int)s;
					int outSize = 0;
					bool b = FALSE;
					snprintf(msg, MAX_MSG, "Input file size %d bytes.", inSize);
					cout << msg << endl;
					char* inPtr = (char*)malloc(MAX_IN);
					char* outPtr = (char*)malloc(MAX_OUT);
					char* const backInPtr = inPtr;
					char* const backOutPtr = outPtr;

					if ((inPtr) && (outPtr))
					{
						in.seekg(0, ios_base::beg);
						in.read(inPtr, inSize);
						if (in)
						{
							b = SourceParser(inPtr, outPtr, inSize, outSize, locators);           // Phase 1 = source parse.
							if (!b)
							{
								snprintf(msg, MAX_MSG, "Source parser internal error.");
								cout << msg << endl;
								status = 11;

							}
							else if (outSize >= MAX_OUT)
							{
								snprintf(msg, MAX_MSG, "Output file size limit, %d bytes.", MAX_OUT);
								cout << msg << endl;
								status = 10;
							}
							else
							{
								char* tempPtr = outPtr;
								b = DestinationWriter(outPtr, outSize, locators);                 // Phase 2 = destination write.
								cout << tempPtr;
								if (!b)
								{
									snprintf(msg, MAX_MSG, "Destination writer internal error.");
									cout << msg << endl;
									status = 12;
								}

							}
							ofstream out;  // output file
							out.open(outName, ios::binary | ios::out);
							if (out.is_open())
							{
								out.write(backOutPtr, outSize);
								if (out.fail())
								{
									cout << "Error writing output file." << endl;
									status = 9;
								}
								else
								{
									int n = (int)out.tellp();
									snprintf(msg, MAX_MSG, "%d bytes saved to output file.", n);
									cout << endl << msg << endl;
								}
								out.close();
							}
							else
							{
								cout << "Output file create error." << endl;
								status = 8;
							}
						}
						else
						{
							int n = (int)in.gcount();
							snprintf(msg, MAX_MSG, "Error reading input file, stop after %d bytes.", n);
							cout << msg << endl;
							status = 7;
						}
					}
					else
					{
						cout << "Memory allocation error." << endl;
						status = 6;
					}

					if (backInPtr)
					{
						free(backInPtr);
					}
					if (backOutPtr)
					{
						free(backOutPtr);
					}
				}
				in.close();
			}
			else
			{
				cout << "Input file not found." << endl;
				status = 3;
			}
		}
		else
		{
			cout << "Invalid name parameter." << endl;
			status = 2;
		}
	}
	else
	{
		cout << "Invalid command line." << endl;
		status = 1;
	}
	return status;
}
