#include <iostream>
#include <iomanip>
#include <windows.h>
#include "spdid.h"
#include "jedecvendor.h"
using namespace std;

const char* spdRevision      = "SPD revision";
const char* spdNumOfBytes    = "Total number of bytes in the SPD";
const char* spdDeviceType    = "Device type";
const char* spdModuleType    = "Module type";
const char* spdRowColumnBits = "Row/Column address bits";
const char* spdBankAndType   = "Bank address bits and type";
const char* spdRefreshBank   = "Refresh bank bits";
const char* spdTref          = "tREF";
const char* spdProtocol      = "Protocol version";
const char* spdMiscDevice    = "Misc. device configuration";
const char* spdTrpRmin       = "tRP-R, min";
const char* spdTrasRmin      = "tRAS-R, min";
const char* spdTrcdRmin      = "tRCD-R, min";
const char* spdTrrRmin       = "tRR-R, min";
const char* spdTppRmin       = "tPP-R, min";
const char* spdMinCycleA     = "Min tCYCLE for range A";
const char* spdMaxCycleA     = "Max tCYCLE for range A";
const char* spdTcdlyA        = "tCDLY range for range A";
const char* spdTclsTcasA     = "tCLS and tCAS range for range A";
const char* spdMinCycleB     = "Min tCYCLE for range B";
const char* spdMaxCycleB     = "Max tCYCLE for range B";
const char* spdTcdlyB        = "tCDLY range for range B";
const char* spdTclsTcasB     = "tCLS and tCAS range for range B";
const char* spdMinCycleC     = "Min tCYCLE for range C";
const char* spdMaxCycleC     = "Max tCYCLE for range C";
const char* spdTcdlyC        = "tCDLY range for range C";
const char* spdTclsTcasC     = "tCLS and tCAS range for range C";
const char* spdMinCycleD     = "Min tCYCLE for range D";
const char* spdMaxCycleD     = "Max tCYCLE for range D";
const char* spdTcdlyD        = "tCDLY range for range D";
const char* spdTclsTcasD     = "tCLS and tCAS range for range D";
const char* spdTpdnxAmax     = "tPDNXA, max";
const char* spdTpdnxBmax     = "tPDNXB, max";
const char* spdTnapxAmax     = "tNAPXA, max";
const char* spdTnapxBmax     = "tNAPXB, max";
const char* spdFiminFimax    = "fIMIN[11:8], fIMAX[11:8]";
const char* spdFimin70       = "fIMIN[7:0]";
const char* spdFimax70       = "fIMAX[7:0]";
const char* spdOdfMapping    = "ODF mapping";
const char* spdTcctrlMax     = "tCCTRL, max";
const char* spdTtempMax      = "tTEMP, max";
const char* spdTtceMin       = "tTCE, min";
const char* spdTrasrMax      = "tRAS-R, max";
const char* spdTnlimitMax    = "tNLIMIT, max";
const char* spdActPch30      = "ACTREPT[3:0], PCHREPT[3:0]";
const char* spdCpcRdr30      = "CPCHREPT_DC[3:0], WRREPT_DC[3:0]";
const char* spdRetWpr30      = "RETREPT_DC[3:0], WRREPT_DC[3:0]";
const char* spdReserved      = "Reserved";
const char* spdFras118       = "fRAS[11:8]";
const char* spdFras70        = "fRAS[7:0]";
const char* spdPmaxTj        = "PMAX, hi, PMAX, lo, Tj";
const char* spdHeatSpreader  = "HeatSpreader, thermal sensor, Tplate";
const char* spdPstbyHi       = "PSTBY, hi";
const char* spdPactiHi       = "PACTI, hi";
const char* spdPactrwHi      = "PACTRW, hi";
const char* spdPstbyLo       = "PSTBY, lo";
const char* spdPactiLo       = "PACTI, lo";
const char* spdPactrwLo      = "PACTRW, lo";
const char* spdPnap          = "PNAP";
const char* spdPresa         = "PRESA";
const char* spdPresb         = "PRESB";
const char* spdChecksum062   = "Checksum for bytes 0-3Eh (0-62)";
const char* spdModuleManuf   = "Module manufacturer ID code";
const char* spdModuleLoc     = "Module manufacturer location";
const char* spdModulePart    = "Module part number";
const char* spdModuleRev     = "Module manufacturer revision code";
const char* spdModuleYear    = "Module manufacturing year";
const char* spdModuleWeek    = "Module manufacturing week";
const char* spdModuleSerial  = "Module serial number";
const char* spdNumOfDev      = "Number of devices on module";
const char* spdModuleData    = "Module data width";
const char* spdDeviceEnables = "Device enables";
const char* spdModuleVdd     = "Module Vdd[3:0], Vinterface[3:0]";
const char* spdModuleVddT    = "Module Vdd tolerance";
const char* spdCdly01cdly3   = "CDLY0/1 for tCDLY=3";
const char* spdCdly01cdly4   = "CDLY0/1 for tCDLY=4";
const char* spdCdly01cdly5   = "CDLY0/1 for tCDLY=5";
const char* spdCdly01cdly6   = "CDLY0/1 for tCDLY=6";
const char* spdCdly01cdly7   = "CDLY0/1 for tCDLY=7";
const char* spdCdly01cdly8   = "CDLY0/1 for tCDLY=8";
const char* spdCdly01cdly9   = "CDLY0/1 for tCDLY=9";
const char* spdCdly01cdly10  = "CDLY0/1 for tCDLY=10";
const char* spdCdly01cdly11  = "CDLY0/1 for tCDLY=11";
const char* spdCdly01cdly12  = "CDLY0/1 for tCDLY=12";
const char* spdCdly01cdly13  = "CDLY0/1 for tCDLY=13";
const char* spdCdly01cdly14  = "CDLY0/1 for tCDLY=14";
const char* spdCdly01cdly15  = "CDLY0/1 for tCDLY=15";
const char* spdChecksum99126 = "Checksum for bytes 63h-7Eh (99-126)";

struct SPD_BYTE_DESCRIPTOR
{
	const char* byteName;
	void(*printHandler)(char*, int, byte, byte*);
};

void helperPrintBinary(byte a, char* p)
{
	for (int i = 0; i < 8; i++)
	{
		int shift = 7 - i;
		int bit = (a >> shift) & 1;
		snprintf(p++, 2, "%d", bit);
	}
}
void helperPartName(char* dst, char* src, int m)
{
	for (int i = 0; i < 18; i++)
	{
		char c = *src++;
		if (c == 0)
		{
			break;
		}
		if ((c < ' ') || (c > '}'))
		{
			c = '.';
		}
		m -= snprintf(dst++, m, "%c", c);
	}
	*dst = 0;
}

void printSpdRevision(char* s, int n, byte myData, byte* allData)
{
	const char* rev;
	switch (myData)
	{
	case 0:
	case 255:
		rev = "invalid.";
		break;
	case 1:
		rev = "SPD rev. 0.7.";
		break;
	case 2:
		rev = "SPD rev. 1.0.";
		break;
	default:
		rev = "unknown.";
		break;
	}
	snprintf(s, n, rev);
}
void printSpdNumOfBytes(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (myData > 14)
	{
		snprintf(s, n, "reserved.");
	}
	else
	{
		int bytes = 1 << myData;
		snprintf(s, n, "%d bytes.", bytes);
	}
}
void printSpdDeviceType(char* s, int n, byte myData, byte* allData)
{
	const char* type;
	switch (myData)
	{
	case 0:
	case 255:
		type = "invalid.";
		break;
	case 1:
		type = "Direct Rambus RDRAM device.";
		break;
	default:
		type = "reserved.";
		break;
	}
	snprintf(s, n, type);
}
void printSpdModuleType(char* s, int n, byte myData, byte* allData)
{
	const char* type;
	switch (myData)
	{
	case 0:
	case 255:
		type = "invalid.";
		break;
	case 1:
		type = "RIMM module.";
		break;
	case 2:
		type = "SO-RIMM module.";
		break;
	case 3:
		type = "Embedded (with controller).";
		break;
	case 4:
		type = "32-bit RIMM module.";
		break;
	default:
		type = "reserved.";
		break;
	}
	snprintf(s, n, type);
}
void printSpdRowColumnBits(char* s, int n, byte myData, byte* allData)
{
	int r = (myData >> 4) & 0x0F;
	int c = myData & 0x0F;
	snprintf(s, n, "row bits: %d, column bits: %d", r, c);
}
void printSpdBankAndType(char* s, int n, byte myData, byte* allData)
{
	int db = ( myData >> 7 ) & 0x01;
	int sb = ( myData >> 6 ) & 0x01;
	int banks = 1 << ( myData & 0x07);
	snprintf(s, n, "doubled: %d, split: %d, banks: %d", db, sb, banks);
}
void printSpdRefreshBank(char* s, int n, byte myData, byte* allData)
{
	byte bankBits = (*(allData + 5) & 0x07) - myData;
	if (bankBits < 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		int banks = 1 << bankBits;
		snprintf(s, n, "simultaneously refreshed banks: %d.", banks);
	}
}
void printSpdTref(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "refresh period: %d ms.", myData);
	}
}
void printSpdProtocol(char* s, int n, byte myData, byte* allData)
{
	const char* type;
	switch (myData)
	{
	case 0:
	case 255:
		type = "invalid.";
		break;
	case 1:
		type = "protocol version 0.";
		break;
	case 2:
		type = "protocol version 1.";
		break;
	case 4:
		type = "protocol version 3.";
		break;
	default:
		type = "reserved.";
		break;
	}
	snprintf(s, n, type);
}
void printSpdMiscDevice(char* s, int n, byte myData, byte* allData)
{
	float tdqs = 0.5;
	if (myData & 0x01)
	{
		tdqs = 1.5;
	}
	int lpsr = (myData >> 1) & 0x01;
	int s28ieco = (myData >> 2) & 0x01;
	int s3pdnd = (myData >> 7) & 0x01;
	snprintf(s, n, "tDQS, min: %.1f SCK, LPSR: %d, S28IECO: %d, S3PDN dis: %d.",
		     tdqs, lpsr, s28ieco, s3pdnd);
}
void printSpdTrpRmin(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "minimum precharge-to-RAS time: %d clks.", myData);
	}
}
void printSpdTrasRmin(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "minimum RAS-to-precharge time: %d clks.", myData);
	}
}
void printSpdTrcdRmin(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "minimum activate-to-CAS time: %d clks.", myData);
	}
}
void printSpdTrrRmin(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "minimum RAS-to-RAS time: %d clks.", myData);
	}
}
void printSpdTppRmin(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "minimum precharge-to-precharge time: %d clks.", myData);
	}
}
void printTpsAndFmhz(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "this range not used.");
	}
	else
	{
		double ps = myData * 128;
		double ns = ps / 1000.0;
		double mhz = 1000.0 / ns;
		snprintf(s, n, "tCYCLE: %.3f ns, %.3f MHz.", ns, mhz);
	}
}
void printSpdTcdlyABCD(char* s, int n, byte myData, byte* allData)
{
	int dmin = (myData & 0xF0) >> 4;
	int dmax = myData & 0x0F;
	if (myData == 0)
	{
		snprintf(s, n, "this range not used.");
	}
	else if ((dmin == 0) || (dmax == 0))
	{
		snprintf(s, n, "this range is invalid.");
	}
	else
	{
		snprintf(s, n, "tCDLY min: %d, tCDLY max: %d clks.", dmin, dmax);
	}
}
void printSpdTclsTcasABCD(char* s, int n, byte myData, byte* allData)
{
	int clsmin = (myData & 0xC0) >> 6;
	int clsmax = (myData & 0x30) >> 4;
	int casmin = (myData & 0x30) >> 2;
	int casmax = myData & 0x03;
	if (myData == 0)
	{
		snprintf(s, n, "this range not used.");
	}
	else if ((clsmin == 0) || (clsmax == 0) || (casmin == 0) || (casmax == 0))
	{
		snprintf(s, n, "this range is invalid.");
	}
	else
	{
		snprintf(s, n, "tCLS min: %d, tCLS max: %d, tCAS min: %d, tCAS max: %d clks.",
			clsmin, clsmax, casmin, casmax);
	}
}
void printSpdTus(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%d uS.", myData);
	}
}
void printSpdTpdnxBmax(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%d CFM cycles.", myData * 64);
	}
}
void printSpdTns(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%d ns.", myData);
	}
}
void printSpdFiminFimax(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "part of fMIN, fMAX bit fields, see below.");
}
void printSpdFimin70(char* s, int n, byte myData, byte* allData)
{
	int high = ((*(allData + 35) << 4) & 0x0F00);
	int low = myData & 0xFF;
	int f = high + low;
	if (f < 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "%d MHz.", f);
	}
}
void printSpdFimax70(char* s, int n, byte myData, byte* allData)
{
	int high = ((*(allData + 35) << 8) & 0x0F00);
	int low = myData & 0xFF;
	int f = high + low;
	if (f < 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "%d MHz.", f);
	}
}
void printSpdOdfMapping(char* s, int n, byte myData, byte* allData)
{
	const char* map;
	int index = myData & 0x03;
	switch (index)
	{
	case 0:
		map = "0%% (ASYM=0) or undefined (ASYM=1).";
		break;
	case 1:
		map = "0%% (ASYM=0) or 12.5%% (ASYM=1).";
		break;
	case 2:
	case 3:
		map = "reserved.";
		break;
	default:
		map = "?.";
		break;
	}
	snprintf(s, n, map);
}
void printSpdTms(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%d ms.", myData);
	}
}
void printSpdTclks(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%d clks.", myData);
	}
}
void printSpdTclksDual(char* s, int n, byte myData, byte* allData)
{
	int high = (myData >> 4) & 0x0F;
	int low = myData & 0x0F;
	snprintf(s, n, "%d clks, %d clks.", high, low);
}
void printSpdFras118(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "part of fRAS bit fields, see below.");
}
void printSpdFras70(char* s, int n, byte myData, byte* allData)
{
	int high = ((*(allData + 50) << 8) & 0x0F00);
	int low = myData & 0xFF;
	int f = high + low;
	if (f < 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "%d MHz.", f);
	}
}
void printSpdPmaxTj(char* s, int n, byte myData, byte* allData)
{
	int pmaxhi = (myData >> 7) & 0x01;
	int pmaxlo = (myData >> 6) & 0x01;
	int tj = (myData & 0x3F) + 64;
	snprintf(s, n, "PMAX,HI: %d, PMAX,LO: %d, Tj: %d C.", pmaxhi, pmaxlo, tj);
}
void printSpdHeatSpreader(char* s, int n, byte myData, byte* allData)
{
	int spreader = (myData >> 7) & 0x01;
	int sensor = (myData >> 6) & 0x01;
	int tplate = (myData & 0x3F) + 64;
	snprintf(s, n, "spreader: %d, sensor: %d, Tplate: %d C.", 
		spreader, sensor, tplate);
}
void printSpdPstbyHi(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not known, assume a sub-optimal value.");
	}
	else
	{
		snprintf(s, n, "%d ma.", myData);
	}
}
void printSpdPactiHi(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not known, assume a sub-optimal value.");
	}
	else
	{
		snprintf(s, n, "%d ma.", myData * 2);
	}
}
void printSpdPactrwHi(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not known, assume a sub-optimal value.");
	}
	else
	{
		snprintf(s, n, "%d ma.", myData * 8);
	}
}
void printSpdPnap(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not known, assume a sub-optimal value.");
	}
	else
	{
		snprintf(s, n, "%d ua.", myData * 128);
	}
}
void printSpdPresa(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "reserved for a future thermal management.");
}
void printSpdChecksum062(char* s, int n, byte myData, byte* allData)
{
	int sum = 0;
	int temp = 0;
	for (int i = 0; i < 63; i++)
	{
		temp = (*allData++) & 0xFF;
		sum += temp;
	}
	const char* result = "failed";
	int a = sum & 0xFF;
	int b = myData & 0xFF;
	if (a == b)
	{
		result = "passed";
	}
	snprintf(s, n, "calculated: %04Xh, %s.", sum, result);
}
void printSpdModuleManuf(char* s, int n, byte myData, byte* allData)
{
	const char* vendor = decodeModuleManufacturer(allData+64);
	snprintf(s, n, "%s.", vendor);
}
void printSpdSeeAbove(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "see above.");
}
void printSpdModuleLoc(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "vendor-specific.");
}
void printSpdModulePart(char* s, int n, byte myData, byte* allData)
{
	char partName[19+1];
	char* dst = partName;
	char* src = (char*)allData + 73;
	int m = 19;
	helperPartName(dst, src, m);
	snprintf(s, n, "%s.", partName);
}
void printSpdModuleRev(char* s, int n, byte myData, byte* allData)
{
	int byte1 = myData & 0xFF;
	int byte2 = *(allData + 92) & 0xFF;
	int rev = byte1 + (byte2 << 8);
	char revStr[3+1];
	int revTemp = rev;
	char* dst = revStr;
	int m = 3;
	for (int i = 0; i < 2; i++)
	{
		char c = revTemp & 0xFF;
		revTemp >>= 8;
		if (c == 0)
		{
			break;
		}
		if ((c < ' ') || (c > '}'))
		{
			c = '.';
		}
		m -= snprintf(dst++, m, "%c", c);
	}
	*dst = 0;
	snprintf(s, n, "%d = %04Xh, '%s'", rev, rev, revStr);
}
void printSpdModuleYear(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	int year = 1900 + a;
	if (a == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (year < 1995)
	{
		snprintf(s, n, "invalid (%d).", year);
	}
	else
	{
		snprintf(s, n, "%d.", year);
	}
}
void printSpdModuleWeek(char* s, int n, byte myData, byte* allData)
{
	int week = myData & 0xFF;
	if (week == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (week > 52)
	{
		snprintf(s, n, "invalid (%d).", week);
	}
	else
	{
		snprintf(s, n, "%d.", week);
	}
}
void printSpdModuleSerial(char* s, int n, byte myData, byte* allData)
{
	int byte0 = myData & 0xFF;
	int byte1 = *(allData + 96) & 0xFF;
	int byte2 = *(allData + 97) & 0xFF;
	int byte3 = *(allData + 98) & 0xFF;
	int serialNumber = byte0 + (byte1 << 8) + (byte2 << 16) + (byte3 << 24);
	if (serialNumber == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (serialNumber == -1)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		long long extNum = serialNumber & 0xFFFFFFFF;
		snprintf(s, n, "%llu = %08Xh.", extNum, serialNumber);
	}
}
void printSpdNumOfDev(char* s, int n, byte myData, byte* allData)
{
	int chips = myData & 0x1F;
	if (chips == 0)
	{
		snprintf(s, n, "invalid.");
	}
	snprintf(s, n, "%d chips.", chips);
}
void printSpdModuleData(char* s, int n, byte myData, byte* allData)
{
	int data = myData & 0x1F;
	if (data == 0)
	{
		snprintf(s, n, "invalid.");
	}
	if ((data != 16) && (data != 18) && (data != 32) && (data != 36))
	{
		snprintf(s, n, "reserved value (%d bit).", data);
	}
	else
	{
		snprintf(s, n, "%d bit.", data);
	}
}
void printSpdDeviceEn1(char* s, int n, byte myData, byte* allData)
{
	char mask[9];
	if (myData == 0)
	{
		int k = snprintf(mask, 8, "absent");
		*(mask + k) = 0;
	}
	else
	{
		helperPrintBinary(myData, mask);
	}
	snprintf(s, n, "chips[08-01]: %s.", mask);
}
void printSpdDeviceEn2(char* s, int n, byte myData, byte* allData)
{
	char mask[9];
	if (myData == 0)
	{
		int k = snprintf(mask, 8, "absent");
		*(mask + k) = 0;
	}
	else
	{
		helperPrintBinary(myData, mask);
	}
	snprintf(s, n, "chips[16-09]: %s.", mask);
}
void printSpdDeviceEn3(char* s, int n, byte myData, byte* allData)
{
	char mask[9];
	if (myData == 0)
	{
		int k = snprintf(mask, 8, "absent");
		*(mask + k) = 0;
	}
	else
	{
		helperPrintBinary(myData, mask);
	}
	snprintf(s, n, "chips[24-17]: %s.", mask);
}
void printSpdDeviceEn4(char* s, int n, byte myData, byte* allData)
{
	char mask[9];
	if (myData == 0)
	{
		int k = snprintf(mask, 8, "absent");
		*(mask + k) = 0;
	}
	else
	{
		helperPrintBinary(myData, mask);
	}
	snprintf(s, n, "chips[32-25]: %s.", mask);
}
void printSpdModuleVdd(char* s, int n, byte myData, byte* allData)
{
	int vdd = (myData >> 4) & 0x0F;
	int vio = myData & 0x0F;
	const char* svdd;
	const char* svio;
	switch (vdd)
	{
	case 0:
		svdd = "invalid";
		break;
	case 1:
		svdd = "2.5V";
		break;
	case 2:
		svdd = "1.8V";
		break;
	default:
		svdd = "reserved";
		break;
	}
	switch (vio)
	{
	case 0:
		svio = "Direct Rambus 1.8V Vterm.";
		break;
	default:
		svio = "reserved.";
		break;
	}
	snprintf(s, n, "VDD: %s, VIO: %s", svdd, svio);
}
void printSpdModuleVddT(char* s, int n, byte myData, byte* allData)
{
	int vdddctol = (myData >> 4) & 0x0F;
	int vddactol = myData & 0x0F;
	const char* sdc;
	const char* sac;
	switch (vdddctol)
	{
	case 5:
		sdc = "5%";
		break;
	default:
		sdc = "reserved";
		break;
	}
	switch (vddactol)
	{
	case 2:
		sac = "2%";
		break;
	default:
		sac = "reserved";
		break;
	}
	snprintf(s, n, "Module VDD DC tolerance: %s, AC tolerance: %s.", sdc, sac);
}
void printSpdCdly01cdly(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "unsupported.");
	}
	else
	{
		int cdly0 = (myData >> 4) & 0x0F;
		int cdly1 = myData & 0x0F;
		snprintf(s, n, "CDLY0: %d, CDLY1: %d.", cdly0, cdly1);
	}
}
void printSpdChecksum99126(char* s, int n, byte myData, byte* allData)
{
	int sum = 0;
	int temp = 0;
	for (int i = 99; i < 127; i++)
	{
		temp = *(allData + i) & 0xFF;
		sum += temp;
	}
	const char* result = "failed";
	int a = sum & 0xFF;
	int b = myData & 0xFF;
	if (a == b)
	{
		result = "passed";
	}
	snprintf(s, n, "calculated: %04Xh, %s.", sum, result);
}
void printSpdReserved(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "reserved.");
}

SPD_BYTE_DESCRIPTOR rambusDecoder[] =
{
	{ spdRevision       , printSpdRevision      },
	{ spdNumOfBytes     , printSpdNumOfBytes    },
	{ spdDeviceType     , printSpdDeviceType    },
	{ spdModuleType     , printSpdModuleType    },
	{ spdRowColumnBits  , printSpdRowColumnBits },
	{ spdBankAndType    , printSpdBankAndType   },
	{ spdRefreshBank    , printSpdRefreshBank   },
	{ spdTref           , printSpdTref          },
	{ spdProtocol       , printSpdProtocol      },
	{ spdMiscDevice     , printSpdMiscDevice    },
	{ spdTrpRmin        , printSpdTrpRmin       },
	{ spdTrasRmin       , printSpdTrasRmin      },
	{ spdTrcdRmin       , printSpdTrcdRmin      },
	{ spdTrrRmin        , printSpdTrrRmin       },
	{ spdTppRmin        , printSpdTppRmin       },
	{ spdMinCycleA      , printTpsAndFmhz       },
	{ spdMaxCycleA      , printTpsAndFmhz       },
	{ spdTcdlyA         , printSpdTcdlyABCD     },
	{ spdTclsTcasA      , printSpdTclsTcasABCD  },
	{ spdMinCycleB      , printTpsAndFmhz       },
	{ spdMaxCycleB      , printTpsAndFmhz       },
	{ spdTcdlyB         , printSpdTcdlyABCD     },
	{ spdTclsTcasB      , printSpdTclsTcasABCD  },
	{ spdMinCycleC      , printTpsAndFmhz       },
	{ spdMaxCycleC      , printTpsAndFmhz       },
	{ spdTcdlyC         , printSpdTcdlyABCD     },
	{ spdTclsTcasC      , printSpdTclsTcasABCD  },
	{ spdMinCycleD      , printTpsAndFmhz       },
	{ spdMaxCycleD      , printTpsAndFmhz       },
	{ spdTcdlyD         , printSpdTcdlyABCD     },
	{ spdTclsTcasD      , printSpdTclsTcasABCD  },
	{ spdTpdnxAmax      , printSpdTus           },
	{ spdTpdnxBmax      , printSpdTpdnxBmax     },
	{ spdTnapxAmax      , printSpdTns           },
	{ spdTnapxBmax      , printSpdTns           },
	{ spdFiminFimax     , printSpdFiminFimax    },
	{ spdFimin70        , printSpdFimin70       },
	{ spdFimax70        , printSpdFimax70       },
	{ spdOdfMapping     , printSpdOdfMapping    },
	{ spdTcctrlMax      , printSpdTms           },
	{ spdTtempMax       , printSpdTms           },
	{ spdTtceMin        , printSpdTclks         },
	{ spdTrasrMax       , printSpdTus           },
	{ spdTnlimitMax     , printSpdTus           },
	{ spdActPch30       , printSpdTclksDual     },
	{ spdCpcRdr30       , printSpdTclksDual     },
	{ spdRetWpr30       , printSpdTclksDual     },
	{ spdReserved       , printSpdReserved      },
	{ spdReserved       , printSpdReserved      },
	{ spdReserved       , printSpdReserved      },
	{ spdFras118        , printSpdFras118       },
	{ spdFras70         , printSpdFras70        },
	{ spdPmaxTj         , printSpdPmaxTj        },
	{ spdHeatSpreader   , printSpdHeatSpreader  },
	{ spdPstbyHi        , printSpdPstbyHi       },
	{ spdPactiHi        , printSpdPactiHi       },
	{ spdPactrwHi       , printSpdPactrwHi      },
	{ spdPstbyLo        , printSpdPstbyHi       },
	{ spdPactiLo        , printSpdPactiHi       },
	{ spdPactrwLo       , printSpdPactrwHi      },
	{ spdPnap           , printSpdPnap          },
	{ spdPresa          , printSpdPresa         },
	{ spdPresb          , printSpdPresa         },
	{ spdChecksum062    , printSpdChecksum062   },
	{ spdModuleManuf    , printSpdModuleManuf   },
	{ spdModuleManuf    , printSpdSeeAbove      },
	{ spdModuleManuf    , printSpdSeeAbove      },
	{ spdModuleManuf    , printSpdSeeAbove      },
	{ spdModuleManuf    , printSpdSeeAbove      },
	{ spdModuleManuf    , printSpdSeeAbove      },
	{ spdModuleManuf    , printSpdSeeAbove      },
	{ spdModuleManuf    , printSpdSeeAbove      },
	{ spdModuleLoc      , printSpdModuleLoc     },
	{ spdModulePart     , printSpdModulePart    },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModulePart     , printSpdSeeAbove      },
	{ spdModuleRev      , printSpdModuleRev     },
	{ spdModuleRev      , printSpdSeeAbove      },
	{ spdModuleYear     , printSpdModuleYear    },
	{ spdModuleWeek     , printSpdModuleWeek    },
	{ spdModuleSerial   , printSpdModuleSerial  },
	{ spdModuleSerial   , printSpdSeeAbove      },
	{ spdModuleSerial   , printSpdSeeAbove      },
	{ spdModuleSerial   , printSpdSeeAbove      },
	{ spdNumOfDev       , printSpdNumOfDev      },
	{ spdModuleData     , printSpdModuleData    },
	{ spdDeviceEnables  , printSpdDeviceEn1     },
	{ spdDeviceEnables  , printSpdDeviceEn2     },
	{ spdDeviceEnables  , printSpdDeviceEn3     },
	{ spdDeviceEnables  , printSpdDeviceEn4     },
	{ spdModuleVdd      , printSpdModuleVdd     },
	{ spdModuleVddT     , printSpdModuleVddT    },
	{ spdReserved       , printSpdReserved      },
	{ spdReserved       , printSpdReserved      },
	{ spdReserved       , printSpdReserved      },
	{ spdReserved       , printSpdReserved      },
	{ spdReserved       , printSpdReserved      },
	{ spdReserved       , printSpdReserved      },
	{ spdReserved       , printSpdReserved      },
	{ spdCdly01cdly3    , printSpdCdly01cdly    },
	{ spdCdly01cdly4    , printSpdCdly01cdly    },
	{ spdCdly01cdly5    , printSpdCdly01cdly    },
	{ spdCdly01cdly6    , printSpdCdly01cdly    },
	{ spdCdly01cdly7    , printSpdCdly01cdly    },
	{ spdCdly01cdly8    , printSpdCdly01cdly    },
	{ spdCdly01cdly9    , printSpdCdly01cdly    },
	{ spdCdly01cdly10   , printSpdCdly01cdly    },
	{ spdCdly01cdly11   , printSpdCdly01cdly    },
	{ spdCdly01cdly12   , printSpdCdly01cdly    },
	{ spdCdly01cdly13   , printSpdCdly01cdly    },
	{ spdCdly01cdly14   , printSpdCdly01cdly    },
	{ spdCdly01cdly15   , printSpdCdly01cdly    },
	{ spdChecksum99126  , printSpdChecksum99126 }
};
const int RAMBUS_COUNT = sizeof(rambusDecoder) / sizeof(SPD_BYTE_DESCRIPTOR);

int parseSpd(byte* ptr, int size )
{
	cout << endl;
	parmGroup("Raw hex dump of SPD data");
	parmDump(ptr, size);
	cout << endl;

	// DEBUG.
	int type = *(ptr + 2);
	if (type != 1)
	{
		parmError("SPD parsing yet supported only for Direct Rambus SPD.");
		return 0;
	}
	// DEBUG.

	parmGroup("Module summary");
	const char* vendorName = decodeModuleManufacturer(ptr + 64);
	char partName[19 + 1];
	char* dst = partName;
	char* src = (char*)ptr + 73;
	int m = 19;
	helperPartName(dst, src, m);
	char speedName[32];
	int high = ((*(ptr + 50) << 8) & 0x0F00);
	int low = *(ptr + 51) & 0xFF;
	int f = (high + low) * 2;
	if (f < 0)
	{
		snprintf(speedName, 20, "PC???");
	}
	else
	{
		snprintf(speedName, 20, "PC%d", f);
	}
	char sizeName[48];
	int rowBits = (*(ptr + 4) >> 4) & 0x0F;
	int colBits = *(ptr + 4) & 0x0F;
	int bankBits = *(ptr + 5) & 0x07;
	long long chipsCount = *(ptr + 99);
	long long busFactor = *(ptr + 100) / 8;
	int finalFactor = 20 - 3;
	long long moduleSize = (chipsCount * busFactor) << rowBits << colBits << bankBits >> finalFactor;
	if (moduleSize < 0)
	{
		snprintf(sizeName, 48, "[Module size calculation failed.]");
	}
	else
	{
		snprintf(sizeName, 48, "%lldMB", moduleSize);
	}
	char moduleString [MAX_TEXT_STRING];
	snprintf(moduleString, MAX_TEXT_STRING, "%s %s %s %s", 
		vendorName, partName, speedName, sizeName);
	parmSummary(moduleString);
	cout << endl;

	parmGroup("Offset (hex) and parameter name          | Parameter value (hex) and comments");
	char nameBuf[MAX_TEXT_STRING];
	char valueBuf[MAX_TEXT_STRING];
	byte* ptr1 = ptr;
	for (int i = 0; i < RAMBUS_COUNT; i++)
	{
		snprintf(nameBuf, MAX_TEXT_STRING, "[%02X] %s", i, rambusDecoder[i].byteName);
		char* p = valueBuf;
		int n = MAX_TEXT_STRING;
		int m = snprintf(p, n, "%02X, ", *ptr1);
		p += m;
		n -= m;
		void(*handler)(char*, int, byte, byte*) = rambusDecoder[i].printHandler;
		if (handler)
		{
			handler(p, n, *ptr1, ptr);
		}
		parmValue(nameBuf, valueBuf);
		ptr1++;
	}

	cout << endl;
	return 0;
}
