/*

Данные и процедуры для формирования информации о конфигурации системы в виде
дерева (tree). На данном этапе, для отладки визуализации дерева, используется
эмуляция на основе предопределенных данных, без сканирования системной конфигурации.
По классификации TreeHolder/TreeBuilder/TreeViewer это Holder.
По классификации Model/View/Controller это Model.

*/

#include "AppModel.h"
#include "AppView.h"
#include "AppController.h"

const char* ICON_NAMES[] =
{ "This computer ( EMULATED DATA )", "Closed", "Opened", "Closed and light", "Opened and light",
  "ACPI", "ACPI HAL", "PCI", "SCSI", "User mode bus", "System tree", "Other devices",
  "Bluetooth", "Audio inputs and outputs", "Video adapters", "Embedded software", "Disk drives",
  "Audio, game and video devices", "Keyboards", "Software components", "Computer", "IDE/ATAPI controllers",
  "USB controllers", "Mass storage controllers", "Monitors", "Mouses and other pointing devices", "Print queues",
  "Mobile devices", "LPT and COM ports", "Software devices", "Processors", "Network adapters",
  "System devices", "Human Interface Devices", "USB devices", "Security devices", "Image processing devices" };
const int ICON_IDS[] =
{ IDI_APPLICATION_TITLE, IDI_ITEM_CLOSED, IDI_ITEM_OPENED, IDI_ITEM_CLOSED_LIGHT, IDI_ITEM_OPENED_LIGHT,
  IDI_ACPI, IDI_ACPI_HAL, IDI_PCI, IDI_SCSI, IDI_USER_MODE_BUS, IDI_SYSTEM_TREE, IDI_OTHER_DEVICES,
  IDI_BLUETOOTH, IDI_AUDIO_IN_OUT, IDI_VIDEO_ADAPTERS, IDI_UEFI, IDI_MASS_STORAGE,
  IDI_HIGH_DEFINITION_AUDIO, IDI_KEYBOARDS, IDI_SOFTWARE_COMPONENTS, IDI_THIS_COMPUTER, IDI_IDE,
  IDI_USB, IDI_PCI_IDE, IDI_VIDEO_DISPLAYS, IDI_MOUSES, IDI_PRINT_QUEUES,
  IDI_MOBILE_DEVICES, IDI_LPT_AND_COM, IDI_SOFTWARE_DEFINED, IDI_PROCESSORS, IDI_NETWORK_ADAPTERS,
  IDI_ROOT_ENUMERATOR, IDI_HID, IDI_USB_MASS_STORAGE, IDI_SECURITY_DEVICES, IDI_IMAGE_PROC };
const int ICON_COUNT = sizeof(ICON_IDS) / sizeof(int);
HICON iconHandles[ICON_COUNT];

LPCSTR STRINGS_BLUETOOTH[] = { "Intel(R) Wireless Bluetooth", "Microsoft Bluetooth enumerator", "Some Bluetooth adapter" };
int ICONS_BLUETOOTH[] = { ID_BLUETOOTH, ID_BLUETOOTH, ID_BLUETOOTH };
int COUNT_BLUETOOTH = sizeof(ICONS_BLUETOOTH) / sizeof(int);

LPCSTR STRINGS_AUDIOINOUT[] = { "Speakers", "Microphone" };
int ICONS_AUDIOINOUT[] = { ID_AUDIOINOUT, ID_AUDIOINOUT };
int COUNT_AUDIOINOUT = sizeof(ICONS_AUDIOINOUT) / sizeof(int);

LPCSTR STRINGS_VIDEOADAPTER[] = { "GTX850", "RTX3060Ti" };
int ICONS_VIDEOADAPTER[] = { ID_VIDEOADAPTER, ID_VIDEOADAPTER };
int COUNT_VIDEOADAPTER = sizeof(ICONS_VIDEOADAPTER) / sizeof(int);

LPCSTR STRINGS_EMBEDDEDSOFT[] = { "AMI UEFI firmware", "UEFI Video Driver" };
int ICONS_EMBEDDEDSOFT[] = { ID_EMBEDDEDSOFT, ID_EMBEDDEDSOFT };
int COUNT_EMBEDDEDSOFT = sizeof(ICONS_EMBEDDEDSOFT) / sizeof(int);

LPCSTR STRINGS_DISKDRIVE[] = { "Samsung", "Toshiba" };
int ICONS_DISKDRIVE[] = { ID_DISKDRIVE, ID_DISKDRIVE };
int COUNT_DISKDRIVE = sizeof(ICONS_DISKDRIVE) / sizeof(int);

LPCSTR STRINGS_AUDIO[] = { "HD Webcam", "High Definition Audio", "NVIDIA WDM" };
int ICONS_AUDIO[] = { ID_AUDIO, ID_AUDIO, ID_AUDIO };
int COUNT_AUDIO = sizeof(ICONS_AUDIO) / sizeof(int);

LPCSTR STRINGS_KEYBOARD[] = { "Keyboard" };
int ICONS_KEYBOARD[] = { ID_KEYBOARD };
int COUNT_KEYBOARD = sizeof(ICONS_KEYBOARD) / sizeof(int);

LPCSTR STRINGS_SOFTCOMPONENTS[] = { "Intel WiFi" };
int ICONS_SOFTCOMPONENTS[] = { ID_SOFTCOMPONENTS };
int COUNT_SOFTCOMPONENTS = sizeof(ICONS_SOFTCOMPONENTS) / sizeof(int);

LPCSTR STRINGS_COMPUTER[] = { "x64-based computer with ACPI" };
int ICONS_COMPUTER[] = { ID_COMPUTER };
int COUNT_COMPUTER = sizeof(ICONS_COMPUTER) / sizeof(int);

LPCSTR STRINGS_IDECTRL[] = { "Standard SATA AHCI" };
int ICONS_IDECTRL[] = { ID_IDECTRL };
int COUNT_IDECTRL = sizeof(ICONS_IDECTRL) / sizeof(int);

LPCSTR STRINGS_USBCTRL[] = { "USB3 xHCI", "USB Root Hub" };
int ICONS_USBCTRL[] = { ID_USBCTRL, ID_USBCTRL };
int COUNT_USBCTRL = sizeof(ICONS_USBCTRL) / sizeof(int);

LPCSTR STRINGS_STORAGECTRL[] = { "Microsoft storage" };
int ICONS_STORAGECTRL[] = { ID_STORAGECTRL };
int COUNT_STORAGECTRL = sizeof(ICONS_STORAGECTRL) / sizeof(int);

LPCSTR STRINGS_MONITOR[] = { "ASUS" };
int ICONS_MONITOR[] = { ID_MONITOR };
int COUNT_MONITOR = sizeof(ICONS_MONITOR) / sizeof(int);

LPCSTR STRINGS_MOUSE[] = { "Mouse" };
int ICONS_MOUSE[] = { ID_MOUSE };
int COUNT_MOUSE = sizeof(ICONS_MOUSE) / sizeof(int);

LPCSTR STRINGS_PRINTER[] = { "Canon Pixma IP2700" };
int ICONS_PRINTER[] = { ID_PRINTER };
int COUNT_PRINTER = sizeof(ICONS_PRINTER) / sizeof(int);

LPCSTR STRINGS_MOBILEDEVICES[] = { "USB Flash Drive" };
int ICONS_MOBILEDEVICES[] = { ID_MOBILEDEVICES };
int COUNT_MOBILEDEVICES = sizeof(ICONS_MOBILEDEVICES) / sizeof(int);

LPCSTR STRINGS_PORTS[] = { "COM1", "COM2", "LPT" };
int ICONS_PORTS[] = { ID_PORTS, ID_PORTS, ID_PORTS };
int COUNT_PORTS = sizeof(ICONS_PORTS) / sizeof(int);

LPCSTR STRINGS_SOFTDEVICES[] = { "Microsoft Root Enumerator" };
int ICONS_SOFTDEVICES[] = { ID_SOFTDEVICES };
int COUNT_SOFTDEVICES = sizeof(ICONS_SOFTDEVICES) / sizeof(int);

LPCSTR STRINGS_PROCESSOR[] = { "Intel Core i7", "Intel Core i7", "Intel Core i7", "Intel Core i7" };
int ICONS_PROCESSOR[] = { ID_PROCESSOR, ID_PROCESSOR, ID_PROCESSOR, ID_PROCESSOR };
int COUNT_PROCESSOR = sizeof(ICONS_PROCESSOR) / sizeof(int);

LPCSTR STRINGS_NETWORK[] = { "Realtek LAN" };
int ICONS_NETWORK[] = { ID_NETWORK };
int COUNT_NETWORK = sizeof(ICONS_NETWORK) / sizeof(int);

LPCSTR STRINGS_SYSTEM[] = { "DMA", "IRQ", "Timer", "RTC" };
int ICONS_SYSTEM[] = { ID_SYSTEM, ID_SYSTEM, ID_SYSTEM, ID_SYSTEM };
int COUNT_SYSTEM = sizeof(ICONS_SYSTEM) / sizeof(int);

LPCSTR STRINGS_HID[] = { "Input device" };
int ICONS_HID[] = { ID_HID };
int COUNT_HID = sizeof(ICONS_HID) / sizeof(int);

LPCSTR STRINGS_USBDEVICES[] = { "Webcam", "Mouse", "Printer" };
int ICONS_USBDEVICES[] = { ID_USBDEVICES, ID_USBDEVICES, ID_USBDEVICES };
int COUNT_USBDEVICES = sizeof(ICONS_USBDEVICES) / sizeof(int);

LPCSTR STRINGS_SECURITY[] = { "Trust Platform Module TPM 2.0" };
int ICONS_SECURITY[] = { ID_SECURITY };
int COUNT_SECURITY = sizeof(ICONS_SECURITY) / sizeof(int);

LPCSTR STRINGS_IMAGEDEVICES[] = { "Logitech HD Webcam C310" };
int ICONS_IMAGEDEVICES[] = { ID_IMAGEDEVICES };
int COUNT_IMAGEDEVICES = sizeof(ICONS_IMAGEDEVICES) / sizeof(int);

TREECHILDS nodeChilds[] = {
	{ COUNT_BLUETOOTH      , ICONS_BLUETOOTH     , STRINGS_BLUETOOTH      } ,
	{ COUNT_AUDIOINOUT     , ICONS_AUDIOINOUT    , STRINGS_AUDIOINOUT     } ,
	{ COUNT_VIDEOADAPTER   , ICONS_VIDEOADAPTER  , STRINGS_VIDEOADAPTER   } ,
	{ COUNT_EMBEDDEDSOFT   , ICONS_EMBEDDEDSOFT  , STRINGS_EMBEDDEDSOFT   } ,
	{ COUNT_DISKDRIVE      , ICONS_DISKDRIVE     , STRINGS_DISKDRIVE      } ,
	{ COUNT_AUDIO          , ICONS_AUDIO         , STRINGS_AUDIO          } ,
	{ COUNT_KEYBOARD       , ICONS_KEYBOARD      , STRINGS_KEYBOARD       } ,
	{ COUNT_SOFTCOMPONENTS , ICONS_SOFTCOMPONENTS, STRINGS_SOFTCOMPONENTS } ,
	{ COUNT_COMPUTER       , ICONS_COMPUTER      , STRINGS_COMPUTER       } ,
	{ COUNT_IDECTRL        , ICONS_IDECTRL       , STRINGS_IDECTRL        } ,
	{ COUNT_USBCTRL        , ICONS_USBCTRL       , STRINGS_USBCTRL        } ,
	{ COUNT_STORAGECTRL    , ICONS_STORAGECTRL   , STRINGS_STORAGECTRL    } ,
	{ COUNT_MONITOR        , ICONS_MONITOR       , STRINGS_MONITOR        } ,
	{ COUNT_MOUSE          , ICONS_MOUSE         , STRINGS_MOUSE          } ,
	{ COUNT_PRINTER        , ICONS_PRINTER       , STRINGS_PRINTER        } ,
	{ COUNT_MOBILEDEVICES  , ICONS_MOBILEDEVICES , STRINGS_MOBILEDEVICES  } ,
	{ COUNT_PORTS          , ICONS_PORTS         , STRINGS_PORTS          } ,
	{ COUNT_SOFTDEVICES    , ICONS_SOFTDEVICES   , STRINGS_SOFTDEVICES    } ,
	{ COUNT_PROCESSOR      , ICONS_PROCESSOR     , STRINGS_PROCESSOR      } ,
	{ COUNT_NETWORK        , ICONS_NETWORK       , STRINGS_NETWORK        } ,
	{ COUNT_SYSTEM         , ICONS_SYSTEM        , STRINGS_SYSTEM         } ,
	{ COUNT_HID            , ICONS_HID           , STRINGS_HID            } ,
	{ COUNT_USBDEVICES     , ICONS_USBDEVICES    , STRINGS_USBDEVICES     } ,
	{ COUNT_SECURITY       , ICONS_SECURITY      , STRINGS_SECURITY       } ,
	{ COUNT_IMAGEDEVICES   , ICONS_IMAGEDEVICES  , STRINGS_IMAGEDEVICES   }
};

PTREENODE tree = NULL;
POINT base = { 0, 0 };

bool InitializeTree()
{
	HICON hIcon = NULL;
	HMODULE hModule = GetModuleHandle(NULL);
	for (int i = 0; i < ICON_COUNT; i++)
	{
		hIcon = (HICON)LoadImage(hModule, MAKEINTRESOURCE(ICON_IDS[i]),
			IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		iconHandles[i] = hIcon;
		if (!hIcon) break;
	}
	return hIcon;
}

void DeInitializeTree()
{
	for (int i = 0; i < ICON_COUNT; i++) { if (iconHandles[i]) DestroyIcon(iconHandles[i]); }
}

UINT GetIconIdByIndex(UINT index)
{
	UINT id = 0;
	if (index < ICON_COUNT)
	{
		id = ICON_IDS[index];
	}
	return id;
}

HICON GetIconHandleByIndex(UINT index)
{
	HICON hIcon = NULL;
	if (index < ICON_COUNT)
	{
		hIcon = iconHandles[index];
	}
	return hIcon;
}

LPCSTR GetIconNameByIndex(UINT index)
{
	LPCSTR name = NULL;
	if (index < ICON_COUNT)
	{
		name = ICON_NAMES[index];
	}
	return name;
}

PTREENODE GetTree()
{
	return tree;
}

void SetTree(PTREENODE t)
{
	tree = t;
}

POINT GetBase()
{
	return base;
}

void SetBase(POINT b)
{
	base = b;
}
