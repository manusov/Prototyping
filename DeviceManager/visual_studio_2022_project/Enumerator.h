/* ----------------------------------------------------------------------------------------
Devices enumerator.
---------------------------------------------------------------------------------------- */

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
#include "TreeController.h"

UINT EnumerateSystem(LPTSTR pBase, UINT64 pMax, PGROUPSORT sortControl, UINT SORT_CONTROL_LENGTH);
LPTSTR GetDeviceStringProperty(_In_ HDEVINFO Devs, _In_ PSP_DEVINFO_DATA DevInfo, _In_ DWORD Prop);
LPTSTR GetDeviceDescription(_In_ HDEVINFO Devs, _In_ PSP_DEVINFO_DATA DevInfo);

#endif  // ENUMERATOR_H

