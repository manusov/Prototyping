/*

Заголовочный файл для енумератора устройств.
Special thanks to:
https://github.com/microsoft/Windows-driver-samples/tree/main/setup/devcon

*/

#pragma once
#ifndef ENUMERATOR_H
#define ENUMERATOR_H

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <strsafe.h>

UINT EnumerateSystem(LPTSTR pBase, UINT64 pMax, PGROUPSORT sortControl, UINT SORT_CONTROL_LENGTH);
LPTSTR GetDeviceStringProperty(_In_ HDEVINFO Devs, _In_ PSP_DEVINFO_DATA DevInfo, _In_ DWORD Prop);
LPTSTR GetDeviceDescription(_In_ HDEVINFO Devs, _In_ PSP_DEVINFO_DATA DevInfo);

#endif  // ENUMERATOR_H

