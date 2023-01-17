/*

Заголовочный файл для данных и процедур, используемых при формировании информации о
конфигурации системы в виде дерева (tree). На данном этапе, для отладки визуализации
дерева, используется эмуляция на основе предопределенных данных, без сканирования
системной конфигурации.
По классификации TreeHolder/TreeBuilder/TreeViewer это Holder.
По классификации Model/View/Controller это Model.

*/

#pragma once
#ifndef APPMODEL_H
#define APPMODEL_H

#include <windows.h>
#include <windowsx.h>
#include "resource.h"

// This required because icon ids can be not sequental,
// This transit enumeration can be removed at fully-controlled pure-assembler source,
// if "IDI_" and "ID_" indexes sequental.
typedef enum {
	ID_APPLICATION_TITLE, ID_ITEM_CLOSED, ID_ITEM_OPENED, ID_ITEM_CLOSED_LIGHT, ID_ITEM_OPENED_LIGHT,
	ID_ACPI, ID_ACPI_HAL, ID_PCI, ID_SCSI, ID_USER_MODE_BUS, ID_SYSTEM_TREE, ID_OTHER_DEVICES,
	ID_BLUETOOTH, ID_AUDIOINOUT, ID_VIDEOADAPTER, ID_EMBEDDEDSOFT, ID_DISKDRIVE,
	ID_AUDIO, ID_KEYBOARD, ID_SOFTCOMPONENTS, ID_COMPUTER, ID_IDECTRL,
	ID_USBCTRL, ID_STORAGECTRL, ID_MONITOR, ID_MOUSE, ID_PRINTER,
	ID_MOBILEDEVICES, ID_PORTS, ID_SOFTDEVICES, ID_PROCESSOR, ID_NETWORK,
	ID_SYSTEM, ID_HID, ID_USBDEVICES, ID_SECURITY, ID_IMAGEDEVICES
} TREEICONS;

#define ID_START ID_BLUETOOTH
#define TREE_COUNT_CHILD 25
#define TREE_COUNT_TOTAL (TREE_COUNT_CHILD + 1)
#define TREE_COUNT_ALLOCATED 1000

// Structure for device node, used by TreeBuilder as DESTINATION data.
typedef struct TREENODE {
	HICON hNodeIcon;      // Icon handle for this device node, this and 2 next handles must be pre-loaded by LoadImage function, handle = F(resource id).
	HICON hClosedIcon;    // Icon handle for CLOSED state of device node, left to right arrow.
	HICON hOpenedIcon;    // Icon handle for CLOSED state of device node, up to down arrow.
	LPCSTR szNodeName;    // Pointer to node name null-terminated text string.
	TREENODE* childLink;  // Pointer to sequence of child nodes of this node, NULL if no child nodes.
	UINT childCount;      // Length of sequence of child nodes of this node.
	RECT clickArea;       // Area for mouse click detection: Xleft, Yup, Xright, Ydown, build by GUI routine, not by caller.
	UINT openable : 1;    // Binary flag: last level node, cannot be opened (0) or contain childs and can be opened (1)
	UINT opened : 1;      // Binary flag: node closed (0) or opened (1).
	UINT marked : 1;      // Binary flag: node not marked (0) or marked by keyboard selection (1).
	UINT prevMouse : 1;   // Binary flag: previous mouse cursor position status, outside open-close icon (0) or inside (1).
} *PTREENODE;

// Structure for list childs of device node, used by TreeBuilder as EMULATED SOURCE data.
typedef struct TREECHILDS {
	int childCount;             // Number of childs.
	int* childIconIndexes;      // Pointer to array of integers: indexes of child icons.
	LPCSTR* childNamesStrings;  // Pointer to array of pointers to strings: names of child nodes.
} *PTREECHILDS;

// Load and unload icons for device manager.
bool InitializeTree();
void DeInitializeTree();

// Service functions for tree builder.
UINT GetIconIdByIndex(UINT);
HICON GetIconHandleByIndex(UINT);
LPCSTR GetIconNameByIndex(UINT);

// Getter and setter for pointer to tree and base coordinates.
PTREENODE GetTree();
void SetTree(PTREENODE);
POINT GetBase();
void SetBase(POINT);

// Pointer to array with child nodes per each category.
TREECHILDS nodeChilds[];

#endif  // APPMODEL_H

