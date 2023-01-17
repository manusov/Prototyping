/*

Процедура для формирования последовательности структур в оперативной памяти, используемой
для представления информации GUI окна диспетчера устройств в виде дерева (tree).
По классификации TreeHolder/TreeBuilder/TreeViewer это Builder.
По классификации Model/View/Controller это Controller.

*/

#include "AppModel.h"
#include "AppView.h"
#include "AppController.h"
#include "Enumerator.h"

PTREENODE pTreeBase = NULL;
LPSTR pEnumBase = NULL;

PTREENODE BuildEmulatedTree()
{
	pTreeBase = (PTREENODE)malloc(TREE_COUNT_ALLOCATED * sizeof(TREENODE));
	PTREENODE p = pTreeBase;
	if (p)
	{
		// Root node "This computer"
		p->hNodeIcon = GetIconHandleByIndex(ID_COMPUTER);
		p->hClosedIcon = GetIconHandleByIndex(ID_ITEM_CLOSED);
		p->hOpenedIcon = GetIconHandleByIndex(ID_ITEM_OPENED);
		p->szNodeName = GetIconNameByIndex(ID_COMPUTER);
		p->childLink = p + 1;
		p->childCount = TREE_COUNT_CHILD;
		p->clickArea.left = 0;
		p->clickArea.top = 0;
		p->clickArea.right = 0;
		p->clickArea.bottom = 0;
		p->openable = 1;
		p->opened = 1;
		p->marked = 1;
		p->prevMouse = 0;
		// Cycle for tree nodes = categories.
		PTREECHILDS pSrcChilds = nodeChilds;
		PTREENODE pDstChilds = pTreeBase + TREE_COUNT_TOTAL;
		for (int i = 0; i < TREE_COUNT_CHILD; i++)
		{
			int childCount = pSrcChilds->childCount;
			p++;
			p->hNodeIcon = GetIconHandleByIndex(ID_START + i);
			p->hClosedIcon = GetIconHandleByIndex(ID_ITEM_CLOSED);
			p->hOpenedIcon = GetIconHandleByIndex(ID_ITEM_OPENED);
			p->szNodeName = GetIconNameByIndex(ID_START + i);
			p->childLink = pDstChilds;
			p->childCount = childCount;
			p->clickArea.left = 0;
			p->clickArea.top = 0;
			p->clickArea.right = 0;
			p->clickArea.bottom = 0;
			p->openable = 1;
			p->opened = 0;
			p->marked = 0;
			p->prevMouse = 0;
			// Cycle for sub-trees nodes = devices in each categories.
			LPCSTR* pChildNamesStrings = pSrcChilds->childNamesStrings;
			int* pChildIconsIndexes = pSrcChilds->childIconIndexes;
			for (int j = 0; j < childCount; j++)
			{
				pDstChilds->hNodeIcon = GetIconHandleByIndex(*pChildIconsIndexes++);
				pDstChilds->hClosedIcon = GetIconHandleByIndex(ID_ITEM_CLOSED);
				pDstChilds->hOpenedIcon = GetIconHandleByIndex(ID_ITEM_OPENED);
				pDstChilds->szNodeName = *pChildNamesStrings++;
				pDstChilds->childLink = NULL;
				pDstChilds->childCount = 0;
				pDstChilds->clickArea.left = 0;
				pDstChilds->clickArea.top = 0;
				pDstChilds->clickArea.right = 0;
				pDstChilds->clickArea.bottom = 0;
				pDstChilds->openable = 0;
				pDstChilds->opened = 0;
				pDstChilds->marked = 0;
				pDstChilds->prevMouse = 0;
				pDstChilds++;
			}
			pSrcChilds++;
		}
	}
	return pTreeBase;
}

#define SYSTEM_TREE_MEMORY_MAX 1024*1024*2
LPCSTR MAIN_SYSTEM_NAME = "This computer";
int MAIN_SYSTEM_ICON_INDEX = ID_COMPUTER;
GROUPSORT sortControl[] = {
	{ "HTREE"    , "System tree"               , ID_SYSTEM_TREE   , new std::vector<LPCSTR> },
	{ "ROOT"     , "Root enumerator"           , ID_SYSTEM        , new std::vector<LPCSTR> },
	{ "SWD"      , "Software defined devices"  , ID_SOFTDEVICES   , new std::vector<LPCSTR> },
	{ "ACPI"     , "ACPI"                      , ID_ACPI          , new std::vector<LPCSTR> },
	{ "ACPI_HAL" , "ACPI HAL"                  , ID_ACPI_HAL      , new std::vector<LPCSTR> },
	{ "UEFI"     , "UEFI"                      , ID_EMBEDDEDSOFT  , new std::vector<LPCSTR> },
	{ "SCSI"     , "SCSI"                      , ID_SCSI          , new std::vector<LPCSTR> },
	{ "STORAGE"  , "Mass storage"              , ID_DISKDRIVE     , new std::vector<LPCSTR> },
	{ "HID"      , "Human interface devices"   , ID_HID           , new std::vector<LPCSTR> },
	{ "PCI"      , "PCI"                       , ID_PCI           , new std::vector<LPCSTR> },
	{ "USB"      , "USB"                       , ID_USBCTRL       , new std::vector<LPCSTR> },
	{ "USBSTOR"  , "USB mass storage"          , ID_USBDEVICES    , new std::vector<LPCSTR> },
	{ "BTH"      , "Bluetooth"                 , ID_BLUETOOTH     , new std::vector<LPCSTR> },
	{ "DISPLAY"  , "Video displays"            , ID_MONITOR       , new std::vector<LPCSTR> },
	{ "HDAUDIO"  , "High definition audio"     , ID_AUDIO         , new std::vector<LPCSTR> },
	{ "UMB"      , "User mode bus"             , ID_USER_MODE_BUS , new std::vector<LPCSTR> },
	{ "IDE"      , "IDE/ATAPI controllers"     , ID_IDECTRL       , new std::vector<LPCSTR> },
	{ "PCIIDE"   , "PCI IDE/ATAPI controllers" , ID_STORAGECTRL   , new std::vector<LPCSTR> },
	{ "OTHER"    , "Other devices types"       , ID_OTHER_DEVICES , new std::vector<LPCSTR> }
};
const UINT SORT_CONTROL_LENGTH = sizeof(sortControl) / sizeof(GROUPSORT);

PTREENODE BuildSystemTree()
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
			p->hNodeIcon = GetIconHandleByIndex(MAIN_SYSTEM_ICON_INDEX);
			p->hClosedIcon = GetIconHandleByIndex(ID_ITEM_CLOSED);
			p->hOpenedIcon = GetIconHandleByIndex(ID_ITEM_OPENED);
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
				p->hNodeIcon = GetIconHandleByIndex(pSortCtrl->iconIndex);
				p->hClosedIcon = GetIconHandleByIndex(ID_ITEM_CLOSED);
				p->hOpenedIcon = GetIconHandleByIndex(ID_ITEM_OPENED);
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
				// 
				// UINT classChilds = (UINT)((pSortCtrl->childStrings->size()) / 2);
				UINT classChilds = (UINT)(pSortCtrl->childStrings->size());
				//
				if (classChilds)
				{
					PTREENODE pDeviceBase = p + 1;
					std::vector<LPCSTR>::iterator vit = pSortCtrl->childStrings->begin();
					for (UINT j = 0; j < classChilds; j++)
					{
						p++;
						p->hNodeIcon = GetIconHandleByIndex(pSortCtrl->iconIndex);
						p->hClosedIcon = GetIconHandleByIndex(ID_ITEM_CLOSED);
						p->hOpenedIcon = GetIconHandleByIndex(ID_ITEM_OPENED);

						// BUG. EVEN STRINGS = PATH, ODD STRINGS = FRIENDLY NAMES. REQUIRED TRANSLATE.
						//
						// LPCSTR s = *vit++;
						// vit++;
						//
						// vit++;
						// LPCSTR s = *vit++;
						//
						LPCSTR s = *vit++;
						//

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

void ReleaseSystemTree()
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
