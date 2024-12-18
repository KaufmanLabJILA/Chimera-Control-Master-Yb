#include "stdafx.h" 
#include "ScriptingWindow.h" 
#include "afxwin.h" 
#include "MainWindow.h" 
#include "openWithExplorer.h" 
#include "saveWithExplorer.h" 
#include "commonFunctions.h" 
#include "textPromptDialog.h" 
#include "AuxiliaryWindow.h" 


ScriptingWindow::ScriptingWindow() : CDialog()
{}


IMPLEMENT_DYNAMIC(ScriptingWindow, CDialog)

BEGIN_MESSAGE_MAP(ScriptingWindow, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_SIZE()

	//ON_EN_CHANGE(IDC_MOOG_EDIT, &ScriptingWindow::moogEditChange)
	ON_EN_CHANGE(IDC_AWG_EDIT, &ScriptingWindow::awgEditChange)
	ON_EN_CHANGE(IDC_GMOOG_EDIT, &ScriptingWindow::gmoogEditChange)
	ON_EN_CHANGE(IDC_MASTER_EDIT, &ScriptingWindow::masterEditChange)
	ON_EN_CHANGE(IDC_DDS_EDIT, &ScriptingWindow::ddsEditChange)

	ON_COMMAND(IDOK, &ScriptingWindow::catchEnter)

	ON_COMMAND_RANGE(MENU_ID_RANGE_BEGIN, MENU_ID_RANGE_END, &ScriptingWindow::passCommonCommand)

	//ON_CBN_SELENDOK(IDC_MOOG_FUNCTION_COMBO, &ScriptingWindow::handleMoogScriptComboChange)
	ON_CBN_SELENDOK(IDC_AWG_FUNCTION_COMBO, &ScriptingWindow::handleAWGScriptComboChange)
	ON_CBN_SELENDOK(IDC_GMOOG_FUNCTION_COMBO, &ScriptingWindow::handleGmoogScriptComboChange)
	ON_CBN_SELENDOK(IDC_MASTER_FUNCTION_COMBO, &ScriptingWindow::handleMasterFunctionChange)

	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, ScriptingWindow::OnToolTipText)
END_MESSAGE_MAP()


void ScriptingWindow::handleMasterFunctionChange()
{
	try
	{
		masterScript.functionChangeHandler(mainWindowFriend->getProfileSettings().categoryPath);
		masterScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
		masterScript.updateSavedStatus(true);
	}
	catch (Error& err)
	{
		errBox(err.what());
	}
}


void ScriptingWindow::masterEditChange()
{
	try
	{
		masterScript.handleEditChange();
		SetTimer(SYNTAX_TIMER_ID, SYNTAX_TIMER_LENGTH, NULL);
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}
}


Communicator* ScriptingWindow::comm()
{
	return mainWindowFriend->getComm();
}

void ScriptingWindow::catchEnter()
{
	errBox("Secret Message!");
}


void ScriptingWindow::OnSize(UINT nType, int cx, int cy)
{
	SetRedraw(false);

	awgScript.rearrange(cx, cy, mainWindowFriend->getFonts());
	gmoogScript.rearrange(cx, cy, mainWindowFriend->getFonts());
	ddsScript.rearrange(cx, cy, mainWindowFriend->getFonts());
	masterScript.rearrange(cx, cy, mainWindowFriend->getFonts());
	statusBox.rearrange(cx, cy, mainWindowFriend->getFonts());
	profileDisplay.rearrange(cx, cy, mainWindowFriend->getFonts());

	awgScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	gmoogScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	ddsScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	masterScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());

	SetRedraw(true);
	RedrawWindow();
}

// special handling for long tooltips. 
BOOL ScriptingWindow::OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	try
	{
		awgScript.handleToolTip(pNMHDR, pResult);
		gmoogScript.handleToolTip(pNMHDR, pResult);
		ddsScript.handleToolTip(pNMHDR, pResult);
		masterScript.handleToolTip(pNMHDR, pResult);
	}
	catch (Error&)
	{
		// worked. 
		return TRUE;
	}
	return FALSE;
}


BOOL ScriptingWindow::PreTranslateMessage(MSG* pMsg)
{
	for (UINT toolTipInc = 0; toolTipInc < tooltips.size(); toolTipInc++)
	{
		tooltips[toolTipInc]->RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void ScriptingWindow::handleAWGScriptComboChange()
{
	//verticalNiawgScript.childComboChangeHandler( mainWindowFriend, auxWindowFriend );
}

void ScriptingWindow::handleGmoogScriptComboChange()
{
	//verticalNiawgScript.childComboChangeHandler( mainWindowFriend, auxWindowFriend ); 
}

void ScriptingWindow::handleAgilentScriptComboChange()
{
	//intensityAgilent.agilentScript.childComboChangeHandler( mainWindowFriend, auxWindowFriend); 
}


// this gets called when closing. The purpose here is to redirect the default, very abrupt close that would normally happen. 
void ScriptingWindow::OnCancel()
{
	passCommonCommand(ID_FILE_MY_EXIT);
}


BOOL ScriptingWindow::OnInitDialog()
{
	EnableToolTips(TRUE);
	// don't redraw until the first OnSize. 
	SetRedraw(false);

	int id = 2000;

	POINT startLocation = { 0, 28 };
	awgScript.initialize(480, 900, startLocation, tooltips, this, id, "AWG",
		"AWG 1 (Subharmonicon) Script",
		{ IDC_AWG_FUNCTION_COMBO, IDC_AWG_EDIT },
		mainWindowFriend->getRgbs()["theme BG2"]);

	startLocation = { 480, 28 };
	gmoogScript.initialize(480, 900, startLocation, tooltips, this, id, "Gmoog",
		"Gigamoog 1 Script",
		{ IDC_GMOOG_FUNCTION_COMBO, IDC_GMOOG_EDIT },
		mainWindowFriend->getRgbs()["theme BG1"]);

	startLocation = { 960, 28 };
	ddsScript.initialize(480, 900, startLocation, tooltips, this, id, "DDS",
		"DDS Script", { IDC_DDS_FUNCTION_COMBO, IDC_DDS_EDIT }, mainWindowFriend->getRgbs()["theme BG2"]);

	startLocation = { 1440, 28 };
	masterScript.initialize(480, 900, startLocation, tooltips, this, id, "Master", "Master Script",
		{ IDC_MASTER_FUNCTION_COMBO, IDC_MASTER_EDIT }, mainWindowFriend->getRgbs()["theme BG1"]);

	startLocation = { 1700, 3 };
	statusBox.initialize(startLocation, id, this, 300, tooltips);
	profileDisplay.initialize({ 0,3 }, id, this, tooltips);

	CMenu menu;
	menu.LoadMenu(IDR_MAIN_MENU);
	SetMenu(&menu);
	SetRedraw(true);
	return TRUE;
}


void ScriptingWindow::fillMasterThreadInput(MasterThreadInput* input)
{
	//input->agilents.push_back( &intensityAgilent ); 
	//input->intensityAgilentNumber = input->agilents.size() - 1; 
}


void ScriptingWindow::OnTimer(UINT_PTR eventID)
{
	//moogScript.handleTimerCall(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
	//	auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	awgScript.handleTimerCall(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	gmoogScript.handleTimerCall(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	ddsScript.handleTimerCall(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	masterScript.handleTimerCall(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
}


void ScriptingWindow::checkScriptSaves()
{
	//moogScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	awgScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	gmoogScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	ddsScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	masterScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
}


std::string ScriptingWindow::getSystemStatusString()
{
	return "\nIntensity Agilent: N/A"; /*intensityAgilent.getDeviceIdentity();*/
}


void ScriptingWindow::loadFriends(MainWindow* mainWin, CameraWindow* camWin, AuxiliaryWindow* masterWin)
{
	mainWindowFriend = mainWin;
	cameraWindowFriend = camWin;
	auxWindowFriend = masterWin;
}


/*
	This function retuns the names (just the names) of currently active scripts.
*/
scriptInfo<std::string> ScriptingWindow::getScriptNames()
{
	scriptInfo<std::string> names;
	// Order matters! 
	//TODO: Ask why 
	//names.moog = moogScript.getScriptName();
	names.awg = awgScript.getScriptName();
	names.gmoog = gmoogScript.getScriptName();
	names.DDS = ddsScript.getScriptName();
	return names;
}

/*
	This function returns indicators of whether a given script has been saved or not.
*/
scriptInfo<bool> ScriptingWindow::getScriptSavedStatuses()
{
	scriptInfo<bool> status;

	status.awg = awgScript.savedStatus();
	status.gmoog = gmoogScript.savedStatus();
	status.DDS = ddsScript.savedStatus();

	return status;
}

/*
	This function returns the current addresses of all files in all scripts.
*/
scriptInfo<std::string> ScriptingWindow::getScriptAddresses()
{
	scriptInfo<std::string> addresses;
	//addresses.moog = moogScript.getScriptPathAndName();
	addresses.awg = awgScript.getScriptPathAndName();
	addresses.gmoog = gmoogScript.getScriptPathAndName();
	addresses.DDS = ddsScript.getScriptPathAndName();
	addresses.master = masterScript.getScriptPathAndName();

	return addresses;
}

/*
	This function handles the coloring of all controls on this window.
*/
HBRUSH ScriptingWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	brushMap brushes = mainWindowFriend->getBrushes();
	rgbMap rgbs = mainWindowFriend->getRgbs();
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
	{
		int num = (pWnd->GetDlgCtrlID());
		CBrush* result = statusBox.handleColoring(num, pDC, brushes, rgbs);
		if (result)
		{
			return *result;
		}
		else
		{
			pDC->SetTextColor(rgbs["theme foreground"]);
			pDC->SetBkColor(rgbs["theme BG0"]);
			return *brushes["theme BG0"];
		}
	}
	case CTLCOLOR_LISTBOX:
	{
		pDC->SetTextColor(rgbs["theme orange"]);
		pDC->SetBkColor(rgbs["theme selection"]);
		return *brushes["theme selection"];
	}
	default:
	{
		return *brushes["theme BG1"];
	}
	}
} 

void ScriptingWindow::awgEditChange()
{
	awgScript.handleEditChange();
	SetTimer(SYNTAX_TIMER_ID, SYNTAX_TIMER_LENGTH, NULL);
}

void ScriptingWindow::gmoogEditChange()
{
	gmoogScript.handleEditChange();
	SetTimer(SYNTAX_TIMER_ID, SYNTAX_TIMER_LENGTH, NULL);
}

void ScriptingWindow::ddsEditChange()
{
	ddsScript.handleEditChange();
	SetTimer(SYNTAX_TIMER_ID, SYNTAX_TIMER_LENGTH, NULL);
}

/// Commonly Called Functions 
/*
	The following set of functions, mostly revolving around saving etc. of the script files, are called by all of the
	window objects because they are associated with the menu at the top of each screen
*/
///  

void ScriptingWindow::newAWGScript()
{
	try
	{
		awgScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		awgScript.newScript();
		updateConfigurationSavedStatus(false);
		awgScript.updateScriptNameText(getProfile().categoryPath);
		awgScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}



// just a quick shortcut. 
profileSettings ScriptingWindow::getProfile()
{
	return mainWindowFriend->getProfileSettings();
}


void ScriptingWindow::openAWGScript(CWnd* parent)
{
	try
	{
		awgScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		std::string moogOpenName = openWithExplorer(parent, AWG_SCRIPT_EXTENSION);
		awgScript.openParentScript(moogOpenName, getProfile().categoryPath,
			mainWindowFriend->getRunInfo());
		updateConfigurationSavedStatus(false);
		awgScript.updateScriptNameText(getProfile().categoryPath);
		awgScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),//
			auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames()); //
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}


void ScriptingWindow::saveAWGScript()
{
	try
	{
		awgScript.saveScript(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		awgScript.updateScriptNameText(getProfile().categoryPath);
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}


void ScriptingWindow::saveAWGScriptAs(CWnd* parent)
{
	try
	{
		std::string extensionNoPeriod = awgScript.getExtension();
		if (extensionNoPeriod.size() == 0)
		{
			return;
		}
		extensionNoPeriod = extensionNoPeriod.substr(1, extensionNoPeriod.size());
		std::string newScriptAddress = saveWithExplorer(parent, extensionNoPeriod, getProfileSettings());
		awgScript.saveScriptAs(newScriptAddress, mainWindowFriend->getRunInfo());
		updateConfigurationSavedStatus(false);
		awgScript.updateScriptNameText(getProfile().categoryPath);
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}

void ScriptingWindow::newGmoogScript()
{
	try
	{
		gmoogScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		gmoogScript.newScript();
		updateConfigurationSavedStatus(false);
		gmoogScript.updateScriptNameText(getProfile().categoryPath);
		gmoogScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}

void ScriptingWindow::openGmoogScript(CWnd* parent)
{
	try
	{
		gmoogScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		std::string moogOpenName = openWithExplorer(parent, GIGAMOOG_SCRIPT_EXTENSION);
		gmoogScript.openParentScript(moogOpenName, getProfile().categoryPath,
			mainWindowFriend->getRunInfo());
		updateConfigurationSavedStatus(false);
		gmoogScript.updateScriptNameText(getProfile().categoryPath);
		gmoogScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}


void ScriptingWindow::saveGmoogScript()
{
	try
	{
		gmoogScript.saveScript(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		gmoogScript.updateScriptNameText(getProfile().categoryPath);
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}


void ScriptingWindow::saveGmoogScriptAs(CWnd* parent)
{
	try
	{
		std::string extensionNoPeriod = gmoogScript.getExtension();
		if (extensionNoPeriod.size() == 0)
		{
			return;
		}
		extensionNoPeriod = extensionNoPeriod.substr(1, extensionNoPeriod.size());
		std::string newScriptAddress = saveWithExplorer(parent, extensionNoPeriod, getProfileSettings());
		gmoogScript.saveScriptAs(newScriptAddress, mainWindowFriend->getRunInfo());
		updateConfigurationSavedStatus(false);
		gmoogScript.updateScriptNameText(getProfile().categoryPath);
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}

void ScriptingWindow::newDdsScript()
{
	try
	{
		ddsScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		ddsScript.newScript();
		updateConfigurationSavedStatus(false);
		ddsScript.updateScriptNameText(getProfile().categoryPath);
		ddsScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}

void ScriptingWindow::openDdsScript(CWnd* parent)
{
	try
	{
		ddsScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		std::string ddsOpenName = openWithExplorer(parent, DDS_SCRIPT_EXTENSION);
		ddsScript.openParentScript(ddsOpenName, getProfile().categoryPath,
			mainWindowFriend->getRunInfo());
		updateConfigurationSavedStatus(false);
		ddsScript.updateScriptNameText(getProfile().categoryPath);
		ddsScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}


void ScriptingWindow::saveDdsScript()
{
	try
	{
		ddsScript.saveScript(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		ddsScript.updateScriptNameText(getProfile().categoryPath);
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}


void ScriptingWindow::saveDdsScriptAs(CWnd* parent)
{
	try
	{
		std::string extensionNoPeriod = ddsScript.getExtension();
		if (extensionNoPeriod.size() == 0)
		{
			return;
		}
		extensionNoPeriod = extensionNoPeriod.substr(1, extensionNoPeriod.size());
		std::string newScriptAddress = saveWithExplorer(parent, extensionNoPeriod, getProfileSettings());
		ddsScript.saveScriptAs(newScriptAddress, mainWindowFriend->getRunInfo());
		updateConfigurationSavedStatus(false);
		ddsScript.updateScriptNameText(getProfile().categoryPath);
	}
	catch (Error& err)
	{
		comm()->sendError(err.what());
	}

}


void ScriptingWindow::updateScriptNamesOnScreen()
{; 
	//moogScript.updateScriptNameText(getProfile().categoryPath);
	awgScript.updateScriptNameText(getProfile().categoryPath);
	gmoogScript.updateScriptNameText(getProfile().categoryPath);
	ddsScript.updateScriptNameText(getProfile().categoryPath);
}


void ScriptingWindow::recolorScripts()
{
	//moogScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
	//	auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	awgScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	gmoogScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	ddsScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	masterScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
}

void ScriptingWindow::handleOpenConfig(std::ifstream& configFile, int versionMajor, int versionMinor)
{
	ProfileSystem::checkDelimiterLine(configFile, "SCRIPTS");
	// the reading for the scripts is simple enough at the moment that I just read everything here. 
	configFile.get();
	std::string /*moogName,*/ awgName, gmoogName, ddsName, masterName;
	//getline(configFile, moogName);
	getline(configFile, awgName);
	getline(configFile, gmoogName);
	getline(configFile, ddsName);
	getline(configFile, masterName);

	ProfileSystem::checkDelimiterLine(configFile, "END_SCRIPTS");

	try
	{
		//openVerticalScript( vertName ); 
		//openMoogScript(moogName);
		openAWGScript(awgName);
	}
	catch (Error& err)
	{
		/*int answer = promptBox( "ERROR: Failed to open vertical script file: " + vertName + ", with error \r\n"
								+ err.whatStr( ) + "\r\nAttempt to find file yourself?", MB_YESNO );*/
		//int answer = promptBox("ERROR: Failed to open moog script file: " + moogName + ", with error \r\n"
		//	+ err.whatStr() + "\r\nAttempt to find file yourself?", MB_YESNO);
		int answer = promptBox("ERROR: Failed to open awg script file: " + awgName + ", with error \r\n"
			+ err.whatStr() + "\r\nAttempt to find file yourself?", MB_YESNO);
		if (answer == IDYES)
		{
		//	openMoogScript(openWithExplorer(NULL, MOOG_SCRIPT_EXTENSION));
			openAWGScript(openWithExplorer(NULL, AWG_SCRIPT_EXTENSION));
		}
	}
	try
	{
		openGmoogScript(gmoogName);
	}
	catch (Error& err)
	{
		int answer = promptBox("ERROR: Failed to open gmoog script file: " + gmoogName + ", with error \r\n"
			+ err.whatStr() + "\r\nAttempt to find file yourself?", MB_YESNO);
		if (answer == IDYES)
		{
			//openVerticalScript( openWithExplorer( NULL, "nScript" ) ); 
			openGmoogScript(openWithExplorer(NULL, GIGAMOOG_SCRIPT_EXTENSION));
		}
	}
	try
	{
		openDdsScript(ddsName);
	}
	catch (Error& err)
	{
		int answer = promptBox("ERROR: Failed to open DDS script file: " + ddsName + ", with error \r\n"
			+ err.whatStr() + "\r\nAttempt to find file yourself?", MB_YESNO);
		if (answer == IDYES)
		{
			//openVerticalScript( openWithExplorer( NULL, "nScript" ) );
			//openMoogScript(openWithExplorer(NULL, "ddsScript"));
			openAWGScript(openWithExplorer(NULL, "awgScript"));
		}
	}
	try
	{
		openMasterScript(masterName);
	}
	catch (Error& err)
	{
		int answer = promptBox("ERROR: Failed to open master script file: " + masterName + ", with error \r\n"
			+ err.whatStr() + "\r\nAttempt to find file yourself?", MB_YESNO);
		if (answer == IDYES)
		{
			openMasterScript(openWithExplorer(NULL, "mScript"));
		}
	}
	considerScriptLocations();
	recolorScripts();
}


void ScriptingWindow::newMasterScript()
{
	masterScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	masterScript.newScript();
	updateConfigurationSavedStatus(false);
	masterScript.updateScriptNameText(getProfile().categoryPath);
	masterScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
		auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
}

std::string ScriptingWindow::openMasterScriptFolder(CWnd* parent)
{
	try
	{
		std::string horizontalOpenName = getFolderWithExplorer(parent, MASTER_SCRIPT_EXTENSION);
		return horizontalOpenName;
	}
	catch (Error& err)
	{
		comm()->sendError("New Master function Failed: " + err.whatStr() + "\r\n");
	}
}

std::string ScriptingWindow::openAWGScriptFolder(CWnd* parent)
{
	try
	{
		std::string horizontalOpenName = getFolderWithExplorer(parent, AWG_SCRIPT_EXTENSION);
		return horizontalOpenName;
	}
	catch (Error& err)
	{
		comm()->sendError("New AWG function Failed: " + err.whatStr() + "\r\n");
	}
}

int ScriptingWindow::openMasterScriptByPath(std::string filepath)
{
	try
	{
		masterScript.openParentScript(filepath, getProfile().categoryPath, mainWindowFriend->getRunInfo(), false);
		masterScript.updateScriptNameText(getProfile().categoryPath);
		//masterScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			//auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
		return 0;
	}
	catch (Error& err)
	{
		comm()->sendError("openMasterScriptByPath function Failed: " + err.whatStr() + "\r\n");
		return 1;
	}
}

int ScriptingWindow::openAWGScriptByPath(std::string filepath)
{
	try
	{
		awgScript.openParentScript(filepath, getProfile().categoryPath, mainWindowFriend->getRunInfo(), false); 
		awgScript.updateScriptNameText(getProfile().categoryPath);
		//awgScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			//auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
		return 0;
	}
	catch (Error& err)
	{
		comm()->sendError("openAWGScriptByPath function Failed: " + err.whatStr() + "\r\n");
		return 1;
	}
}

void ScriptingWindow::openMasterScript(CWnd* parent)
{
	try
	{
		masterScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
		std::string horizontalOpenName = openWithExplorer(parent, MASTER_SCRIPT_EXTENSION);
		masterScript.openParentScript(horizontalOpenName, getProfile().categoryPath, mainWindowFriend->getRunInfo());
		updateConfigurationSavedStatus(false);
		masterScript.updateScriptNameText(getProfile().categoryPath);
		masterScript.colorEntireScript(auxWindowFriend->getAllVariables(), mainWindowFriend->getRgbs(),
			auxWindowFriend->getTtlNames(), auxWindowFriend->getDacNames());
	}
	catch (Error& err)
	{
		comm()->sendError("New Master function Failed: " + err.whatStr() + "\r\n");
	}
}


void ScriptingWindow::saveMasterScript()
{
	masterScript.saveScript(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	masterScript.updateScriptNameText(getProfile().categoryPath);
}


void ScriptingWindow::saveMasterScriptAs(CWnd* parent)
{
	std::string extensionNoPeriod = masterScript.getExtension();
	if (extensionNoPeriod.size() == 0)
	{
		return;
	}
	extensionNoPeriod = extensionNoPeriod.substr(1, extensionNoPeriod.size());
	std::string newScriptAddress = saveWithExplorer(parent, extensionNoPeriod, getProfileSettings());
	masterScript.saveScriptAs(newScriptAddress, mainWindowFriend->getRunInfo());
	updateConfigurationSavedStatus(false);
	masterScript.updateScriptNameText(getProfile().categoryPath);
}


void ScriptingWindow::newMasterFunction()
{
	try
	{
		masterScript.newFunction();
	}
	catch (Error& exception)
	{
		comm()->sendError("New Master function Failed: " + exception.whatStr() + "\r\n");
	}
}


void ScriptingWindow::saveMasterFunction()
{
	try
	{
		masterScript.saveAsFunction();
	}
	catch (Error& exception)
	{
		comm()->sendError("Save Master Script Function Failed: " + exception.whatStr() + "\r\n");
	}

}

void ScriptingWindow::saveMasterFunction_nocatch()
{
	masterScript.saveAsFunction();
}

void ScriptingWindow::deleteMasterFunction()
{
	// todo. Right now you can just delete the file itself... 
}


void ScriptingWindow::handleNewConfig(std::ofstream& saveFile)
{
	saveFile << "SCRIPTS\n";
	saveFile << "NONE" << "\n";
	saveFile << "NONE" << "\n";
	saveFile << "NONE" << "\n";
	saveFile << "NONE" << "\n";
	saveFile << "END_SCRIPTS\n";
	//intensityAgilent.handleNewConfig( saveFile ); 
}


void ScriptingWindow::handleSavingConfig(std::ofstream& saveFile)
{
	scriptInfo<std::string> addresses = getScriptAddresses();
	// order matters! 
	saveFile << "SCRIPTS\n";
	//saveFile << addresses.moog << "\n";
	saveFile << addresses.awg << "\n";
	saveFile << addresses.gmoog << "\n";
	saveFile << addresses.DDS << "\n";
	saveFile << addresses.master << "\n";
	saveFile << "END_SCRIPTS\n";
}


void ScriptingWindow::checkMasterSave()
{
	masterScript.checkSave(getProfile().categoryPath, mainWindowFriend->getRunInfo());
}


void ScriptingWindow::openMasterScript(std::string name)
{
	masterScript.openParentScript(name, getProfile().categoryPath, mainWindowFriend->getRunInfo());
}

void ScriptingWindow::openDdsScript(std::string name)
{
	ddsScript.openParentScript(name, getProfile().categoryPath, mainWindowFriend->getRunInfo());
}

void ScriptingWindow::openAWGScript(std::string name)
{
	awgScript.openParentScript(name, getProfile().categoryPath, mainWindowFriend->getRunInfo());
}


void ScriptingWindow::openGmoogScript(std::string name)
{
	gmoogScript.openParentScript(name, getProfile().categoryPath, mainWindowFriend->getRunInfo());
}



void ScriptingWindow::considerScriptLocations()
{
	//moogScript.considerCurrentLocation(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	awgScript.considerCurrentLocation(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	gmoogScript.considerCurrentLocation(getProfile().categoryPath, mainWindowFriend->getRunInfo());
	ddsScript.considerCurrentLocation(getProfile().categoryPath, mainWindowFriend->getRunInfo());
}


/// End common functions 
void ScriptingWindow::passCommonCommand(UINT id)
{
	// pass the command id to the common function, filling in the pointers to the windows which own objects needed. 
	commonFunctions::handleCommonMessage(id, this, mainWindowFriend, this, cameraWindowFriend, auxWindowFriend);
}


void ScriptingWindow::changeBoxColor(systemInfo<char> colors)
{
	statusBox.changeColor(colors);
}


void ScriptingWindow::updateProfile(std::string text)
{
	profileDisplay.update(text);
}


profileSettings ScriptingWindow::getProfileSettings()
{
	return mainWindowFriend->getProfileSettings();
}


void ScriptingWindow::updateConfigurationSavedStatus(bool status)
{
	mainWindowFriend->updateConfigurationSavedStatus(status);
}

