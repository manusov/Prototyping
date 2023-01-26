/* ----------------------------------------------------------------------------------------
Devices enumerator.
---------------------------------------------------------------------------------------- */

#include "Enumerator.h"

 UINT EnumerateSystem(LPTSTR pBase, UINT64 pMax, PGROUPSORT sortControl, UINT SORT_CONTROL_LENGTH)
{
     int enumCount = 0;
     HDEVINFO hDevinfo = SetupDiGetClassDevsEx(NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES, NULL, NULL, NULL);
     if ((hDevinfo) && (hDevinfo != INVALID_HANDLE_VALUE))
     {
         SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;
         devInfoListDetail.cbSize = sizeof(devInfoListDetail);
         if (SetupDiGetDeviceInfoListDetail(hDevinfo, &devInfoListDetail))
         {
             SP_DEVINFO_DATA devInfo;
             devInfo.cbSize = sizeof(devInfo);
             for (DWORD devIndex = 0; SetupDiEnumDeviceInfo(hDevinfo, devIndex, &devInfo); devIndex++)
             {
                 TCHAR devID[MAX_DEVICE_ID_LEN];
                 BOOL b = TRUE;
                 SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;

                 // Get device enumeration path string.
                 devInfoListDetail.cbSize = sizeof(devInfoListDetail);
                 if ((!SetupDiGetDeviceInfoListDetail(hDevinfo, &devInfoListDetail)) ||
                     (CM_Get_Device_ID_Ex(devInfo.DevInst, devID, MAX_DEVICE_ID_LEN, 0, devInfoListDetail.RemoteMachineHandle) != CR_SUCCESS))
                 {
                     StringCchCopy(devID, ARRAYSIZE(devID), TEXT("?"));
                     b = FALSE;  // this flag yet redundant
                 }

                 LPCSTR pathString = NULL;
                 LPCSTR nameString = NULL;
                 LPCSTR summaryString = NULL;
                 
                 // Copy device enumeration path string.
                 size_t length = strlen(devID);
                 strncpy_s(pBase, (size_t)pMax, devID, length);
                 pathString = pBase;
                 pBase += (length + 1);
                 pMax -= (length + 1);
                 if (pMax <= 0) break;

                 // Get device friendly description string (name).
                 LPTSTR descr = GetDeviceDescription(hDevinfo, &devInfo);
                 if (descr)
                 {
                     // Copy device friendly description string.
                     size_t length = strlen(descr);
                     strncpy_s(pBase, (size_t)pMax, descr, length);
                     nameString = pBase;
                     pBase += (length + 1);
                     pMax -= (length + 1);
                     delete[] descr;
                 }
                 if (pMax <= 0) break;

                 // Build and copy summary (name and path) string.
                 UINT adv = 0;
                 if (nameString && pathString)
                 {
                     adv = snprintf(pBase, (size_t)pMax, "%s  ( %s )", nameString, pathString);
                 }
                 else if (nameString)
                 {
                     adv = snprintf(pBase, (size_t)pMax, "%s", nameString);
                 }
                 else if (pathString)
                 {
                     adv = snprintf(pBase, (size_t)pMax, "%s", pathString);
                 }
                 else
                 {
                     adv = snprintf(pBase, (size_t)pMax, "< No description strings >");
                 }
                 summaryString = pBase;
                 pBase += adv;
                 pMax -= adv;
                 if (pMax <= 0) break;
                 *pBase = 0;
                 pBase++;
                 pMax--;
                 if (pMax <= 0) break;

                 // Classify string pair entry and add to sort control list.
                 if (pathString)
                 {
                     PGROUPSORT p = sortControl;
                     BOOL recognized = FALSE;
                     for (UINT i = 0; i < (SORT_CONTROL_LENGTH - 1); i++)  // -1 because last entry is "OTHER"
                     {
                         // Build pattern for comparision.
                         TCHAR pattern[MAX_DEVICE_ID_LEN + 2];
                         LPTSTR patternTemp = pattern;
                         size_t patternLength = strlen(p->groupPattern);
                         strncpy_s(patternTemp, MAX_DEVICE_ID_LEN, p->groupPattern, patternLength);
                         patternTemp += patternLength;
                         *(patternTemp++) = '\\';
                         *(patternTemp++) = 0;
                         // Compare pathString and pattern for detect device class.
                         patternLength++;
                         if ( (strlen(pathString) > patternLength) && (!strncmp(pathString, pattern, patternLength)))
                         {   
                             // This fragment executed if strings match.
                             // p->childStrings->push_back(pathString);
                             // p->childStrings->push_back(nameString);
                             p->childStrings->push_back(summaryString);
                             
                             recognized = TRUE;
                             break;
                         }
                         p++;
                     }
                     if (!recognized)

                     {   // This fragment executed if all strings mismatch, classify as "OTHER" device.
                         // p->childStrings->push_back(pathString);
                         // p->childStrings->push_back(nameString);
                         p->childStrings->push_back(summaryString);
                     }
                     enumCount++;
                 }
             }

             // Terminate sequence of strings.
             if (pMax < 0)
             {
                 enumCount = 0;  // Invalidate results if buffer overflows.
             }
         }
         if (!SetupDiDestroyDeviceInfoList(hDevinfo)) enumCount = 0;
     }
     return enumCount;
}

LPTSTR GetDeviceDescription(_In_ HDEVINFO Devs, _In_ PSP_DEVINFO_DATA DevInfo)
/*++

Routine Description:

    Return a string containing a description of the device, otherwise NULL
    Always try friendly name first

Arguments:

    Devs    )_ uniquely identify device
    DevInfo )

Return Value:

    string containing description

--*/

{
    LPTSTR desc;
    desc = GetDeviceStringProperty(Devs, DevInfo, SPDRP_FRIENDLYNAME);
    if (!desc) {
        desc = GetDeviceStringProperty(Devs, DevInfo, SPDRP_DEVICEDESC);
    }
    return desc;
}

LPTSTR GetDeviceStringProperty(_In_ HDEVINFO Devs, _In_ PSP_DEVINFO_DATA DevInfo, _In_ DWORD Prop)
/*++

Routine Description:

    Return a string property for a device, otherwise NULL

Arguments:

    Devs    )_ uniquely identify device
    DevInfo )
    Prop     - string property to obtain

Return Value:

    string containing description

--*/

{
    LPTSTR buffer;
    DWORD size;
    DWORD reqSize;
    DWORD dataType;
    DWORD szChars;

    size = 1024; // initial guess
    buffer = new TCHAR[(size / sizeof(TCHAR)) + 1];
    if (!buffer) {
        return NULL;
    }
    while (!SetupDiGetDeviceRegistryProperty(Devs, DevInfo, Prop, &dataType, (LPBYTE)buffer, size, &reqSize)) {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto failed;
        }
        if (dataType != REG_SZ) {
            goto failed;
        }
        size = reqSize;
        delete[] buffer;
        buffer = new TCHAR[(size / sizeof(TCHAR)) + 1];
        if (!buffer) {
            goto failed;
        }
    }
    szChars = reqSize / sizeof(TCHAR);
    buffer[szChars] = TEXT('\0');
    return buffer;

failed:
    if (buffer) {
        delete[] buffer;
    }
    return NULL;
}
