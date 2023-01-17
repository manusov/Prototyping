/*

Заголовочный файл для диалоговой процедуры GUI окна диспетчера устройств,
которая визуализирует информацию в виде дерева (tree).
По классификации TreeHolder/TreeBuilder/TreeViewer это Viewer.
По классификации Model/View/Controller это View.
Special thanks to:
https://learn.microsoft.com/en-us/windows/win32/controls/scroll-a-bitmap-in-scroll-bars

*/

#pragma once
#ifndef APPVIEWL_H
#define APPVIEW_H

#include <windows.h>
#include <windowsx.h>

#define X_BASE_TREE 10
#define Y_BASE_TREE 10
#define X_SIZE 16
#define Y_SIZE 16
#define X_STEP 18
#define Y_STEP 18

#define BACKGROUND_BRUSH  RGB(213, 240, 213)
#define SELECTED_BRUSH    RGB(245, 245, 120)

// Window callback procedure for device manager window.
LRESULT CALLBACK AppViewer(HWND, UINT, WPARAM, LPARAM);

// Helpers for mouse click and position detection.
bool DetectMouseClick(int xPos, int yPos, PTREENODE p);
bool DetectMousePosition(int xPos, int yPos, PTREENODE p);

// Helper for mark nodes in the tree, direction flag means:
// 0 = increment, mark next node or no changes if last node currently marked,
// 1 = decrement, mark previous node or no changes if first (root) node currently marked.
// Returns pointer to selected node.
PTREENODE HelperMarkNode(BOOL direction);

// Helper for update open-close icon light depend on mouse cursor position near icon.
void HelperOpenCloseMouseLight(HWND hWnd, PTREENODE p, HDC hdcScreenCompat, int mouseX, int mouseY,
	int xCurrentScroll, int yCurrentScroll, BOOL& fSize, BOOL forceUpdate);

// Helper for unmark items stay invisible after parent item close.
// void HelperMarkedClosedChilds(PTREENODE rootP, PTREENODE parentP, PTREENODE& openNode, BOOL& fTab, BOOL changedState);
void HelperMarkedClosedChilds(PTREENODE pParent, PTREENODE& openNode, BOOL fTab);

// Helper for draw tree by nodes linked list and base coordinate point.
// Returns tree array (xleft, ytop, xright, ybottom),
// This parameters better calculate during draw, because depend on font size,
// current active font settings actual during draw.
RECT HelperDrawTreeSized(PTREENODE pNodeList, POINT basePoint, BOOL fTab, HDC hDC, HFONT hFont);

// Helper for node sequence of one layer.
// Returns layer array (xleft, ytop, xright, ybottom),
// This parameters better calculate during draw, because depend on font size,
// current active font settings actual during draw.
RECT HelperDrawNodeLayerSized(PTREENODE pNodeList, int nodeCount, int treeBaseX, int treeBaseY,
	int iconStepX, int iconStepY, int iconSizeX, int iconSizeY, BOOL fTab, HDC hDC, HFONT hFont);

// Helper for adjust horizontal scrolling parameters.
// The horizontal scrolling range is defined by 
// (bitmap_width) - (client_width). The current horizontal 
// scroll value remains within the horizontal scrolling range.
void HelperAdjustScrollX(HWND hWnd, SCROLLINFO& scrollInfo, RECT& treeDimension,
	int xNewSize, int& xMaxScroll, int& xMinScroll, int& xCurrentScroll);

// Helper for adjust vertical scrolling parameters.
// The vertical scrolling range is defined by 
// (bitmap_height) - (client_height). The current vertical 
// scroll value remains within the vertical scrolling range. 
void HelperAdjustScrollY(HWND hWnd, SCROLLINFO& scrollInfo, RECT& treeDimension,
	int yNewSize, int& yMaxScroll, int& yMinScroll, int& yCurrentScroll);

// Helper for make horizontal scrolling by given signed offset.
void HelperMakeScrollX(HWND hWnd, SCROLLINFO& scrollInfo,
	int xMaxScroll, int& xCurrentScroll, BOOL& fScroll, int addX);

// Helper for make vertical scrolling by given signed offset.
void HelperMakeScrollY(HWND hWnd, SCROLLINFO& scrollInfo,
	int yMaxScroll, int& yCurrentScroll, BOOL& fScroll, int addY);

#endif  // APPVIEW_H

