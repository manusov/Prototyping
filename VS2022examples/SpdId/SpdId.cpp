
#include <iostream>
#include <iomanip>
#include <windows.h>
#include "loadspd.h"
#include "parsespd.h"
#include "spdid.h"
using namespace std;

int   dataSize = 0;
byte* dataPtr  = NULL;

int main(int argc, char** argv)
{
    cout << BUILD_STRING << endl;
    cout << USAGE_STRING << endl;
    dataPtr = new byte[MAX_SPD_BUFFER];
    if (!dataPtr)
    {
        cout << "Memory allocation error." << endl;
        return 1;
    }
    if (!parmInit())
    {
        cout << "Console output failed." << endl;
        return 2;
    }
    dataSize = loadSpd(argc, argv, dataPtr, MAX_SPD_BUFFER);
    if (dataSize <= 0)
    {
        cout << "Get SPD info failed." << endl;
        return 3;
    }
    parseSpd(dataPtr, dataSize);
    delete[] dataPtr;
    dataPtr = NULL;
    return 0;
}

HANDLE hStdout;
CONSOLE_SCREEN_BUFFER_INFO csbi;
BOOL parmInit()
{
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) return FALSE;
    return GetConsoleScreenBufferInfo(hStdout, &csbi);
}
void parmValue(const char s1[], const char s2[])
{
    WORD defaultColor = csbi.wAttributes;
    WORD color = defaultColor & 0xF0 | PARM_COLOR;
    SetConsoleTextAttribute(hStdout, color);
    cout << left << setw(PARM_WIDTH) << s1;
    color = defaultColor & 0xF0 | VALUE_COLOR;
    SetConsoleTextAttribute(hStdout, color);
    cout << s2 << endl;
    SetConsoleTextAttribute(hStdout, defaultColor);
}
void parmGroup(const char s[])
{
    WORD defaultColor = csbi.wAttributes;
    WORD color = defaultColor & 0xF0 | GROUP_COLOR;
    SetConsoleTextAttribute(hStdout, color);
    cout << "[" << s << "]" << endl;
    SetConsoleTextAttribute(hStdout, defaultColor);
}
void parmError(const char s[])
{
    WORD defaultColor = csbi.wAttributes;
    WORD color = defaultColor & 0xF0 | ERROR_COLOR;
    SetConsoleTextAttribute(hStdout, color);
    cout << "[" << s << "]" << endl;
    SetConsoleTextAttribute(hStdout, defaultColor);
}
void parmSummary(const char s[])
{
    WORD defaultColor = csbi.wAttributes;
    WORD color = defaultColor & 0xF0 | SUMMARY_COLOR;
    SetConsoleTextAttribute(hStdout, color);
    cout << s << endl;
    SetConsoleTextAttribute(hStdout, defaultColor);
}
void parmDump(byte* ptr, int size)
{
    // initialization
    WORD defaultColor = csbi.wAttributes;
    WORD color;
    char buf[MAX_TEXT_STRING];
    int address = 0;
    byte* ptr1;
    char* buf1;
    int n;
    int m;
    int k;
    // cycle for dump strings
    int lines = size / DUMP_BYTES_PER_LINE;
    if ((size % DUMP_BYTES_PER_LINE) > 0)
    {
        lines++;
    }
    for (int i = 0; i < lines; i++)
    {
        // print address part of dump string
        color = defaultColor & 0xF0 | DUMP_ADDRESS_COLOR;
        SetConsoleTextAttribute(hStdout, color);
        buf1 = buf;
        buf1 += snprintf(buf, MAX_TEXT_STRING, "%04X  ", address);
        *buf1 = 0;
        cout << buf;
        // print data part of dump string
        color = defaultColor & 0xF0 | DUMP_DATA_COLOR;
        SetConsoleTextAttribute(hStdout, color);
        if (size > DUMP_BYTES_PER_LINE)
        {
            n = DUMP_BYTES_PER_LINE;
        }
        else
        {
            n = size;
        }
        m = MAX_TEXT_STRING;
        ptr1 = ptr;
        buf1 = buf;
        for (int j = 0; j < n; j++)
        {
            k = snprintf(buf1, m, "%02X ", *ptr1++);
            buf1 += k;
            m -= k;
        }
        for (int j = 0; j < (DUMP_BYTES_PER_LINE - n); j++)
        {
            k = snprintf(buf1, m, "   ");
            buf1 += k;
            m -= k;
        }
        k = snprintf(buf1, m, " ");
        buf1 += k;
        m -= k;
        *buf1 = 0;
        cout << buf;
        // print text part of dump string
        color = defaultColor & 0xF0 | DUMP_TEXT_COLOR;
        SetConsoleTextAttribute(hStdout, color);
        m = MAX_TEXT_STRING;
        ptr1 = ptr;
        buf1 = buf;
        for (int j = 0; j < n; j++)
        {
            char c = *ptr1++;
            if ((c < ' ') || (c > '}'))
            {
                c = '.';
            }
            k = snprintf(buf1, m, "%c", c);
            buf1 += k;
            m -= k;
        }
        for (int j = 0; j < (DUMP_BYTES_PER_LINE - n); j++)
        {
            k = snprintf(buf1, m, " ");
            buf1 += k;
            m -= k;
        }
        *buf1 = 0;
        cout << buf << endl;
        // cycle for dump strings
        address += DUMP_BYTES_PER_LINE;
        ptr += DUMP_BYTES_PER_LINE;
        size -= DUMP_BYTES_PER_LINE;
    }
    // restore color
    SetConsoleTextAttribute(hStdout, defaultColor);
}
#define SMALL_STRING 30
// Prints to the provided buffer a nice number of bytes (KB, MB, GB, etc)
void printSize(char* buf, ULONGLONG bytes)
{
    const char* suffixes[7];
    suffixes[0] = "Bytes";
    suffixes[1] = "KB";
    suffixes[2] = "MB";
    suffixes[3] = "GB";
    suffixes[4] = "TB";
    suffixes[5] = "PB";
    suffixes[6] = "EB";
    int s = 0;             // which suffix to use
    double count = (double)bytes;
    while (count >= 1024 && s < 7)
    {
        s++;
        count /= 1024;
    }
    if (count - floor(count) == 0.0)
        snprintf(buf, SMALL_STRING, "%d %s", (int)count, suffixes[s]);
    else
        snprintf(buf, SMALL_STRING, "%.2f %s", count, suffixes[s]);
}


