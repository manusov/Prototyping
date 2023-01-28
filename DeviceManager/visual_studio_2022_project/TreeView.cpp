/* ----------------------------------------------------------------------------------------
Class for create GUI window with tree visualization.
---------------------------------------------------------------------------------------- */

#include "TreeView.h"

TreeView::TreeView()
{
	// Reserved functionality.
}
TreeView::~TreeView()
{
	// Reserved functionality.
}
void TreeView::SetAndInitModel(TreeModel* p)
{
	pModel = p;
}
LRESULT CALLBACK TreeView::AppViewer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// These variables are required by BeginPaint, EndPaint, BitBlt. 
	PAINTSTRUCT ps;              // Temporary storage for paint info
	static HDC hdcScreen;        // DC for entire screen 
	static HDC hdcScreenCompat;  // memory DC for screen 
	static HBITMAP hbmpCompat;   // bitmap handle to old DC 
	static BITMAP bmp;           // bitmap data structure 
	static BOOL fBlt;            // TRUE if BitBlt occurred 
	static BOOL fScroll;         // TRUE if scrolling occurred 
	static BOOL fSize;           // TRUE if fBlt & WM_SIZE 
	// This variable used for horizontal and vertical scrolling both.
	SCROLLINFO si;               // Temporary storage for scroll info
	// These variables are required for horizontal scrolling. 
	static int xMinScroll;       // minimum horizontal scroll value 
	static int xCurrentScroll;   // current horizontal scroll value 
	static int xMaxScroll;       // maximum horizontal scroll value 
	// These variables are required for vertical scrolling. 
	static int yMinScroll;       // minimum vertical scroll value 
	static int yCurrentScroll;   // current vertical scroll value 
	static int yMaxScroll;       // maximum vertical scroll value 
	// This variable are required for adjust scrolling by visualized tree size.
	static RECT treeDimension;
	// This variables are required for nodes selection by TAB key, use background color.
	static BOOL fTab;            // TRUE if selection mode activated
	static PTREENODE openNode;   // This node opened if SPACE, Gray+, Gray- pressed at selection mode
	// This variables are required for text font and background brush
	static HFONT hFont;          // Font handle
	static HBRUSH bgndBrush;     // Brush handle for background
	// This variables are required for restore context
	static HGDIOBJ backupBmp;
	static HBRUSH backupBrush;
	// Window callback procedure entry point.
	switch (uMsg)
	{
	case WM_CREATE:
	{
		// Fill window with background color.
		bgndBrush = CreateSolidBrush(BACKGROUND_BRUSH);
		SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)bgndBrush);
		// Create font
		hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Verdana"));
		// Create a normal DC and a memory DC for the entire 
		// screen. The normal DC provides a snapshot of the 
		// screen contents. The memory DC keeps a copy of this 
		// snapshot in the associated bitmap. 
		hdcScreen = CreateDC("DISPLAY", (PCTSTR)NULL, (PCTSTR)NULL, (CONST DEVMODE*) NULL);
		hdcScreenCompat = CreateCompatibleDC(hdcScreen);
		// Retrieve the metrics for the bitmap associated with the 
		// regular device context. 
		bmp.bmBitsPixel =
			(BYTE)GetDeviceCaps(hdcScreen, BITSPIXEL);
		bmp.bmPlanes = (BYTE)GetDeviceCaps(hdcScreen, PLANES);
		bmp.bmWidth = GetDeviceCaps(hdcScreen, HORZRES);
		bmp.bmHeight = 4096; // GetDeviceCaps(hdcScreen, VERTRES);  // BUG. TOO BIG CONTEXT BUFFER REQUIRED FOR THIS METHOD, BUT OVERFLOW STILL POSSIBLE.
		// The width must be byte-aligned. 
		bmp.bmWidthBytes = ((bmp.bmWidth + 15) & ~15) / 8;
		// Create a bitmap for the compatible DC. 
		hbmpCompat = CreateBitmap(bmp.bmWidth, bmp.bmHeight,
			bmp.bmPlanes, bmp.bmBitsPixel, (CONST VOID*) NULL);
		// Select the bitmap for the compatible DC.
		backupBmp = SelectObject(hdcScreenCompat, hbmpCompat);
		// Select the brush for the compatible DC.
		backupBrush = (HBRUSH)SelectObject(hdcScreenCompat, bgndBrush);
		// Initialize the flags. 
		fBlt = FALSE;
		fScroll = FALSE;
		fSize = FALSE;
		// Initialize the horizontal scrolling variables. 
		xMinScroll = 0;
		xCurrentScroll = 0;
		xMaxScroll = 0;
		// Initialize the vertical scrolling variables. 
		yMinScroll = 0;
		yCurrentScroll = 0;
		yMaxScroll = 0;
		// Draw device manager tree for first show window, mark show required.
		BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);
		treeDimension = HelperDrawTreeSized(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont);
		fSize = TRUE;
		// Initialize pointer for open items by SPACE, Gray+, Gray- keys.
		openNode = pModel->GetTree();
		// This for compatibility with MSDN example.
		fBlt = TRUE;
	}
	break;

	case WM_PAINT:
	{
		// Open paint context.
		BeginPaint(hWnd, &ps);
		// Paint bufferred copy.
		BitBlt(ps.hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcScreenCompat, xCurrentScroll, yCurrentScroll, SRCCOPY);
		// Close paint context.
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_SIZE:
	{
		int xNewSize = GET_X_LPARAM(lParam);
		int yNewSize = GET_Y_LPARAM(lParam);
		// Construction from original MSDN source, inspect it.
		if (fBlt)
			fSize = TRUE;
		// The horizontal scrolling range is defined by 
		// (tree_width) - (client_width). The current horizontal 
		// scroll value remains within the horizontal scrolling range.
		HelperAdjustScrollX(hWnd, si, treeDimension, xNewSize, xMaxScroll, xMinScroll, xCurrentScroll);
		// The vertical scrolling range is defined by 
		// (tree_height) - (client_height). The current vertical 
		// scroll value remains within the vertical scrolling range. 
		HelperAdjustScrollY(hWnd, si, treeDimension, yNewSize, yMaxScroll, yMinScroll, yCurrentScroll);
	}
	break;

	case WM_LBUTTONUP:
	{
		int mouseX = GET_X_LPARAM(lParam) + xCurrentScroll;
		int mouseY = GET_Y_LPARAM(lParam) + yCurrentScroll;
		int n = 0;
		PTREENODE p = pModel->GetTree();
		if (DetectMouseClick(mouseX, mouseY, p) && (p->openable))
		{
			p->opened = ~(p->opened);
			HelperMarkedClosedChilds(p, openNode, fTab);
			BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
			treeDimension = HelperDrawTreeSized(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont);
			fSize = TRUE;
			InvalidateRect(hWnd, NULL, false);
		}
		else
		{
			n = p->childCount;
			p = p->childLink;
			if (p)
			{
				for (int i = 0; i < n; i++)
				{
					if (DetectMouseClick(mouseX, mouseY, p) && (p->openable))
					{
						p->opened = ~(p->opened);
						HelperMarkedClosedChilds(p, openNode, fTab);
						BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
						treeDimension = HelperDrawTreeSized(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont);
						fSize = TRUE;
						InvalidateRect(hWnd, NULL, false);
						break;
					}
					p++;
				}
			}
		}

		RECT r = { 0,0,0,0 };
		if (GetClientRect(hWnd, &r))
		{
			int xNewSize = r.right - r.left;
			int yNewSize = r.bottom - r.top;
			// Construction from original MSDN source, inspect it.
			if (fBlt)
				fSize = TRUE;
			// The horizontal scrolling range is defined by 
			// (tree_width) - (client_width). The current horizontal 
			// scroll value remains within the horizontal scrolling range.
			HelperAdjustScrollX(hWnd, si, treeDimension, xNewSize, xMaxScroll, xMinScroll, xCurrentScroll);
			// The vertical scrolling range is defined by 
			// (tree_height) - (client_height). The current vertical 
			// scroll value remains within the vertical scrolling range. 
			HelperAdjustScrollY(hWnd, si, treeDimension, yNewSize, yMaxScroll, yMinScroll, yCurrentScroll);
		}
		// Restore Open/Close icon lighting by mouse cursor after node clicked.
		HelperOpenCloseMouseLight(hWnd, p, hdcScreenCompat, mouseX, mouseY,
			xCurrentScroll, yCurrentScroll, fSize, TRUE);
	}
	break;

	case WM_MOUSEMOVE:
	{
		int mouseX = GET_X_LPARAM(lParam) + xCurrentScroll;
		int mouseY = GET_Y_LPARAM(lParam) + yCurrentScroll;
		HelperOpenCloseMouseLight(hWnd, pModel->GetTree(), hdcScreenCompat, mouseX, mouseY,
			xCurrentScroll, yCurrentScroll, fSize, FALSE);
	}
	break;

	case WM_HSCROLL:
	{
		int addX = 0;
		int selector = LOWORD(wParam);
		int value = HIWORD(wParam);
		switch (selector)
		{
		case SB_PAGEUP:
			addX = -50;  // User clicked the scroll bar shaft left the scroll box. 
			break;
		case SB_PAGEDOWN:
			addX = 50;  // User clicked the scroll bar shaft right the scroll box. 
			break;
		case SB_LINEUP:
			addX = -3;  // User clicked the left arrow. 
			break;
		case SB_LINEDOWN:
			addX = 3;   // User clicked the right arrow. 
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			addX = value - xCurrentScroll;  // User dragged the scroll box.
			break;
		default:
			addX = 0;
			break;
		}
		if (addX != 0)
		{
			HelperMakeScrollX(hWnd, si, xMaxScroll, xCurrentScroll, fScroll, addX);
		}
	}
	break;

	case WM_VSCROLL:
	{
		int addY = 0;
		int selector = LOWORD(wParam);
		int value = HIWORD(wParam);

		switch (selector)
		{
		case SB_PAGEUP:
			addY = -50;  // User clicked the scroll bar shaft above the scroll box. 
			break;
		case SB_PAGEDOWN:
			addY = 50;   // User clicked the scroll bar shaft below the scroll box. 
			break;
		case SB_LINEUP:
			addY = -3;   // User clicked the top arrow. 
			break;
		case SB_LINEDOWN:
			addY = 3;    // User clicked the bottom arrow. 
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			addY = value - yCurrentScroll;  // User dragged the scroll box.
			break;
		default:
			addY = 0;
			break;
		}
		if (addY != 0)
		{
			HelperMakeScrollY(hWnd, si, yMaxScroll, yCurrentScroll, fScroll, addY);
		}
	}
	break;

	case WM_MOUSEWHEEL:
	{
		int addY = -(short)HIWORD(wParam) / WHEEL_DELTA * 30;
		if (addY != 0)
		{
			HelperMakeScrollY(hWnd, si, yMaxScroll, yCurrentScroll, fScroll, addY);
		}
	}
	break;

	case WM_KEYDOWN:
	{
		int addX = 0;
		int addY = 0;

		switch (wParam)
		{
		case VK_LEFT:
			addX = -3;
			break;
		case VK_RIGHT:
			addX = 3;
			break;
		case VK_UP:
			if (fTab)
			{
				openNode = HelperMarkNode(FALSE);
				BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
				HelperDrawTreeSized(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont);
				fSize = TRUE;
				InvalidateRect(hWnd, NULL, false);
			}
			else
			{
				addY = -3;
			}
			break;
		case VK_DOWN:
			if (fTab)
			{
				openNode = HelperMarkNode(TRUE);
				BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
				HelperDrawTreeSized(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont);
				fSize = TRUE;
				InvalidateRect(hWnd, NULL, false);
			}
			else
			{
				addY = 3;
			}
			break;
		case VK_PRIOR:
			addY = -50;
			break;
		case VK_NEXT:
			addY = 50;
			break;
		case VK_HOME:
			addY = -yCurrentScroll;
			break;
		case VK_END:
			addY = yMaxScroll;
			break;
		case VK_TAB:
			fTab = ~fTab;
			BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
			HelperDrawTreeSized(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont);
			fSize = TRUE;
			InvalidateRect(hWnd, NULL, false);
			break;

		case VK_ADD:
		case VK_SUBTRACT:
		case VK_SPACE:
			if (fTab && openNode && (openNode->openable))
			{
				openNode->opened = ~(openNode->opened);
				HelperMarkedClosedChilds(openNode, openNode, fTab);
				BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
				treeDimension = HelperDrawTreeSized(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont);
				fSize = TRUE;
				InvalidateRect(hWnd, NULL, false);

				RECT r = { 0,0,0,0 };
				if (GetClientRect(hWnd, &r))
				{
					int xNewSize = r.right - r.left;
					int yNewSize = r.bottom - r.top;
					if (fBlt)
						fSize = TRUE;
					HelperAdjustScrollX(hWnd, si, treeDimension, xNewSize, xMaxScroll, xMinScroll, xCurrentScroll);
					HelperAdjustScrollY(hWnd, si, treeDimension, yNewSize, yMaxScroll, yMinScroll, yCurrentScroll);
				}
			}
			break;
		default:
			break;
		}
		if (addX != 0)
		{
			HelperMakeScrollX(hWnd, si, xMaxScroll, xCurrentScroll, fScroll, addX);
		}
		else if (addY != 0)
		{
			HelperMakeScrollY(hWnd, si, yMaxScroll, yCurrentScroll, fScroll, addY);
		}
	}
	break;

	case WM_DESTROY:
	{
		if (backupBrush) SelectObject(hdcScreenCompat, backupBrush);
		if (backupBmp) SelectObject(hdcScreenCompat, backupBmp);
		if (hbmpCompat) DeleteObject(hbmpCompat);
		if (hdcScreenCompat) DeleteDC(hdcScreenCompat);
		if (hdcScreen) DeleteDC(hdcScreen);
		if (hFont) DeleteObject(hFont);
		PostQuitMessage(0);
	}
	break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

// ========== Helpers ==========
// Helpers for mouse click and position detection
bool TreeView::DetectMouseClick(int xPos, int yPos, PTREENODE p)
{
	return (xPos > p->clickArea.left) && (xPos < p->clickArea.right) && (yPos > p->clickArea.top) && (yPos < p->clickArea.bottom);
}
bool TreeView::DetectMousePosition(int xPos, int yPos, PTREENODE p)
{
	return (xPos > (p->clickArea.left + 1)) && (xPos < (p->clickArea.right - 1)) && (yPos > (p->clickArea.top + 1)) && (yPos < (p->clickArea.bottom - 1));
}
// Helper for mark nodes in the tree, direction flag means:
// 0 = increment, mark next node or no changes if last node currently marked,
// 1 = decrement, mark previous node or no changes if first (root) node currently marked.
// Returns pointer to selected node.
PTREENODE TreeView::HelperMarkNode(BOOL direction)
{
	PTREENODE p1 = pModel->GetTree();
	PTREENODE pFound = NULL;
	PTREENODE pNext = NULL;
	PTREENODE pBack = NULL;
	PTREENODE pTemp = NULL;

	if (p1)
	{
		if (p1->marked)
		{
			pFound = p1;
		}
		if ((p1->openable) && (p1->opened) && (p1->childLink))
		{
			PTREENODE p2 = p1->childLink;
			if (p2)
			{
				pTemp = p1;
				for (UINT i = 0; i < (p1->childCount); i++)
				{
					if (pFound && (!pNext)) pNext = p2;
					if (p2->marked) pFound = p2;
					if (pFound && pTemp && (!pBack)) pBack = pTemp;
					if ((p2->openable) && (p2->opened) && (p2->childLink))
					{
						PTREENODE p3 = p2->childLink;
						if (p3)
						{
							pTemp = p2;
							for (UINT j = 0; j < (p2->childCount); j++)
							{
								if (pFound && (!pNext)) pNext = p3;
								if (p3->marked) pFound = p3;
								if (pFound && pTemp && (!pBack)) pBack = pTemp;
								pTemp = p3;
								p3++;
							}
						}
					}
					else
					{
						pTemp = p2;
					}
					p2++;
				}
			}
		}
	}

	PTREENODE retPointer = NULL;
	if (direction && pFound && pNext)
	{
		pFound->marked = 0;
		pNext->marked = 1;
		retPointer = pNext;
	}
	else if ((!direction) && pFound && pBack)
	{
		pFound->marked = 0;
		pBack->marked = 1;
		retPointer = pBack;
	}
	return retPointer;
}
// Helper for update open-close icon light depend on mouse cursor position near icon.
void TreeView::HelperOpenCloseMouseLight(HWND hWnd, PTREENODE p, HDC hdcScreenCompat, int mouseX, int mouseY,
	int xCurrentScroll, int yCurrentScroll, BOOL& fSize, BOOL forceUpdate)
{
	RECT scrolledArea;

	// root node
	bool currentMouse = (DetectMousePosition(mouseX, mouseY, p) && (p->openable));
	if (((currentMouse != p->prevMouse) || forceUpdate) && p->openable)
	{
		int index;
		if (p->opened)
		{
			currentMouse ? index = ID_OPENED_LIGHT : index = ID_OPENED;
		}
		else
		{
			currentMouse ? index = ID_CLOSED_LIGHT : index = ID_CLOSED;
		}
		HICON hIcon = pModel->GetIconHandleByIndex(index);
		DrawIconEx(hdcScreenCompat, p->clickArea.left, p->clickArea.top, hIcon,
			X_ICON_SIZE, Y_ICON_SIZE, 0, NULL, DI_NORMAL | DI_COMPAT);
		fSize = TRUE;
		scrolledArea.left = p->clickArea.left - xCurrentScroll;
		scrolledArea.top = p->clickArea.top - yCurrentScroll;
		scrolledArea.right = p->clickArea.right - xCurrentScroll;
		scrolledArea.bottom = p->clickArea.bottom - yCurrentScroll;
		InvalidateRect(hWnd, &scrolledArea, false);
		p->prevMouse = currentMouse;
	}

	// child nodes, only if root node opened
	if (p->opened)
	{
		int count = p->childCount;
		p = p->childLink;
		if (p)
		{
			for (int i = 0; i < count; i++)
			{
				currentMouse = (DetectMousePosition(mouseX, mouseY, p) && (p->openable));
				if (((currentMouse != p->prevMouse) || forceUpdate) && p->openable)
				{
					int index;
					if (p->opened)
					{
						currentMouse ? index = ID_OPENED_LIGHT : index = ID_OPENED;
					}
					else
					{
						currentMouse ? index = ID_CLOSED_LIGHT : index = ID_CLOSED;
					}
					HICON hIcon = pModel->GetIconHandleByIndex(index);
					DrawIconEx(hdcScreenCompat, p->clickArea.left, p->clickArea.top, hIcon,
						X_ICON_SIZE, Y_ICON_SIZE, 0, NULL, DI_NORMAL | DI_COMPAT);
					fSize = TRUE;
					scrolledArea.left = p->clickArea.left - xCurrentScroll;
					scrolledArea.top = p->clickArea.top - yCurrentScroll;
					scrolledArea.right = p->clickArea.right - xCurrentScroll;
					scrolledArea.bottom = p->clickArea.bottom - yCurrentScroll;
					InvalidateRect(hWnd, &scrolledArea, false);
					p->prevMouse = currentMouse;
				}
				p++;
			}
		}
	}
}
// Helper for unmark items stay invisible after parent item close.
void TreeView::HelperMarkedClosedChilds(PTREENODE pParent, PTREENODE& openNode, BOOL fTab)
{
	if (fTab && (pParent->openable) && (!pParent->opened))
	{
		PTREENODE p1 = pParent->childLink;
		PTREENODE pMarked = NULL;
		UINT n1 = pParent->childCount;
		if (p1)
		{
			for (UINT i = 0; i < n1; i++)
			{
				if (p1->marked)
				{
					pMarked = p1;
					break;
				}
				if ((p1->openable) && (!p1->opened))
				{
					PTREENODE p2 = p1->childLink;
					UINT n2 = p1->childCount;
					if (p2)
					{
						for (UINT j = 0; j < n2; j++)
						{
							if (p2->marked)
							{
								pMarked = p2;
								break;
							}
							p2++;
						}
					}
				}
				if (pMarked) break;
				p1++;
			}

			if (pMarked)
			{
				pMarked->marked = 0;  // Unmark child node because it now hide.
				pParent->marked = 1;  // Mark parent node instead hide child node.
				openNode = pParent;   // Change current selected node for keyboard operations.
			}
		}
	}
}
// Helper for draw tree by nodes linked list and base coordinate point.
// Returns tree array (xleft, ytop, xright, ybottom),
// This parameters better calculate during draw, because depend on font size,
// current active font settings actual during draw.
RECT TreeView::HelperDrawTreeSized(PTREENODE pNodeList, POINT basePoint, BOOL fTab, HDC hDC, HFONT hFont)
{
	RECT treeDimension = { 0,0,0,0 };
	RECT rtemp;
	if (pNodeList)
	{
		// Draw root node.
		treeDimension = HelperDrawNodeLayerSized(pNodeList, 1, basePoint.x, basePoint.y,
			X_ICON_STEP, Y_ICON_STEP, X_ICON_SIZE, Y_ICON_SIZE, fTab, hDC, hFont);
		PTREENODE childLink = pNodeList->childLink;
		int childCount = pNodeList->childCount;
		int skipY = 0;
		if ((childLink) && (childCount) && (pNodeList->opened))
		{
			// Draw devices categories nodes.
			rtemp = HelperDrawNodeLayerSized(childLink, childCount, basePoint.x + X_ICON_STEP, basePoint.y + Y_ICON_STEP,
				X_ICON_STEP, Y_ICON_STEP, X_ICON_SIZE, Y_ICON_SIZE, fTab, hDC, hFont);
			// Update maximum X size.
			if (rtemp.right > treeDimension.right) { treeDimension.right = rtemp.right; }
			// Cycle for draw child nodes.
			for (int i = 0; i < childCount; i++)
			{
				PTREENODE childChildLink = childLink->childLink;
				int childChildCount = childLink->childCount;
				if (childChildLink && childChildCount && childLink->openable && childLink->opened)
				{
					// Draw devices nodes as childs of opened categories nodes.

					LONG t;
					(childChildLink->openable) ? t = basePoint.x + X_ICON_STEP * 2 : t = basePoint.x + X_ICON_STEP * 3;

					rtemp = HelperDrawNodeLayerSized(childChildLink, childChildCount,

						 // basePoint.x + X_ICON_STEP * 3, basePoint.y + Y_ICON_STEP * 2 + Y_ICON_STEP * skipY,
						 // basePoint.x + X_ICON_STEP * 2, basePoint.y + Y_ICON_STEP * 2 + Y_ICON_STEP * skipY,
						    t, basePoint.y + Y_ICON_STEP * 2 + Y_ICON_STEP * skipY,

						X_ICON_STEP, Y_ICON_STEP, X_ICON_SIZE, Y_ICON_SIZE, fTab, hDC, hFont);
					// Update maximum X size.
					if (rtemp.right > treeDimension.right) { treeDimension.right = rtemp.right; }
					// calculate Y size
					skipY += childChildCount;
				}
				skipY++;
				childLink++;
			}
		}
		POINT base = pModel->GetBase();
		treeDimension.left = basePoint.x;
		treeDimension.top = basePoint.y;
		treeDimension.bottom = basePoint.y + Y_ICON_STEP * 2 + Y_ICON_STEP * skipY + base.y;
	}
	return treeDimension;
}
// This for early start X scroll bar show.
#define X_ADDEND 16
// Helper for node sequence of one layer.
// Returns layer array (xleft, ytop, xright, ybottom),
// This parameters better calculate during draw, because depend on font size,
// current active font settings actual during draw.
RECT TreeView::HelperDrawNodeLayerSized(PTREENODE pNodeList, int nodeCount, int nodeBaseX, int nodeBaseY,
	int iconStepX, int iconStepY, int iconSizeX, int iconSizeY, BOOL fTab, HDC hDC, HFONT hFont)
{
	RECT layerDimension = { 0,0,0,0 };
	HFONT hOldFont = NULL;
	if (hFont) hOldFont = (HFONT)SelectObject(hDC, hFont);
	int oldBkMode = SetBkMode(hDC, TRANSPARENT);
	int tempX = 0;
	int skipY = 0;
	for (int i = 0; i < nodeCount; i++)
	{
		// Draw open/close icon, node icon and text string.
		HICON hIcon;
		if (pNodeList->openable)
		{
			pNodeList->opened ? hIcon = pNodeList->hOpenedIcon : hIcon = pNodeList->hClosedIcon;
			// Draw open-close icon
			DrawIconEx(hDC, nodeBaseX, nodeBaseY + iconStepY * i + iconStepY * skipY, hIcon, iconSizeX, iconSizeY, 0, NULL, DI_NORMAL | DI_COMPAT);
			tempX = nodeBaseX + iconStepX;
		}
		else
		{
			tempX = nodeBaseX;
		}
		hIcon = pNodeList->hNodeIcon;
		// Draw node icon
		DrawIconEx(hDC, tempX, nodeBaseY + iconStepY * i + iconStepY * skipY, hIcon, iconSizeX, iconSizeY, 0, NULL, DI_NORMAL | DI_COMPAT);
		int length = (int)strlen(pNodeList->szNodeName);

		if ((fTab) && (pNodeList->marked))
		{
			// Draw node text string, TAB selection ACTIVE and this node marked
			int oldBkMode = SetBkMode(hDC, OPAQUE);
			COLORREF oldBkColor = SetBkColor(hDC, SELECTED_BRUSH);
			TextOut(hDC, tempX + iconStepX, nodeBaseY + iconStepY * i + iconStepY * skipY, pNodeList->szNodeName, length);
			if (oldBkMode) SetBkMode(hDC, oldBkMode);
			if (oldBkColor != CLR_INVALID) SetBkColor(hDC, oldBkColor);
		}
		else
		{
			// Draw node text string, TAB selection mode NOT ACTIVE or this node not marked
			TextOut(hDC, tempX + iconStepX, nodeBaseY + iconStepY * i + iconStepY * skipY, pNodeList->szNodeName, length);
		}

		// Set coordinates for mouse clicks detections
		pNodeList->clickArea.left = nodeBaseX;
		pNodeList->clickArea.right = nodeBaseX + iconSizeX;
		pNodeList->clickArea.top = nodeBaseY + iconStepY * i + iconStepY * skipY;
		pNodeList->clickArea.bottom = nodeBaseY + iconStepY * i + iconStepY * skipY + iconSizeY;
		// Set coordinates for this node, later used by scroll parameters detection
		POINT base = pModel->GetBase();
		int tempSX = tempX + iconStepX + base.x + X_ADDEND;
		SIZE textSize = { 0,0 };
		if (GetTextExtentPoint32(hDC, pNodeList->szNodeName, length, &textSize))
		{
			tempSX += textSize.cx;
		}
		if (tempSX > layerDimension.right) { layerDimension.right = tempSX; }
		// Advance screen coordinates and list pointer.
		if (pNodeList->opened) skipY += pNodeList->childCount;
		pNodeList++;
	}
	if (oldBkMode) SetBkMode(hDC, oldBkMode);
	if (hOldFont) SelectObject(hDC, hOldFont);
	layerDimension.left = nodeBaseX;
	layerDimension.top = nodeBaseY;
	layerDimension.bottom = nodeBaseY + iconStepY * nodeCount + iconStepY * skipY;
	return layerDimension;
}
// Helper for adjust horizontal scrolling parameters.
// The horizontal scrolling range is defined by 
// (bitmap_width) - (client_width). The current horizontal 
// scroll value remains within the horizontal scrolling range.
void TreeView::HelperAdjustScrollX(HWND hWnd, SCROLLINFO& scrollInfo, RECT& treeDimension,
	int xNewSize, int& xMaxScroll, int& xMinScroll, int& xCurrentScroll)
{
	int tempSize = treeDimension.right - treeDimension.left;     // added
	xMaxScroll = max(tempSize - xNewSize, 0);                    // max(bmp.bmWidth - xNewSize, 0);
	xCurrentScroll = min(xCurrentScroll, xMaxScroll);
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	scrollInfo.nMin = xMinScroll;
	scrollInfo.nMax = tempSize;                                  // bmp.bmWidth;
	scrollInfo.nPage = xNewSize;
	scrollInfo.nPos = xCurrentScroll;
	SetScrollInfo(hWnd, SB_HORZ, &scrollInfo, TRUE);
}
// Helper for adjust vertical scrolling parameters.
// The vertical scrolling range is defined by 
// (bitmap_height) - (client_height). The current vertical 
// scroll value remains within the vertical scrolling range. 
void TreeView::HelperAdjustScrollY(HWND hWnd, SCROLLINFO& scrollInfo, RECT& treeDimension,
	int yNewSize, int& yMaxScroll, int& yMinScroll, int& yCurrentScroll)
{
	int tempSize = treeDimension.bottom - treeDimension.top;     // added
	yMaxScroll = max(tempSize - yNewSize, 0);                    // max(bmp.bmHeight - yNewSize, 0);
	yCurrentScroll = min(yCurrentScroll, yMaxScroll);
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	scrollInfo.nMin = yMinScroll;
	scrollInfo.nMax = tempSize;                                  // bmp.bmHeight;
	scrollInfo.nPage = yNewSize;
	scrollInfo.nPos = yCurrentScroll;
	SetScrollInfo(hWnd, SB_VERT, &scrollInfo, TRUE);
}
// Helper for make horizontal scrolling by given signed offset.
void TreeView::HelperMakeScrollX(HWND hWnd, SCROLLINFO& scrollInfo,
	int xMaxScroll, int& xCurrentScroll, BOOL& fScroll, int addX)
{
	int xNewPos = xCurrentScroll + addX;
	// New position must be between 0 and the screen width. 
	xNewPos = max(0, xNewPos);
	xNewPos = min(xMaxScroll, xNewPos);
	// If the current position does not change, do not scroll.
	if (xNewPos != xCurrentScroll)
	{
		// Set the scroll flag to TRUE. 
		fScroll = TRUE;
		// Update the current scroll position. 
		xCurrentScroll = xNewPos;
		// Update the scroll bar position.
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_POS;
		scrollInfo.nPos = xCurrentScroll;
		SetScrollInfo(hWnd, SB_HORZ, &scrollInfo, TRUE);
		// Request for all window repaint
		InvalidateRect(hWnd, NULL, false);
	}
}
// Helper for make vertical scrolling by given signed offset.
void TreeView::HelperMakeScrollY(HWND hWnd, SCROLLINFO& scrollInfo,
	int yMaxScroll, int& yCurrentScroll, BOOL& fScroll, int addY)
{
	int yNewPos = yCurrentScroll + addY;
	// New position must be between 0 and the screen height. 
	yNewPos = max(0, yNewPos);
	yNewPos = min(yMaxScroll, yNewPos);
	// If the current position does not change, do not scroll.
	if (yNewPos != yCurrentScroll)
	{
		// Set the scroll flag to TRUE. 
		fScroll = TRUE;
		// Update the current scroll position. 
		yCurrentScroll = yNewPos;
		// Update the scroll bar position.
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_POS;
		scrollInfo.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &scrollInfo, TRUE);
		// Request for all window repaint
		InvalidateRect(hWnd, NULL, false);
	}
}

// Storage for model class.
TreeModel* TreeView::pModel = NULL;
