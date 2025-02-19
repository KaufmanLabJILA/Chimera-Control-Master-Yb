#include "stdafx.h"
#include "DebugOptionsControl.h"
#include "commonFunctions.h"
#include "MainWindow.h"
#include "CameraWindow.h"
#include "AuxiliaryWindow.h"
#include <future>
#include "resource.h"
//#include "TestWin.cpp"

MainWindow::MainWindow(UINT id, CDialog* splash) : CDialog(id), profile(PROFILES_PATH),
masterConfig(MASTER_CONFIGURATION_FILE_ADDRESS),
appSplash(splash), dds(DDS_FPGA_PORT, 115200),
gmoog(GIGAMOOG_IPADDRESS, GIGAMOOG_PORT), awg0(AWG_PORT0, AWG_BAUD, AWG_SAFEMODE0), awg1(AWG_PORT1, AWG_BAUD, AWG_SAFEMODE1), zynq_tcp(), idler() //115200
{
	// create all the main rgbs and brushes. I want to make sure this happens before other windows are created.
	mainRGBs["Light Green"] = RGB(163, 190, 140);
	mainRGBs["Slate Grey"] = RGB(101, 115, 126);
	mainRGBs["Pale Pink"] = RGB(180, 142, 173);
	mainRGBs["Musky Red"] = RGB(191, 97, 106);

	mainRGBs["theme BG0"] = RGB(15, 15, 15);
	mainRGBs["theme BG1"] = RGB(29, 31, 33);
	mainRGBs["theme BG2"] = RGB(45, 45, 45);
	mainRGBs["theme input"] = RGB(40, 42, 46);
	mainRGBs["theme comment"] = RGB(150, 152, 150);
	mainRGBs["theme selection"] = RGB(55, 59, 65);
	mainRGBs["theme foreground"] = RGB(197, 200, 198);
	mainRGBs["theme red"] = RGB(213, 78, 83);
	mainRGBs["theme purple"] = RGB(195, 151, 216);
	mainRGBs["theme aqua"] = RGB(112, 192, 177);
	mainRGBs["theme green"] = RGB(185, 202, 74);
	mainRGBs["theme blue"] = RGB(122, 166, 218);
	mainRGBs["theme orange"] = RGB(231, 140, 69);
	mainRGBs["theme yellow"] = RGB(231, 197, 71);
	mainRGBs["Solarized Magenta"] = RGB(211, 54, 130);
	mainRGBs["Slate Green"] = RGB(23, 84, 81);
	mainRGBs["Dark Grey"] = RGB(15, 15, 15);
	mainRGBs["Dark Grey Red"] = RGB(20, 12, 12);
	mainRGBs["Medium Grey"] = RGB(30, 30, 30);
	mainRGBs["Light Grey"] = RGB(60, 60, 60);
	mainRGBs["Green"] = RGB(50, 200, 50);
	mainRGBs["Red"] = RGB(200, 50, 50);
	mainRGBs["Blue"] = RGB(50, 50, 200);
	mainRGBs["Gold"] = RGB(218, 165, 32);
	mainRGBs["White"] = RGB(255, 255, 255);
	mainRGBs["Light Red"] = RGB(255, 100, 100);
	mainRGBs["Dark Red"] = RGB(150, 0, 0);
	mainRGBs["Light Blue"] = RGB(100, 100, 255);
	mainRGBs["Forest Green"] = RGB(34, 139, 34);
	mainRGBs["Dark Green"] = RGB(0, 50, 0);
	mainRGBs["Dull Red"] = RGB(107, 35, 35);
	mainRGBs["Dark Lavender"] = RGB(100, 100, 205);
	mainRGBs["Teal"] = RGB(0, 255, 255);
	mainRGBs["Tan"] = RGB(210, 180, 140);
	mainRGBs["Purple"] = RGB(147, 112, 219);
	mainRGBs["Orange"] = RGB(255, 165, 0);
	mainRGBs["Brown"] = RGB(139, 69, 19);
	mainRGBs["Black"] = RGB(0, 0, 0);
	mainRGBs["Dark Blue"] = RGB(0, 0, 75);
	// there are less brushes because these are only used for backgrounds.
	// the following single-line statements are all equivalent to these two statements:
	// mainBrushes["Name"] = new CBrush;
	// mainBrushes["Name"]->CreateSolidBrush(...);
	(mainBrushes["Dark Red"] = new CBrush)->CreateSolidBrush(mainRGBs["Dark Red"]);
	(mainBrushes["Gold"] = new CBrush)->CreateSolidBrush(mainRGBs["Gold"]);
	(mainBrushes["Dark Grey"] = new CBrush)->CreateSolidBrush(mainRGBs["Dark Grey"]);
	(mainBrushes["Dark Grey Red"] = new CBrush)->CreateSolidBrush(mainRGBs["Dark Grey Red"]);
	(mainBrushes["Medium Grey"] = new CBrush)->CreateSolidBrush(mainRGBs["Medium Grey"]);
	(mainBrushes["Light Grey"] = new CBrush)->CreateSolidBrush(mainRGBs["Light Grey"]);
	(mainBrushes["Green"] = new CBrush)->CreateSolidBrush(mainRGBs["Green"]);
	(mainBrushes["Red"] = new CBrush)->CreateSolidBrush(mainRGBs["Red"]);
	(mainBrushes["theme foreground"] = new CBrush)->CreateSolidBrush(mainRGBs["theme foreground"]);
	(mainBrushes["Dull Red"] = new CBrush)->CreateSolidBrush(mainRGBs["Dull Red"]);
	(mainBrushes["Dark Blue"] = new CBrush)->CreateSolidBrush(mainRGBs["Dark Blue"]);
	(mainBrushes["Dark Green"] = new CBrush)->CreateSolidBrush(mainRGBs["Dark Green"]);
	(mainBrushes["theme blue"] = new CBrush)->CreateSolidBrush(mainRGBs["theme blue"]);
	(mainBrushes["theme BG0"] = new CBrush)->CreateSolidBrush(mainRGBs["theme BG0"]);
	(mainBrushes["theme BG1"] = new CBrush)->CreateSolidBrush(mainRGBs["theme BG1"]);
	(mainBrushes["theme input"] = new CBrush)->CreateSolidBrush(mainRGBs["theme input"]);
	(mainBrushes["theme BG2"] = new CBrush)->CreateSolidBrush(mainRGBs["theme BG2"]);
	/// the following are all equivalent to:
	// mainFonts["Font name"] = new CFont;
	// mainFonts["Font name"].CreateFontA(...);
	(mainFonts["Smaller Font Max"] = new CFont)
		->CreateFontA(27, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Normal Font Max"] = new CFont)
		->CreateFontA(34, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Heading Font Max"] = new CFont)
		->CreateFontA(42, 0, 0, 0, FW_DONTCARE, TRUE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Old Sans Black"));
	(mainFonts["Code Font Max"] = new CFont)
		->CreateFontA(32, 0, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Consolas"));
	(mainFonts["Larger Font Max"] = new CFont)
		->CreateFontA(40, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Smaller Font Large"] = new CFont)
		->CreateFontA(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Normal Font Large"] = new CFont)
		->CreateFontA(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Heading Font Large"] = new CFont)
		->CreateFontA(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Old Sans Black"));
	(mainFonts["Code Font Large"] = new CFont)
		->CreateFontA(16, 0, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Consolas"));
	(mainFonts["Larger Font Large"] = new CFont)
		->CreateFontA(40, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Smaller Font Med"] = new CFont)
		->CreateFontA(8, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Normal Font Med"] = new CFont)
		->CreateFontA(12, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Heading Font Med"] = new CFont)
		->CreateFontA(16, 0, 0, 0, FW_DONTCARE, TRUE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Old Sans Black"));
	(mainFonts["Code Font Med"] = new CFont)
		->CreateFontA(10, 0, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Consolas"));
	(mainFonts["Larger Font Med"] = new CFont)
		->CreateFontA(22, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Smaller Font Small"] = new CFont)
		->CreateFontA(6, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Normal Font Small"] = new CFont)
		->CreateFontA(8, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	(mainFonts["Heading Font Small"] = new CFont)
		->CreateFontA(12, 0, 0, 0, FW_DONTCARE, TRUE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Old Sans Black"));
	(mainFonts["Code Font Small"] = new CFont)
		->CreateFontA(7, 0, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Consolas"));
	(mainFonts["Larger Font Small"] = new CFont)
		->CreateFontA(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
}

IMPLEMENT_DYNAMIC(MainWindow, CDialog)

BEGIN_MESSAGE_MAP(MainWindow, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_CBN_SELENDOK(IDC_SEQUENCE_COMBO, &MainWindow::handleSequenceCombo)
	ON_NOTIFY(NM_DBLCLK, IDC_SMS_TEXTING_LISTVIEW, &MainWindow::handleDblClick)
	ON_NOTIFY(NM_RCLICK, IDC_SMS_TEXTING_LISTVIEW, &MainWindow::handleRClick)
	ON_EN_CHANGE(IDC_CONFIGURATION_NOTES, &MainWindow::notifyConfigUpdate)
	ON_EN_CHANGE(IDC_REPETITION_EDIT, &MainWindow::notifyConfigUpdate)
	ON_REGISTERED_MESSAGE(eRepProgressMessageID, &MainWindow::onRepProgress)
	ON_REGISTERED_MESSAGE(eStatusTextMessageID, &MainWindow::onStatusTextMessage)
	ON_REGISTERED_MESSAGE(eNormalFinishMessageID, &MainWindow::onNormalFinishMessage)
	ON_REGISTERED_MESSAGE(eErrorTextMessageID, &MainWindow::onErrorMessage)
	ON_REGISTERED_MESSAGE(eFatalErrorMessageID, &MainWindow::onFatalErrorMessage)
	ON_REGISTERED_MESSAGE(eColoredEditMessageID, &MainWindow::onColoredEditMessage)
	ON_REGISTERED_MESSAGE(eDebugMessageID, &MainWindow::onDebugMessage)
	ON_REGISTERED_MESSAGE(eNoAtomsAlertMessageID, &MainWindow::onNoAtomsAlertMessage)
	ON_COMMAND_RANGE(ID_ACCELERATOR_ESC, ID_ACCELERATOR_ESC, &MainWindow::passCommonCommand)
	ON_COMMAND_RANGE(ID_ACCELERATOR_F5, ID_ACCELERATOR_F5, &MainWindow::passCommonCommand)
	ON_COMMAND_RANGE(ID_ACCELERATOR_F4, ID_ACCELERATOR_F4, &MainWindow::passCommonCommand)
	ON_COMMAND_RANGE(ID_ACCELERATOR_F2, ID_ACCELERATOR_F2, &MainWindow::passCommonCommand)
	ON_COMMAND_RANGE(ID_ACCELERATOR_F1, ID_ACCELERATOR_F1, &MainWindow::passCommonCommand)
	ON_COMMAND_RANGE(MENU_ID_RANGE_BEGIN, MENU_ID_RANGE_END, &MainWindow::passCommonCommand)
	ON_COMMAND_RANGE(IDC_DEBUG_OPTIONS_RANGE_BEGIN, IDC_DEBUG_OPTIONS_RANGE_END, &MainWindow::passDebugPress)
	ON_COMMAND_RANGE(IDC_MAIN_OPTIONS_RANGE_BEGIN, IDC_MAIN_OPTIONS_RANGE_END, &MainWindow::passMainOptionsPress)
	ON_COMMAND_RANGE(IDC_MAIN_STATUS_BUTTON, IDC_MAIN_STATUS_BUTTON, &MainWindow::passClear)
	ON_COMMAND_RANGE(IDC_ERROR_STATUS_BUTTON, IDC_ERROR_STATUS_BUTTON, &MainWindow::passClear)
	ON_COMMAND_RANGE(IDC_DEBUG_STATUS_BUTTON, IDC_DEBUG_STATUS_BUTTON, &MainWindow::passClear)
	ON_COMMAND(IDC_SELECT_CONFIG_COMBO, &MainWindow::passConfigPress)
	ON_COMMAND(IDOK, &MainWindow::catchEnter)
END_MESSAGE_MAP()


void MainWindow::passConfigPress()
{
	try
	{
		profile.handleSelectConfigButton(this, TheScriptingWindow, this, TheAuxiliaryWindow, TheCameraWindow);
	}
	catch (Error& err)
	{
		comm.sendError(err.what());
	}
}


//void MainWindow::passNiawgIsOnPress( )
//{
//	if ( niawg.isOn() )
//	{
//		niawg.turnOff( );
//		menu.CheckMenuItem( ID_NIAWG_NIAWGISON, MF_UNCHECKED );
//	}
//	else
//	{
//		niawg.turnOn( );
//		menu.CheckMenuItem( ID_NIAWG_NIAWGISON, MF_CHECKED );
//	}
//}

void MainWindow::passGmoogIsOnPress()
{
	if (gmoog.rearrangerActive)
	{
		gmoog.rearrangerActive = false;
		menu.CheckMenuItem(ID_GIGAMOOG_REARRANGERACTIVE, MF_UNCHECKED);
	}
	else
	{
		gmoog.rearrangerActive = true;
		menu.CheckMenuItem(ID_GIGAMOOG_REARRANGERACTIVE, MF_CHECKED);
	}
}

void MainWindow::passIdleSequenceIsOnPress()
{
	if (idler.idleSequenceActive)
	{
		idler.idleSequenceActive = false;
		menu.CheckMenuItem(ID_IDLE_SEQUENCE, MF_UNCHECKED);
	}
	else
	{
		idler.idleSequenceActive = true;
		menu.CheckMenuItem(ID_IDLE_SEQUENCE, MF_CHECKED);
	}
}

void MainWindow::passAutoAlignIsOnPress()
{
	if (gmoog.autoTweezerOffsetActive)
	{
		gmoog.autoTweezerOffsetActive = false;
		gmoog.xPixelOffsetAuto = 0;
		gmoog.yPixelOffsetAuto = 0;
		gmoog.subpixelIndexOffsetAuto = 12;
		gmoog.xOffsetAuto = NULL;
		gmoog.yOffsetAuto = NULL;

		menu.CheckMenuItem(ID_GIGAMOOG_AUTOTWEEZERALIGNACTIVE, MF_UNCHECKED);
	}
	else
	{
		gmoog.autoTweezerOffsetActive = true;
		menu.CheckMenuItem(ID_GIGAMOOG_AUTOTWEEZERALIGNACTIVE, MF_CHECKED);
	}
}


LRESULT MainWindow::onNoAtomsAlertMessage(WPARAM wp, LPARAM lp)
{
	try
	{
		if (TheCameraWindow->wantsAutoPause())
		{
			masterThreadManager.pause();
			menu.CheckMenuItem(ID_RUNMENU_PAUSE, MF_CHECKED);
			comm.sendColorBox(Master, 'Y');
		}
		auto asyncbeep = std::async(std::launch::async, [] { Beep(1000, 500); });
		time_t t = time(0);
		struct tm now;
		localtime_s(&now, &t);

		std::string message = "Experiment Stopped loading atoms at ";
		if (now.tm_hour < 10)
		{
			message += "0";
		}
		message += str(now.tm_hour) + ":";
		if (now.tm_min < 10)
		{
			message += "0";
		}
		message += str(now.tm_min) + ":";
		if (now.tm_sec < 10)
		{
			message += "0";
		}
		message += str(now.tm_sec);
		texter.sendMessage(message, &python, "Loading");
	}
	catch (Error& err)
	{
		comm.sendError(err.what());
	}
	return 0;
}


BOOL MainWindow::OnInitDialog()
{
	eMainWindowHwnd = GetSafeHwnd();
	// don't redraw until the first OnSize.
	SetRedraw(false);

	///// initialize niawg.
	//try
	//{
	//	niawg.initialize( );
	//}
	//catch ( Error& except )
	//{
	//	errBox( "ERROR: NIAWG failed to start! Error: " + except.whatStr( ) );
	//	return -1;
	//}

	//try
	//{
	//	niawg.setDefaultWaveforms( this );
	//	// but the default starts in the horizontal configuration, so switch back and start in this config.
	//	restartNiawgDefaults( );
	//}
	//catch ( Error& exception )
	//{
	//	errBox( "ERROR: failed to start niawg default waveforms! Niawg gave the following error message: " + exception.whatStr( ) );
	//}
	// not done with the script, it will not stay on the NIAWG, so I need to keep track of it so thatI can reload it onto the NIAWG when necessary.	
	/// Initialize Windows
	std::string which = "";
	try
	{
		which = "Scripting";
		TheScriptingWindow = new ScriptingWindow;
		which = "Camera";
		TheCameraWindow = new CameraWindow;
		which = "Auxiliary";
		TheAuxiliaryWindow = new AuxiliaryWindow;
	}
	catch (Error& err)
	{
		errBox("FATAL ERROR: " + which + " Window constructor failed! Error: " + err.what());
		return -1;
	}

	TheScriptingWindow->loadFriends(this, TheCameraWindow, TheAuxiliaryWindow);
	TheCameraWindow->loadFriends(this, TheScriptingWindow, TheAuxiliaryWindow);
	TheAuxiliaryWindow->loadFriends(this, TheScriptingWindow, TheCameraWindow);

	try
	{
		// these each call oninitdialog after the create call. Hence the try / catch.
		TheScriptingWindow->Create(IDD_LARGE_TEMPLATE, GetDesktopWindow());
		TheCameraWindow->Create(IDD_LARGE_TEMPLATE, GetDesktopWindow());
		TheAuxiliaryWindow->Create(IDD_LARGE_TEMPLATE, GetDesktopWindow());
	}
	catch (Error& err)
	{
		errBox(err.what());
	}

	/// initialize main window controls.
	comm.initialize(this, TheScriptingWindow, TheCameraWindow, TheAuxiliaryWindow);
	int id = 1000;
	POINT controlLocation = { 0,0 };
	mainStatus.initialize(controlLocation, this, id, 975, "EXPERIMENT STATUS", mainRGBs["theme blue"], tooltips, IDC_MAIN_STATUS_BUTTON);
	controlLocation = { 480, 0 };
	errorStatus.initialize(controlLocation, this, id, 480, "ERROR STATUS", mainRGBs["theme red"], tooltips, IDC_ERROR_STATUS_BUTTON);
	debugStatus.initialize(controlLocation, this, id, 480, "DEBUG STATUS", mainRGBs["theme aqua"], tooltips, IDC_DEBUG_STATUS_BUTTON);
	controlLocation = { 960, 0 };
	profile.initialize(controlLocation, this, id, tooltips);
	controlLocation = { 960, 175 };
	notes.initialize(controlLocation, this, id, tooltips);
	controlLocation = { 1440, 50 };
	repetitionControl.initialize(controlLocation, tooltips, this, id);
	settings.initialize(id, controlLocation, this, tooltips);
	//rearrangeControl.initialize( id, controlLocation, this, tooltips );
	debugger.initialize(id, controlLocation, this, tooltips);
	texter.initialize(controlLocation, this, id, tooltips, mainRGBs);
	controlLocation = { 960, 910 };
	boxes.initialize(controlLocation, id, this, 960, tooltips);
	shortStatus.initialize(controlLocation, this, id, tooltips);
	menu.LoadMenu(IDR_MAIN_MENU);
	SetMenu(&menu);
	// just initializes the rectangles.
	TheCameraWindow->redrawPictures(true);
	try
	{
		masterConfig.load(this, TheAuxiliaryWindow, TheCameraWindow);
	}
	catch (Error& err)
	{
		errBox(err.what());
	}


	ShowWindow(SW_MAXIMIZE);
	TheCameraWindow->ShowWindow(SW_MAXIMIZE);
	TheScriptingWindow->ShowWindow(SW_MAXIMIZE);
	TheAuxiliaryWindow->ShowWindow(SW_MAXIMIZE);
	std::vector<CDialog*> windows = { TheAuxiliaryWindow, TheScriptingWindow, this, TheCameraWindow };
	EnumDisplayMonitors(NULL, NULL, monitorHandlingProc, reinterpret_cast<LPARAM>(&windows));
	// hide the splash just before the first window requiring input pops up.
	appSplash->ShowWindow(SW_HIDE);

	/// summarize system status.
	std::string initializationString;
	try
	{
		initializationString += getSystemStatusString();
		initializationString += TheAuxiliaryWindow->getSystemStatusMsg();
		initializationString += TheCameraWindow->getSystemStatusString();
		initializationString += TheScriptingWindow->getSystemStatusString();
		infoBox(initializationString);
	}
	catch (Error& err)
	{
		errBox(err.what());
	}

	int tcp_connect;
	try
	{
		tcp_connect = zynq_tcp.connectTCP(ZYNQ_ADDRESS);
	}
	catch (Error& err)
	{
		tcp_connect = 1;
		errBox(err.what());
	}
	if (tcp_connect == 0) {
		/*char buffer[80];
		sprintf_s(buffer, 80, "Connected to Zynq server. Sent %u Bytes.\n", tcp_connect);
		setShortStatus(buffer);*/
		setShortStatus("Connected to Zynq server. Disconnecting now.");
		zynq_tcp.disconnect();
	}
	else
	{
		setShortStatus("Connection to Zynq server failed.\n");
	}

	updateConfigurationSavedStatus(true);
	return TRUE;
}


// just notifies the profile object that the configuration is no longer saved.
void MainWindow::notifyConfigUpdate()
{
	profile.updateConfigurationSavedStatus(false);
}


void MainWindow::catchEnter()
{
	// the default handling is to close the window, so I need to catch it.
}


BOOL CALLBACK MainWindow::monitorHandlingProc(_In_ HMONITOR hMonitor, _In_ HDC hdcMonitor,
	_In_ LPRECT lprcMonitor, _In_ LPARAM dwData)
{
	static UINT count = 0;
	std::vector<CDialog*>* windows = reinterpret_cast<std::vector<CDialog*>*>(dwData);
	//if ( count == 2 )
	//{
	//	// skip the high monitor.
	//	count++;
	//	return TRUE;
	//}
	if (count < 5)
	{
		windows->at(count)->MoveWindow(lprcMonitor);
	}
	count++;
	return TRUE;
}

void MainWindow::handlePause()
{
	if (masterThreadManager.runningStatus())
	{
		if (masterThreadManager.getIsPaused())
		{
			// then it's currently paused, so unpause it.
			menu.CheckMenuItem(ID_RUNMENU_PAUSE, MF_UNCHECKED);
			masterThreadManager.unPause();
			comm.sendColorBox(Master, 'G');
		}
		else
		{
			// then not paused so pause it.
			menu.CheckMenuItem(ID_RUNMENU_PAUSE, MF_CHECKED);
			comm.sendColorBox(Master, 'Y');
			masterThreadManager.pause();
		}
	}
	else
	{
		comm.sendStatus("Can't pause, experiment was not running.\r\n");
	}
}


LRESULT MainWindow::onRepProgress(WPARAM wParam, LPARAM lParam)
{
	repetitionControl.updateNumber(lParam);
	return NULL;
}


void MainWindow::handleNewConfig(std::ofstream& newFile)
{
	notes.handleNewConfig(newFile);
	settings.handleNewConfig(newFile);
	debugger.handleNewConfig(newFile);
	repetitionControl.handleNewConfig(newFile);
	//rearrangeControl.handleNewConfig( newFile );
}


void MainWindow::handleSaveConfig(std::ofstream& saveFile)
{
	notes.handleSaveConfig(saveFile);
	settings.handleSaveConfig(saveFile);
	debugger.handleSaveConfig(saveFile);
	repetitionControl.handleSaveConfig(saveFile);
	//rearrangeControl.handleSaveConfig( saveFile );
}


void MainWindow::handleOpeningConfig(std::ifstream& configFile, int versionMajor, int versionMinor)
{
	notes.handleOpenConfig(configFile, versionMajor, versionMinor);
	settings.handleOpenConfig(configFile, versionMajor, versionMinor);
	debugger.handleOpenConfig(configFile, versionMajor, versionMinor);
	repetitionControl.handleOpenConfig(configFile, versionMajor, versionMinor);
	//rearrangeControl.handleOpenConfig( configFile, versionMajor, versionMinor );
}


void MainWindow::OnSize(UINT nType, int cx, int cy)
{
	SetRedraw(false);
	profile.rearrange(cx, cy, getFonts());
	notes.rearrange(cx, cy, getFonts());
	debugger.rearrange(cx, cy, getFonts());
	settings.rearrange(cx, cy, getFonts());
	mainStatus.rearrange(cx, cy, getFonts());
	debugStatus.rearrange(cx, cy, getFonts());
	errorStatus.rearrange(cx, cy, getFonts());
	texter.rearrange(cx, cy, getFonts());
	shortStatus.rearrange(cx, cy, getFonts());
	boxes.rearrange(cx, cy, getFonts());
	repetitionControl.rearrange(cx, cy, getFonts());
	//rearrangeControl.rearrange( cx, cy, getFonts( ) );
	SetRedraw();
	RedrawWindow();
}


//void MainWindow::setNiawgRunningState( bool newRunningState )
//{
//	niawg.setRunningState( newRunningState );
//}
//
//
//bool MainWindow::niawgIsRunning()
//{
//	return niawg.niawgIsRunning();
//}



BOOL MainWindow::PreTranslateMessage(MSG* pMsg)
{
	for (UINT toolTipInc = 0; toolTipInc < tooltips.size(); toolTipInc++)
	{
		tooltips[toolTipInc]->RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}


//void MainWindow::setNiawgDefaults()
//{
//	niawg.setDefaultWaveforms(this);
//}


fontMap MainWindow::getFonts()
{
	return mainFonts;
}


void MainWindow::passClear(UINT id)
{
	if (id == IDC_MAIN_STATUS_BUTTON)
	{
		mainStatus.clear();
	}
	else if (id == IDC_ERROR_STATUS_BUTTON)
	{
		errorStatus.clear();
	}
	else if (id == IDC_DEBUG_STATUS_BUTTON)
	{
		debugStatus.clear();
	}
}


void MainWindow::OnCancel()
{
	passCommonCommand(ID_FILE_MY_EXIT);
}


void MainWindow::OnClose()
{
	passCommonCommand(WM_CLOSE);
}


//void MainWindow::stopNiawg()
//{
//	niawg.turnOff();
//}


UINT MainWindow::getRepNumber()
{
	return repetitionControl.getRepetitionNumber();
}



std::string MainWindow::getSystemStatusString()
{
	std::string status;
	status = ">>> NIAWG <<<\n";
	if (!NIAWG_SAFEMODE)
	{
		status += "Code System is Active!\n";
		//status += niawg.fgenConduit.getDeviceInfo();
	}
	else
	{
		status += "Code System is disabled! Enable in \"constants.h\"\n";
	}
	return status;
}


bool MainWindow::masterIsRunning()
{
	return masterThreadManager.runningStatus();
}


RunInfo MainWindow::getRunInfo()
{
	return systemRunningInfo;
}


//void MainWindow::restartNiawgDefaults()
//{
//	niawg.restartDefault();
//}

HBRUSH MainWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
	{
		int num = pWnd->GetDlgCtrlID();
		CBrush* ret = shortStatus.handleColor(pWnd, pDC, mainRGBs, mainBrushes);
		if (ret)
		{
			return *ret;
		}
		ret = boxes.handleColoring(num, pDC, mainBrushes, mainRGBs);
		if (ret)
		{
			return *ret;
		}
		else
		{
			pDC->SetTextColor(mainRGBs["theme foreground"]);
			pDC->SetBkColor(mainRGBs["theme BG1"]);
			return *mainBrushes["theme BG1"];
		}
	}
	case CTLCOLOR_EDIT:
	{
		pDC->SetTextColor(mainRGBs["theme blue"]);
		pDC->SetBkColor(mainRGBs["theme input"]);
		return *mainBrushes["theme input"];
	}
	case CTLCOLOR_LISTBOX:
	{
		pDC->SetTextColor(mainRGBs["theme foreground"]);
		pDC->SetBkColor(mainRGBs["theme input"]);
		return *mainBrushes["theme input"];
	}
	default:
	{
		return *mainBrushes["theme BG1"];
	}
	}
	return NULL;
}


void MainWindow::passCommonCommand(UINT id)
{
	// pass the command id to the common function, filling in the pointers to the windows which own objects needed.
	try
	{
		commonFunctions::handleCommonMessage(id, this, this, TheScriptingWindow, TheCameraWindow, TheAuxiliaryWindow);
	}
	catch (Error& exception)
	{
		errBox(exception.what());
	}
}


void MainWindow::startMaster(MasterThreadInput* input, bool isTurnOnMot)
{
	masterThreadManager.startExperimentThread(input);
}


void MainWindow::fillMotInput(MasterThreadInput* input)
{
	input->comm = &comm;
	VariableSystem::generateKey(input->variables, input->settings.randomizeVariations);
	for (auto& variable : input->variables)
	{
		if (variable.constant)
		{
			input->constants.push_back(variable);
		}
	}
	// the mot procedure doesn't need the NIAWG at all.
	input->runNiawg = false;
	input->skipNext = NULL;
	//input->rearrangeInfo = rearrangeControl.getParams( );
	//input->rearrangeInfo.active = false;

}


void MainWindow::fillMasterThreadInput(MasterThreadInput* input)
{
	input->python = &this->python;
	input->masterScriptAddress = profile.getMasterAddressFromConfig();
	input->ddsScriptAddress = profile.getDdsAddressFromConfig();
	input->awgScriptAddress = profile.getAWGAddressFromConfig();
	input->gmoogScriptAddress = profile.getGmoogAddressFromConfig();
	input->settings = settings.getOptions();
	input->repetitionNumber = getRepNumber();
	input->debugOptions = debugger.getOptions();
	input->profile = profile.getProfileSettings();

	input->comm = &comm;
	input->awg0 = &awg0;
	input->awg1 = &awg1;
	input->gmoog = &gmoog;
	input->dds = &dds;
	input->idler = &idler;

	VariableSystem::generateKey(input->variables, input->settings.randomizeVariations);
	// it's important to do this after the key is generated so that the constants have their values.
	for (auto& variable : input->variables)
	{
		if (variable.constant)
		{
			input->constants.push_back(variable);
		}
	}
	//input->rearrangeInfo = rearrangeControl.getParams( );
}


void MainWindow::logParams(DataLogger* logger, MasterThreadInput* input)
{
	logger->logMasterParameters(input);
}


profileSettings MainWindow::getProfileSettings()
{
	return profile.getProfileSettings();
}


void MainWindow::checkProfileReady()
{
	profile.allSettingsReadyCheck(TheScriptingWindow, this, TheAuxiliaryWindow, TheCameraWindow);
}


void MainWindow::checkProfileSave()
{
	profile.checkSaveEntireProfile(TheScriptingWindow, this, TheAuxiliaryWindow, TheCameraWindow);
}


void MainWindow::updateConfigurationSavedStatus(bool status)
{
	profile.updateConfigurationSavedStatus(status);
}


std::string MainWindow::getNotes()
{
	return notes.getConfigurationNotes();
}


void MainWindow::setNotes(std::string newNotes)
{
	notes.setConfigurationNotes(newNotes);
}


debugInfo MainWindow::getDebuggingOptions()
{
	return debugger.getOptions();
}


void MainWindow::setDebuggingOptions(debugInfo options)
{
	debugger.setOptions(options);
}


mainOptions MainWindow::getMainOptions()
{
	return settings.getOptions();
}


void MainWindow::updateStatusText(std::string whichStatus, std::string text)
{
	std::transform(whichStatus.begin(), whichStatus.end(), whichStatus.begin(), ::tolower);
	if (whichStatus == "error")
	{
		errorStatus.addStatusText(text);
	}
	else if (whichStatus == "debug")
	{
		debugStatus.addStatusText(text);
	}
	else if (whichStatus == "main")
	{
		mainStatus.addStatusText(text);
	}
	else
	{
		thrower("Main Window's updateStatusText function recieved a bad argument for which status"
			" control to update. Options are \"error\", \"debug\", and \"main\", but recieved " + whichStatus);
	}
}


void MainWindow::addTimebar(std::string whichStatus)
{
	std::transform(whichStatus.begin(), whichStatus.end(), whichStatus.begin(), ::tolower);
	if (whichStatus == "error")
	{
		errorStatus.appendTimebar();
	}
	else if (whichStatus == "debug")
	{
		debugStatus.appendTimebar();
	}
	else if (whichStatus == "main")
	{
		mainStatus.appendTimebar();
	}
	else
	{
		thrower("Main Window's addTimebar function recieved a bad argument for which status"
			" control to update. Options are \"error\", \"debug\", and \"main\", but recieved " + whichStatus + ". This"
			"exception can be safely ignored.");
	}
}


void MainWindow::setShortStatus(std::string text)
{
	shortStatus.setText(text);
}


void MainWindow::changeShortStatusColor(std::string color)
{
	shortStatus.setColor(color);
}


void MainWindow::passDebugPress(UINT id)
{
	profile.updateConfigurationSavedStatus(false);
}


void MainWindow::passMainOptionsPress(UINT id)
{
	profile.updateConfigurationSavedStatus(false);
}


void MainWindow::handleDblClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	texter.updatePersonInfo();
	profile.updateConfigurationSavedStatus(false);
}


void MainWindow::handleRClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	texter.deletePersonInfo();
	profile.updateConfigurationSavedStatus(false);
}


void MainWindow::handleSequenceCombo()
{
	try
	{
		profile.sequenceChangeHandler();
	}
	catch (Error& err)
	{
		getComm()->sendError(err.what());
	}
}


void MainWindow::changeBoxColor(systemInfo<char> colors)
{
	boxes.changeColor(colors);
	if (colors.camera == 'R' || colors.intensity == 'R' || colors.niawg == 'R')
	{
		changeShortStatusColor("R");
	}
	else if (colors.camera == 'Y' || colors.intensity == 'Y' || colors.niawg == 'Y')
	{
		changeShortStatusColor("Y");
	}
	else if (colors.camera == 'G' || colors.intensity == 'G' || colors.niawg == 'G')
	{
		changeShortStatusColor("G");
	}
	else
	{
		changeShortStatusColor("B");
	}
}


void MainWindow::abortMasterThread()
{
	if (masterThreadManager.runningStatus())
	{
		masterThreadManager.abort();
	}
	else
	{
		thrower("Can't abort, experiment was not running.\r\n");
	}
}

void MainWindow::abortIdlerThread()
{
	idler.killIdler = true;
	if (masterThreadManager.idlerStatus())
	{
		idler.killIdler = true;
		masterThreadManager.abortIdler();
	}
}

bool MainWindow::experimentIsPaused()
{
	return masterThreadManager.getIsPaused();
}



LRESULT MainWindow::onStatusTextMessage(WPARAM wParam, LPARAM lParam)
{
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	mainStatus.addStatusText(statusMessage);
	return 0;
}


LRESULT MainWindow::onErrorMessage(WPARAM wParam, LPARAM lParam)
{
	// for simple warnings, it just posts the message.
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	if (statusMessage != "")
	{
		errorStatus.addStatusText(statusMessage);
		Beep(550, 300);
	}
	return 0;
}


LRESULT MainWindow::onFatalErrorMessage(WPARAM wParam, LPARAM lParam)
{
	// normal msg stuff
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	errorStatus.addStatusText(statusMessage);
	// resseting things.
	//TheScriptingWindow->setIntensityDefault();
	std::string msgText = "Exited with Error!";
	changeShortStatusColor("R");
	//comm.sendColorBox( Niawg, 'R' );
	try
	{
		//niawg.restartDefault();
		comm.sendError("EXITED WITH ERROR!");
		//comm.sendColorBox( Niawg, 'R' );
		comm.sendStatus("EXITED WITH ERROR!");
	}
	catch (Error& except)
	{
		comm.sendError("EXITED WITH ERROR! " + except.whatStr());
		//comm.sendColorBox( Niawg, 'R' );
		//comm.sendStatus("EXITED WITH ERROR!\r\nNIAWG RESTART FAILED!\r\n");
	}
	//setNiawgRunningState( false );
	Beep(850, 50);
	Sleep(50);
	Beep(830, 50);
	return 0;
}

void MainWindow::stopRearranger()
{
	//niawg.turnOffRerng( );
}

void MainWindow::waitForRearranger()
{
	//niawg.waitForRerng( );
}


LRESULT MainWindow::onNormalFinishMessage(WPARAM wParam, LPARAM lParam)
{
	//TheScriptingWindow->setIntensityDefault();
	std::string msgText = "Passively Outputting Default Waveform";
	setShortStatus(msgText);
	changeShortStatusColor("B");
	stopRearranger();
	TheCameraWindow->wakeRearranger();

	try
	{
		waitForRearranger();
	}
	catch (Error& err)
	{
		comm.sendError(err.what());
	}
	return 0;
}


void MainWindow::handleFinish()
{
	time_t t = time(0);
	struct tm now;
	localtime_s(&now, &t);
	std::string message = "Experiment Completed at ";
	if (now.tm_hour < 10)
	{
		message += "0";
	}
	message += str(now.tm_hour) + ":";
	if (now.tm_min < 10)
	{
		message += "0";
	}
	message += str(now.tm_min) + ":";
	if (now.tm_sec < 10)
	{
		message += "0";
	}
	message += str(now.tm_sec);

	try
	{
		texter.sendMessage(message, &python, "Finished");
	}
	catch (Error& err)
	{
		comm.sendError(err.what());
	}
}


Communicator* MainWindow::getComm()
{
	return &comm;
}


LRESULT MainWindow::onColoredEditMessage(WPARAM wParam, LPARAM lParam)
{
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	setShortStatus(statusMessage);
	return 0;
}


LRESULT MainWindow::onDebugMessage(WPARAM wParam, LPARAM lParam)
{
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	debugStatus.addStatusText(statusMessage);
	return 0;
}


brushMap MainWindow::getBrushes()
{
	return mainBrushes;
}


rgbMap MainWindow::getRgbs()
{
	return mainRGBs;
}