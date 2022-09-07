/*
NVAPI information. Engineering sample.
TODO.
1) Tachometer read error.
2) Other information view, include power (watts) read add.
3) Support all variants of frequency read. Learn variants, include sensors array.
4) Support all variants of temperature read. Learn variants, include sensors array.
5) Code optimization and comments.
*/

#include <iostream>
#include <iomanip>
#include <windows.h>
#include "nvapi\nvapi.h"
#define PARM_COLOR   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define VALUE_COLOR  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define GROUP_COLOR  FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define ERROR_COLOR  FOREGROUND_RED | FOREGROUND_INTENSITY
#define NAME_COLOR   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define PARM_WIDTH   42
#define VALUE_WIDTH  60
#define SMALL_STRING 160
#define NVAPIQUERY_UTILIZATION_TYPE_GPU 0
#ifdef  _WIN64
#define BUILD_STRING "NVAPI info v0.00.02 (x64)."
#define NATIVE_WIDTH 64
#pragma comment(lib, "lib\\amd64\\nvapi64.lib")
#else
#define BUILD_STRING "NVAPI info v0.00.02 (ia32)."
#define NATIVE_WIDTH 32
#pragma comment(lib, "lib\\x86\\nvapi.lib")
#endif
using namespace std;

// color console support data
HANDLE hStdout;
CONSOLE_SCREEN_BUFFER_INFO csbi;
char buffer[SMALL_STRING];

// helpers
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
void parmName(const char s[])
{
    WORD defaultColor = csbi.wAttributes;
    WORD color = defaultColor & 0xF0 | NAME_COLOR;
    SetConsoleTextAttribute(hStdout, color);
    cout << s << endl;
    SetConsoleTextAttribute(hStdout, defaultColor);
}
void writeHex64(char* s, size_t limit, DWORD64 value)
{
    DWORD32 low = value & 0xFFFFFFFFL;
    DWORD32 high = (value >> 32) & 0xFFFFFFFFL;
    snprintf(s, limit, "%08X%08X", high, low);
}
void printMegabytes(char* s, unsigned int kb)
{
    double mb = kb / 1024.0;
    DWORD64 bytes = ((DWORD64)kb) << 10;
    char temp[SMALL_STRING];
    writeHex64(temp, SMALL_STRING, bytes);
    snprintf(s, SMALL_STRING, "%.1f MB (%sh).", mb, temp);
}
void printClocks(char* s, unsigned int currentKhz, unsigned int baseKhz, unsigned int boostKhz)
{
    double currentMHz = (double)currentKhz / 1000.0;
    double baseMHz = (double)baseKhz / 1000.0;
    double boostMHz = (double)boostKhz / 1000.0;
    char currentStr[SMALL_STRING];
    char baseStr[SMALL_STRING];
    char boostStr[SMALL_STRING];
    if (currentKhz != 0)
    {
        snprintf(currentStr, SMALL_STRING, "%.1f MHz", currentMHz);
    }
    else
    {
        snprintf(currentStr, SMALL_STRING, "?");
    }
    if (baseKhz != 0)
    {
        snprintf(baseStr, SMALL_STRING, "%.1f MHz", baseMHz);
    }
    else
    {
        snprintf(baseStr, SMALL_STRING, "?");
    }
    if (boostKhz != 0)
    {
        snprintf(boostStr, SMALL_STRING, "%.1f MHz", boostMHz);
    }
    else
    {
        snprintf(boostStr, SMALL_STRING, "?");
    }
    snprintf(s, SMALL_STRING, "%s (base=%s, boost=%s).", currentStr, baseStr, boostStr);
}

// helpers structures
struct PCI_ENTRY
{
    NvU32 pDeviceId;
    NvU32 pSubSystemId;
    NvU32 pRevisionId;
    NvU32 pExtDeviceId;
};
struct CLK_ENTRY
{
    NV_GPU_CLOCK_FREQUENCIES_V2 currentClk;
    NV_GPU_CLOCK_FREQUENCIES_V2 baseClk;
    NV_GPU_CLOCK_FREQUENCIES_V2 boostClk;
};
// this data can be shared between information handlers sequentally calls,
// layout is handler-specific, memory size minimized by union.
union NVAPI_DATA
{
    NvAPI_ShortString gpuName;
    NV_DISPLAY_DRIVER_MEMORY_INFO gpuMemoryStatus;
    NV_GPU_DYNAMIC_PSTATES_INFO_EX gpuPerformance;
    NvU32 gpuCoreCount;
    PCI_ENTRY gpuPciId;
    NvU32 gpuVideoBiosRevision;
    NvU32 gpuVideoBiosOemRevision;
    NvAPI_ShortString gpuVideoBiosString;
    NvU32 gpuPcieWidth;
    CLK_ENTRY gpuClocks;
    NV_GPU_THERMAL_SETTINGS gpuThermal;
    NvU32 gpuFanTachometer;
} nvapiData;
NvAPI_Status nvRetValue;

// information handlers
bool infoGpuName(NvPhysicalGpuHandle handle, char* s)  // get GPU name
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvRetValue = NvAPI_GPU_GetFullName(handle, nvapiData.gpuName);
    if (nvRetValue == NVAPI_OK)
    {
        snprintf(s, SMALL_STRING, "%s.", nvapiData.gpuName);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get full name failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuMemoryTotal(NvPhysicalGpuHandle handle, char* s)  // get GPU frame buffer memory information, total memory
{
    nvapiData.gpuMemoryStatus.version = MAKE_NVAPI_VERSION(NV_DISPLAY_DRIVER_MEMORY_INFO_V2, 2);
    nvRetValue = NvAPI_GPU_GetMemoryInfo(handle, &nvapiData.gpuMemoryStatus);
    if (nvRetValue == NVAPI_OK)
    {
        printMegabytes(buffer, nvapiData.gpuMemoryStatus.dedicatedVideoMemory);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU memory information failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuMemoryAvailable(NvPhysicalGpuHandle handle, char* s)  // get GPU frame buffer memory information, available memory
{
    if (nvRetValue == NVAPI_OK)
    {
        printMegabytes(buffer, nvapiData.gpuMemoryStatus.availableDedicatedVideoMemory);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU memory information failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuMemoryCurrentAvailable(NvPhysicalGpuHandle handle, char* s)  // get GPU frame buffer memory information, current available memory
{
    if (nvRetValue == NVAPI_OK)
    {
        printMegabytes(buffer, nvapiData.gpuMemoryStatus.curAvailableDedicatedVideoMemory);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU memory information failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuMemoryShared(NvPhysicalGpuHandle handle, char* s)  // get GPU frame buffer memory information, shared system memory
{
    if (nvRetValue == NVAPI_OK)
    {
        printMegabytes(buffer, nvapiData.gpuMemoryStatus.sharedSystemMemory);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU memory information failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuMemorySystem(NvPhysicalGpuHandle handle, char* s)  // get GPU frame buffer memory information, system video memory
{
    if (nvRetValue == NVAPI_OK)
    {
        printMegabytes(buffer, nvapiData.gpuMemoryStatus.systemVideoMemory);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU memory information failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuMemoryUtilization(NvPhysicalGpuHandle handle, char* s)  // get GPU frame buffer memory information, dedicated video memory utilization
{
    if (nvRetValue == NVAPI_OK)
    {
        unsigned int totalMemory = nvapiData.gpuMemoryStatus.dedicatedVideoMemory;
        unsigned int usedMemory = nvapiData.gpuMemoryStatus.dedicatedVideoMemory - nvapiData.gpuMemoryStatus.curAvailableDedicatedVideoMemory;
        if (totalMemory != 0)
        {
            double utilization = (double)usedMemory / (double)totalMemory * 100.0;
            snprintf(s, SMALL_STRING, "%.2f %%.", utilization);
        }
        else
        {
            snprintf(s, SMALL_STRING, "n/a.");
        }

        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU memory information failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuUtilization(NvPhysicalGpuHandle handle, char* s)  // get GPU utilization
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvapiData.gpuPerformance.version = MAKE_NVAPI_VERSION(NV_GPU_DYNAMIC_PSTATES_INFO_EX, 1);
    nvRetValue = NvAPI_GPU_GetDynamicPstatesInfoEx(handle, &nvapiData.gpuPerformance);
    if (nvRetValue == NVAPI_OK)
    {
        if (nvapiData.gpuPerformance.utilization[NVAPIQUERY_UTILIZATION_TYPE_GPU].bIsPresent)
        {
            double utilization = nvapiData.gpuPerformance.utilization[NVAPIQUERY_UTILIZATION_TYPE_GPU].percentage;
            snprintf(s, SMALL_STRING, "%.2f %%.", utilization);
            return true;
        }
        else
        {
            snprintf(s, SMALL_STRING, "Get GPU utilization not supported.");
            return false;
        }
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU performance data failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuCoreCount(NvPhysicalGpuHandle handle, char* s)  // get GPU core count
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvRetValue = NvAPI_GPU_GetGpuCoreCount(handle, &nvapiData.gpuCoreCount);
    if (nvRetValue == NVAPI_OK)
    {
        if (nvapiData.gpuCoreCount != 0)
        {
            snprintf(s, SMALL_STRING, "%d.", nvapiData.gpuCoreCount);
            return true;
        }
        else
        {
            snprintf(s, SMALL_STRING, "Get GPU core count not supported.");
            return false;
        }
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU core count failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuPciDevice(NvPhysicalGpuHandle handle, char* s)  // Get GPU PCI identifiers
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvRetValue = NvAPI_GPU_GetPCIIdentifiers(handle,
        &nvapiData.gpuPciId.pDeviceId, &nvapiData.gpuPciId.pSubSystemId,
        &nvapiData.gpuPciId.pRevisionId, &nvapiData.gpuPciId.pExtDeviceId);
    if (nvRetValue == NVAPI_OK)
    {
        int vendorId = nvapiData.gpuPciId.pDeviceId & 0xFFFF;
        int deviceId = (nvapiData.gpuPciId.pDeviceId >> 16) & 0xFFFF;
        snprintf(s, SMALL_STRING, "vendor=%04Xh, device=%04Xh.", vendorId, deviceId);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU PCI identifiers failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuPciSubsystem(NvPhysicalGpuHandle handle, char* s)
{
    if (nvRetValue == NVAPI_OK)
    {
        int vendorId = nvapiData.gpuPciId.pSubSystemId & 0xFFFF;
        int deviceId = (nvapiData.gpuPciId.pSubSystemId >> 16) & 0xFFFF;
        snprintf(s, SMALL_STRING, "sub-vendor=%04Xh, device=%04Xh.", vendorId, deviceId);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU PCI identifiers failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuPciRevision(NvPhysicalGpuHandle handle, char* s)
{
    if (nvRetValue == NVAPI_OK)
    {
        snprintf(s, SMALL_STRING, "%08Xh.", nvapiData.gpuPciId.pRevisionId);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU PCI identifiers failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuPciExtendedDevice(NvPhysicalGpuHandle handle, char* s)
{
    if (nvRetValue == NVAPI_OK)
    {
        snprintf(s, SMALL_STRING, "%08Xh.", nvapiData.gpuPciId.pExtDeviceId);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU PCI identifiers failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuVideoBiosRevision(NvPhysicalGpuHandle handle, char* s)  // get video BIOS revision
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvRetValue = NvAPI_GPU_GetVbiosRevision(handle, &nvapiData.gpuVideoBiosRevision);
    if (nvRetValue == NVAPI_OK)
    {
        snprintf(s, SMALL_STRING, "%08Xh.", nvapiData.gpuVideoBiosRevision);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get video BIOS revision failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuVideoBiosOemRevision(NvPhysicalGpuHandle handle, char* s)  // get video BIOS OEM revision
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvRetValue = NvAPI_GPU_GetVbiosOEMRevision(handle, &nvapiData.gpuVideoBiosOemRevision);
    if (nvRetValue == NVAPI_OK)
    {
        snprintf(s, SMALL_STRING, "%08Xh.", nvapiData.gpuVideoBiosOemRevision);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get video BIOS OEM revision failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuVideoBiosString(NvPhysicalGpuHandle handle, char* s)  // get video BIOS string
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvRetValue = NvAPI_GPU_GetVbiosVersionString(handle, nvapiData.gpuVideoBiosString);
    if (nvRetValue == NVAPI_OK)
    {
        snprintf(s, SMALL_STRING, "%s.", nvapiData.gpuVideoBiosString);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get video BIOS string failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuPcieWidth(NvPhysicalGpuHandle handle, char* s)  // get GPU downstream PCIe width
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvRetValue = NvAPI_GPU_GetCurrentPCIEDownstreamWidth(handle, &nvapiData.gpuPcieWidth);
    if (nvRetValue == NVAPI_OK)
    {
        if (nvapiData.gpuPcieWidth != 0)
        {
            snprintf(s, SMALL_STRING, "x%d.", nvapiData.gpuPcieWidth);
            return true;
        }
        else
        {
            snprintf(s, SMALL_STRING, "Get GPU current downstream PCIe width not supported.");
            return false;
        }
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU current downstream PCIe width failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuGraphicsClk(NvPhysicalGpuHandle handle, char* s)    // get GPU public clock "Graphics clock"
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvapiData.gpuClocks.currentClk.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
    nvapiData.gpuClocks.baseClk.ClockType = NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK;
    nvapiData.gpuClocks.boostClk.ClockType = NV_GPU_CLOCK_FREQUENCIES_BOOST_CLOCK;
    nvapiData.gpuClocks.currentClk.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    nvapiData.gpuClocks.baseClk.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    nvapiData.gpuClocks.boostClk.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    NvAPI_Status nvRetCurrent = NvAPI_GPU_GetAllClockFrequencies(handle, &nvapiData.gpuClocks.currentClk);
    NvAPI_Status nvRetBase = NvAPI_GPU_GetAllClockFrequencies(handle, &nvapiData.gpuClocks.baseClk);
    NvAPI_Status nvRetBoost = NvAPI_GPU_GetAllClockFrequencies(handle, &nvapiData.gpuClocks.boostClk);
    nvRetValue = nvRetCurrent;
    if (nvRetValue == NVAPI_OK)
    {
        unsigned int currentKhz = 0;
        unsigned int baseKhz = 0;
        unsigned int boostKhz = 0;
        if (nvapiData.gpuClocks.currentClk.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].bIsPresent)
        {
            currentKhz = nvapiData.gpuClocks.currentClk.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
        }
        if (nvapiData.gpuClocks.baseClk.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].bIsPresent)
        {
            baseKhz = nvapiData.gpuClocks.baseClk.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
        }
        if (nvapiData.gpuClocks.boostClk.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].bIsPresent)
        {
            boostKhz = nvapiData.gpuClocks.boostClk.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
        }
        printClocks(s, currentKhz, baseKhz, boostKhz);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU public clocks failed (current=%d, base=%d, boost=%d).", nvRetCurrent, nvRetBase, nvRetBoost);
        return false;
    }
}
bool infoGpuProcessorClk(NvPhysicalGpuHandle handle, char* s)   // get GPU public clock "Processor clock"
{
    if (nvRetValue == NVAPI_OK)
    {
        unsigned int currentKhz = 0;
        unsigned int baseKhz = 0;
        unsigned int boostKhz = 0;
        if (nvapiData.gpuClocks.currentClk.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].bIsPresent)
        {
            currentKhz = nvapiData.gpuClocks.currentClk.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].frequency;
        }
        if (nvapiData.gpuClocks.baseClk.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].bIsPresent)
        {
            baseKhz = nvapiData.gpuClocks.baseClk.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].frequency;
        }
        if (nvapiData.gpuClocks.boostClk.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].bIsPresent)
        {
            boostKhz = nvapiData.gpuClocks.boostClk.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].frequency;
        }
        printClocks(s, currentKhz, baseKhz, boostKhz);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU public clocks failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuVideoClk(NvPhysicalGpuHandle handle, char* s)       // get GPU public clock "Video clock"
{
    if (nvRetValue == NVAPI_OK)
    {
        unsigned int currentKhz = 0;
        unsigned int baseKhz = 0;
        unsigned int boostKhz = 0;
        if (nvapiData.gpuClocks.currentClk.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].bIsPresent)
        {
            currentKhz = nvapiData.gpuClocks.currentClk.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].frequency;
        }
        if (nvapiData.gpuClocks.baseClk.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].bIsPresent)
        {
            baseKhz = nvapiData.gpuClocks.baseClk.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].frequency;
        }
        if (nvapiData.gpuClocks.boostClk.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].bIsPresent)
        {
            boostKhz = nvapiData.gpuClocks.boostClk.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].frequency;
        }
        printClocks(s, currentKhz, baseKhz, boostKhz);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU public clocks failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuMemoryClk(NvPhysicalGpuHandle handle, char* s)      // get GPU public clock "Memory clock"
{
    if (nvRetValue == NVAPI_OK)
    {
        unsigned int currentKhz = 0;
        unsigned int baseKhz = 0;
        unsigned int boostKhz = 0;
        if (nvapiData.gpuClocks.currentClk.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].bIsPresent)
        {
            currentKhz = nvapiData.gpuClocks.currentClk.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;
        }
        if (nvapiData.gpuClocks.baseClk.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].bIsPresent)
        {
            baseKhz = nvapiData.gpuClocks.baseClk.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;
        }
        if (nvapiData.gpuClocks.boostClk.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].bIsPresent)
        {
            boostKhz = nvapiData.gpuClocks.boostClk.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;
        }
        printClocks(s, currentKhz, baseKhz, boostKhz);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU public clocks failed (%d).", nvRetValue);
        return false;
    }
}
bool infoGpuInternalTemperature(NvPhysicalGpuHandle handle, char* s)  // get GPU internal temperature
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvapiData.gpuThermal.version = NV_GPU_THERMAL_SETTINGS_VER;
    nvapiData.gpuThermal.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;
    nvapiData.gpuThermal.sensor[0].controller = NVAPI_THERMAL_CONTROLLER_GPU_INTERNAL;
    nvRetValue = NvAPI_GPU_GetThermalSettings(handle, NVAPI_THERMAL_TARGET_NONE, &nvapiData.gpuThermal);
    if (nvRetValue == NVAPI_OK)
    {
        unsigned int current = nvapiData.gpuThermal.sensor[0].currentTemp;
        unsigned int min = nvapiData.gpuThermal.sensor[0].defaultMinTemp;
        unsigned int max = nvapiData.gpuThermal.sensor[0].defaultMaxTemp;
        snprintf(s, SMALL_STRING, "%dC (min=%dC, max=%dC).", current, min, max);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get GPU internal temperature failed (%d).", nvRetValue);
        return false;
    }

}
bool infoGpuFanTachometer(NvPhysicalGpuHandle handle, char* s)  // get GPU fan tachometer
{
    memset(&nvapiData, 0, sizeof(nvapiData));
    nvRetValue = NvAPI_GPU_GetTachReading(handle, &nvapiData.gpuFanTachometer);
    if (nvRetValue == NVAPI_OK)
    {
        snprintf(s, SMALL_STRING, "%d RPM.", nvapiData.gpuFanTachometer);
        return true;
    }
    else
    {
        snprintf(s, SMALL_STRING, "Get fan tachometer failed (%d).", nvRetValue);
        return false;
    }
}

// entry definition for list of information handlers
struct NVAPI_INFO_ENTRY
{
    const char* name;
    bool (*printHandler)(NvPhysicalGpuHandle, char*);
};
// list of information handlers
NVAPI_INFO_ENTRY sequence[] =
{
    { "GPU name"                                 , infoGpuName                   },
    { "Dedicated video memory"                   , infoGpuMemoryTotal            },
    { "Available dedicated video memory"         , infoGpuMemoryAvailable        },
    { "Current available dedicated video memory" , infoGpuMemoryCurrentAvailable },
    { "Shared system memory"                     , infoGpuMemoryShared           },
    { "System video memory"                      , infoGpuMemorySystem           },
    { "Dedicated video memory utilization"       , infoGpuMemoryUtilization      },
    { "GPU utilization"                          , infoGpuUtilization            },
    { "GPU core count"                           , infoGpuCoreCount              },
    { "GPU PCI device ID"                        , infoGpuPciDevice              },
    { "GPU PCI subsystem ID"                     , infoGpuPciSubsystem           },
    { "GPU PCI revision ID"                      , infoGpuPciRevision            },
    { "GPU PCI extended device ID"               , infoGpuPciExtendedDevice      },
    { "Video BIOS revision"                      , infoGpuVideoBiosRevision      },
    { "Video BIOS OEM revision"                  , infoGpuVideoBiosOemRevision   },
    { "Video BIOS version string"                , infoGpuVideoBiosString        },
    { "GPU current downstream PCIe width"        , infoGpuPcieWidth              },
    { "Graphics clock"                           , infoGpuGraphicsClk            },
    { "Processor clock"                          , infoGpuProcessorClk           },
    { "Video clock"                              , infoGpuVideoClk               },
    { "Memory clock"                             , infoGpuMemoryClk              },
    { "GPU internal temperature"                 , infoGpuInternalTemperature    },
    { "GPU fan tachometer"                       , infoGpuFanTachometer          }
};
const int SEQUENCE_COUNT = sizeof(sequence) / sizeof(NVAPI_INFO_ENTRY);

// application entry point
int main(int argc, char* argv[])
{
    int status = -1;
    if (parmInit())
    {
        snprintf(buffer, SMALL_STRING, "\r\n%s", BUILD_STRING);
        parmName(buffer);
        NvAPI_Status nvRetValue = NVAPI_ERROR;
        // Before any of the NVAPI functions can be used NvAPI_Initialize() must be called.
        nvRetValue = NvAPI_Initialize();
        if (nvRetValue == NVAPI_OK)
        {
            snprintf(buffer, SMALL_STRING, "NVAPI initialization OK.");
            parmName(buffer);
            NvU32 uiNumGPUs = 0;
            NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS];
            // Get the number of GPUs and actual GPU handles
            nvRetValue = NvAPI_EnumPhysicalGPUs(nvGPUHandle, &uiNumGPUs);
            if (nvRetValue == NVAPI_OK)
            {
                if (uiNumGPUs != 0)
                {
                    snprintf(buffer, SMALL_STRING, "NVIDIA GPU detected: %d.", uiNumGPUs);
                    parmName(buffer);
                    // Iterate through all of the detected compatible GPUs.
                    for (unsigned int iDevIDX = 0; iDevIDX < uiNumGPUs; iDevIDX++)
                    {
                        cout << endl;
                        snprintf(buffer, SMALL_STRING, "Device %d", iDevIDX);
                        parmGroup(buffer);
                        for (int i = 0; i < SEQUENCE_COUNT; i++)
                        {
                            if ((sequence[i].printHandler)(nvGPUHandle[iDevIDX], buffer))
                            {
                                parmValue(sequence[i].name, buffer);
                            }
                            else
                            {
                                parmError(buffer);
                            }
                        }
                        cout << endl;
                    }
                }
                else
                {
                    snprintf(buffer, SMALL_STRING, "NVIDIA GPU not detected.");
                    parmError(buffer);
                }
            }
            else
            {
                snprintf(buffer, SMALL_STRING, "NVIDIA GPU detectedion error.");
                parmError(buffer);
            }
            // Unload NVAPI use.
            nvRetValue = NvAPI_Unload();
            snprintf(buffer, SMALL_STRING, "NVAPI unload called (%d).", nvRetValue);
            parmName(buffer);
            status = 0;
        }
        else
        {
            snprintf(buffer, SMALL_STRING, "NVAPI initialization failed (%d).", nvRetValue);
            parmError(buffer);
            status = 1;
        }
    }
    else
    {
        snprintf(buffer, SMALL_STRING, "Color console initialization failed.");
        cout << buffer << endl;
        status = 2;
    }
    // Done.
    snprintf(buffer, SMALL_STRING, "Done (%d).", status);
    cout << buffer << endl;
    return status;
}
