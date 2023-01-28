/* ----------------------------------------------------------------------------------------
Class for build constant tree as linked list of nodes descriptors.
Version with extended nodes nesting level.
At application debug, this class used as tree extended version builder for data emulation.
At real system information show, this class not used.
---------------------------------------------------------------------------------------- */

#include "TreeControllerExt.h"

TreeControllerExt::TreeControllerExt()
{
	// Reserved functionality.
}

TreeControllerExt::~TreeControllerExt()
{
	// Reserved functionality.
}

PTREENODE TreeControllerExt::BuildTree()
{
	pTreeBase = NULL;
	if (pModel)
	{
		pTreeBase = (PTREENODE)malloc(TREE_COUNT_ALLOCATED * 3 * sizeof(TREENODE));
		
		PTREENODE pDst1 = pTreeBase;
		PTREENODE pDst2 = pTreeBase + TREE_COUNT_ALLOCATED;
		PTREENODE pDst3 = pTreeBase + TREE_COUNT_ALLOCATED * 2;
		
		int nCount1 = pModel->getNodeChildsCount();
		// 0
		if (pDst1)
		{
			// Root node "This computer"
			pDst1->hNodeIcon = pModel->GetIconHandleByIndex(ID_THIS_COMPUTER);
			pDst1->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
			pDst1->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);
			pDst1->szNodeName = pModel->GetIconNameByIndex(ID_THIS_COMPUTER);
			pDst1->childLink = pDst1 + 1;
			pDst1->childCount = nCount1;
			pDst1->clickArea.left = 0;
			pDst1->clickArea.top = 0;
			pDst1->clickArea.right = 0;
			pDst1->clickArea.bottom = 0;
			pDst1->openable = 1;
			pDst1->opened = 1;
			pDst1->marked = 1;
			pDst1->prevMouse = 0;
			// Cycle for tree nodes = categories.
			PTREECHILDS pSrc = pModel->getNodeChilds();
//			PTREENODE pDst2 = pTreeBase + nCount1 + 1;
			// 1
			for (int i = 0; i < nCount1; i++)
			{
				int nCount2 = pSrc->childCount;
				pDst1++;
				pDst1->hNodeIcon = pModel->GetIconHandleByIndex(pSrc->childIconIndexes[0]);
				pDst1->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
				pDst1->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);
				pDst1->szNodeName = pModel->GetIconNameByIndex(pSrc->childIconIndexes[0]);
				pDst1->childLink = pDst2;
				pDst1->childCount = nCount2;
				pDst1->clickArea.left = 0;
				pDst1->clickArea.top = 0;
				pDst1->clickArea.right = 0;
				pDst1->clickArea.bottom = 0;
				pDst1->openable = 1;
				pDst1->opened = 0;
				pDst1->marked = 0;
				pDst1->prevMouse = 0;
				// Cycle for sub-trees nodes = devices in each categories.
				LPCSTR* pChildNamesStrings = pSrc->childNamesStrings;
				int* pChildIconsIndexes = pSrc->childIconIndexes;
//				PTREENODE pDst3 = pDst2 + (size_t)(nCount1 * nCount2);
				int nCount3 = 10;  // Yet const for debug.
				// 2
				for (int j = 0; j < nCount2; j++)
				{
					pDst2->hNodeIcon = pModel->GetIconHandleByIndex(*pChildIconsIndexes++);
					pDst2->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
					pDst2->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);
					pDst2->szNodeName = *pChildNamesStrings++;
					pDst2->childLink = pDst3;
					pDst2->childCount = nCount3;
					pDst2->clickArea.left = 0;
					pDst2->clickArea.top = 0;
					pDst2->clickArea.right = 0;
					pDst2->clickArea.bottom = 0;
					pDst2->openable = 1;         // last from openables
					pDst2->opened = 0;
					pDst2->marked = 0;
					pDst2->prevMouse = 0;
					// 3
					for (int k = 0; k < nCount3; k++)
					{
						pDst3->hNodeIcon = pDst2->hNodeIcon;
						pDst3->hClosedIcon = pDst2->hClosedIcon;
						pDst3->hOpenedIcon = pDst2->hOpenedIcon;
						pDst3->szNodeName = pDst2->szNodeName;
						pDst3->childLink = NULL;
						pDst3->childCount = 0;
						pDst3->clickArea.left = 0;
						pDst3->clickArea.top = 0;
						pDst3->clickArea.right = 0;
						pDst3->clickArea.bottom = 0;
						pDst3->openable = 0;         // first from non-openables
						pDst3->opened = 0;
						pDst3->marked = 0;
						pDst3->prevMouse = 0;
						pDst3++;
					}
					// 3
					pDst2++;
				}
				// 2
				pSrc++;
			}
			// 1
		}
		// 0
	}
	return pTreeBase;
}
