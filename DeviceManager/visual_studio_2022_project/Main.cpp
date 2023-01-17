/*

��������.
ENGINEERING SAMPLE.

������������� ������������ ��������� ���������� � ���� ������ (Device Manager Tree).
�� ����� ������� ������������ ������, ������������ ���������������� ����������,
��� ��������� ������������ ������������ �������. ����� ��������� �������� �
�������� Windows ����������� WinAPI.

Visual Studio 2022 project context:
------------------------------------
1) �������� ������� ��� ���� ����������.
2) ������������� \ ����� �������� \ ������������ ������������� ���������.
3) ����������� \ ���� \ �������������� ����������� \ �������� ���������� setupapi.lib.
4) ����������� \ ������� \ Windows.

Special thanks to:
Microsoft enumerator sample:
https://github.com/microsoft/Windows-driver-samples/tree/main/setup/devcon
Icons library and editor.
http://ru.softoware.org/apps/download-open-icon-library-for-windows.html
http://www.sibcode.com/junior-icon-editor/

-------------------------
Device Manager modeling.
-------------------------
TODO/ROADMAP.
-----------------
1.1)+  Select icons set for device manager requirements, variant for first experiments.
	   OK, selected 16x16. Use Open Icon Library.
1.2)+  Try alternatives 16x16, 22x22, 24x24 and 32x32 pixel formats.
	   OK, see DrawIconSet project in this solution.
1.3)+  "Closed" and "Opened" icons.
	   OK, created with Junior Icon Editor from Windows 10 Device Manager snapshoot, setup it at MainPC.
1.4)+  "Entire System" icon.
	   OK, import it from Open Icon Library.
1.5)+  Text strings.
	   OK, strings set yet for device manager modeling only.
1.6)+  Open-Close icons must change color when mouse cursor located at icon. Yet separate supported just-draw, without double bufferring.
*** detail notes ***
+ ������ ���������� ��������� DeviceManagerWndProc_v2. �� ������:
  https://learn.microsoft.com/en-us/windows/win32/controls/scroll-a-bitmap-in-scroll-bars
+ ����� ������ ��������� � �������� � ������� ������������, ����������� � BitBlt � WM_PAINT.
+ ��������� ������� ���� - �������� � �������� ����� � �������� � ������� ������������,
  �������� ���������� �������� �����������, ������������ ������� ������������.
+ ��������� ����������� ���� - ��������� ����� �� �������� �������� � ��������, �� �������
  ��������� ������ ����. ��� �������� ������� ������� ��������� �� ��������� �������
  ��������, ������� ���������� �� ������������ (�� ��������������) ������ �����������,
  ��������������� ��� ��������� ��������� WM_MOUSE.
  �������������� ������� - � �������������� ������, �� � ������������ ������� �����������
  � ��� ����������� ����, ��� ������� ������������.
+ ��������� ����������, ������������ ������ ��������� ���������� �����������. �� ������
  ����� ���������� ��������� �������� ������������� ��������� ������� ����.
+ ��������� ����������, ������������� ��������� ����� ���� � ����������� ������� ����
  � ������ ���������-��������.
+ ��������� ������������� ���������� ��������� ����.
+ ��������� ����������, �������� ������������ �� ����� ����������� ��� ��������������
  �������� ���������� �����. Handling message SB_THUMBTRACK.
*** end of detail notes ***
1.7)+  Double bufferring, dynamical revisualisation optimize. See NCRB sources.
	   OK, BitBlt function used.
1.8)+  Make vertical and horizontal scroll.
	   OK. Handling messages WM_HSCROLL, WM_VSCROLL, WM_MOUSEWHEEL.
1.9)-  Close all items when close-open root item.
	   REJECTED. Original Windows 10 Device Manager not have this property
	   (opened items still opened after all tree open-close).
1.10)- Change icons colors by JUST DRAW, without InvalidateRect() and WM_PAINT sequence, for fast draw.
	   REJECTED. This optimization done by partial invalidation after update bufferred copy.
1.11)- Open-close nodes items by just draw, without InvalidateRect() and WM_PAINT sequence, for fast draw.
	   REJECTED. This non-optimal because background redraw required after tree redraw.
1.12)- Vertical and horizontal scroll by just draw, without InvalidateRect() and WM_PAINT sequence, for fast draw.
	   REJECTED. This non-optimal because background redraw required after tree redraw.
1.13)- Inspect with just-draw (without InvalidateRect()) optimization criteria, for fast draw.
	   REJECTED. Just-draw to just visualized graphics context not used. See above.
1.14)- Inspect with double bufferring optimization criteria, for clear draw.
	   DEFERRED to next refactoring phase.
1.15)+ Bug: Window content not revisualized if window overlap, under Windows 7 under Oracle VirtualBox.
	   ������� ��������������� ���������� ���� � ��������� ��������� ��� Windows 10 � �������� �����������
	   ����, ��� ��������, ���������� �������� � Windows 7. ����� ��������� � Windows 7 under Oracle VirtualBox.
	   ����������� ������ ���� ������ ��� �������, ��� ���������� �����������, ��� �������� ����������
	   ��������� ������������������.
	   OK. Without additional invalidation. Remove fSize, fScroll criterias at WM_PAINT handler.
	   But remember about future refactoring and speed optimization.
-----------------
Start viewer v3.
-----------------
2.1)+  �������� ���������� ������ ������������ �������� ������������� ������, � �� ��������� �������������
	   ��������� (������ �������� ������ ��������). ����� ����, ��� ���������� ����� �������������� ��������
	   ��������� ������: ����������� ������ �� ���������� �� ������ � ������������, �� � � ������ ������.
	   ������ ������ ������ ����� �����, ������������ ����� �������� ������ ���� �� ���������.
2.2)+  ��������� ������ � ������������ �������� ��������� ����� ��������� ��� Y_BASE_TREE ������ 15.
-------------
TODO/FUTURE.
-------------
3.1)+  Keyboard support at device manager GUI elements.
		+ ��������� � ����������.
		+ ��������� ����� � ����������.
		+ �������� � �������� ����� � ����������, SPACE, GRAY PLUS, GRAY MINUS.
		+ ���������� ���������� ���������� ��� �������� � �������� ����� � ����������.
	   https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-char
	   https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
	   https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	   Support Left, Right, Up, Down, PgUp, PgDn, Home, End keys.
	   Use WM_CHAR or WM_KEYDOWN messages.
3.2)+  Bug with non-consistent open-close color after click.
3.3)+  Bug if close parent node of node, marked by keyboard.
	   ���� ���������� �� ����������� ����� �������, ������� ������ ���� �����, ���������� �� ����� 0
	   (�������� ����� marked � ����� �������: ��� ������� ��������� � ��������������� � ����� ���������
	   openNode), � ����� ������������� ������ ������ ���� �������� ���������� ����� fTab.
	   �������������� ������� ������� �� ���������� ����������� ���� �� ���������:
	   ��������� ������� �� ������������ ����, �������� �������� ����� ������������� ������.
	   �������� ���������: case VK_SPACE (include gray +/-) and WM_LBUTTONUP.
	   ������������ HelperMarkedClosedChilds().
3.4)+  ��������� ������ friendly-���� ��������� � ��������.
	   ���������� ����� ������� ������������ ��������� ������� �� ���������� �����.
-----------------
Start viewer v5.
-----------------

4.1)   Refactoring.
       - Correct icon assignments, use correct SCSI icon.
	   - Redesign icon for Software Defined Devices.
	   - ACPI, ACPI_HAL, PCI ...
4.2)   ������ ����� ��� 32 � 64-������� ����������.
4.3)   Refactoring. �������� �� ������ ���������� ��������� ������ � ���������� ��������� ������������.

4.4)   Make recursion instead fixed levels count (root-categories-devices).
	   ����� ����������� �� ���������� ������� �������� � �� ���������� �����.
4.5)   Make device manager as set of classes.
	   Model, View, Controller = TreeHolder, TreeViewer, TreeBuilder.
4.6)   GUI esthetic questions. Fonts, colors, images content.
4.7)   Mode can be dynamically selected depend on screen resolution and physical size.
4.8)   Optimize by size. Optimize resolution, color depth and format (ICO, PNG) by code size. Packed is better.
4.9)   Optimize by speed. Minimize updated regions square and operations count. Selectable write and invalidation.
	   Detect situations redraw one region many times.
4.10)  ���������� ������������ ������� ���������. ��������� � ���������� ������ ����������.
-----------------
Start viewer v6.
-----------------
5.1)   ����������� � ����������. ��������� � ������� 4.2 ����.
	   �������� ���������� ������ ������������ �������� ������������� ������, � �� ��������� �������������
	   ��������� (������ �������� ������ ��������). ����� ����, ��� ���������� ����� �������������� ��������
	   ��������� ������: ����������� ������ �� ���������� �� ������ � ������������, �� � � ������ ������.
	   ������ ������ ������ ����� �����, ������������ ����� �������� ������ ���� �� ���������.
	   �������� ������� ����������� ��� ������ ���������� ��� ������� ���� ����������� �� ����:
		- �������� ����� ������������ �������� � �������, ����������� ������� �����������.
		- �������� ��������� ������� ��� ���������� ������ � ������.
	   ����, ���������� ������������� ��������� ��������, ����� ���������� ��� ������ ��������� ������
	   ���������� �� ����� ����������� ���� ����������. �������� ��� ����� ������������� �����������
	   ��������� �� ������:
	   https://learn.microsoft.com/ru-ru/windows/win32/winmsg/wm-size
	   WM_DISPLAYCHANGE:
	   https://learn.microsoft.com/en-us/windows/win32/gdi/wm-displaychange
----------------------------
Device Manager application.
----------------------------
6.1)+  Start works with system scanner-enumerator, first learn MSDN example.
6.2)+  Continue works with system scanner-enumerator, make Device Manager application.
6.3)   Support 48x48 and other pixel formats.
-------------
"_DM" means "Device Manager", design icons set for Device Manager style interface.
-------------

*/

#include <windows.h>
#include "KWnd.h"
#include "AppModel.h"
#include "AppView.h"
#include "AppController.h"
#include "Enumerator.h"

#if _WIN64
const char* BUILD_NAME = "Modeling Device Manager. Engineering sample v0.04.03.x64.";
#elif _WIN32
const char* BUILD_NAME = "Modeling Device Manager. Engineering sample v0.04.03 ia32.";
#else
const char* BUILD_NAME = "UNKNOWN BUILD MODE.";
#endif

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	// Initializing context.
	if (!InitializeTree())
	{
		MessageBox(NULL, "Icons array initialization failed.", "Error", MB_OK);
		return 1;
	}

	// Build tree list.
	// Select one of this sources of treeList: 
	// BuildEmulatedTree() = emulated tree with constants or 
	// BuildSystemTree() = get computer configuration by WinAPI.
	
	// (1 of 2) Emulated tree for debug, uncoment next line for data from fixed table.
	// PTREENODE treeList = BuildEmulatedTree();
	//
	
	// (2 of 2) System tree by WinAPI, uncomment next line for debug with real system information.
	PTREENODE treeList = BuildSystemTree();
	//

	if (!treeList)
	{
		MessageBox(NULL, "Build system tree failed.", "Error", MB_OK);
		ReleaseSystemTree();
		DeInitializeTree();
		return 2;
	}
	SetTree(treeList);
	// Set tree base coordinates.
	POINT treeBase = { X_BASE_TREE, Y_BASE_TREE };
	SetBase(treeBase);
	// View tree.
	MSG msg;
	KWnd mainWnd((LPCTSTR)BUILD_NAME, hInstance, nCmdShow,
		AppViewer,
		NULL,
		// 590, 280, 450, 580,
		590, 280, 800, 640,
		CS_HREDRAW | CS_VREDRAW,
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		NULL);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	// De-initialize context, exit application.
	ReleaseSystemTree();
	DeInitializeTree();
	return (int)msg.wParam;

}

