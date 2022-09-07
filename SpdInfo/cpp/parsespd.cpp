/*

TODO.

1) Bugs with JEDEC vendor ID, DDR3. See JEDEC spec JEP-106.
2) Bugs with serial number pointers + X, SDRAM/DDR/DDR2. Optimize with offsets.
2) Verify module size, DDR3+.
2) Calculate CRC for DDR3+ SPD data.
3) DDR3 other bytes, DDR4, DDR5.
4) Optimization with helpers procedures: especially module size xxxMB, module speed PCxxx.
5) Procedures decodeModuleManufacturer, decodeModuleManufacturerDdr3 can return NULL, check.
6) Strings and routines naming optimizing.
7) Helpers for printDdr3fineTck and 4 next.
8) Add new vendors to JEDEC vendors data base;

*/

#include <iostream>
#include <iomanip>
#include <windows.h>
#include "spdinfo.h"
#include "jedecvendor.h"
using namespace std;

typedef enum {
	UNKNOWN = 0,
	DIRECT_RAMBUS = 1,
	RAMBUS = 2,
	FPM_DRAM = 3,
	EDO = 4,
	PIPELINED_NIBBLE = 5,
	SDR_SDRAM = 6,
	MULTIPLEXED_ROM = 7,
	DDR_SGRAM = 8,
	DDR_SDRAM = 9,
	DDR2_SDRAM = 10,
	DDR3_SDRAM = 11,
	DDR4_SDRAM = 12,
	DDR5_SDRAM = 13,
	N_RAM_TYPES = 14
} RamIndexType;
typedef enum {
	ID_RDRAM = 1,
	ID_SDR = 4,
	ID_DDR = 7,
	ID_DDR2 = 8,
	ID_DDR3 = 11,
	ID_DDR4 = 12,
	ID_DDR5 = 18,
} RamNativeType;

const char* spdRevision = "SPD revision";
const char* spdNumOfBytes = "Total number of bytes in the SPD";
const char* spdDeviceType = "Device type";
const char* spdModuleType = "Module type";
const char* spdRowColumnBits = "Row/Column address bits";
const char* spdBankAndType = "Bank address bits and type";
const char* spdRefreshBank = "Refresh bank bits";
const char* spdTref = "tREF";
const char* spdProtocol = "Protocol version";
const char* spdMiscDevice = "Misc. device configuration";
const char* spdTrpRmin = "tRP-R, min";
const char* spdTrasRmin = "tRAS-R, min";
const char* spdTrcdRmin = "tRCD-R, min";
const char* spdTrrRmin = "tRR-R, min";
const char* spdTppRmin = "tPP-R, min";
const char* spdMinCycleA = "Min tCYCLE for range A";
const char* spdMaxCycleA = "Max tCYCLE for range A";
const char* spdTcdlyA = "tCDLY range for range A";
const char* spdTclsTcasA = "tCLS and tCAS range for range A";
const char* spdMinCycleB = "Min tCYCLE for range B";
const char* spdMaxCycleB = "Max tCYCLE for range B";
const char* spdTcdlyB = "tCDLY range for range B";
const char* spdTclsTcasB = "tCLS and tCAS range for range B";
const char* spdMinCycleC = "Min tCYCLE for range C";
const char* spdMaxCycleC = "Max tCYCLE for range C";
const char* spdTcdlyC = "tCDLY range for range C";
const char* spdTclsTcasC = "tCLS and tCAS range for range C";
const char* spdMinCycleD = "Min tCYCLE for range D";
const char* spdMaxCycleD = "Max tCYCLE for range D";
const char* spdTcdlyD = "tCDLY range for range D";
const char* spdTclsTcasD = "tCLS and tCAS range for range D";
const char* spdTpdnxAmax = "tPDNXA, max";
const char* spdTpdnxBmax = "tPDNXB, max";
const char* spdTnapxAmax = "tNAPXA, max";
const char* spdTnapxBmax = "tNAPXB, max";
const char* spdFiminFimax = "fIMIN[11:8], fIMAX[11:8]";
const char* spdFimin70 = "fIMIN[7:0]";
const char* spdFimax70 = "fIMAX[7:0]";
const char* spdOdfMapping = "ODF mapping";
const char* spdTcctrlMax = "tCCTRL, max";
const char* spdTtempMax = "tTEMP, max";
const char* spdTtceMin = "tTCE, min";
const char* spdTrasrMax = "tRAS-R, max";
const char* spdTnlimitMax = "tNLIMIT, max";
const char* spdActPch30 = "ACTREPT[3:0], PCHREPT[3:0]";
const char* spdCpcRdr30 = "CPCHREPT_DC[3:0], WRREPT_DC[3:0]";
const char* spdRetWpr30 = "RETREPT_DC[3:0], WRREPT_DC[3:0]";
const char* spdReserved = "Reserved";
const char* spdFras118 = "fRAS[11:8]";
const char* spdFras70 = "fRAS[7:0]";
const char* spdPmaxTj = "PMAX, hi, PMAX, lo, Tj";
const char* spdHeatSpreader = "HeatSpreader, thermal sensor, Tplate";
const char* spdPstbyHi = "PSTBY, hi";
const char* spdPactiHi = "PACTI, hi";
const char* spdPactrwHi = "PACTRW, hi";
const char* spdPstbyLo = "PSTBY, lo";
const char* spdPactiLo = "PACTI, lo";
const char* spdPactrwLo = "PACTRW, lo";
const char* spdPnap = "PNAP";
const char* spdPresa = "PRESA";
const char* spdPresb = "PRESB";
const char* spdChecksum062 = "Checksum for bytes 0-3Eh (0-62)";
const char* spdModuleManuf = "Module manufacturer ID code";
const char* spdModuleLoc = "Module manufacturer location";
const char* spdModulePart = "Module part number";
const char* spdModuleRev = "Module manufacturer revision code";
const char* spdModuleYear = "Module manufacturing year";
const char* spdModuleWeek = "Module manufacturing week";
const char* spdModuleSerial = "Module serial number";
const char* spdNumOfDev = "Number of devices on module";
const char* spdModuleData = "Module data width";
const char* spdDeviceEnables = "Device enables";
const char* spdModuleVdd = "Module Vdd[3:0], Vinterface[3:0]";
const char* spdModuleVddT = "Module Vdd tolerance";
const char* spdCdly01cdly3 = "CDLY0/1 for tCDLY=3";
const char* spdCdly01cdly4 = "CDLY0/1 for tCDLY=4";
const char* spdCdly01cdly5 = "CDLY0/1 for tCDLY=5";
const char* spdCdly01cdly6 = "CDLY0/1 for tCDLY=6";
const char* spdCdly01cdly7 = "CDLY0/1 for tCDLY=7";
const char* spdCdly01cdly8 = "CDLY0/1 for tCDLY=8";
const char* spdCdly01cdly9 = "CDLY0/1 for tCDLY=9";
const char* spdCdly01cdly10 = "CDLY0/1 for tCDLY=10";
const char* spdCdly01cdly11 = "CDLY0/1 for tCDLY=11";
const char* spdCdly01cdly12 = "CDLY0/1 for tCDLY=12";
const char* spdCdly01cdly13 = "CDLY0/1 for tCDLY=13";
const char* spdCdly01cdly14 = "CDLY0/1 for tCDLY=14";
const char* spdCdly01cdly15 = "CDLY0/1 for tCDLY=15";
const char* spdChecksum99126 = "Checksum for bytes 63h-7Eh (99-126)";

const char* spdUsedBytes = "SPD bytes used for data";
const char* spdRomBytes = "SPD EPROM physical size";
const char* spdRowBits = "Row address bits";
const char* spdColumnBits = "Column address bits";
const char* spdRanks = "Physical ranks";
const char* spdModuleWidth = "DIMM module data width";
const char* spdVoltage = "Voltage interface";
const char* spdCycleAtMaxCl = "Cycle time at maximum CAS latency";
const char* spdAccessFromClk = "Access from clock";
const char* spdDimmConfig = "DIMM ECC and parity configuration";
const char* spdRefresh = "Refresh rate and type";
const char* spdChipsWidth = "DRAM chips data width";
const char* spdErrorCheck = "Error checking DRAM data width";
const char* spdMinClkB2B = "Min back-to-back random column, nCCD";
const char* spdBurstLengts = "Burst lengths supported";
const char* spdBanks = "Logical banks";
const char* spdCasLatencies = "CAS latencies supported";
const char* spdCsLatencies = "CS latencies supported";
const char* spdWrLatencies = "Write latencies";
const char* spdModuleAttr = "Module attributes";
const char* spdDeviceAttr = "Device attributes";
const char* spdMinClkClM1 = "Min clock cycle time at CL-1";
const char* spdMaxDataClM1 = "Max access time from clk at CL-1";
const char* spdMinClkClM2 = "Min clock cycle time at CL-2";
const char* spdMaxDataClM2 = "Max access time from clk at CL-2";
const char* spdMinRowPrchg = "Min row precharge time, tRP";
const char* spdMinRowAct = "Min row active to row active, tRRD";
const char* spdMinRasToCas = "Min RAS to CAS delay, tRCD";
const char* spdMinRasPulse = "Min RAS pulse width, tRAS";
const char* spdModuleBankDen = "Module bank density";
const char* spdAddrCmdSet = "Addr and cmd setup time before clk";
const char* spdAddrCmdHold = "Addr and cmd hold time after clk";
const char* spdDataInputSet = "Data input setup time before clock";
const char* spdDataInputHold = "Data input hold time after clock";
const char* spdSuperset = "Superset memory-specific features";
const char* spdManufSpec = "Manufacturer specific data";
const char* spdVendorSpec = "Vendor specific data";

const char* spdMinClkClM05 = "Min clock cycle time at CL-0.5";
const char* spdMaxDataClM05 = "Max access time from clk at CL-0.5";
const char* spdMinAutoRef = "Min act-to-act/auto refresh, tRC";
const char* spdMinAutoRefTo = "Min auto refresh to active, tRFC";
const char* spdMaxCycleTime = "Max cycle time";
const char* spdMaxDqsSkew = "Max DQS-DQ skew, tDQSQ max";
const char* spdReadSkew = "Read data hold skew, tQHS";
const char* spdDimmHeight = "DIMM height";

const char* spdRanksPckgH = "Ranks, package and height";
const char* spdDimmMech = "DIMM mechanical characteristics";
const char* spdDimmType = "DIMM type";
const char* spdWriteRec = "Write recovery time";
const char* spdWriteToRead = "Write to read command delay";
const char* spdReadToPrec = "Read to precharge delay";
const char* spdProbeChar = "Memory probe characteristics";
const char* spdExtTrcTrfc = "Extension for tRC and tRFC";
const char* spdPllRelock = "PLL relock time";
const char* spdTcaseMax = "Tcase max";
const char* spdThermRes = "Thermal resistance of DRAM";
const char* spdDramTriseAp = "DRAM T rise, activate/precharge";
const char* spdDramTriseP = "DRAM T rise, precharge";
const char* spdDramTrisePp = "DRAM T rise, precharge/powerdown";
const char* spdDramTriseAs = "DRAM T rise, active standby";
const char* spdDramTriseApf = "DRAM T rise, active powerdown, fast";
const char* spdDramTriseAps = "DRAM T rise, active powerdown, slow";
const char* spdDramTriseBr = "DRAM T rise, burst read";
const char* spdDramTriseBrf = "DRAM T rise, burst refresh";
const char* spdDramTriseBir = "DRAM T rise, bank interleave read";
const char* spdThermResPll = "Thermal resistance of PLL";
const char* spdThermResReg = "Thermal resistance of register";
const char* spdPllTriseA = "PLL T rise, active";
const char* spdRegTriseA = "Register T rise, active";

const char* spdDensBank = "SDRAM chips density and logical banks";
const char* spdAddressing = "SDRAM addressing";
const char* spdNomVoltage = "Module nominal voltage";
const char* spdModuleOrg = "Module organization";
const char* spdBusWidth = "Module bus width";
const char* spdFineTime = "Fine timebase (FTB) dividend/divisor";
const char* spdMediumTdvdn = "Medium timebase (MTB) dividend";
const char* spdMediumTdivs = "Medium timebase (MTB) divisor";
const char* spdMinCycleTime = "Min cycle time (MTB), tCKmin";
const char* spdMinCasLat = "Min CAS latency (MTB), tAAmin";
const char* spdMinWriteRecM = "Min write recovery (MTB), tWRmin";
const char* spdMinRasToCasM = "Min RAS-to-CAS delay (MTB), tRCDmin";
const char* spdMinRowActTaM = "Min row act to row act (MTB), tRRDmin";
const char* spdMinRowPrechM = "Min row precharge (MTB), tRPmin";
const char* spdUpTrasTrcM = "Upper nibbles for tRAS and tRC";
const char* spdMinActToPrM = "Min act to precharge (MTB), tRASmin";
const char* spdMinActToAcM = "Min act to act/ref (MTB), tRCmin";
const char* spdMinRefRecLoM = "Min ref recov (MTB), low, tRFCmin";
const char* spdMinRefRecHiM = "Min ref recov (MTB), high, tRFCmin";
const char* spdMinWrToRdM = "Min write to read  (MTB), tWTR";
const char* spdMinRdToPcM = "Min read to precharge (MTB), tRTP";
const char* spdUpTfawM = "Upper nibble for tFAW";
const char* spdMinFawM = "Min four act window (MTB), tFAWmin";
const char* spdOptional = "SDRAM optional features";
const char* spdThermRef = "SDRAM thermal and refresh options";
const char* spdThermSens = "Module thermal sensor";
const char* spdDeviceChips = "SDRAM chips and die type";
const char* spdFineTck = "Fine offset for tCKmin";
const char* spdFineTaa = "Fine offset for tAAmin";
const char* spdFineTrcd = "Fine offset for tRCDmin";
const char* spdFineTrp = "Fine offset for tRPmin";
const char* spdFineTrc = "Fine offset for tRCmin";
const char* spdMaxActCount = "SDRAM maximum active count (MAC)";
const char* spdFamilySpec = "Family-specific";
const char* spdCrcLow = "CRC, low byte";
const char* spdCrcHigh = "CRC, high byte";
const char* spdChipManuf = "DRAM chips manufacturer ID code";

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
	bool flag = true;
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
		if ((c == ' ') && flag)
		{
			flag = false;
		}
		else
		{
			m -= snprintf(dst++, m, "%c", c);
		}
	}
	*dst = 0;
	// remove right spaces
	for (int i = 0; i < 18; i++)
	{
		dst--;
		if (*dst == ' ')
		{
			*dst = 0;
		}
		else
		{
			break;
		}
	}
}

void printRdramRevision(char* s, int n, byte myData, byte* allData)
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
void printRdramNumOfBytes(char* s, int n, byte myData, byte* allData)
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
void printRdramDeviceType(char* s, int n, byte myData, byte* allData)
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
void printRdramModuleType(char* s, int n, byte myData, byte* allData)
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
void printRdramRowColumnBits(char* s, int n, byte myData, byte* allData)
{
	int r = (myData >> 4) & 0x0F;
	int c = myData & 0x0F;
	snprintf(s, n, "row bits: %d, column bits: %d", r, c);
}
void printRdramBankAndType(char* s, int n, byte myData, byte* allData)
{
	int db = (myData >> 7) & 0x01;
	int sb = (myData >> 6) & 0x01;
	int banks = 1 << (myData & 0x07);
	snprintf(s, n, "doubled: %d, split: %d, banks: %d", db, sb, banks);
}
void printRdramRefreshBank(char* s, int n, byte myData, byte* allData)
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
void printRdramTref(char* s, int n, byte myData, byte* allData)
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
void printRdramProtocol(char* s, int n, byte myData, byte* allData)
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
void printRdramMiscDevice(char* s, int n, byte myData, byte* allData)
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
void printRdramTrpRmin(char* s, int n, byte myData, byte* allData)
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
void printRdramTrasRmin(char* s, int n, byte myData, byte* allData)
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
void printRdramTrcdRmin(char* s, int n, byte myData, byte* allData)
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
void printRdramTrrRmin(char* s, int n, byte myData, byte* allData)
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
void printRdramTppRmin(char* s, int n, byte myData, byte* allData)
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
void printRdramTpsAndFmhz(char* s, int n, byte myData, byte* allData)
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
void printRdramTcdlyABCD(char* s, int n, byte myData, byte* allData)
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
void printRdramTclsTcasABCD(char* s, int n, byte myData, byte* allData)
{
	int clsmin = (myData & 0xC0) >> 6;
	int clsmax = (myData & 0x30) >> 4;
	int casmin = (myData & 0x0C) >> 2;
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
void printRdramTus(char* s, int n, byte myData, byte* allData)
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
void printRdramTpdnxBmax(char* s, int n, byte myData, byte* allData)
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
void printRdramTns(char* s, int n, byte myData, byte* allData)
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
void printRdramFiminFimax(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "part of fMIN, fMAX bit fields, see below.");
}
void printRdramFimin70(char* s, int n, byte myData, byte* allData)
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
void printRdramFimax70(char* s, int n, byte myData, byte* allData)
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
void printRdramOdfMapping(char* s, int n, byte myData, byte* allData)
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
void printRdramTms(char* s, int n, byte myData, byte* allData)
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
void printRdramTclks(char* s, int n, byte myData, byte* allData)
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
void printRdramTclksDual(char* s, int n, byte myData, byte* allData)
{
	int high = (myData >> 4) & 0x0F;
	int low = myData & 0x0F;
	snprintf(s, n, "%d clks, %d clks.", high, low);
}
void printRdramFras118(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "part of fRAS bit fields, see below.");
}
void printRdramFras70(char* s, int n, byte myData, byte* allData)
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
void printRdramPmaxTj(char* s, int n, byte myData, byte* allData)
{
	int pmaxhi = (myData >> 7) & 0x01;
	int pmaxlo = (myData >> 6) & 0x01;
	int tj = (myData & 0x3F) + 64;
	snprintf(s, n, "PMAX,HI: %d, PMAX,LO: %d, Tj: %d C.", pmaxhi, pmaxlo, tj);
}
void printRdramHeatSpreader(char* s, int n, byte myData, byte* allData)
{
	int spreader = (myData >> 7) & 0x01;
	int sensor = (myData >> 6) & 0x01;
	int tplate = (myData & 0x3F) + 64;
	snprintf(s, n, "spreader: %d, sensor: %d, Tplate: %d C.",
		spreader, sensor, tplate);
}
void printRdramPstbyHi(char* s, int n, byte myData, byte* allData)
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
void printRdramPactiHi(char* s, int n, byte myData, byte* allData)
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
void printRdramPactrwHi(char* s, int n, byte myData, byte* allData)
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
void printRdramPnap(char* s, int n, byte myData, byte* allData)
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
void printRdramPresa(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "reserved for a future thermal management.");
}
void printRdramChecksum062(char* s, int n, byte myData, byte* allData)
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
void printRdramModuleManuf(char* s, int n, byte myData, byte* allData)
{
	const char* vendor = decodeModuleManufacturer(allData + 64);
	snprintf(s, n, "%s.", vendor);
}
void printRdramSeeAbove(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "see above.");
}
void printRdramModuleLoc(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "vendor-specific.");
}
void printRdramModulePart(char* s, int n, byte myData, byte* allData)
{
	char partName[19 + 1];
	char* dst = partName;
	char* src = (char*)allData + 73;
	int m = 19;
	helperPartName(dst, src, m);
	snprintf(s, n, "%s.", partName);
}
void printRdramModuleRev(char* s, int n, byte myData, byte* allData)
{
	int byte1 = myData & 0xFF;
	int byte2 = *(allData + 92) & 0xFF;
	int rev = byte1 + (byte2 << 8);
	char revStr[3 + 1];
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
	snprintf(s, n, "%d = %04Xh, '%s'.", rev, rev, revStr);
}
void printRdramModuleYear(char* s, int n, byte myData, byte* allData)
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
void printRdramModuleWeek(char* s, int n, byte myData, byte* allData)
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
void printRdramModuleSerial(char* s, int n, byte myData, byte* allData)
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
void printRdramNumOfDev(char* s, int n, byte myData, byte* allData)
{
	int chips = myData & 0x1F;
	if (chips == 0)
	{
		snprintf(s, n, "invalid.");
	}
	snprintf(s, n, "%d chips.", chips);
}
void printRdramModuleData(char* s, int n, byte myData, byte* allData)
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
void printRdramDeviceEn1(char* s, int n, byte myData, byte* allData)
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
void printRdramDeviceEn2(char* s, int n, byte myData, byte* allData)
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
void printRdramDeviceEn3(char* s, int n, byte myData, byte* allData)
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
void printRdramDeviceEn4(char* s, int n, byte myData, byte* allData)
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
void printRdramModuleVdd(char* s, int n, byte myData, byte* allData)
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
void printRdramModuleVddT(char* s, int n, byte myData, byte* allData)
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
void printRdramCdly01cdly(char* s, int n, byte myData, byte* allData)
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
void printRdramChecksum99126(char* s, int n, byte myData, byte* allData)
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
void printRdramReserved(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "reserved.");
}

void helperRanksDensitySdram(int data, int& rank1, int& rank2)
{
	rank1 = 0;
	rank2 = 0;
	int mask = 1;
	for (int i = 0; i < 8; i++)
	{
		if (data & mask)
		{
			if (rank1)
			{
				rank2 = mask;
				break;
			}
			else
			{
				rank1 = mask;
			}
		}
		mask <<= 1;
	}
	rank1 *= 4;
	rank2 *= 4;
}

void printSdramUsedBytes(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%d bytes.", myData);
	}
}
void printSdramRomBytes(char* s, int n, byte myData, byte* allData)
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
void printSdramDeviceType(char* s, int n, byte myData, byte* allData)
{
	const char* type;
	switch (myData)
	{
	case ID_SDR:
		type = "SDR SDRAM";
		break;
	case ID_DDR:
		type = "DDR SDRAM";
		break;
	case ID_DDR2:
		type = "DDR2 SDRAM";
		break;
	case ID_DDR3:
		type = "DDR3 SDRAM";
		break;
	case ID_DDR4:
		type = "DDR4 SDRAM";
		break;
	case ID_DDR5:
		type = "DDR5 SDRAM";
		break;
	default:
		type = "unknown";
		break;
	}
	snprintf(s, n, "%s.", type);
}
void printSdramRowColumnBits(char* s, int n, byte myData, byte* allData)
{
	int rank0 = myData & 0x0F;
	int rank1 = (myData >> 4) & 0x0F;
	if (rank1 == 0)
	{
		snprintf(s, n, "%d.", rank0);
	}
	else if (rank1 == rank0)
	{
		snprintf(s, n, "rank 0: %d, rank 1: %d.", rank0, rank1);
	}
	else
	{
		snprintf(s, n, "rank 0: %d, rank 1: %d (asymmetric).", rank0, rank1);
	}
}
void printSdramRanks(char* s, int n, byte myData, byte* allData)
{
	if ((myData == 0) || (myData > 16))
	{
		snprintf(s, n, "invalid (%d).", myData);
	}
	else
	{
		snprintf(s, n, "%d.", myData);
	}
}
void printSdramModuleWidth(char* s, int n, byte myData, byte* allData)
{
	int low = myData & 0xFF;
	int high = (*(allData + 7) & 0xFF) << 8;
	int width = low + high;
	if (width == 0)
	{
		snprintf(s, n, "invalid.");
	}
	else
	{
		snprintf(s, n, "%d-bit.", width);
	}
}
void printSdramVoltage(char* s, int n, byte myData, byte* allData)
{
	const char* voltage;
	switch (myData)
	{
	case 0:
		voltage = "5.0V/TTL";
		break;
	case 1:
		voltage = "LVTTL";
		break;
	case 2:
		voltage = "HSTL 1.5V";
		break;
	case 3:
		voltage = "SSTL 3.3V";
		break;
	case 4:
		voltage = "SSTL 2.5V";
		break;
	default:
		voltage = "unknown";
		break;
	}
	snprintf(s, n, "%s.", voltage);
}
void printSdramCycleTime(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not available.");
	}
	else
	{
		const char* speed;
		int high = (myData >> 4) & 0x0F;
		int low = myData & 0x0F;
		switch (myData)
		{
		case 0xF0:
			speed = "PC66";
			break;
		case 0xA0:
			speed = "PC100";
			break;
		case 0x75:
			speed = "PC133";
			break;
		default:
			speed = "non-standard speed grade";
			break;
		}
		snprintf(s, n, "%d.%d ns (%s).", high, low, speed);
	}
}
void printSdramAccessFromClk(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not available.");
	}
	else
	{
		int high = (myData >> 4) & 0x0F;
		int low = myData & 0x0F;
		snprintf(s, n, "%d.%d ns.", high, low);
	}
}
void printSdramConfig(char* s, int n, byte myData, byte* allData)
{
	const char* type;
	switch (myData)
	{
	case 0:
		type = "none";
		break;
	case 1:
		type = "parity";
		break;
	case 2:
		type = "ECC";
		break;
	default:
		type = "unknown";
		break;
	}
	snprintf(s, n, "%s.", type);
}
void printSdramRefresh(char* s, int n, byte myData, byte* allData)
{
	const char* type;
	int selfRef = (myData & 0x80) >> 7;
	int refPeriod = myData & 0x7F;
	switch (refPeriod)
	{
	case 0:
		type = "normal 15.625 us";
		break;
	case 1:
		type = "reduced (.25x) 3.9 us";
		break;
	case 2:
		type = "reduced (.5x) 7.8 us";
		break;
	case 3:
		type = "extended (2x) 31.3 us";
		break;
	case 4:
		type = "extended (4x) 62.5 us";
		break;
	case 5:
		type = "extended (8x) 125 us";
		break;
	default:
		type = "unknown";
		break;
	}
	snprintf(s, n, "self-refresh: %d, refresh type: %s.", selfRef, type);
}
void printSdramChipsWidth(char* s, int n, byte myData, byte* allData)
{
	int width = myData & 0x7F;
	int asymmetric = (myData & 0x80) >> 7;
	if (width == 0)
	{
		snprintf(s, n, "invalid.");
	}
	else if (asymmetric)
	{
		snprintf(s, n, "rank 0: %d-bit, rank 1: %d-bit.", width, width * 2);
	}
	else
	{
		snprintf(s, n, "%d-bit.", width);
	}
}
void printSdramCheckWidth(char* s, int n, byte myData, byte* allData)
{
	int width = myData & 0x7F;
	int asymmetric = (myData & 0x80) >> 7;
	if (width == 0)
	{
		snprintf(s, n, "none.");
	}
	else if (asymmetric)
	{
		snprintf(s, n, "rank 0: %d-bit, rank 1: %d-bit.", width, width * 2);
	}
	else
	{
		snprintf(s, n, "%d-bit.", width);
	}
}
void printSdramMinClkB2B(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "%d clks.", myData);
}
void printSdramBurstLengts(char* s, int n, byte myData, byte* allData)
{
	int blpage = (myData & 0x80) >> 7;
	int bl8 = (myData & 0x08) >> 3;
	int bl4 = (myData & 0x04) >> 2;
	int bl2 = (myData & 0x02) >> 1;
	int bl1 = myData & 0x01;
	snprintf(s, n, "Burst lengths: Page: %d, BL8: %d, BL4: %d, BL2: %d, BL1: %d.",
		blpage, bl8, bl4, bl2, bl1);
}
void printSdramCasLatencies(char* s, int n, byte myData, byte* allData)
{
	int mask = 1;
	int m = n;
	int k;
	char* p = s;
	for (int i = 1; i < 9; i++)
	{
		if (myData & mask)
		{
			if (p != s)
			{
				k = snprintf(p, m, ", ");
				m += k;
				p += k;
			}
			else
			{
				k = snprintf(p, m, "means ");
				m += k;
				p += k;
			}
			k = snprintf(p, m, "%d", i);
			m += k;
			p += k;
		}
		mask <<= 1;
	}
	snprintf(p, m, " clks.");
}
void printSdramCsLatencies(char* s, int n, byte myData, byte* allData)
{
	int mask = 1;
	int m = n;
	int k;
	char* p = s;
	for (int i = 0; i < 7; i++)
	{
		if (myData & mask)
		{
			if (p != s)
			{
				k = snprintf(p, m, ", ");
				m += k;
				p += k;
			}
			k = snprintf(p, m, "%d", i);
			m += k;
			p += k;
		}
		mask <<= 1;
	}
	if (myData & 0x80)
	{
		if (p != s)
		{
			k = snprintf(p, m, ", ");
			m += k;
			p += k;
		}
		k = snprintf(p, m, "unknown value supported");
		m += k;
		p += k;
	}
	snprintf(p, m, " clks.");
}
void printSdramModuleAttr(char* s, int n, byte myData, byte* allData)
{
	int b6 = (myData & 0x40) >> 6;
	int b5 = (myData & 0x20) >> 5;
	int b4 = (myData & 0x10) >> 4;
	int b3 = (myData & 0x08) >> 3;
	int b2 = (myData & 0x04) >> 2;
	int b1 = (myData & 0x02) >> 1;
	int b0 = myData & 0x01;
	snprintf(s, n, "Rdn:%d, DifClk:%d, RDQMB:%d, BDQMB:%d, PLL:%d, RAC:%d, BAC:%d,",
		b6, b5, b4, b3, b2, b1, b0);
}
void printSdramDeviceAttr(char* s, int n, byte myData, byte* allData)
{
	int b5 = 10 - ((myData & 0x20) >> 5) * 5;
	int b4 = 10 - ((myData & 0x10) >> 4) * 5;
	int b3 = (myData & 0x08) >> 3;
	int b2 = (myData & 0x04) >> 2;
	int b1 = (myData & 0x02) >> 1;
	int b0 = myData & 0x01;
	snprintf(s, n, "UPVCC:%d%%, LOVCC:%d%%, WT1RD:%d, PCHGALL:%d, AUTOPCHG:%d, ERASPCHG:%d,",
		b5, b4, b3, b2, b1, b0);
}
void printSdramNs(char* s, int n, byte myData, byte* allData)
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
void printSdramBankDen(char* s, int n, byte myData, byte* allData)
{
	int rank1;
	int rank2;
	helperRanksDensitySdram(myData, rank1, rank2);
	if (rank1 == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (rank2 == 0)
	{
		snprintf(s, n, "%d MB.", rank1);
	}
	else
	{
		snprintf(s, n, "rank0: %d MB, rank1: %d MB.", rank1, rank2);
	}
}
void printSdramTimeSigned(char* s, int n, byte myData, byte* allData)
{
	int high = (myData >> 4) & 0x07;
	int low = myData & 0x0F;
	const char* sign = "+";
	if (myData & 0x80)
	{
		sign = "-";
	}
	snprintf(s, n, "%s%d.%d ns.", sign, high, low);
}
void printSdramRevision(char* s, int n, byte myData, byte* allData)
{
	int high = (myData >> 4) & 0x0F;
	int low = myData & 0x0F;
	if (myData == 0)
	{
		snprintf(s, n, "initial release.");
	}
	else if (high == 0)
	{
		snprintf(s, n, "rev. %d.", low);
	}
	else if (high < 2)
	{
		snprintf(s, n, "rev. %d.%d.", high, low);
	}
	else
	{
		snprintf(s, n, "unknown.");
	}
}
void printSdramModuleLoc(char* s, int n, byte myData, byte* allData)
{
	const char* location;
	switch (myData)
	{
	case 1:
		location = "SCI-SGP";
		break;
	case 2:
		location = "EEMS";
		break;
	case 3:
		location = "TBD";
		break;
	default:
		location = "not defined";
		break;
	}
	snprintf(s, n, "%s", location);
}
void printSdramChipRev(char* s, int n, byte myData, byte* allData)
{
	int byte1 = myData & 0xFF;
	int byte2 = *(allData + 92) & 0xFF;
	if ((byte1 < 4) && (byte2 < 4))
	{
		const char* chipRev;
		switch (byte1)
		{
		case 1:
			chipRev = "A";
			break;
		case 2:
			chipRev = "B";
			break;
		case 3:
			chipRev = "C";
			break;
		default:
			chipRev = "unknown";
			break;
		}
		snprintf(s, n, "chip revision code: %s", chipRev);
	}
	else
	{
		int rev = byte1 + (byte2 << 8);
		char revStr[3 + 1];
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
}
void printSdramBoardRev(char* s, int n, byte myData, byte* allData)
{
	int byte1 = *(allData + 91) & 0xFF;
	int byte2 = myData & 0xFF;
	if ((byte1 < 4) && (byte2 < 4))
	{
		const char* boardRev;
		switch (byte2)
		{
		case 1:
			boardRev = "A";
			break;
		case 2:
			boardRev = "B";
			break;
		case 3:
			boardRev = "C";
			break;
		default:
			boardRev = "unknown";
			break;
		}
		snprintf(s, n, "board revision code: %s", boardRev);
	}
	else
	{
		printRdramSeeAbove(s, n, myData, allData);
	}
}
void printSdramManufSpec(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "manufacturer-specific.");
}
void printSdramIntelPc(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "reserved for Intel PC66/PC100.");
	}
	else if ((myData != 66) && (myData != 100))
	{
		snprintf(s, n, "invalid value for Intel PC66/PC100.");
	}
	else
	{
		snprintf(s, n, "Intel PC%d.", myData);
	}
}
void printSdramIntelPcAttr(char* s, int n, byte myData, byte* allData)
{
	if ((*allData + 126) == 0)
	{
		snprintf(s, n, "reserved.");
	}
	else
	{
		int b7 = (myData & 0x80) >> 7;
		int b6 = (myData & 0x40) >> 6;
		int b5 = (myData & 0x20) >> 5;
		int b4 = (myData & 0x10) >> 4;
		int b3 = 90 + ((myData & 0x08) >> 3) * 10;
		int b2 = (myData & 0x04) >> 2;
		int b1 = (myData & 0x02) >> 1;
		int b0 = myData & 0x01;
		snprintf(s, n, "CLK0:%d, CLK1:%d, CLK2:%d, CLK3:%d, Tj:%dC, CL3:%d, CL2:%d, CAP:%d",
			b7, b6, b5, b4, b3, b2, b1, b0);
	}
}

void helperRanksDensityDdr(int data, int& rank1, int& rank2)
{
	rank1 = 0;
	rank2 = 0;
	int mask = 0x80;;
	int sizeEncode[] = { 512, 256, 128, 64, 32, 4096, 2048, 1024 };
	for (int i = 0; i < 8; i++)
	{
		if (data & mask)
		{
			if (rank1)
			{
				rank2 = sizeEncode[i];
				break;
			}
			else
			{
				rank1 = sizeEncode[i];
			}
		}
		mask >>= 1;
	}
}

void printDdrVoltage(char* s, int n, byte myData, byte* allData)
{
	const char* voltage;
	switch (myData)
	{
	case 0:
		voltage = "TTL (5V tolerant)";
		break;
	case 1:
		voltage = "LVTTL (not 5V tolerant)";
		break;
	case 2:
		voltage = "HSTL 1.5V";
		break;
	case 3:
		voltage = "SSTL 3.3V";
		break;
	case 4:
		voltage = "SSTL 2.5V";
		break;
	case 5:
		voltage = "SSTL 1.8V";
		break;
	default:
		voltage = "unknown";
		break;
	}
	snprintf(s, n, "%s.", voltage);
}
void printDdrCycleTime(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not available.");
	}
	else
	{
		const char* speed;
		int mhz;
		int high = (myData >> 4) & 0x0F;
		int low = myData & 0x0F;
		switch (myData)
		{
		case 0xA0:
			speed = "PC1600";
			mhz = 200;
			break;
		case 0x75:
			speed = "PC2100";
			mhz = 266;
			break;
		case 0x60:
			speed = "PC2700";
			mhz = 333;
			break;
		case 0x50:
			speed = "PC3200";
			mhz = 400;
			break;
		default:
			speed = "non-standard speed grade";
			mhz = 0;
			break;
		}
		if (mhz == 0)
		{
			snprintf(s, n, "%d.%d ns (%s).", high, low, speed);
		}
		else
		{
			snprintf(s, n, "%d.%d ns (DDR@%dMHz, %s).", high, low, mhz, speed);
		}
	}
}
void printDdrAccessFromClk(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not available.");
	}
	else
	{
		int high = (myData >> 4) & 0x0F;
		int low = myData & 0x0F;
		snprintf(s, n, "0.%d%d ns.", high, low);
	}
}
void printDdrCasLatencies(char* s, int n, byte myData, byte* allData)
{
	int mask = 1;
	int m = n;
	int k;
	char* p = s;
	for (int i = 0; i < 7; i++)
	{
		if (myData & mask)
		{
			if (p != s)
			{
				k = snprintf(p, m, ", ");
				m += k;
				p += k;
			}
			else
			{
				k = snprintf(p, m, "means ");
				m += k;
				p += k;
			}
			double latency = 1.0 + i * 0.5;
			k = snprintf(p, m, "%.1f", latency);
			m += k;
			p += k;
		}
		mask <<= 1;
	}
	snprintf(p, m, " clks.");
}
void printDdrModuleAttr(char* s, int n, byte myData, byte* allData)
{
	int b5 = (myData & 0x20) >> 5;
	int b4 = (myData & 0x10) >> 4;
	int b3 = (myData & 0x08) >> 3;
	int b2 = (myData & 0x04) >> 2;
	int b1 = (myData & 0x02) >> 1;
	int b0 = myData & 0x01;
	snprintf(s, n, "DifClk:%d, FEText:%d, FETint:%d, PLL:%d, RAC:%d, BAC:%d,",
		b5, b4, b3, b2, b1, b0);
}
void printDdrDeviceAttr(char* s, int n, byte myData, byte* allData)
{
	int b7 = (myData & 0x80) >> 7;
	int b6 = (myData & 0x40) >> 6;
	int b5 = (myData & 0x20) >> 5;
	int b4 = (myData & 0x10) >> 4;
	int b0 = myData & 0x01;
	const char* s5 = "TBD";
	if (!b5)
	{
		s5 = "0.2V";
	}
	const char* s4 = "TBD";
	if (!b4)
	{
		s4 = "0.2V";
	}
	snprintf(s, n, "FastAP: %d, CoAP: %d, UVCC: %s, LVCC: %s, WeakDrv: %d.",
		b7, b6, s5, s4, b0);
}
void printDdr025ns(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		double ns = myData * 0.25;
		snprintf(s, n, "%.2f ns.", ns);
	}
}
void printDdrBankDen(char* s, int n, byte myData, byte* allData)
{
	int rank1;
	int rank2;
	helperRanksDensityDdr(myData, rank1, rank2);
	if (rank1 == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (rank2 == 0)
	{
		snprintf(s, n, "%d MB.", rank1);
	}
	else
	{
		snprintf(s, n, "rank0: %d MB, rank1: %d MB.", rank1, rank2);
	}
}
void printDdr01ns(char* s, int n, byte myData, byte* allData)
{
	int high = (myData & 0xF0) >> 4;
	int low = myData & 0x0F;
	double ns = high * 0.1 + low * 0.01;
	snprintf(s, n, "%.2f ns.", ns);
}
void printDdr001ns(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		double ns = myData * 0.01;
		snprintf(s, n, "%.2f ns.", ns);
	}
}
void printDdrDimmH(char* s, int n, byte myData, byte* allData)
{
	int height = myData & 0x03;
	const char* sHeight;
	switch (height)
	{
	case 0:
		sHeight = "no DIMM height available";
		break;
	case 1:
		sHeight = "1.125 to 1.25 inch";
		break;
	case 2:
		sHeight = "1.7 inch";
		break;
	default:
		sHeight = "other value";
		break;
	}
	snprintf(s, n, "%s.", sHeight);
}

int helperDimmTypeDdr2(int x)
{
	const int usedValues[] = { 0, 1, 2, 4, 6, 7, 8, 0x10, 0x20 };
	int N = sizeof(usedValues) / sizeof(int);
	for (int i = 0; i < N; i++)
	{
		if (usedValues[i] == x)
		{
			return i;
		}
	}
	return -1;
}
void helperRanksDensityDdr2(int data, int& rank1, int& rank2)
{
	rank1 = 0;
	rank2 = 0;
	int mask = 0x80;;
	int sizeEncode[] = { 512, 256, 128, 16384, 8192, 4096, 2048, 1024 };
	for (int i = 0; i < 8; i++)
	{
		if (data & mask)
		{
			if (rank1)
			{
				rank2 = sizeEncode[i];
				break;
			}
			else
			{
				rank1 = sizeEncode[i];
			}
		}
		mask >>= 1;
	}
}

void printDdr2rowBits(char* s, int n, byte myData, byte* allData)
{
	int rows = myData & 0x1F;
	if ((rows == 0) || (rows > 31))
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%d.", rows);
	}
}
void printDdr2columnBits(char* s, int n, byte myData, byte* allData)
{
	int columns = myData & 0x0F;
	if (columns == 0)
	{
		snprintf(s, n, "undefined.");
	}
	{
		snprintf(s, n, "%d.", columns);
	}
}
void printDdr2ranksPcHg(char* s, int n, byte myData, byte* allData)
{
	int h = (myData & 0xE0) >> 5;
	int p = (myData & 0x10) >> 4;
	int c = (myData & 0x08) >> 3;
	int r = (myData & 0x07) + 1;
	const char* height;
	switch (h)
	{
	case 0:
		height = ">25.4 mm";
		break;
	case 1:
		height = "25.4 mm";
		break;
	case 2:
		height = "30<H<25.4 mm";
		break;
	case 3:
		height = "30.0 mm";
		break;
	case 4:
		height = "30.5 mm";
		break;
	case 5:
		height = ">30.5 mm";
		break;
	default:
		height = "unknown";
		break;
	}
	const char* package;
	switch (p)
	{
	case 0:
		package = "planar";
		break;
	default:
		package = "stack";
		break;
	}
	snprintf(s, n, "height: %s, package: %s, card on card: %d, ranks: %d.",
		height, package, c, r);
}
void printDdr2moduleWidth(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%d-bit.", myData);
	}
}
void printDdr2cycleTime(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "not available.");
	}
	else
	{
		const char* speed;
		int mhz;
		int high = (myData >> 4) & 0x0F;
		int low = myData & 0x0F;
		double timings[] =
		{ 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,
		  0.25, 0.33, 0.66, 0.75, 0.0, 0.0 };
		double ns = high + timings[low];
		switch (myData)
		{
		case 0x50:
			speed = "PC3200";
			mhz = 400;
			break;
		case 0x3D:
			speed = "PC4200";
			mhz = 533;
			break;
		case 0x30:
			speed = "PC5300";
			mhz = 667;
			break;
		case 0x25:
			speed = "PC6400";
			mhz = 800;
			break;
		default:
			speed = "non-standard speed grade";
			mhz = 0;
			break;
		}
		if (mhz == 0)
		{
			snprintf(s, n, "%.2f. ns (%s).", ns, speed);
		}
		else
		{
			snprintf(s, n, "%.2f ns (DDR2@%dMHz, %s).", ns, mhz, speed);
		}
	}
}
void printDdr2config(char* s, int n, byte myData, byte* allData)
{
	int acp = (myData & 0x4) >> 2;
	int decc = (myData & 0x2) >> 1;
	int dp = myData & 0x1;
	snprintf(s, n, "addr/cmd parity: %d, data ECC: %d, data parity: %d.",
		acp, decc, dp);
}
void printDdr2eccWidth(char* s, int n, byte myData, byte* allData)
{
	if (myData == 0)
	{
		snprintf(s, n, "none.");
	}
	else
	{
		snprintf(s, n, "%d-bit.", myData);
	}
}
void printDdr2burstLengts(char* s, int n, byte myData, byte* allData)
{
	int bl8 = (myData & 0x08) >> 3;
	int bl4 = (myData & 0x04) >> 2;
	snprintf(s, n, "Burst lengths: BL8: %d, BL4: %d.", bl8, bl4);
}
void printDdr2casLatencies(char* s, int n, byte myData, byte* allData)
{
	int mask = 4;
	int m = n;
	int k;
	char* p = s;
	for (int i = 2; i < 8; i++)
	{
		if (myData & mask)
		{
			if (p != s)
			{
				k = snprintf(p, m, ", ");
				m += k;
				p += k;
			}
			else
			{
				k = snprintf(p, m, "means ");
				m += k;
				p += k;
			}
			k = snprintf(p, m, "%d", i);
			m += k;
			p += k;
		}
		mask <<= 1;
	}
	snprintf(p, m, " clks.");
}
void printDdr2dimmMech(char* s, int n, byte myData, byte* allData)
{
	int thickness = myData & 0x07;
	if (thickness == 0)
	{
		snprintf(s, n, "not specified.");
	}
	else if (thickness > 4)
	{
		snprintf(s, n, "reserved.");
	}
	else
	{
		int index = helperDimmTypeDdr2(*(allData + 20));
		if (index <= 0)
		{
			snprintf(s, n, "unknown module type.");
		}
		else
		{
			const char* mechTypes[4][8] =
			{ { "x<4.10", "x<4.10", "x<3.80", "x<3.80",
				"x<3.85", "x<3.85", "x<3.80", "x<3.80" },
			  { "4.10<x<6.75", "4.10<x<6.75", "3.80<x<TBD", "3.80<x<TBD",
				"3.85<x<6.45", "3.85<x<6.45", "3.80<x<7.10", "3.80<x<7.10" },
			  { "6.75<x<7.55", "6.75<x<7.55", "TBD<x<TBD", "TBD<x<TBD",
				"6.45<x<7.25", "6.45<x<7.25", "7.10<x", "7.10<x" },
			  { "7.55<x", "7.55<x", "TBD<x", "TBD<x",
				"7.25<x", "7.25<x", "reserved", "reserved" }
			};
			const char* mechType = mechTypes[thickness - 1][index - 1];
			snprintf(s, n, "thickness: %s mm", mechType);
		}
	}
}
void printDdr2dimmType(char* s, int n, byte myData, byte* allData)
{
	const char* types[] =
	{ "undefined", "RDIMM", "UDIMM", "SO-DIMM",
	  "72b-SO-CDIMM", "72b-SO-RDIMM", "Micro-DIMM", "Mini-RDIMM", "Mini-UDIMM" };
	const double widths[] =
	{ 0.0, 133.35, 133.35, 67.60, 67.60, 67.60, 54.00, 82.00, 82.00 };
	int index = helperDimmTypeDdr2(myData);
	if (index < 0)
	{
		snprintf(s, n, "reserved.");
	}
	else
	{
		const char* type = types[index];
		double width = widths[index];
		snprintf(s, n, "%s, width: %.2f mm.", type, width);
	}
}
void printDdr2moduleAttr(char* s, int n, byte myData, byte* allData)
{
	int probe = (myData & 0x40) >> 6;
	int fetsw = (myData & 0x10) >> 4;
	int plls = (myData & 0x0C) >> 2;
	const char* pllStrings[] = { "none", "1", "2", "reserved" };
	const char* pllString = pllStrings[plls];
	int regs;
	int registered = *(allData + 20) & 0x11;
	if (registered)
	{
		regs = myData & 0x03 + 1;
	}
	else
	{
		regs = 0;
	}
	const char* regStrings[] = { "none", "1", "2", "3", "4" };
	const char* regString = regStrings[regs];
	snprintf(s, n, "probe: %d, FETsw: %d, PLLs: %s, Regs: %s.",
		probe, fetsw, pllString, regString);
}
void printDdr2deviceAttr(char* s, int n, byte myData, byte* allData)
{
	int pasr = (myData & 0x04) >> 2;
	int odt = (myData & 0x02) >> 1;
	int weak = myData & 0x01;
	snprintf(s, n, "PASR: %d, 50 ohm ODT: %d, weak driver: %d", pasr, odt, weak);
}
void printDdr2bankDen(char* s, int n, byte myData, byte* allData)
{
	int rank1;
	int rank2;
	helperRanksDensityDdr2(myData, rank1, rank2);
	if (rank1 == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (rank2 == 0)
	{
		snprintf(s, n, "%d MB.", rank1);
	}
	else
	{
		snprintf(s, n, "rank0: %d MB, rank1: %d MB.", rank1, rank2);
	}
}
void printDdr2extTrcTrfc(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "extension for tRC, tRFC bit fields, see below.");
}
void printDdr2autoRef(char* s, int n, byte myData, byte* allData)
{
	int ns = myData & 0xFF;
	int index = ((*(allData + 40) & 0x70) >> 4);
	double nsa[] = { 0.0, 0.25, 0.33, 0.5, 0.66, 0.75, 0.0, 0.0 };
	double trc = ns + nsa[index];
	snprintf(s, n, "%.2f", trc);
}
void printDdr2autoRefTo(char* s, int n, byte myData, byte* allData)
{
	int ns = myData & 0xFF;
	int ext = *(allData + 40);
	int nsa256 = (ext & 0x01) * 256;
	int index = (ext & 0x0E) >> 1;
	double nsa[] = { 0.0, 0.25, 0.33, 0.5, 0.66, 0.75, 0.0, 0.0 };
	double trfc = ns + nsa[index] + nsa256;
	snprintf(s, n, "%.2f", trfc);
}
void printDdr2tcaseMax(char* s, int n, byte myData, byte* allData)
{
	int t = ((myData & 0xF0) >> 4) * 2 + 85;
	double d = (myData & 0x0F) * 0.4;
	snprintf(s, n, "Tcasemax: %d C, DT4R4W delta: %.1f C.", t, d);
}
void printDdr2thermRes(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	if (a == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (a == 255)
	{
		snprintf(s, n, "exceed 127.5 C/W.");
	}
	else
	{
		double tr = a * 0.5;
		snprintf(s, n, "%.1f C/W.", tr);
	}
}
void printDdr2triseAp(char* s, int n, byte myData, byte* allData)
{
	int t = ((myData & 0xFC) >> 2);
	int htdr = ((myData & 0x02) >> 1);
	int htsr = myData & 0x01;
	double dt0 = t * 0.3 + 2.8;
	snprintf(s, n, "DT0: %.1f C, high temp: double refresh: %d, self refresh: %d",
		dt0, htdr, htsr);
}
void printDdr2dramTriseP(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	if (a == 0)
	{
		snprintf(s, n, "undefined.");
	}
	if (a == 255)
	{
		snprintf(s, n, "exceed 25.5 C.");
	}
	else
	{
		double t = a * 0.1;
		snprintf(s, n, "%.3f C.", t);
	}
}
void printDdr2dramTrisePp(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	if (a == 0)
	{
		snprintf(s, n, "undefined.");
	}
	if (a == 255)
	{
		snprintf(s, n, "exceed 3.825 C.");
	}
	else
	{
		double t = a * 0.015;
		snprintf(s, n, "%.3f C.", t);
	}
}
void printDdr2dramTriseAs(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	if (a == 0)
	{
		snprintf(s, n, "undefined.");
	}
	if (a == 255)
	{
		snprintf(s, n, "exceed 38.25 C.");
	}
	else
	{
		double t = a * 0.15;
		snprintf(s, n, "%.3f C.", t);
	}
}
void printDdr2dramTriseApf(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	if (a == 0)
	{
		snprintf(s, n, "undefined.");
	}
	if (a == 255)
	{
		snprintf(s, n, "exceed 12.75 C.");
	}
	else
	{
		double t = a * 0.05;
		snprintf(s, n, "%.2f C.", t);
	}
}
void printDdr2dramTriseAps(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	if (a == 0)
	{
		snprintf(s, n, "undefined.");
	}
	if (a == 255)
	{
		snprintf(s, n, "exceed 6.375 C.");
	}
	else
	{
		double t = a * 0.025;
		snprintf(s, n, "%.3f C.", t);
	}
}
void printDdr2dramTriseBr(char* s, int n, byte myData, byte* allData)
{
	int a = (myData & 0xFE) >> 1;
	int b = myData & 0x01;
	const char* dt4w;
	if (b)
	{
		dt4w = "DT4W is less than DT4R";
	}
	else
	{
		dt4w = "DT4W is greater than or equal DT4R";
	}
	if (a == 0)
	{
		snprintf(s, n, "undefined, %s.", dt4w);
	}
	if (a == 127)
	{
		snprintf(s, n, "exceed 50.8 C, %s.", dt4w);
	}
	else
	{
		double t = a * 0.4;
		snprintf(s, n, "%.1f C, %s.", t, dt4w);
	}
}
void printDdr2dramTriseBrf(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	if (a == 255)
	{
		snprintf(s, n, "exceed 127.5 C.");
	}
	else
	{
		double tr = a * 0.5;
		snprintf(s, n, "%.1f C.", tr);
	}
}
void printDdr2pllTriseA(char* s, int n, byte myData, byte* allData)
{
	int a = myData & 0xFF;
	if (a == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else if (a == 255)
	{
		snprintf(s, n, "exceed 63.75 C.");
	}
	else
	{
		double t = a * 0.25;
		snprintf(s, n, "%.3f C.", t);
	}
}
void printDdr2regTriseA(char* s, int n, byte myData, byte* allData)
{
	int b0 = myData & 0x01;
	int b72 = (myData & 0xFC) >> 2;
	if (b72 == 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		if (b0)
		{
			if (b72 == 0x3F)
			{
				snprintf(s, n, "exceed 47.25 C, 100%% register data output toggle.");
			}
			else
			{
				double t = b72 * 1.25;
				snprintf(s, n, "%.2f C, 100%% register data output toggle.", t);
			}
		}
		else
		{
			if (b72 == 0x3F)
			{
				snprintf(s, n, "exceed 78.75 C, 50%% register data output toggle.");
			}
			else
			{
				double t = b72 * 0.75;
				snprintf(s, n, "%.2f C, 50%% register data output toggle.", t);
			}
		}
	}
}

int helperChipSizeDdr3(byte data)
{
	int size;
	int index = data & 0x0F;
	if (index > 6)
	{
		size = -1;
	}
	else
	{
		size = 256 << index;
	}
	return size;
}
int helperPrintValidatedDdr3(char* p, int m, int value)
{
	int k;
	if (value < 0)
	{
		k = snprintf(p, m, "reserved");
	}
	else
	{
		k = snprintf(p, m, "%d", value);
	}
	return k;
}
double helperFtbDdr3(byte data)
{
	int dividend = (data & 0xF0) >> 4;
	int divisor = data & 0x0F;
	double t;
	if ((dividend == 0) || (divisor == 0))
	{
		t = -1;
	}
	else
	{
		t = (double)dividend / divisor;
	}
	return t;
}
double helperMtbDdr3(byte data1, byte data2)
{
	int dividend = data1 & 0xFF;
	int divisor = data2 & 0xFF;
	double t;
	if ((dividend == 0) || (divisor == 0))
	{
		t = -1;
	}
	else
	{
		t = (double)dividend / divisor;
	}
	return t;
}

void printDdr3usedBytes(char* s, int n, byte myData, byte* allData)
{
	int b7 = (myData & 0x80) >> 7;
	int b64 = (myData & 0x70) >> 4;
	int b30 = myData & 0x0F;
	int crcCover;
	if (b7)
	{
		crcCover = 116;
	}
	else
	{
		crcCover = 125;
	}
	const char* total;
	switch (b64)
	{
	case 0:
		total = "undefined";
		break;
	case 1:
		total = "256";
		break;
	default:
		total = "reserved";
		break;
	}
	const char* used;
	switch (b30)
	{
	case 0:
		used = "undefined";
		break;
	case 1:
		used = "128";
		break;
	case 2:
		used = "176";
		break;
	case 3:
		used = "256";
		break;
	default:
		used = "reserved";
		break;
	}
	snprintf(s, n, "total bytes: %s, used: %s, CRC covered 0-%d.", total, used, crcCover);
}
void printDdr3moduleType(char* s, int n, byte myData, byte* allData)
{
	int index = myData & 0x0F;
	const char* type;
	switch (index)
	{
	case 0:
		type = "undefined";
		break;
	case 1:
		type = "RDIMM (width=133.35 mm)";
		break;
	case 2:
		type = "UDIMM (width=133.35 mm)";
		break;
	case 3:
		type = "SO-DIMM (width=67.6 mm)";
		break;
	case 4:
		type = "Micro-DIMM (width=TBD mm)";
		break;
	case 5:
		type = "Mini-RDIMM (width=82.0 mm)";
		break;
	case 6:
		type = "Mini-UDIMM (width=82.0 mm)";
		break;
	case 7:
		type = "Mini-CDIMM (width=67.6 mm)";
		break;
	case 8:
		type = "72b SO-UDIMM (width=67.6 mm)";
		break;
	case 9:
		type = "72b SO-RDIMM (width=67.6 mm)";
		break;
	case 10:
		type = "72b SO-CDIMM (width=67.6 mm)";
		break;
	case 11:
		type = "LRDIMM (width=133.35 mm)";
		break;
	case 12:
		type = "16b SO-DIMM (width=67.6 mm)";
		break;
	case 13:
		type = "32b SO-DIMM (width=67.6 mm)";
		break;
	default:
		type = "reserved";
		break;
	}
	snprintf(s, n, "%s.", type);
}
void printDdr3densBank(char* s, int n, byte myData, byte* allData)
{
	int chipIndex = myData & 0x0F;
	const char* chipSizeName;
	switch (chipIndex)
	{
	case 0:
		chipSizeName = "256 Mbit";
		break;
	case 1:
		chipSizeName = "512 Mbit";
		break;
	case 2:
		chipSizeName = "1 Gbit";
		break;
	case 3:
		chipSizeName = "2 Gbit";
		break;
	case 4:
		chipSizeName = "4 Gbit";
		break;
	case 5:
		chipSizeName = "8 Gbit";
		break;
	case 6:
		chipSizeName = "16 Gbit";
		break;
	default:
		chipSizeName = "reserved";
		break;
	}
	char* p = s;
	int m = n;
	int k;
	k = snprintf(p, m, "chip size: %s, logical banks: ", chipSizeName);
	p += k;
	m -= k;
	int bankIndex = (myData & 0x70) >> 4;
	int banks;
	if (bankIndex > 3)
	{
		banks = -1;
	}
	else
	{
		banks = 8 << bankIndex;
	}
	k = helperPrintValidatedDdr3(p, m, banks);
	p += k;
	m -= k;
	snprintf(p, m, ".");
}
void printDdr3addressing(char* s, int n, byte myData, byte* allData)
{
	int rowIndex = (myData & 0x38) >> 3;
	int rows;
	if (rowIndex > 4)
	{
		rows = -1;
	}
	else
	{
		rows = 12 + rowIndex;
	}
	int colIndex = myData & 0x07;
	int columns;
	if (colIndex > 3)
	{
		columns = -1;
	}
	else
	{
		columns = 9 + colIndex;
	}
	char* p = s;
	int m = n;
	int k;
	k = snprintf(p, m, "rows: ");
	p += k;
	m -= k;
	k = helperPrintValidatedDdr3(p, m, rows);
	p += k;
	m -= k;
	k = snprintf(p, m, ", columns: ");
	p += k;
	m -= k;
	k = helperPrintValidatedDdr3(p, m, columns);
	p += k;
	m -= k;
	snprintf(p, m, ".");
}
void printDdr3nomVoltage(char* s, int n, byte myData, byte* allData)
{
	char* p = s;
	int m = n;
	int k;
	bool f = false;
	k = snprintf(p, m, "supported voltages: ");
	p += k;
	m -= k;
	if ((myData & 0x06) || (!(myData & 0x01)))
	{
		if (myData & 0x04)
		{
			k = snprintf(p, m, "1.25V");
			p += k;
			m -= k;
			f = true;
		}
		if (myData & 0x02)
		{
			if (f)
			{
				k = snprintf(p, m, ", ");
				p += k;
				m -= k;
			}
			k = snprintf(p, m, "1.35V");
			p += k;
			m -= k;
			f = true;
		}
		if (!(myData & 0x01))
		{
			if (f)
			{
				k = snprintf(p, m, ", ");
				p += k;
				m -= k;
			}
			k = snprintf(p, m, "1.5V");
			p += k;
			m -= k;
		}
		snprintf(p, m, ".");
	}
	else
	{
		snprintf(p, m, "none.");
	}
}
void printDdr3moduleOrg(char* s, int n, byte myData, byte* allData)
{
	int rankIndex = (myData & 0x38) >> 3;
	int ranks;
	if (rankIndex > 4)
	{
		ranks = -1;
	}
	else
	{
		ranks = 1 + rankIndex;
	}
	int widthIndex = myData & 0x07;
	int width;
	if (widthIndex > 3)
	{
		width = -1;
	}
	else
	{
		width = 4 << widthIndex;
	}
	char* p = s;
	int m = n;
	int k;
	k = snprintf(p, m, "physical ranks: ");
	p += k;
	m -= k;
	k = helperPrintValidatedDdr3(p, m, ranks);
	p += k;
	m -= k;
	k = snprintf(p, m, ", SDRAM device width, bits: ");
	p += k;
	m -= k;
	k = helperPrintValidatedDdr3(p, m, width);
	p += k;
	m -= k;
	snprintf(p, m, ".");
}
void printDdr3moduleWidth(char* s, int n, byte myData, byte* allData)
{
	int redundantIndex = (myData & 0x18) >> 3;
	int redundant;
	if (redundantIndex > 2)
	{
		redundant = -1;
	}
	else
	{
		redundant = 8 * redundantIndex;
	}
	int dataBusIndex = myData & 0x07;
	int dataBus;
	if (dataBusIndex > 3)
	{
		dataBus = -1;
	}
	else
	{
		dataBus = 8 << dataBusIndex;
	}
	char* p = s;
	int m = n;
	int k;
	k = snprintf(p, m, "data bits: ");
	p += k;
	m -= k;
	k = helperPrintValidatedDdr3(p, m, dataBus);
	p += k;
	m -= k;
	k = snprintf(p, m, ", redundant: ");
	p += k;
	m -= k;
	k = helperPrintValidatedDdr3(p, m, redundant);
	p += k;
	m -= k;
	if ((dataBus > 0) && (redundant >= 0))
	{
		k = snprintf(p, m, ", total: ");
		p += k;
		m -= k;
		k = helperPrintValidatedDdr3(p, m, dataBus + redundant);
		p += k;
		m -= k;
	}
	snprintf(p, m, ".");
}
void printDdr3fineTime(char* s, int n, byte myData, byte* allData)
{
	double t = helperFtbDdr3(myData);
	if (t < 0.0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%g picoseconds.", t);
	}
}
void printDdr3mediumTime(char* s, int n, byte myData, byte* allData)
{
	double t = helperMtbDdr3(myData, *(allData + 11));
	if (t < 0.0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "%g nanoseconds.", t);
	}
}
void printDdr3minCycle(char* s, int n, byte myData, byte* allData)
{
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	if ((mtbunits < 0.0) || (myData == 0))
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		int clocks = myData & 0xFF;
		double ns = clocks * mtbunits;
		double mhz = 1000.0 / ns;
		int fspec = (int)(mhz * 2.0);
		const char* pcspec;
		switch (fspec)
		{
		case 800:
			pcspec = "6400";
			break;
		case 1066:
			pcspec = "8500";
			break;
		case 1333:
			pcspec = "10600";
			break;
		case 1600:
			pcspec = "12800";
			break;
		case 1866:
			pcspec = "14900";
			break;
		case 2133:
			pcspec = "17000";
			break;
		default:
			pcspec = "?";
			break;
		}
		snprintf(s, n, "%g ns, %g MHz (DDR3-%d, PC3-%s).", ns, mhz, fspec, pcspec);
	}
}
void printDdr3casLatencies(char* s, int n, byte myData, byte* allData)
{
	int low = myData & 0xFF;
	int high = (*(allData + 15)) & 0xFF;
	int latencies = (high << 8) + low;
	int mask = 1;
	int m = n;
	int k;
	char* p = s;
	for (int i = 4; i < 19; i++)
	{
		if (latencies & mask)
		{
			if (p != s)
			{
				k = snprintf(p, m, ", ");
				m += k;
				p += k;
			}
			else
			{
				k = snprintf(p, m, "means ");
				m += k;
				p += k;
			}
			k = snprintf(p, m, "%d", i);
			m += k;
			p += k;
		}
		mask <<= 1;
	}
	snprintf(p, m, " clks.");
}
void printDdr3mtbNs(char* s, int n, byte myData, byte* allData)
{
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	if ((mtbunits < 0.0) || (myData == 0))
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		double ns = mtbunits * myData;
		snprintf(s, n, "%g ns.", ns);
	}
}
void printDdr3upTrasTrc(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "upper nibbles of tRAS, tRC bit fields, see below.");
}
void printDdr3tRas(char* s, int n, byte myData, byte* allData)
{
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	if ((mtbunits < 0.0) || (myData == 0))
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		int low = myData & 0xFF;
		int high = ((*(allData + 21)) & 0x0F) << 8;
		int clocks = low + high;
		double ns = mtbunits * clocks;
		snprintf(s, n, "%g ns.", ns);
	}
}
void printDdr3tRc(char* s, int n, byte myData, byte* allData)
{
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	if ((mtbunits < 0.0) || (myData == 0))
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		int low = myData & 0xFF;
		int high = ((*(allData + 21)) & 0xF0) << 4;
		int clocks = low + high;
		double ns = mtbunits * clocks;
		snprintf(s, n, "%g ns.", ns);
	}
}
void printDdr3lowTrfcMin(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "low byte of tRFCmin, see below.");
}
void printDdr3tRfcMin(char* s, int n, byte myData, byte* allData)
{
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	if ((mtbunits < 0.0) || (myData == 0))
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		int high = (myData & 0xFF) << 8;
		int low = (*(allData + 24)) & 0xFF;
		int clocks = low + high;
		double ns = mtbunits * clocks;
		snprintf(s, n, "%g ns.", ns);
	}
}
void printDdr3upTfaw(char* s, int n, byte myData, byte* allData)
{
	snprintf(s, n, "upper nibbles of tFAW, bit field, see below.");
}
void printDdr3tFaw(char* s, int n, byte myData, byte* allData)
{
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	if ((mtbunits < 0.0) || (myData == 0))
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		int low = myData & 0xFF;
		int high = ((*(allData + 28)) & 0x0F) << 8;
		int clocks = low + high;
		double ns = mtbunits * clocks;
		snprintf(s, n, "%g ns.", ns);
	}
}
void printDdr3optionFeat(char* s, int n, byte myData, byte* allData)
{
	int dllOff = (myData & 0x80) >> 7;
	int rzq7 = (myData & 0x02) >> 1;
	int rzq6 = myData & 0x01;
	snprintf(s, n, "DLL off mode: %d, RZQ/7: %d, RZQ/6: %d.", dllOff, rzq7, rzq6);
}
void printDdr3thermRefOpt(char* s, int n, byte myData, byte* allData)
{
	int pasr = (myData & 0x80) >> 7;
	int odts = (myData & 0x08) >> 3;
	int asr = (myData & 0x04) >> 2;
	int etrr = (myData & 0x02) >> 1;
	int etr = myData & 0x01;
	snprintf(s, n, "PASR: %d, ODTS: %d, ASR: %d, ETRR: %d, ETR: %d.",
		pasr, odts, asr, etrr, etr);
}
void printDdr3thermSensor(char* s, int n, byte myData, byte* allData)
{
	if (myData & 0x80)
	{
		snprintf(s, n, "present.");
	}
	else
	{
		snprintf(s, n, "absent.");
	}
}
void printDdr3deviceChips(char* s, int n, byte myData, byte* allData)
{
	int type = (myData & 0x80) >> 7;
	int die = (myData & 0x70) >> 4;
	int load = myData & 0x03;
	const char* sType;
	const char* sDie;
	const char* sLoad;
	switch (type)
	{
	case 0:
		sType = "monolitic";
		break;
	default:
		sType = "multi die";
		break;
	}
	switch (die)
	{
	case 0:
		sDie = "n/a";
		break;
	case 1:
		sDie = "1";
		break;
	case 2:
		sDie = "2";
		break;
	case 3:
		sDie = "4";
		break;
	case 4:
		sDie = "8";
		break;
	default:
		sDie = "reserved";
		break;
	}
	switch (load)
	{
	case 0:
		sLoad = "n/a";
		break;
	case 1:
		sLoad = "multi load";
		break;
	case 2:
		sLoad = "single load";
		break;
	default:
		sLoad = "reserved";
		break;
	}
	snprintf(s, n, "type: %s, die count: %s, signal load: %s.",
		sType, sDie, sLoad);
}
void printDdr3fineTck(char* s, int n, byte myData, byte* allData)
{
	double ftbunits = helperFtbDdr3(*(allData + 9));
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	int tckMtb = *(allData + 12) & 0xFF;
	int tckFtb = (char)myData;
	double ns = mtbunits * tckMtb + (ftbunits * tckFtb) / 1000.0;
	if (ns <= 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "tCKmin: %g ns.", ns);
	}
}
void printDdr3fineTaa(char* s, int n, byte myData, byte* allData)
{
	double ftbunits = helperFtbDdr3(*(allData + 9));
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	int taaMtb = *(allData + 16) & 0xFF;
	int taaFtb = (char)myData;
	double ns = mtbunits * taaMtb + (ftbunits * taaFtb) / 1000.0;
	if (ns <= 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "tAAmin: %g ns.", ns);
	}
}
void printDdr3fineTrcd(char* s, int n, byte myData, byte* allData)
{
	double ftbunits = helperFtbDdr3(*(allData + 9));
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	int trcdMtb = *(allData + 18) & 0xFF;
	int trcdFtb = (char)myData;
	double ns = mtbunits * trcdMtb + (ftbunits * trcdFtb) / 1000.0;
	if (ns <= 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "tRCDmin: %g ns.", ns);
	}
}
void printDdr3fineTrp(char* s, int n, byte myData, byte* allData)
{
	double ftbunits = helperFtbDdr3(*(allData + 9));
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	int trpMtb = *(allData + 20) & 0xFF;
	int trpFtb = (char)myData;
	double ns = mtbunits * trpMtb + (ftbunits * trpFtb) / 1000.0;
	if (ns <= 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "tRPmin: %g ns.", ns);
	}

}
void printDdr3fineTrc(char* s, int n, byte myData, byte* allData)
{
	double ftbunits = helperFtbDdr3(*(allData + 9));
	double mtbunits = helperMtbDdr3(*(allData + 10), *(allData + 11));
	int trcMtb = (*(allData + 23) & 0xFF) + ((*(allData + 21) & 0xF0) << 4);
	int trcFtb = (char)myData;
	double ns = mtbunits * trcMtb + (ftbunits * trcFtb) / 1000.0;
	if (ns <= 0)
	{
		snprintf(s, n, "undefined.");
	}
	else
	{
		snprintf(s, n, "tRCmin: %g ns.", ns);
	}

}
void printDdr3maxActCount(char* s, int n, byte myData, byte* allData)
{
	int tmaw = (myData & 0x30) >> 4;
	const char* sTmaw;
	switch (tmaw)
	{
	case 0:
		sTmaw = "8192*tREFI";
		break;
	case 1:
		sTmaw = "4096*tREFI";
		break;
	case 2:
		sTmaw = "2048*tREFI";
		break;
	default:
		sTmaw = "reserved";
		break;
	}
	int mac = myData & 0x0F;
	const char* sMac;
	switch (mac)
	{
	case 0:
		sMac = "untested";
		break;
	case 1:
		sMac = "700K";
		break;
	case 2:
		sMac = "600K";
		break;
	case 3:
		sMac = "500K";
		break;
	case 4:
		sMac = "400K";
		break;
	case 5:
		sMac = "300K";
		break;
	case 6:
		sMac = "200K";
		break;
	case 8:
		sMac = "unrestricted";
		break;
	default:
		sMac = "reserved";
		break;
	}
	snprintf(s, n, "tMAW: %s, MAC: %s.", sTmaw, sMac);
}
void printDdr3moduleManuf(char* s, int n, byte myData, byte* allData)
{
	const char* vendor = decodeModuleManufacturerDdr3(allData + 117);
	snprintf(s, n, "%s.", vendor);
}
void printDdr3spd(char* s, int n, byte myData, byte* allData)
{
	int count;
	if ((*(allData + 0)) & 0x80)
	{
		count = 116 + 1;
	}
	else
	{
		count = 125 + 1;
	}
	byte* ptr = allData;
	int crc = 0;
	int i = 0;
	while (--count >= 0)
	{
		crc = crc ^ (int)*ptr++ << 8;
		for (i = 0; i < 8; ++i)
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
	}

	int saved = (((*(allData + 127)) & 0xFF) << 8) + (myData & 0xFF);
	int calculated = crc & 0xFFFF;
	if (calculated == saved)
	{
		snprintf(s, n, "passed, %04Xh.",
			saved);
	}
	else
	{
		snprintf(s, n, "failed, saved: %04Xh, calculated: %04Xh.",
			saved, calculated);
	}
}
void printDdr3ModulePart(char* s, int n, byte myData, byte* allData)
{
	char partName[19 + 1];
	char* dst = partName;
	char* src = (char*)allData + 128;
	int m = 19;
	helperPartName(dst, src, m);
	snprintf(s, n, "%s.", partName);
}
void printDdr3chipsManuf(char* s, int n, byte myData, byte* allData)
{
	const char* vendor = decodeModuleManufacturerDdr3(allData + 148);
	snprintf(s, n, "%s.", vendor);
}
void printDdr3moduleSerial(char* s, int n, byte myData, byte* allData)
{
	// TODO. Optimize printDdr3ModuleSerial + printRdramModuleSerial.
	// only offsets different.
	int byte0 = myData & 0xFF;
	int byte1 = *(allData + 123) & 0xFF;
	int byte2 = *(allData + 124) & 0xFF;
	int byte3 = *(allData + 125) & 0xFF;
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

SPD_BYTE_DESCRIPTOR directRambusDecoder[] =
{
	{ spdRevision       , printRdramRevision      },
	{ spdNumOfBytes     , printRdramNumOfBytes    },
	{ spdDeviceType     , printRdramDeviceType    },
	{ spdModuleType     , printRdramModuleType    },
	{ spdRowColumnBits  , printRdramRowColumnBits },
	{ spdBankAndType    , printRdramBankAndType   },
	{ spdRefreshBank    , printRdramRefreshBank   },
	{ spdTref           , printRdramTref          },
	{ spdProtocol       , printRdramProtocol      },
	{ spdMiscDevice     , printRdramMiscDevice    },
	{ spdTrpRmin        , printRdramTrpRmin       },
	{ spdTrasRmin       , printRdramTrasRmin      },
	{ spdTrcdRmin       , printRdramTrcdRmin      },
	{ spdTrrRmin        , printRdramTrrRmin       },
	{ spdTppRmin        , printRdramTppRmin       },
	{ spdMinCycleA      , printRdramTpsAndFmhz    },
	{ spdMaxCycleA      , printRdramTpsAndFmhz    },
	{ spdTcdlyA         , printRdramTcdlyABCD     },
	{ spdTclsTcasA      , printRdramTclsTcasABCD  },
	{ spdMinCycleB      , printRdramTpsAndFmhz    },
	{ spdMaxCycleB      , printRdramTpsAndFmhz    },
	{ spdTcdlyB         , printRdramTcdlyABCD     },
	{ spdTclsTcasB      , printRdramTclsTcasABCD  },
	{ spdMinCycleC      , printRdramTpsAndFmhz    },
	{ spdMaxCycleC      , printRdramTpsAndFmhz    },
	{ spdTcdlyC         , printRdramTcdlyABCD     },
	{ spdTclsTcasC      , printRdramTclsTcasABCD  },
	{ spdMinCycleD      , printRdramTpsAndFmhz    },
	{ spdMaxCycleD      , printRdramTpsAndFmhz    },
	{ spdTcdlyD         , printRdramTcdlyABCD     },
	{ spdTclsTcasD      , printRdramTclsTcasABCD  },
	{ spdTpdnxAmax      , printRdramTus           },
	{ spdTpdnxBmax      , printRdramTpdnxBmax     },
	{ spdTnapxAmax      , printRdramTns           },
	{ spdTnapxBmax      , printRdramTns           },
	{ spdFiminFimax     , printRdramFiminFimax    },
	{ spdFimin70        , printRdramFimin70       },
	{ spdFimax70        , printRdramFimax70       },
	{ spdOdfMapping     , printRdramOdfMapping    },
	{ spdTcctrlMax      , printRdramTms           },
	{ spdTtempMax       , printRdramTms           },
	{ spdTtceMin        , printRdramTclks         },
	{ spdTrasrMax       , printRdramTus           },
	{ spdTnlimitMax     , printRdramTus           },
	{ spdActPch30       , printRdramTclksDual     },
	{ spdCpcRdr30       , printRdramTclksDual     },
	{ spdRetWpr30       , printRdramTclksDual     },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdFras118        , printRdramFras118       },
	{ spdFras70         , printRdramFras70        },
	{ spdPmaxTj         , printRdramPmaxTj        },
	{ spdHeatSpreader   , printRdramHeatSpreader  },
	{ spdPstbyHi        , printRdramPstbyHi       },
	{ spdPactiHi        , printRdramPactiHi       },
	{ spdPactrwHi       , printRdramPactrwHi      },
	{ spdPstbyLo        , printRdramPstbyHi       },
	{ spdPactiLo        , printRdramPactiHi       },
	{ spdPactrwLo       , printRdramPactrwHi      },
	{ spdPnap           , printRdramPnap          },
	{ spdPresa          , printRdramPresa         },
	{ spdPresb          , printRdramPresa         },
	{ spdChecksum062    , printRdramChecksum062   },
	{ spdModuleManuf    , printRdramModuleManuf   },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleLoc      , printRdramModuleLoc     },
	{ spdModulePart     , printRdramModulePart    },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModuleRev      , printRdramModuleRev     },
	{ spdModuleRev      , printRdramSeeAbove      },
	{ spdModuleYear     , printRdramModuleYear    },
	{ spdModuleWeek     , printRdramModuleWeek    },
	{ spdModuleSerial   , printRdramModuleSerial  },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdNumOfDev       , printRdramNumOfDev      },
	{ spdModuleData     , printRdramModuleData    },
	{ spdDeviceEnables  , printRdramDeviceEn1     },
	{ spdDeviceEnables  , printRdramDeviceEn2     },
	{ spdDeviceEnables  , printRdramDeviceEn3     },
	{ spdDeviceEnables  , printRdramDeviceEn4     },
	{ spdModuleVdd      , printRdramModuleVdd     },
	{ spdModuleVddT     , printRdramModuleVddT    },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdCdly01cdly3    , printRdramCdly01cdly    },
	{ spdCdly01cdly4    , printRdramCdly01cdly    },
	{ spdCdly01cdly5    , printRdramCdly01cdly    },
	{ spdCdly01cdly6    , printRdramCdly01cdly    },
	{ spdCdly01cdly7    , printRdramCdly01cdly    },
	{ spdCdly01cdly8    , printRdramCdly01cdly    },
	{ spdCdly01cdly9    , printRdramCdly01cdly    },
	{ spdCdly01cdly10   , printRdramCdly01cdly    },
	{ spdCdly01cdly11   , printRdramCdly01cdly    },
	{ spdCdly01cdly12   , printRdramCdly01cdly    },
	{ spdCdly01cdly13   , printRdramCdly01cdly    },
	{ spdCdly01cdly14   , printRdramCdly01cdly    },
	{ spdCdly01cdly15   , printRdramCdly01cdly    },
	{ spdChecksum99126  , printRdramChecksum99126 }
};
const int DIRECT_RAMBUS_COUNT = sizeof(directRambusDecoder) / sizeof(SPD_BYTE_DESCRIPTOR);
SPD_BYTE_DESCRIPTOR sdrSdramDecoder[] =
{
	{ spdUsedBytes      , printSdramUsedBytes     },
	{ spdRomBytes       , printSdramRomBytes      },
	{ spdDeviceType     , printSdramDeviceType    },
	{ spdRowBits        , printSdramRowColumnBits },
	{ spdColumnBits     , printSdramRowColumnBits },
	{ spdRanks          , printSdramRanks         },
	{ spdModuleWidth    , printSdramModuleWidth   },
	{ spdModuleWidth    , printRdramSeeAbove      },
	{ spdVoltage        , printSdramVoltage       },
	{ spdCycleAtMaxCl   , printSdramCycleTime     },
	{ spdAccessFromClk  , printSdramAccessFromClk },
	{ spdDimmConfig     , printSdramConfig        },
	{ spdRefresh        , printSdramRefresh       },
	{ spdChipsWidth     , printSdramChipsWidth    },
	{ spdErrorCheck     , printSdramCheckWidth    },
	{ spdMinClkB2B      , printSdramMinClkB2B     },
	{ spdBurstLengts    , printSdramBurstLengts   },
	{ spdBanks          , printSdramRanks         },
	{ spdCasLatencies   , printSdramCasLatencies  },
	{ spdCsLatencies    , printSdramCsLatencies   },
	{ spdWrLatencies    , printSdramCsLatencies   },
	{ spdModuleAttr     , printSdramModuleAttr    },
	{ spdDeviceAttr     , printSdramDeviceAttr    },
	{ spdMinClkClM1     , printSdramCycleTime     },
	{ spdMaxDataClM1    , printSdramAccessFromClk },
	{ spdMinClkClM2     , printSdramCycleTime     },
	{ spdMaxDataClM2    , printSdramAccessFromClk },
	{ spdMinRowPrchg    , printSdramNs            },
	{ spdMinRowAct      , printSdramNs            },
	{ spdMinRasToCas    , printSdramNs            },
	{ spdMinRasPulse    , printSdramNs            },
	{ spdModuleBankDen  , printSdramBankDen       },
	{ spdAddrCmdSet     , printSdramTimeSigned    },
	{ spdAddrCmdHold    , printSdramTimeSigned    },
	{ spdDataInputSet   , printSdramTimeSigned    },
	{ spdDataInputHold  , printSdramTimeSigned    },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdSuperset       , printRdramReserved      },
	{ spdRevision       , printSdramRevision      },
	{ spdChecksum062    , printRdramChecksum062   },
	{ spdModuleManuf    , printRdramModuleManuf   },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleLoc      , printSdramModuleLoc     },
	{ spdModulePart     , printRdramModulePart    },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModuleRev      , printSdramChipRev       },
	{ spdModuleRev      , printSdramBoardRev      },
	{ spdModuleWeek     , printRdramModuleWeek    },
	{ spdModuleYear     , printRdramModuleYear    },
	{ spdModuleSerial   , printRdramModuleSerial  },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdVendorSpec     , printSdramIntelPc       },
	{ spdVendorSpec     , printSdramIntelPcAttr   },
};
const int SDR_SDRAM_COUNT = sizeof(sdrSdramDecoder) / sizeof(SPD_BYTE_DESCRIPTOR);
SPD_BYTE_DESCRIPTOR ddrSdramDecoder[] =
{
	{ spdUsedBytes      , printSdramUsedBytes     },
	{ spdRomBytes       , printSdramRomBytes      },
	{ spdDeviceType     , printSdramDeviceType    },
	{ spdRowBits        , printSdramRowColumnBits },
	{ spdColumnBits     , printSdramRowColumnBits },
	{ spdRanks          , printSdramRanks         },
	{ spdModuleWidth    , printSdramModuleWidth   },
	{ spdModuleWidth    , printRdramSeeAbove      },
	{ spdVoltage        , printDdrVoltage         },
	{ spdCycleAtMaxCl   , printDdrCycleTime       },
	{ spdAccessFromClk  , printDdrAccessFromClk   },
	{ spdDimmConfig     , printSdramConfig        },
	{ spdRefresh        , printSdramRefresh       },
	{ spdChipsWidth     , printSdramChipsWidth    },
	{ spdErrorCheck     , printSdramCheckWidth    },
	{ spdMinClkB2B      , printSdramMinClkB2B     },
	{ spdBurstLengts    , printSdramBurstLengts   },
	{ spdBanks          , printSdramRanks         },
	{ spdCasLatencies   , printDdrCasLatencies    },
	{ spdCsLatencies    , printSdramCsLatencies   },
	{ spdWrLatencies    , printSdramCsLatencies   },
	{ spdModuleAttr     , printDdrModuleAttr      },
	{ spdDeviceAttr     , printDdrDeviceAttr      },
	{ spdMinClkClM05    , printDdrCycleTime       },
	{ spdMaxDataClM05   , printDdrAccessFromClk   },
	{ spdMinClkClM1     , printDdrCycleTime       },
	{ spdMaxDataClM1    , printDdrAccessFromClk   },
	{ spdMinRowPrchg    , printDdr025ns           },
	{ spdMinRowAct      , printDdr025ns           },
	{ spdMinRasToCas    , printDdr025ns           },
	{ spdMinRasPulse    , printSdramNs            },
	{ spdModuleBankDen  , printDdrBankDen         },
	{ spdAddrCmdSet     , printDdr01ns            },
	{ spdAddrCmdHold    , printDdr01ns            },
	{ spdDataInputSet   , printDdr01ns            },
	{ spdDataInputHold  , printDdr01ns            },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdMinAutoRef     , printSdramNs            },
	{ spdMinAutoRefTo   , printSdramNs            },
	{ spdMaxCycleTime   , printDdr025ns           },
	{ spdMaxDqsSkew     , printDdr001ns           },
	{ spdReadSkew       , printDdr001ns           },
	{ spdReserved       , printRdramReserved      },
	{ spdDimmHeight     , printDdrDimmH           },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdRevision       , printSdramRevision      },
	{ spdChecksum062    , printRdramChecksum062   },
	{ spdModuleManuf    , printRdramModuleManuf   },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleLoc      , printSdramModuleLoc     },
	{ spdModulePart     , printRdramModulePart    },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModuleRev      , printSdramChipRev       },
	{ spdModuleRev      , printSdramBoardRev      },
	{ spdModuleWeek     , printRdramModuleWeek    },
	{ spdModuleYear     , printRdramModuleYear    },
	{ spdModuleSerial   , printRdramModuleSerial  },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
};
const int DDR_SDRAM_COUNT = sizeof(ddrSdramDecoder) / sizeof(SPD_BYTE_DESCRIPTOR);
SPD_BYTE_DESCRIPTOR ddr2sdramDecoder[] =
{
	{ spdUsedBytes      , printSdramUsedBytes     },
	{ spdRomBytes       , printSdramRomBytes      },
	{ spdDeviceType     , printSdramDeviceType    },
	{ spdRowBits        , printDdr2rowBits        },
	{ spdColumnBits     , printDdr2columnBits     },
	{ spdRanksPckgH     , printDdr2ranksPcHg      },
	{ spdModuleWidth    , printDdr2moduleWidth    },
	{ spdReserved       , printRdramReserved      },
	{ spdVoltage        , printDdrVoltage         },
	{ spdCycleAtMaxCl   , printDdr2cycleTime      },
	{ spdAccessFromClk  , printDdrAccessFromClk   },
	{ spdDimmConfig     , printDdr2config         },
	{ spdRefresh        , printSdramRefresh       },
	{ spdChipsWidth     , printDdr2moduleWidth    },
	{ spdErrorCheck     , printDdr2eccWidth       },
	{ spdReserved       , printRdramReserved      },
	{ spdBurstLengts    , printDdr2burstLengts    },
	{ spdBanks          , printSdramRanks         },
	{ spdCasLatencies   , printDdr2casLatencies   },
	{ spdDimmMech       , printDdr2dimmMech       },
	{ spdDimmType       , printDdr2dimmType       },
	{ spdModuleAttr     , printDdr2moduleAttr     },
	{ spdDeviceAttr     , printDdr2deviceAttr     },
	{ spdMinClkClM1     , printDdr2cycleTime      },
	{ spdMaxDataClM1    , printDdrAccessFromClk   },
	{ spdMinClkClM2     , printDdr2cycleTime      },
	{ spdMaxDataClM2    , printDdrAccessFromClk   },
	{ spdMinRowPrchg    , printDdr025ns           },
	{ spdMinRowAct      , printDdr025ns           },
	{ spdMinRasToCas    , printDdr025ns           },
	{ spdMinRasPulse    , printSdramNs            },
	{ spdModuleBankDen  , printDdr2bankDen        },
	{ spdAddrCmdSet     , printDdr01ns            },
	{ spdAddrCmdHold    , printDdr01ns            },
	{ spdDataInputSet   , printDdr01ns            },
	{ spdDataInputHold  , printDdr01ns            },
	{ spdWriteRec       , printDdr025ns           },
	{ spdWriteToRead    , printDdr025ns           },
	{ spdReadToPrec     , printDdr025ns           },
	{ spdProbeChar      , printRdramReserved      },
	{ spdExtTrcTrfc     , printDdr2extTrcTrfc     },
	{ spdMinAutoRef     , printDdr2autoRef        },
	{ spdMinAutoRefTo   , printDdr2autoRefTo      },
	{ spdMaxCycleTime   , printDdr2cycleTime      },
	{ spdMaxDqsSkew     , printDdr001ns           },
	{ spdReadSkew       , printDdr001ns           },
	{ spdPllRelock      , printRdramTus           },
	{ spdTcaseMax       , printDdr2tcaseMax       },
	{ spdThermRes       , printDdr2thermRes       },
	{ spdDramTriseAp    , printDdr2triseAp        },
	{ spdDramTriseP     , printDdr2dramTriseP     },
	{ spdDramTrisePp    , printDdr2dramTrisePp    },
	{ spdDramTriseAs    , printDdr2dramTriseAs    },
	{ spdDramTriseApf   , printDdr2dramTriseApf   },
	{ spdDramTriseAps   , printDdr2dramTriseAps   },
	{ spdDramTriseBr    , printDdr2dramTriseBr    },
	{ spdDramTriseBrf   , printDdr2dramTriseBrf   },
	{ spdDramTriseBir   , printDdr2dramTriseBrf   },
	{ spdThermResPll    , printDdr2thermRes       },
	{ spdThermResReg    , printDdr2thermRes       },
	{ spdPllTriseA      , printDdr2pllTriseA      },
	{ spdRegTriseA      , printDdr2regTriseA      },
	{ spdRevision       , printSdramRevision      },
	{ spdChecksum062    , printRdramChecksum062   },
	{ spdModuleManuf    , printRdramModuleManuf   },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleLoc      , printSdramModuleLoc     },
	{ spdModulePart     , printRdramModulePart    },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModuleRev      , printSdramChipRev       },
	{ spdModuleRev      , printSdramBoardRev      },
	{ spdModuleWeek     , printRdramModuleWeek    },
	{ spdModuleYear     , printRdramModuleYear    },
	{ spdModuleSerial   , printRdramModuleSerial  },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
};
const int DDR2_SDRAM_COUNT = sizeof(ddr2sdramDecoder) / sizeof(SPD_BYTE_DESCRIPTOR);
SPD_BYTE_DESCRIPTOR ddr3sdramDecoder[] =
{
	{ spdUsedBytes      , printDdr3usedBytes      },
	{ spdRevision       , printSdramRevision      },
	{ spdDeviceType     , printSdramDeviceType    },
	{ spdModuleType     , printDdr3moduleType     },
	{ spdDensBank       , printDdr3densBank       },
	{ spdAddressing     , printDdr3addressing     },
	{ spdNomVoltage     , printDdr3nomVoltage     },
	{ spdModuleOrg      , printDdr3moduleOrg      },
	{ spdBusWidth       , printDdr3moduleWidth    },
	{ spdFineTime       , printDdr3fineTime       },
	{ spdMediumTdvdn    , printDdr3mediumTime     },
	{ spdMediumTdivs    , printRdramSeeAbove      },
	{ spdMinCycleTime   , printDdr3minCycle       },
	{ spdReserved       , printRdramReserved      },
	{ spdCasLatencies   , printDdr3casLatencies   },
	{ spdCasLatencies   , printRdramSeeAbove      },
	{ spdMinCasLat      , printDdr3mtbNs          },
	{ spdMinWriteRecM   , printDdr3mtbNs          },
	{ spdMinRasToCasM   , printDdr3mtbNs          },
	{ spdMinRowActTaM   , printDdr3mtbNs          },
	{ spdMinRowPrechM   , printDdr3mtbNs          },
	{ spdUpTrasTrcM     , printDdr3upTrasTrc      },
	{ spdMinActToPrM    , printDdr3tRas           },
	{ spdMinActToAcM    , printDdr3tRc            },
	{ spdMinRefRecLoM   , printDdr3lowTrfcMin     },
	{ spdMinRefRecHiM   , printDdr3tRfcMin        },
	{ spdMinWrToRdM     , printDdr3mtbNs          },
	{ spdMinRdToPcM     , printDdr3mtbNs          },
	{ spdUpTfawM        , printDdr3upTfaw         },
	{ spdMinFawM        , printDdr3tFaw           },
	{ spdOptional       , printDdr3optionFeat     },
	{ spdThermRef       , printDdr3thermRefOpt    },
	{ spdThermSens      , printDdr3thermSensor    },
	{ spdDeviceChips    , printDdr3deviceChips    },
	{ spdFineTck        , printDdr3fineTck        },
	{ spdFineTaa        , printDdr3fineTaa        },
	{ spdFineTrcd       , printDdr3fineTrcd       },
	{ spdFineTrp        , printDdr3fineTrp        },
	{ spdFineTrc        , printDdr3fineTrc        },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdMaxActCount    , printDdr3maxActCount    },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdReserved       , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdFamilySpec     , printRdramReserved      },
	{ spdModuleManuf    , printDdr3moduleManuf    },
	{ spdModuleManuf    , printRdramSeeAbove      },
	{ spdModuleLoc      , printSdramModuleLoc     },
	{ spdModuleYear     , printRdramModuleYear    },
	{ spdModuleWeek     , printRdramModuleWeek    },
	{ spdModuleSerial   , printDdr3moduleSerial   },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdModuleSerial   , printRdramSeeAbove      },
	{ spdCrcLow         , printDdr3spd            },
	{ spdCrcHigh        , printRdramSeeAbove      },
	{ spdModulePart     , printDdr3ModulePart     },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModulePart     , printRdramSeeAbove      },
	{ spdModuleRev      , printRdramModuleRev     },
	{ spdModuleRev      , printRdramSeeAbove      },
	{ spdChipManuf      , printDdr3chipsManuf     },
	{ spdChipManuf      , printRdramSeeAbove      },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     },
	{ spdManufSpec      , printSdramManufSpec     }
};
const int DDR3_SDRAM_COUNT = sizeof(ddr3sdramDecoder) / sizeof(SPD_BYTE_DESCRIPTOR);
SPD_BYTE_DESCRIPTOR ddr4sdramDecoder[] =
{
	{ spdUsedBytes      , NULL                 },
	{ spdRevision       , NULL                 },
	{ spdDeviceType     , NULL                 },
};
const int DDR4_SDRAM_COUNT = sizeof(ddr4sdramDecoder) / sizeof(SPD_BYTE_DESCRIPTOR);
SPD_BYTE_DESCRIPTOR ddr5sdramDecoder[] =
{
	{ spdUsedBytes      , NULL                 },
	{ spdRevision       , NULL                 },
	{ spdDeviceType     , NULL                 },
};
const int DDR5_SDRAM_COUNT = sizeof(ddr5sdramDecoder) / sizeof(SPD_BYTE_DESCRIPTOR);

struct RAM_TYPES_DIRECTORY
{
	const char* ramTypeName;
	SPD_BYTE_DESCRIPTOR* ramTypeDecoder;
	int count;
};
RAM_TYPES_DIRECTORY ramTypesDirectory[] =
{
	{ "unknown"          , NULL                , 0                   },
	{ "Direct Rambus"    , directRambusDecoder , DIRECT_RAMBUS_COUNT },
	{ "Rambus"           , NULL                , 0                   },
	{ "FPM DRAM"         , NULL                , 0                   },
	{ "EDO DRAM"         , NULL                , 0                   },
	{ "Pipelined nibble" , NULL                , 0                   },
	{ "SDRAM"            , sdrSdramDecoder     , SDR_SDRAM_COUNT     },
	{ "Multiplexed ROM"  , NULL                , 0                   },
	{ "DDR SGRAM"        , NULL                , 0                   },
	{ "DDR SDRAM"        , ddrSdramDecoder     , DDR_SDRAM_COUNT     },
	{ "DDR2 SDRAM"       , ddr2sdramDecoder    , DDR2_SDRAM_COUNT    },
	{ "DDR3 SDRAM"       , ddr3sdramDecoder    , DDR3_SDRAM_COUNT    },
	{ "DDR4 SDRAM"       , ddr4sdramDecoder    , DDR4_SDRAM_COUNT    },
	{ "DDR5 SDRAM"       , ddr5sdramDecoder    , DDR5_SDRAM_COUNT    }
};

int parseSpd(byte* ptr, int size)
{
	int nativeType = *(ptr + 2);
	int indexType = UNKNOWN;
	switch (nativeType)
	{
	case ID_RDRAM:
		indexType = DIRECT_RAMBUS;
		break;
	case ID_SDR:
		indexType = SDR_SDRAM;
		break;
	case ID_DDR:
		indexType = DDR_SDRAM;
		break;
	case ID_DDR2:
		indexType = DDR2_SDRAM;
		break;
	case ID_DDR3:
		indexType = DDR3_SDRAM;
		break;
	case ID_DDR4:
		indexType = DDR4_SDRAM;
		break;
	case ID_DDR5:
		indexType = DDR5_SDRAM;
		break;
	default:
		indexType = UNKNOWN;
		break;
	}
	cout << endl;

	// module summary for Direct Rambus
	if (indexType == DIRECT_RAMBUS)
	{
		parmGroup("Module");
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
		char moduleString[MAX_TEXT_STRING];
		snprintf(moduleString, MAX_TEXT_STRING, "%s %s %s %s.",
			vendorName, partName, speedName, sizeName);
		parmSummary(moduleString);
	}

	// module summary for SDRAM
	if (indexType == SDR_SDRAM)
	{
		parmGroup("Module");
		const char* vendorName = decodeModuleManufacturer(ptr + 64);
		char partName[19 + 1];
		char* dst = partName;
		char* src = (char*)ptr + 73;
		int m = 19;
		helperPartName(dst, src, m);
		char speedName[32];
		int intelSpec = *(ptr + 126);
		int tlow = *(ptr + 9) & 0xF;
		int thigh = (*(ptr + 9) & 0xF0) >> 4;
		double us = thigh * 0.001 + tlow * 0.0001;
		double mhz = 1.0 / us;
		if ((thigh == 7) && (tlow == 5))
		{
			snprintf(speedName, 32, "PC133");
		}
		else if (((intelSpec == 0x66) || (intelSpec == 0x64)) && (mhz <= 100.0))
		{
			switch (intelSpec)
			{
			case 0x66:
				snprintf(speedName, 32, "PC66");
				break;
			case 0x64:
				snprintf(speedName, 32, "PC100");
				break;
			case 0:
			case 0xFF:
				if ((thigh == 0) && (tlow == 0))
				{
					snprintf(speedName, 32, "?MHz");
				}
				else
				{
					double us = thigh * 0.001 + tlow * 0.0001;
					double mhz = 1.0 / us;
					snprintf(speedName, 32, "%.1fMHz", mhz);
				}
				break;
			default:
				snprintf(speedName, 32, "PC???");
				break;
			}
		}
		else
		{
			snprintf(speedName, 32, "%.1fMHz", mhz);
		}
		char sizeName[48];
		int rankCount = *(ptr + 5);
		int rankDensity = *(ptr + 31);
		int rank1, rank2;
		helperRanksDensitySdram(rankDensity, rank1, rank2);
		int megabytes;
		switch (rankCount)
		{
		case 1:
			megabytes = rank1;
			break;
		case 2:
			if (rank2)
			{
				megabytes = rank1 + rank2;
			}
			else
			{
				megabytes = rank1 * 2;
			}
			break;
		case 3:
		case 4:
			megabytes = rank1 * rankCount;
			break;
		default:
			megabytes = 0;
		}
		if (megabytes == 0)
		{
			snprintf(sizeName, 32, "unknown module size");
		}
		else
		{
			snprintf(sizeName, 32, "%dMB", megabytes);
		}
		char moduleString[MAX_TEXT_STRING];
		snprintf(moduleString, MAX_TEXT_STRING, "%s %s %s %s.",
			vendorName, partName, speedName, sizeName);
		parmSummary(moduleString);
	}

	// module summary for DDR SDRAM
	if (indexType == DDR_SDRAM)
	{
		parmGroup("Module");
		const char* vendorName = decodeModuleManufacturer(ptr + 64);
		char partName[19 + 1];
		char* dst = partName;
		char* src = (char*)ptr + 73;
		int m = 19;
		helperPartName(dst, src, m);
		const char* speedName;
		int tclk = *(ptr + 9);
		switch (tclk)
		{
		case 0xA0:
			speedName = "PC1600";
			break;
		case 0x75:
			speedName = "PC2100";
			break;
		case 0x60:
			speedName = "PC2700";
			break;
		case 0x50:
			speedName = "PC3200";
			break;
		default:
			speedName = "PC???";
			break;
		}
		char sizeName[48];
		int rankCount = *(ptr + 5);
		int rankDensity = *(ptr + 31);
		int rank1, rank2;
		helperRanksDensityDdr(rankDensity, rank1, rank2);
		int megabytes;
		switch (rankCount)
		{
		case 1:
			megabytes = rank1;
			break;
		case 2:
			if (rank2)
			{
				megabytes = rank1 + rank2;
			}
			else
			{
				megabytes = rank1 * 2;
			}
			break;
		case 3:
		case 4:
			megabytes = rank1 * rankCount;
			break;
		default:
			megabytes = 0;
		}
		if (megabytes == 0)
		{
			snprintf(sizeName, 32, "unknown module size");
		}
		else
		{
			snprintf(sizeName, 32, "%dMB", megabytes);
		}
		char moduleString[MAX_TEXT_STRING];
		snprintf(moduleString, MAX_TEXT_STRING, "%s %s %s %s.",
			vendorName, partName, speedName, sizeName);
		parmSummary(moduleString);
	}

	// module summary for DDR2 SDRAM
	if (indexType == DDR2_SDRAM)
	{
		parmGroup("Module");
		const char* vendorName = decodeModuleManufacturer(ptr + 64);
		char partName[19 + 1];
		char* dst = partName;
		char* src = (char*)ptr + 73;
		int m = 19;
		helperPartName(dst, src, m);
		const char* speedName;
		int tclk = *(ptr + 9);
		switch (tclk)
		{
		case 0x50:
			speedName = "PC3200";
			break;
		case 0x3D:
			speedName = "PC4200";
			break;
		case 0x30:
			speedName = "PC5300";
			break;
		case 0x25:
			speedName = "PC6400";
			break;
		default:
			speedName = "PC???";
			break;
		}
		char sizeName[48];
		int rankCount = (*(ptr + 5) & 0x07) + 1;
		int rankDensity = *(ptr + 31);
		int rank1, rank2;
		helperRanksDensityDdr2(rankDensity, rank1, rank2);
		int megabytes;
		switch (rankCount)
		{
		case 1:
			megabytes = rank1;
			break;
		case 2:
			if (rank2)
			{
				megabytes = rank1 + rank2;
			}
			else
			{
				megabytes = rank1 * 2;
			}
			break;
		case 3:
		case 4:
			megabytes = rank1 * rankCount;
			break;
		default:
			megabytes = 0;
		}
		if (megabytes == 0)
		{
			snprintf(sizeName, 32, "unknown module size");
		}
		else
		{
			snprintf(sizeName, 32, "%dMB", megabytes);
		}
		char moduleString[MAX_TEXT_STRING];
		snprintf(moduleString, MAX_TEXT_STRING, "%s %s %s %s.",
			vendorName, partName, speedName, sizeName);
		parmSummary(moduleString);
	}

	// module summary for DDR3 SDRAM
	if (indexType == DDR3_SDRAM)
	{
		parmGroup("Module");
		const char* vendorName = decodeModuleManufacturerDdr3(ptr + 117);
		char partName[19 + 1];
		char* dst = partName;
		char* src = (char*)ptr + 128;
		int m = 19;
		helperPartName(dst, src, m);
		const char* speedName;
		double mtbunits = helperMtbDdr3(*(ptr + 10), *(ptr + 11));
		if ((mtbunits < 0.0) || (ptr + 12 == 0))
		{
			speedName = "PC???";
		}
		else
		{
			int clocks = *(ptr + 12) & 0xFF;
			double ns = clocks * mtbunits;
			double mhz = 1000.0 / ns;
			int fspec = (int)(mhz * 2.0);
			switch (fspec)
			{
			case 800:
				speedName = "PC3-6400";
				break;
			case 1066:
				speedName = "PC3-PC8500";
				break;
			case 1333:
				speedName = "PC3-10600";
				break;
			case 1600:
				speedName = "PC3-12800";
				break;
			case 1866:
				speedName = "PC3-14900";
				break;
			case 2133:
				speedName = "PC3-17000";
				break;
			default:
				speedName = "PC???";
				break;
			}
		}
		char sizeName[48];
		int sdramCapacity = helperChipSizeDdr3(*(ptr + 4));
		int primaryBusWidth;
		int dataBusIndex = (*(ptr + 8)) & 0x07;
		if (dataBusIndex > 3)
		{
			primaryBusWidth = -1;
		}
		else
		{
			primaryBusWidth = 8 << dataBusIndex;
		}
		int sdramWidth;
		int widthIndex = (*(ptr + 7)) & 0x07;
		if (widthIndex > 3)
		{
			sdramWidth = -1;
		}
		else
		{
			sdramWidth = 4 << widthIndex;
		}
		int ranks;
		int rankIndex = ((*(ptr + 7)) & 0x38) >> 3;
		if (rankIndex > 4)
		{
			ranks = -1;
		}
		else
		{
			ranks = 1 + rankIndex;
		}
		int megabytes = sdramCapacity / 8 * primaryBusWidth / sdramWidth * ranks;
		if (megabytes <= 0)
		{
			snprintf(sizeName, 32, "unknown module size");
		}
		else
		{
			snprintf(sizeName, 32, "%dMB", megabytes);
		}
		char moduleString[MAX_TEXT_STRING];
		snprintf(moduleString, MAX_TEXT_STRING, "%s %s %s %s.",
			vendorName, partName, speedName, sizeName);
		parmSummary(moduleString);
	}

	// fundamental memory type
	RAM_TYPES_DIRECTORY rtd = ramTypesDirectory[indexType];
	char typeString[MAX_TEXT_STRING];
	snprintf(typeString, MAX_TEXT_STRING, "Fundamental memory type: %s.", rtd.ramTypeName);
	parmSummary(typeString);
	cout << endl;

	// raw hex dump
	parmGroup("Raw hex dump of SPD data");
	parmDump(ptr, size);
	cout << endl;

	// SPD bytes details
	parmGroup("Offset (hex) and parameter name          | Parameter value (hex) and comments");
	SPD_BYTE_DESCRIPTOR* byteDecoder = rtd.ramTypeDecoder;
	int byteCount = rtd.count;

	if ((byteDecoder == NULL) || (byteCount == 0))
	{
		parmError("Detail per-byte information is not supported for this memory type.           ");
	}
	else
	{
		char nameBuf[MAX_TEXT_STRING];
		char valueBuf[MAX_TEXT_STRING];
		byte* ptr1 = ptr;
		for (int i = 0; i < byteCount; i++)
		{
			snprintf(nameBuf, MAX_TEXT_STRING, "[%02X] %s", i, byteDecoder[i].byteName);
			char* p = valueBuf;
			int n = MAX_TEXT_STRING;
			int m = snprintf(p, n, "%02X, ", *ptr1);
			p += m;
			n -= m;
			void(*handler)(char*, int, byte, byte*) = byteDecoder[i].printHandler;
			if (handler)
			{
				handler(p, n, *ptr1, ptr);
			}
			parmValue(nameBuf, valueBuf);
			ptr1++;
		}
	}
	return 0;
}
