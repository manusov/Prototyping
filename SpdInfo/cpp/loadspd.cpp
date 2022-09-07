#include <windows.h>
#include<iostream>
#include<fstream>
#include "spdinfo.h"
using namespace std;

int loadSpd(int argc, char** argv, byte* ptr, int maxBuffer)
{
	streamsize readSize = 0;
	ifstream in;
	if (argc > 1)
	{
		char* name = argv[1];
		if (name)
		{
			in.open(name, ios::binary | ios::in);
			if (in.is_open())
			{
				in.read((char*)ptr, maxBuffer);
				readSize = in.gcount();
				in.seekg(0, ios_base::end);
				if (readSize < in.tellg())
				{
					readSize = 0;
				}
				in.close();
			}
		}
	}
	return (int)readSize;
}
