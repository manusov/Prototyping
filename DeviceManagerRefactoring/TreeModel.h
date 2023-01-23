/* ----------------------------------------------------------------------------------------
Class for tree data model storage:
icons, text strings, structures with tree nodes descriptors.
Include storage for visualized model.
---------------------------------------------------------------------------------------- */

#pragma once
#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <windows.h>
#include "resource.h"

// Icons indexes must be sequental, even if icon resources identifiers not sequental.
typedef enum {
	ID_APP,
	ID_CLOSED, ID_CLOSED_LIGHT, ID_OPENED, ID_OPENED_LIGHT,
	ID_ACPI, ID_ACPI_HAL, ID_AUDIO, ID_AUDIO_IO, ID_BLUETOOTH, ID_DISPLAYS, ID_HID, ID_IDE,
	ID_IMAGE_PROCESSING, ID_KEYBOARDS, ID_MASS_STORAGE, ID_MOBILE_DEVICES, ID_MOUSES,
	ID_NETWORK, ID_OTHER, ID_PCI, ID_PCI_IDE, ID_PORTS, ID_PRINT, ID_PROCESSORS,
	ID_ROOT_ENUMERATOR, ID_SCSI, ID_SECURITY, ID_SOFT_COMPONENTS, ID_SOFT_DEVICES,
	ID_SYSTEM_TREE, ID_THIS_COMPUTER, ID_UEFI, ID_UM_BUS, ID_USB, ID_USB_STORAGE,
	ID_VIDEO_ADAPTERS
} ICON_INDEXES;
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

class TreeModel
{
public:
	TreeModel();
	~TreeModel();
	// Service functions for tree builder.
	UINT GetIconIdByIndex(int);
	HICON GetIconHandleByIndex(int);
	LPCSTR GetIconNameByIndex(int);
	// Getter and setter for pointer to tree.
	PTREENODE GetTree();
	void SetTree(PTREENODE);
	// Getter and setter for pointer to tree base coordinates.
	POINT GetBase();
	void SetBase(POINT);
	// Getters for emulated constant tree parameters
	PTREECHILDS getNodeChilds();
	int getNodeChildsCount();
	// Getter for initialization status
	BOOL getInitStatus();
private:
	static const char* ICON_NAMES[];
	static const int ICON_IDS[];
	static const int ICON_COUNT;
	static HICON iconHandles[];
	static PTREENODE tree;
	static POINT base;
	// Pointer to array with child nodes per each category,
	// for emulated constant tree parameters.
	static TREECHILDS nodeChilds[];
	static int NODE_CHILDS_COUNT;
	// Resources initialization status
	static BOOL initStatus;
};

#endif  // TREEMODEL_H
