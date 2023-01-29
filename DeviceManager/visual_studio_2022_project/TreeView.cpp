/* ----------------------------------------------------------------------------------------
Class for create GUI window with tree visualization.

UNDER CONSTRUCTION: Upgraded version with state variables and recursive levels.
S = Show region, can be resized by user actions (resize GUI window).
P = Physical region, depends on video hardware parameters (screen X,Y resolution).
V = Virtual region, depends on tree size.
Scroll means shift V relative S.
S maximum X,Y sizes defined by P.

	   |
   ----|----------
   |   | S |     |
   ----|----     |
   |   |         |
   |   |      P  |
   ----|---------|
	   |V

ROADMAP 1: SUPPORT BIG SCROLLABLE TREES WITHOUT MEMORY OVERFLOW:
-----------------------------------------------------------------
1.1) WM_CREATE handler: create GUI window content, show initial state.
1.2) WM_PAINT handler: visualize GUI window content, copy raster info with dual bufferring.
1.3) WM_SIZE handler: resize GUI window, update scroll control variables.
1.4) WM_LBUTTONUP handler: open and close nodes.
1.5) WM_MOUSEMOVE handler: mark nodes depend on mouse cursor near node.
1.6) WM_HSCROLL handler: horizontal scroll.
1.7) WM_VSCROLL handler: vertical scroll.
1.8) WM_MOUSEWHEEL handler: vertical scroll, alternative method.
1.9) WM_KEYDOWN handler: mark, unmark, move pointer, open and close by keys.

ROADMAP 2: RECURSIVE OR STACKED TREE LEVELS, REMOVE TREE LEVEL COUNT LIMITS:
-----------------------------------------------------------------------------
2.1) Method for verify results of next steps, data source = child class TreeControllerExt.
2.2) Recursive or stacked mouse move selection.
2.3) Recursive or stacked open-close nodes, means recursive or stacked draw layers.
2.4) Correct TAB selection for extra layers.
2.5) Verify all handlers in the Window Procedure.

Lines, selected by:
//
...
//
is first objects for refactoring and optimization.
Current used visualization method is slow, too many time required,
because full blanks and redraw.
Note TreeView.cpp use offset change for scroll, not redraw!
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
	PAINTSTRUCT ps;              // Temporary storage for paint info  : ps.hdc = window, can be resized by user.
	static HDC hdcScreen;        // DC for entire screen              : hdcScreen = full screen.
	static HDC hdcScreenCompat;  // memory DC for screen              : hdcScreenCompat = bufferred copy of full screen.
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
		bmp.bmHeight = GetDeviceCaps(hdcScreen, VERTRES);
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
		int dy = 0;
		treeDimension = HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(),
			fTab, hdcScreenCompat, hFont, xCurrentScroll, yCurrentScroll, dy);
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
		BitBlt(ps.hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, hdcScreenCompat, 0, 0, SRCCOPY);
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
		BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
		int dy = 0;
		HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
			xCurrentScroll, yCurrentScroll, dy);
	}
	break;

	case WM_LBUTTONUP:
	{
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);
		PTREENODE p = pModel->GetTree();
		POINT b = pModel->GetBase();

		RECT t = HelperRecursiveMouseClick(p, b, hWnd, hdcScreenCompat, fSize,
			openNode, fTab, bmp, hFont,
			mouseX, mouseY, xCurrentScroll, yCurrentScroll);
		if (t.right && t.bottom)
		{
			treeDimension = t;
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
		HelperRecursiveMouseMove(p, hWnd, hdcScreenCompat, fSize, TRUE,
			mouseX, mouseY, xCurrentScroll, yCurrentScroll);
	}
	break;

	case WM_MOUSEMOVE:
	{
		HelperRecursiveMouseMove(pModel->GetTree(), hWnd, hdcScreenCompat, fSize, FALSE,
			GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), xCurrentScroll, yCurrentScroll);
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
			BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
			int dy = 0;
			HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
				xCurrentScroll, yCurrentScroll, dy);
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
			BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
			int dy = 0;
			HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
				xCurrentScroll, yCurrentScroll, dy);
		}
	}
	break;

	case WM_MOUSEWHEEL:
	{
		int addY = -(short)HIWORD(wParam) / WHEEL_DELTA * 30;
		if (addY != 0)
		{
			HelperMakeScrollY(hWnd, si, yMaxScroll, yCurrentScroll, fScroll, addY);
			BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
			int dy = 0;
			HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
				xCurrentScroll, yCurrentScroll, dy);
		}
	}
	break;

	case WM_KEYDOWN:
	{
		int addX = 0;
		int addY = 0;
		int dy = 0;

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
				openNode = HelperRecursiveMarkNode(FALSE);
				BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
				dy = 0;
				HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
					xCurrentScroll, yCurrentScroll, dy);
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
				openNode = HelperRecursiveMarkNode(TRUE);
				BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
				dy = 0;
				HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
					xCurrentScroll, yCurrentScroll, dy);
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
			dy = 0;
			HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
				xCurrentScroll, yCurrentScroll, dy);
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
				dy = 0;
				treeDimension = HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
					xCurrentScroll, yCurrentScroll, dy);
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
			BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
			dy = 0;
			HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
				xCurrentScroll, yCurrentScroll, dy);
		}
		else if (addY != 0)
		{
			HelperMakeScrollY(hWnd, si, yMaxScroll, yCurrentScroll, fScroll, addY);
			BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
			dy = 0;
			HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(), fTab, hdcScreenCompat, hFont,
				xCurrentScroll, yCurrentScroll, dy);
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
// Helpers.
// Helpers for mouse click and position detection
bool TreeView::DetectMouseClick(int xPos, int yPos, PTREENODE p)
{
	return (xPos > p->clickArea.left) && (xPos < p->clickArea.right) && (yPos > p->clickArea.top) && (yPos < p->clickArea.bottom);
}
bool TreeView::DetectMousePosition(int xPos, int yPos, PTREENODE p)
{
	return (xPos > (p->clickArea.left + 1)) && (xPos < (p->clickArea.right - 1)) && (yPos > (p->clickArea.top + 1)) && (yPos < (p->clickArea.bottom - 1));
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
// Helper for update open-close icon light depend on mouse cursor position near icon.
void TreeView::HelperOpenCloseMouseLightScrolled(HWND hWnd, PTREENODE p, HDC hdcScreenCompat, int mouseX, int mouseY,
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
		scrolledArea.left = p->clickArea.left;
		scrolledArea.top = p->clickArea.top;
		scrolledArea.right = p->clickArea.right;
		scrolledArea.bottom = p->clickArea.bottom;
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
					scrolledArea.left = p->clickArea.left;
					scrolledArea.top = p->clickArea.top;
					scrolledArea.right = p->clickArea.right;
					scrolledArea.bottom = p->clickArea.bottom;
					InvalidateRect(hWnd, &scrolledArea, false);
					p->prevMouse = currentMouse;
				}
				p++;
			}
		}
	}
}
// This part for support recursive tree levels and eliminate level count limits.
void TreeView::HelperRecursiveMouseMove(PTREENODE p, HWND hWnd, HDC hdcScreenCompat, BOOL& fSize, BOOL forceUpdate,
	int mouseX, int mouseY, int xCurrentScroll, int yCurrentScroll)
{
	RECT a;
	bool currentMouse = (DetectMousePosition(mouseX, mouseY, p) && (p->openable));
	if (((currentMouse != p->prevMouse) || forceUpdate) && p->openable)
	{   // Operation for node, addressed by caller.
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
		a.left = p->clickArea.left;
		a.top = p->clickArea.top;
		a.right = p->clickArea.right;
		a.bottom = p->clickArea.bottom;
		InvalidateRect(hWnd, &a, false);
		p->prevMouse = currentMouse;
	}
	if (p->opened)
	{
		int n = p->childCount;
		p = p->childLink;
		if (p && (n > 0))
		{   // Recursive operation for childs nodes of node, addressed by caller.
			for (int i = 0; i < n; i++)
			{
				HelperRecursiveMouseMove(p, hWnd, hdcScreenCompat, fSize, forceUpdate,
					mouseX, mouseY, xCurrentScroll, yCurrentScroll);
				p++;
			}
		}
	}
}
RECT TreeView::HelperRecursiveMouseClick(PTREENODE p, POINT b, HWND hWnd, HDC hdcScreenCompat, BOOL& fSize,
	PTREENODE& openNode, BOOL fTab, BITMAP bmp, HFONT hFont,
	int mouseX, int mouseY, int xCurrentScroll, int yCurrentScroll)
{
	RECT rDimension = { 0,0,0,0 };
	RECT rTemp = { 0,0,0,0 };
	if (DetectMouseClick(mouseX, mouseY, p) && (p->openable))
	{
		p->opened = ~(p->opened);
		HelperMarkedClosedChilds(p, openNode, fTab);
		BitBlt(hdcScreenCompat, 0, 0, bmp.bmWidth, bmp.bmHeight, NULL, 0, 0, PATCOPY);  // This for blank background
		int dy = 0;
		rDimension = HelperRecursiveDrawTree(pModel->GetTree(), pModel->GetBase(),
			fTab, hdcScreenCompat, hFont, xCurrentScroll, yCurrentScroll, dy);
		fSize = TRUE;
		InvalidateRect(hWnd, NULL, false);
	}
	else if (p->openable)
	{
		int n = p->childCount;
		p = p->childLink;
		if (p)
		{
			b.x += X_ICON_STEP;
			for (int i = 0; i < n; i++)
			{
				if (p->openable)
				{
					b.y += Y_ICON_STEP;
					rTemp = HelperRecursiveMouseClick(p, b, hWnd, hdcScreenCompat, fSize,
						openNode, fTab, bmp, hFont,
						mouseX, mouseY, xCurrentScroll, yCurrentScroll);
					// Update maximal dimensions.
					if (rTemp.left < rDimension.left) { rDimension.left = rTemp.left; }
					if (rTemp.right > rDimension.right) { rDimension.right = rTemp.right; }
					if (rTemp.top < rDimension.top) { rDimension.top = rTemp.top; }
					if (rTemp.bottom > rDimension.bottom) { rDimension.bottom = rTemp.bottom; }
				}
				p++;
			}
		}
	}
	return rDimension;
}
// Helper for draw tree by nodes linked list and base coordinate point.
// Returns tree array (xleft, ytop, xright, ybottom),
// This parameters better calculate during draw, because depend on font size,
// current active font settings actual during draw.
RECT TreeView::HelperRecursiveDrawTree(PTREENODE p, POINT b, BOOL fTab, HDC hDC, HFONT hFont,
	int xCurrentScroll, int yCurrentScroll, int& dy)
{
	b.x -= xCurrentScroll;
	b.y -= yCurrentScroll;
	return HelperRecursiveDT(p, b, fTab, hDC, hFont, xCurrentScroll, yCurrentScroll, dy);
}
RECT TreeView::HelperRecursiveDT(PTREENODE p, POINT b, BOOL fTab, HDC hDC, HFONT hFont,
	int xCurrentScroll, int yCurrentScroll, int& dy)
{
	RECT rDimension = { 0,0,0,0 };
	RECT rTemp;
	int skipY = 0;
	if (p)
	{   // Draw node, addressed by caller.
		rDimension = HelperDrawNodeLayerSized(p, 1, b.x, b.y,
			X_ICON_STEP, Y_ICON_STEP, X_ICON_SIZE, Y_ICON_SIZE, fTab, hDC, hFont);

		PTREENODE childLink = p->childLink;
		int childCount = p->childCount;
		if ((childLink) && (childCount) && (p->openable) && (p->opened))
		{
			for (int i = 0; i < childCount; i++)
			{   // Recursive operation for childs nodes of node, addressed by caller.
				PTREENODE childChildLink = childLink->childLink;
				int childChildCount = childLink->childCount;
				POINT b1 = { b.x + X_ICON_STEP * 2, b.y + Y_ICON_STEP + Y_ICON_STEP * skipY };
				if (childLink->openable) { b1.x = b.x + X_ICON_STEP; }
				int dy = 0;
				rTemp = HelperRecursiveDT(childLink, b1, fTab, hDC, hFont, xCurrentScroll, yCurrentScroll, dy);
				// Update maximal dimensions.
				if (rTemp.left < rDimension.left) { rDimension.left = rTemp.left; }
				if (rTemp.right > rDimension.right) { rDimension.right = rTemp.right; }
				if (rTemp.top < rDimension.top) { rDimension.top = rTemp.top; }
				if (rTemp.bottom > rDimension.bottom) { rDimension.bottom = rTemp.bottom; }
				// Update Y size and pointer.
				skipY = skipY + 1 + dy;
				childLink++;
			}
		}
		POINT base = pModel->GetBase();
		rDimension.left = b.x;
		rDimension.top = b.y;
		rDimension.bottom = b.y + Y_ICON_STEP * 2 + Y_ICON_STEP * skipY + base.y;
	}
	dy = skipY;
	return rDimension;
}
// Helper for mark nodes in the tree, direction flag means:
// 0 = increment, mark next node or no changes if last node currently marked,
// 1 = decrement, mark previous node or no changes if first (root) node currently marked.
// Returns pointer to selected node.
PTREENODE TreeView::HelperRecursiveMarkNode(BOOL direction)
{
	PTREENODE p1 = pModel->GetTree();
	PTREENODE pFound = NULL;
	PTREENODE pNext = NULL;
	PTREENODE pBack = NULL;
	PTREENODE pTemp = NULL;

	HelperRecursiveMN(p1, pFound, pNext, pBack, pTemp);

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
void TreeView::HelperRecursiveMN(PTREENODE& p1, PTREENODE& pFound, PTREENODE& pNext, PTREENODE& pBack, PTREENODE& pTemp)
{
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
					if ((p2) && (p2->marked)) pFound = p2;
					if (pFound && pTemp && (!pBack)) pBack = pTemp;

					if ((p2) && (p2->openable) && (p2->opened) && (p2->childLink))
					{
						HelperRecursiveMN(p2, pFound, pNext, pBack, pTemp);
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
}

// Storage for model class.
TreeModel* TreeView::pModel = NULL;

