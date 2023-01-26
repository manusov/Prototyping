/* ----------------------------------------------------------------------------------------
Class for build constant tree as linked list of nodes descriptors.
At application debug, this class used as tree builder for data emulation.
At real system information show, this is parent class.
---------------------------------------------------------------------------------------- */

#pragma once
#ifndef TREECONTROLLER_H
#define TREECONTROLLER_H

#include <windows.h>
#include <vector>
#include "TreeModel.h"

#define TREE_COUNT_ALLOCATED 1000

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

class TreeController
{
public:
	TreeController();
	~TreeController();
	// Don't use constructor and destructor for build and release tree, because
	// dynamical rebuild with model change and partial changes can be required.
	void SetAndInitModel(TreeModel* p);
	virtual PTREENODE BuildTree();
	virtual void ReleaseTree();
protected:
	static PTREENODE pTreeBase;
	static TreeModel* pModel;
};

#endif  // TREECONTROLLER_H

