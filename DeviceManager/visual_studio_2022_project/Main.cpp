/* ----------------------------------------------------------------------------------------

ENGINEERING SAMPLE.
Clone of Windows Device Manager.
Main source file (Head Block).

Project context and settings:

  1) Add resources for application icon and tree nodes icons.
  2) Required differentiation application icon for ia32 and x64,
     both for Debug and Release,
     Build events \ Event before build \ command line:
	 set icon for ia32 command line:
	   "copy Icons_App\app32.ico Icons_App\app.ico"
	 set icon for x64 command line:
       "copy Icons_App\app64.ico Icons_App\app.ico"

This settings both for x86 and x64, both for Debug and Release:
  3) Additional \ Char set \ Multi-byte encoding.
  4) Linker \ System \ Windows.
  5) Linker \ Input \ Additional dependences \ Add library: setupapi.lib.

This settings both for x86 and x64, for Release only:
  6) C++ \ Code build \ Runtime library \ Multi-thread (MT).

Special thanks to:

Microsoft enumerator sample:
https://github.com/microsoft/Windows-driver-samples/tree/main/setup/devcon

Windows GUI examples:
https://learn.microsoft.com/en-us/windows/win32/controls/scroll-a-bitmap-in-scroll-bars
https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createfonta
https://www.piter.com/page/filesforbooks
https://storage.piter.com/support_insale/files_list.php?code=978546901361

Icons library and editor:
http://ru.softoware.org/apps/download-open-icon-library-for-windows.html
https://sourceforge.net/projects/openiconlibrary/
http://www.sibcode.com/junior-icon-editor/

---------------------------------------------------------------------------------------- */

#include <windows.h>
#include "KWnd.h"
#include "TreeModel.h"
#include "TreeView.h"
#include "TreeController.h"
#include "TreeControllerExt.h"
#include "TreeControllerSys.h"

// Select emulated or system scan mode: uncomment this for debug.
// Emulated mode means use constant emulated system configuration info
// WITHOUT get system information.
// #define _EMULATED_MODE

// Select extended emulated mode, with improved nodes nesting level.
// Enable both _EMULATED_MODE, _EXT_EMULATED_MODE for extended mode.
// #define _EXT_EMULATION

#if _WIN64
#ifdef _EMULATED_MODE
const char* BUILD_NAME = "Modeling Device Manager (emulated data). Engineering sample v0.06.01. x64.";
#else
const char* BUILD_NAME = "System view. v0.06.01. x64.";
#endif
#elif _WIN32
#ifdef _EMULATED_MODE
const char* BUILD_NAME = "Modeling Device Manager (emulated data). Engineering sample v0.06.01. ia32.";
#else
const char* BUILD_NAME = "System view. v0.06.01. ia32.";
#endif
#else
const char* BUILD_NAME = "UNKNOWN BUILD MODE.";
#endif

TreeModel* pModel = NULL;
TreeView* pView = NULL;
TreeController* pController = NULL;

LRESULT CALLBACK TransitWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return pView->AppViewer(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// Initialize system context.
	int status = 0;
	pModel = new TreeModel();
	pView = new TreeView();

#ifdef _EMULATED_MODE
#ifdef _EXT_EMULATION
	pController = new TreeControllerExt();
#else
	pController = new TreeController();
#endif
#else
	pController = new TreeControllerSys();
#endif

	if (pModel && pView && pController)
	{
		if (pModel->getInitStatus())
		{
			pController->SetAndInitModel(pModel);
			pView->SetAndInitModel(pModel);
			// Set tree base coordinates.
			POINT treeBase = { X_BASE_TREE, Y_BASE_TREE };
			pModel->SetBase(treeBase);
			// Set tree data.
			PTREENODE tree = pController->BuildTree();
			pModel->SetTree(tree);
			if (tree)
			{
				// Show application window = Device Manager tree.
				MSG msg;
				KWnd mainWnd((LPCTSTR)BUILD_NAME, hInstance, nCmdShow,
					TransitWndProc,
					NULL,
					590, 280, 800, 640,
					CS_HREDRAW | CS_VREDRAW,
					WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
					NULL);
				while (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				status = 3;
			}
		}
		else
		{
			status = 2;
		}
	}
	else
	{
		status = 1;
	}

	// Messages if errors.
	if (status)
	{
		LPCSTR s = "?";
		switch (status)
		{
		case 1:
			s = "Classes initialization failed.";
			break;
		case 2:
			s = "Resources initialization failed.";
			break;
		case 3:
			s = "Build system tree failed.";
			break;
		default:
			s = "Unknown error.";
			break;
		}
		MessageBox(NULL, s, "Device manager error.", MB_OK);
	}

	// Restore system context and exit application.
	if (pController)
	{
		pController->ReleaseTree();
		delete pController;
	}
	if (pView) delete pView;
	if (pModel) delete pModel;
	return status;
}
