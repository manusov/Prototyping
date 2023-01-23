/* ----------------------------------------------------------------------------------------
Class for build constant tree as linked list of nodes descriptors.
At application debug, this class used as tree builder for data emulation.
At real system information show, this is parent class.
---------------------------------------------------------------------------------------- */

#include "TreeController.h"

TreeController::TreeController()
{
	// Reserved functionality.
}
TreeController::~TreeController()
{
	// Reserved functionality.
}
PTREENODE TreeController::BuildTree()
{
	pTreeBase = NULL;
	if (pModel)
	{
		int nodeChildsCount = pModel->getNodeChildsCount();
		pTreeBase = (PTREENODE)malloc(TREE_COUNT_ALLOCATED * sizeof(TREENODE));
		PTREENODE p = pTreeBase;
		if (p)
		{
			// Root node "This computer"
			p->hNodeIcon = pModel->GetIconHandleByIndex(ID_THIS_COMPUTER);
			p->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
			p->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);
			p->szNodeName = pModel->GetIconNameByIndex(ID_THIS_COMPUTER);
			p->childLink = p + 1;
			p->childCount = nodeChildsCount;
			p->clickArea.left = 0;
			p->clickArea.top = 0;
			p->clickArea.right = 0;
			p->clickArea.bottom = 0;
			p->openable = 1;
			p->opened = 1;
			p->marked = 1;
			p->prevMouse = 0;
			// Cycle for tree nodes = categories.
			PTREECHILDS pSrcChilds = pModel->getNodeChilds();
			PTREENODE pDstChilds = pTreeBase + nodeChildsCount + 1;
			for (int i = 0; i < nodeChildsCount; i++)
			{
				int childCount = pSrcChilds->childCount;
				p++;
				p->hNodeIcon = pModel->GetIconHandleByIndex(pSrcChilds->childIconIndexes[0]);
				p->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
				p->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);
				p->szNodeName = pModel->GetIconNameByIndex(pSrcChilds->childIconIndexes[0]);
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
					pDstChilds->hNodeIcon = pModel->GetIconHandleByIndex(*pChildIconsIndexes++);
					pDstChilds->hClosedIcon = pModel->GetIconHandleByIndex(ID_CLOSED);
					pDstChilds->hOpenedIcon = pModel->GetIconHandleByIndex(ID_OPENED);
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
	}
	return pTreeBase;
}
void TreeController::ReleaseTree()
{
	if (pTreeBase)
	{
		free(pTreeBase);
		pTreeBase = NULL;
	}
}
void TreeController::SetAndInitModel(TreeModel* p)
{
	pModel = p;
}

PTREENODE TreeController::pTreeBase = NULL;
TreeModel* TreeController::pModel = NULL;