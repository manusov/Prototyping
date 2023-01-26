/* ----------------------------------------------------------------------------------------
Class for scan system information by WinAPI.
This class builds system information tree as linked list of nodes descriptors.
At application debug, this class not used.
At real system information show, this is child class of TreeController class.
---------------------------------------------------------------------------------------- */

#include "TreeController.h"
#include "TreeControllerSys.h"

TreeControllerSys::TreeControllerSys()
{
	// Reserved functionality.
}
TreeControllerSys::~TreeControllerSys()
{
	// Reserved functionality.
}
PTREENODE TreeControllerSys::BuildTree()
{
	// Build sequence of strings.
	pEnumBase = (LPSTR)malloc(SYSTEM_TREE_MEMORY_MAX);
	UINT countEnum = EnumerateSystem(pEnumBase, SYSTEM_TREE_MEMORY_MAX, sortControl, SORT_CONTROL_LENGTH);

	if (countEnum)
	{
		// Build root node - This computer.
		// pTreeBase = (PTREENODE)malloc((countEnum + 1) * sizeof(TREENODE));  // BUG: +1 because root, but required +X because classes.
		pTreeBase = (PTREENODE)malloc(SYSTEM_TREE_MEMORY_MAX);
		//
		PTREENODE p = pTreeBase;
		if (p)
		{
			// Build root node "This computer".
			p->hNodeIcon = pModel->GetIconHandleByIndex(MAIN_SYSTEM_ICON_INDEX);
			p->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
			p->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);
			p->szNodeName = MAIN_SYSTEM_NAME;
			p->childLink = NULL;
			p->childCount = 0;
			p->clickArea.left = 0;
			p->clickArea.top = 0;
			p->clickArea.right = 0;
			p->clickArea.bottom = 0;
			p->openable = 0;
			p->opened = 0;
			p->marked = 1;
			p->prevMouse = 0;

			// Build tree level 1 - classes nodes, childs of tree nodes.
			PGROUPSORT pSortCtrl = sortControl;
			PTREENODE pClassBase = p + 1;
			UINT rootChilds = 0;
			for (UINT i = 0; i < SORT_CONTROL_LENGTH; i++)
			{
				p++;
				p->hNodeIcon = pModel->GetIconHandleByIndex(pSortCtrl->iconIndex);
				p->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
				p->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);
				p->szNodeName = pSortCtrl->groupName;
				p->childLink = NULL;
				p->childCount = 0;
				p->clickArea.left = 0;
				p->clickArea.top = 0;
				p->clickArea.right = 0;
				p->clickArea.bottom = 0;
				p->openable = 0;
				p->opened = 0;
				p->marked = 0;
				p->prevMouse = 0;
				pSortCtrl++;
				rootChilds++;
			}

			// Build tree level 2 - devices nodes, childs of classes nodes.
			pSortCtrl = sortControl;
			for (UINT i = 0; i < rootChilds; i++)
			{
				UINT classChilds = (UINT)(pSortCtrl->childStrings->size());
				if (classChilds)
				{
					PTREENODE pDeviceBase = p + 1;
					std::vector<LPCSTR>::iterator vit = pSortCtrl->childStrings->begin();
					for (UINT j = 0; j < classChilds; j++)
					{
						p++;
						p->hNodeIcon = pModel->GetIconHandleByIndex(pSortCtrl->iconIndex);
						p->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
						p->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);

						LPCSTR s = *vit++;
						if (s)
						{
							p->szNodeName = s;
						}
						else
						{
							p->szNodeName = "?";
						}

						p->childLink = NULL;
						p->childCount = 0;
						p->clickArea.left = 0;
						p->clickArea.top = 0;
						p->clickArea.right = 0;
						p->clickArea.bottom = 0;
						p->openable = 0;
						p->opened = 0;
						p->marked = 0;
						p->prevMouse = 0;
					}

					pClassBase->childLink = pDeviceBase;
					pClassBase->childCount = classChilds;
					pClassBase->openable = 1;
				}
				pClassBase++;
				pSortCtrl++;
			}

			// Update root node "This computer" after childs enumerated.
			if (rootChilds)
			{
				pTreeBase->childLink = pTreeBase + 1;
				pTreeBase->childCount = rootChilds;
				pTreeBase->openable = 1;
				pTreeBase->opened = 1;
			}
		}
	}
	return pTreeBase;
}
void TreeControllerSys::ReleaseTree()
{
	if (pTreeBase) free(pTreeBase);
	if (pEnumBase) free(pEnumBase);
	PGROUPSORT p = sortControl;
	for (UINT i = 0; i < SORT_CONTROL_LENGTH; i++)
	{
		if (p->childStrings)
		{
			delete(p->childStrings);
			p->childStrings = NULL;
			p++;
		}
	}
}
LPCSTR TreeControllerSys::MAIN_SYSTEM_NAME = "This computer";
int TreeControllerSys::MAIN_SYSTEM_ICON_INDEX = ID_THIS_COMPUTER;
GROUPSORT TreeControllerSys::sortControl[] = {
	{ "HTREE"    , "System tree"               , ID_SYSTEM_TREE      , new std::vector<LPCSTR> },
	{ "ROOT"     , "Root enumerator"           , ID_ROOT_ENUMERATOR  , new std::vector<LPCSTR> },
	{ "SWD"      , "Software defined devices"  , ID_SOFT_DEVICES     , new std::vector<LPCSTR> },
	{ "ACPI"     , "ACPI"                      , ID_ACPI             , new std::vector<LPCSTR> },
	{ "ACPI_HAL" , "ACPI HAL"                  , ID_ACPI_HAL         , new std::vector<LPCSTR> },
	{ "UEFI"     , "UEFI"                      , ID_UEFI             , new std::vector<LPCSTR> },
	{ "SCSI"     , "SCSI"                      , ID_SCSI             , new std::vector<LPCSTR> },
	{ "STORAGE"  , "Mass storage"              , ID_MASS_STORAGE     , new std::vector<LPCSTR> },
	{ "HID"      , "Human interface devices"   , ID_HID              , new std::vector<LPCSTR> },
	{ "PCI"      , "PCI"                       , ID_PCI              , new std::vector<LPCSTR> },
	{ "USB"      , "USB"                       , ID_USB              , new std::vector<LPCSTR> },
	{ "USBSTOR"  , "USB mass storage"          , ID_USB_STORAGE      , new std::vector<LPCSTR> },
	{ "BTH"      , "Bluetooth"                 , ID_BLUETOOTH        , new std::vector<LPCSTR> },
	{ "DISPLAY"  , "Video displays"            , ID_DISPLAYS         , new std::vector<LPCSTR> },
	{ "HDAUDIO"  , "High definition audio"     , ID_AUDIO            , new std::vector<LPCSTR> },
	{ "UMB"      , "User mode bus"             , ID_UM_BUS           , new std::vector<LPCSTR> },
	{ "IDE"      , "IDE/ATAPI controllers"     , ID_IDE              , new std::vector<LPCSTR> },
	{ "PCIIDE"   , "PCI IDE/ATAPI controllers" , ID_PCI_IDE          , new std::vector<LPCSTR> },
	{ "OTHER"    , "Other devices types"       , ID_OTHER            , new std::vector<LPCSTR> }
};
const UINT TreeControllerSys::SORT_CONTROL_LENGTH = sizeof(sortControl) / sizeof(GROUPSORT);
LPSTR TreeControllerSys::pEnumBase = NULL;