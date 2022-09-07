#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "JedecData.h"
using namespace std;

int main(int argc, char** argv)
{
	const char* s = GetVendorName(1, 1);
	if (s == NULL)
	{
		cout << "No data" << endl;
	}
	else
	{
		cout << s << endl;
	}
	return 0;
}
