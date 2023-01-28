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

---------------------------------------------------------------------------------------- */

#pragma once
#ifndef TREEVIEWUPGRADE_H
#define TREEVIEWUPGRADE_H

class TreeViewUpgrade :
    public TreeView
{
public:
    TreeViewUpgrade();
    ~TreeViewUpgrade();
    // Window callback procedure for device manager window.
    LRESULT CALLBACK AppViewer(HWND, UINT, WPARAM, LPARAM);
private:
    // Helper for draw tree by nodes linked list and base coordinate point.
    // Version ...Scrolled for implement scrolling offset by this subroutine.
    // Returns tree array (xleft, ytop, xright, ybottom),
    // This parameters better calculate during draw, because depend on font size,
    // current active font settings actual during draw.
    RECT HelperDrawTreeSizedScrolled(PTREENODE pNodeList, POINT basePoint, BOOL fTab, HDC hDC, HFONT hFont, 
        int xCurrentScroll, int yCurrentScroll);
    // Helper for update open-close icon light depend on mouse cursor position near icon.
    void HelperOpenCloseMouseLightScrolled(HWND hWnd, PTREENODE p, HDC hdcScreenCompat, int mouseX, int mouseY,
        int xCurrentScroll, int yCurrentScroll, BOOL& fSize, BOOL forceUpdate);

// ---------- This part for support recursive tree levels and eliminate level count limits ----------

    void HelperRecursiveMouseMove(PTREENODE p, HWND hWnd, HDC hdcScreenCompat, BOOL& fSize, BOOL forceUpdate,
        int mouseX, int mouseY, int xCurrentScroll, int yCurrentScroll);

    RECT HelperRecursiveMouseClick(PTREENODE p, POINT b, HWND hWnd, HDC hdcScreenCompat, BOOL& fSize,
        PTREENODE& openNode, BOOL fTab, BITMAP bmp, HFONT hFont,
        int mouseX, int mouseY, int xCurrentScroll, int yCurrentScroll);

    RECT HelperRecursiveDrawTree(PTREENODE p, POINT b, BOOL fTab, HDC hDC, HFONT hFont,
        int xCurrentScroll, int yCurrentScroll, int& dy);

    RECT HelperRecursive(PTREENODE p, POINT b, BOOL fTab, HDC hDC, HFONT hFont,
        int xCurrentScroll, int yCurrentScroll, int& dy);

};

#endif  // TREEVIEWUPGRADE_H