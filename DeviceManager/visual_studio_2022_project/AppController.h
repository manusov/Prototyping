/*

Заголовочный файл для процедуры формирования последовательности структур в оперативной памяти.
Последовательность используется для представления информации GUI окна диспетчера устройств в
виде дерева (tree).
По классификации TreeHolder/TreeBuilder/TreeViewer это Builder.
По классификации Model/View/Controller это Controller.

*/

#pragma once
#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <windows.h>
#include <windowsx.h>
#include <vector>

// "G_" means groups.
typedef enum {
	G_HTREE, G_ROOT, G_SWD,
	G_ACPI, G_ACPI_HAL, G_UEFI, G_SCSI, G_STORAGE, G_HID,
	G_PCI, G_USB, G_USBSTOR, G_BTH, G_DISPLAY, G_HDAUDIO,
	G_OTHER
} TREEGROUPS;

// Structure for classifying and sorting system enumeration results by groups
// Note devices names get from WinAPI, groups names get from this structures.
typedef struct GROUPSORT {
	LPCSTR groupPattern;                // Pattern for detect device ID string type.
	LPCSTR groupName;                   // String for name of this group.
	int iconIndex;                      // Index of icon from icon pool for this group and childs.
	std::vector<LPCSTR>* childStrings;  // Pointer to vector with sequence of pointers to null terminated child strings (pairs).
} *PGROUPSORT;

PTREENODE BuildEmulatedTree();
PTREENODE BuildSystemTree();
void ReleaseSystemTree();

#endif  // APPCONTROLLER_H
