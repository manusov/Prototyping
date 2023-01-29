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

#pragma once
#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <windows.h>
#include <windowsx.h>
#include "TreeModel.h"

#define X_BASE_TREE 10
#define Y_BASE_TREE 10
#define X_ICON_SIZE 16
#define Y_ICON_SIZE 16
#define X_ICON_STEP 18
#define Y_ICON_STEP 18
#define BACKGROUND_BRUSH  RGB(213, 240, 213)
#define SELECTED_BRUSH    RGB(245, 245, 120)

class TreeView
{
public:
	TreeView();
	~TreeView();
	void SetAndInitModel(TreeModel* p);
	// Window callback procedure for device manager window.
	virtual LRESULT CALLBACK AppViewer(HWND, UINT, WPARAM, LPARAM);
private:
	// Helpers for mouse click and position detection.
	bool DetectMouseClick(int xPos, int yPos, PTREENODE p);
	bool DetectMousePosition(int xPos, int yPos, PTREENODE p);
	// Helper for unmark items stay invisible after parent item close.
	void HelperMarkedClosedChilds(PTREENODE pParent, PTREENODE& openNode, BOOL fTab);
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
	// Helper for update open-close icon light depend on mouse cursor position near icon.
	void HelperOpenCloseMouseLightScrolled(HWND hWnd, PTREENODE p, HDC hdcScreenCompat, int mouseX, int mouseY,
		int xCurrentScroll, int yCurrentScroll, BOOL& fSize, BOOL forceUpdate);
	// This part for support recursive tree levels and eliminate level count limits.
	// Helper for update open-close icon light depend on mouse cursor position near icon.
	void HelperRecursiveMouseMove(PTREENODE p, HWND hWnd, HDC hdcScreenCompat, BOOL& fSize, BOOL forceUpdate,
		int mouseX, int mouseY, int xCurrentScroll, int yCurrentScroll);
	RECT HelperRecursiveMouseClick(PTREENODE p, POINT b, HWND hWnd, HDC hdcScreenCompat, BOOL& fSize,
		PTREENODE& openNode, BOOL fTab, BITMAP bmp, HFONT hFont,
		int mouseX, int mouseY, int xCurrentScroll, int yCurrentScroll);
	// Helper for draw tree by nodes linked list and base coordinate point.
	// Returns tree array (xleft, ytop, xright, ybottom),
	// This parameters better calculate during draw, because depend on font size,
	// current active font settings actual during draw.
	RECT HelperRecursiveDrawTree(PTREENODE p, POINT b, BOOL fTab, HDC hDC, HFONT hFont,
		int xCurrentScroll, int yCurrentScroll, int& dy);
	RECT HelperRecursiveDT(PTREENODE p, POINT b, BOOL fTab, HDC hDC, HFONT hFont,
		int xCurrentScroll, int yCurrentScroll, int& dy);
	// Helper for mark nodes in the tree, direction flag means:
	// 0 = increment, mark next node or no changes if last node currently marked,
	// 1 = decrement, mark previous node or no changes if first (root) node currently marked.
	// Returns pointer to selected node.
	PTREENODE HelperRecursiveMarkNode(BOOL direction);
	void HelperRecursiveMN(PTREENODE& p1, PTREENODE& pFound, PTREENODE& pNext, PTREENODE& pBack, PTREENODE& pTemp);

	// Link to tree model.
	static TreeModel* pModel;
};

#endif  // TREEVIEW_H