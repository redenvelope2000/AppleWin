#pragma once

#include "RGBMonitor.h"
#include "Harddisk.h"
#include "Disk.h"
#include "Common.h"
#include "Card.h"


struct CmdLine
{
	struct SlotInfo
	{
		SlotInfo()
		{
			isDiskII13 = false;
		}

		bool isDiskII13;
	};

	CmdLine()
	{
		bShutdown = false;
		setFullScreen = -1;
		bBoot = false;
		bSlot0LanguageCard = false;
		bSlot7EmptyOnExit = false;
		bSwapButtons0and1 = false;
		bRemoveNoSlotClock = false;
		snesMaxAltControllerType[0] = false;
		snesMaxAltControllerType[1] = false;
		supportDCD = false;
		enableDumpToRealPrinter = false;
		noDisk2StepperDefer = false;
		szSnapshotName = NULL;
		szScreenshotFilename = NULL;
		uRamWorksExPages = 0;
		uSaturnBanks = 0;
		newVideoType = -1;
		newVideoStyleEnableMask = 0;
		newVideoStyleDisableMask = 0;
		newVideoRefreshRate = VR_NONE;
		clockMultiplier = 0.0;	// 0 => not set from cmd-line
		model = A2TYPE_MAX;
		rgbCard = RGB_Videocard_e::Apple;
		rgbCardForegroundColor = 15;
		rgbCardBackgroundColor = 0;
		bestFullScreenResolution = false;
		userSpecifiedWidth = 0;
		userSpecifiedHeight = 0;

		for (UINT i = 0; i < NUM_SLOTS; i++)
		{
			bSlotEmpty[i] = false;
			slotInsert[i] = CT_Empty;
			szImageName_drive[i][DRIVE_1] = NULL;
			szImageName_drive[i][DRIVE_2] = NULL;
			driveConnected[i][DRIVE_1] = true;
			driveConnected[i][DRIVE_2] = true;
			szImageName_harddisk[i][HARDDISK_1] = NULL;
			szImageName_harddisk[i][HARDDISK_2] = NULL;
		}
	}

	bool bShutdown;
	int setFullScreen;	// tristate: -1 (no cmd line specified), 0="-no-full-screen", 1="-full-screen"
	bool bBoot;
	bool bSlot0LanguageCard;
	bool bSlotEmpty[NUM_SLOTS];
	bool bSlot7EmptyOnExit;
	bool bSwapButtons0and1;
	bool bRemoveNoSlotClock;
	bool snesMaxAltControllerType[2];
	bool supportDCD;
	bool enableDumpToRealPrinter;
	bool noDisk2StepperDefer;	// debug
	SS_CARDTYPE slotInsert[NUM_SLOTS];
	SlotInfo slotInfo[NUM_SLOTS];
	LPCSTR szImageName_drive[NUM_SLOTS][NUM_DRIVES];
	bool driveConnected[NUM_SLOTS][NUM_DRIVES];
	LPCSTR szImageName_harddisk[NUM_SLOTS][NUM_HARDDISKS];
	LPSTR szSnapshotName;
	LPSTR szScreenshotFilename;
	UINT uRamWorksExPages;
	UINT uSaturnBanks;
	int newVideoType;
	int newVideoStyleEnableMask;
	int newVideoStyleDisableMask;
	VideoRefreshRate_e newVideoRefreshRate;
	double clockMultiplier;
	eApple2Type model;
	RGB_Videocard_e rgbCard;
	int rgbCardForegroundColor;
	int rgbCardBackgroundColor;
	std::string strCurrentDir;
	bool bestFullScreenResolution;
	UINT userSpecifiedWidth;
	UINT userSpecifiedHeight;
	std::string wavFileSpeaker;
	std::string wavFileMockingboard;
};

bool ProcessCmdLine(LPSTR lpCmdLine);

extern std::string g_sConfigFile; // INI file to use instead of Registry
extern bool g_bCapturePrintScreenKey;
extern bool g_bRegisterFileTypes;
extern bool g_bHookSystemKey;
extern bool g_bHookAltTab;
extern bool g_bHookAltGrControl;
extern CmdLine g_cmdLine;
