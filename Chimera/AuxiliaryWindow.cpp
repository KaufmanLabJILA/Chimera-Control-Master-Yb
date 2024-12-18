#include "stdafx.h"
#include "AuxiliaryWindow.h"
#include "Control.h"
#include "DioSettingsDialog.h"
#include "DacSettingsDialog.h"
#include "TextPromptDialog.h"
#include "DioSystem.h"
#include "EDacSystem.h"
#include "PicoScrewSystem.h"
#include "commonFunctions.h"
#include "openWithExplorer.h"
#include "saveWithExplorer.h"
#include "agilentStructures.h"

AuxiliaryWindow::AuxiliaryWindow() : CDialog()
//topBottomTek(TOP_BOTTOM_TEK_SAFEMODE, TOP_BOTTOM_TEK_USB_ADDRESS),
//eoAxialTek(EO_AXIAL_TEK_SAFEMODE, EO_AXIAL_TEK_USB_ADDRESS),
//agilents{ TOP_BOTTOM_AGILENT_SETTINGS, AXIAL_AGILENT_SETTINGS,
//FLASHING_AGILENT_SETTINGS, UWAVE_AGILENT_SETTINGS}
{}


IMPLEMENT_DYNAMIC(AuxiliaryWindow, CDialog)


BEGIN_MESSAGE_MAP(AuxiliaryWindow, CDialog)

	ON_WM_TIMER()

	ON_WM_CTLCOLOR()
	ON_WM_SIZE()

	//ON_COMMAND(ID_ACCELERATOR_SINGLESHOT, )

	ON_COMMAND_RANGE(MENU_ID_RANGE_BEGIN, MENU_ID_RANGE_END, &AuxiliaryWindow::passCommonCommand)
	ON_COMMAND_RANGE(TTL_ID_BEGIN, TTL_ID_END, &AuxiliaryWindow::handleTtlPush)

	ON_COMMAND(TTL_HOLD, &handlTtlHoldPush)
	ON_COMMAND(ID_DAC_SET_BUTTON, &SetDacs)
	ON_COMMAND(ID_DAC_MOT_SET_BUTTON, &SetDacsMOT)
	ON_COMMAND(ID_DDS_SET_BUTTON, &SetDDSs)
	ON_COMMAND(ID_PICO_SET_BUTTON, &SetPicoMotor)
	ON_COMMAND(ID_PICO_HOME_SET_BUTTON, &SetPicoMotorHome)
	ON_COMMAND(ID_DDS_PLL_BUTTON, &lockDDSs)
	ON_COMMAND(IDC_ZERO_TTLS, &zeroTtls)
	ON_COMMAND(IDC_ZERO_DACS, &zeroDacs)
	ON_COMMAND(IDOK, &handleEnter)
	//ON_COMMAND(TOP_BOTTOM_PROGRAM, &passTopBottomTekProgram)
	//ON_COMMAND(EO_AXIAL_PROGRAM, &passEoAxialTekProgram)

	//ON_COMMAND_RANGE(IDC_TOP_BOTTOM_CHANNEL1_BUTTON, IDC_UWAVE_PROGRAM, &AuxiliaryWindow::handleAgilentOptions)
	//ON_COMMAND_RANGE(TOP_ON_OFF, AXIAL_FSK, &AuxiliaryWindow::handleTektronicsButtons)

	//ON_CONTROL_RANGE(CBN_SELENDOK, IDC_TOP_BOTTOM_AGILENT_COMBO, IDC_TOP_BOTTOM_AGILENT_COMBO,
	//	&AuxiliaryWindow::handleAgilentCombo)
	//ON_CONTROL_RANGE(CBN_SELENDOK, IDC_AXIAL_AGILENT_COMBO, IDC_AXIAL_AGILENT_COMBO,
	//	&AuxiliaryWindow::handleAgilentCombo)
	//ON_CONTROL_RANGE(CBN_SELENDOK, IDC_FLASHING_AGILENT_COMBO, IDC_FLASHING_AGILENT_COMBO,
	//	&AuxiliaryWindow::handleAgilentCombo)
	//ON_CONTROL_RANGE(CBN_SELENDOK, IDC_UWAVE_AGILENT_COMBO, IDC_UWAVE_AGILENT_COMBO,
	//	&AuxiliaryWindow::handleAgilentCombo)

	ON_CONTROL_RANGE(EN_CHANGE, ID_DAC_FIRST_EDIT, (ID_DAC_FIRST_EDIT + 23), &AuxiliaryWindow::DacEditChange)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_CONFIG_VARS_LISTVIEW, &AuxiliaryWindow::ConfigVarsColumnClick)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_VARS_LISTVIEW, &AuxiliaryWindow::ConfigVarsDblClick)
	ON_NOTIFY(NM_RCLICK, IDC_CONFIG_VARS_LISTVIEW, &AuxiliaryWindow::ConfigVarsRClick)

	ON_NOTIFY(NM_DBLCLK, IDC_GLOBAL_VARS_LISTVIEW, &AuxiliaryWindow::GlobalVarDblClick)
	ON_NOTIFY(NM_RCLICK, IDC_GLOBAL_VARS_LISTVIEW, &AuxiliaryWindow::GlobalVarRClick)
	ON_NOTIFY_RANGE(NM_CUSTOMDRAW, IDC_GLOBAL_VARS_LISTVIEW, IDC_GLOBAL_VARS_LISTVIEW, &AuxiliaryWindow::drawVariables)
	ON_NOTIFY_RANGE(NM_CUSTOMDRAW, IDC_CONFIG_VARS_LISTVIEW, IDC_CONFIG_VARS_LISTVIEW, &AuxiliaryWindow::drawVariables)

	//ON_CONTROL_RANGE(EN_CHANGE, IDC_TOP_BOTTOM_EDIT, IDC_TOP_BOTTOM_EDIT, &AuxiliaryWindow::handleAgilentEditChange)
	//ON_CONTROL_RANGE(EN_CHANGE, IDC_FLASHING_EDIT, IDC_FLASHING_EDIT, &AuxiliaryWindow::handleAgilentEditChange)
	//ON_CONTROL_RANGE(EN_CHANGE, IDC_AXIAL_EDIT, IDC_AXIAL_EDIT, &AuxiliaryWindow::handleAgilentEditChange)
	//ON_CONTROL_RANGE(EN_CHANGE, IDC_UWAVE_EDIT, IDC_UWAVE_EDIT, &AuxiliaryWindow::handleAgilentEditChange)

END_MESSAGE_MAP()

std::pair<UINT, UINT> AuxiliaryWindow::getTtlBoardSize()
{
	return ttlBoard.getTtlBoardSize();
}


void AuxiliaryWindow::handleNewConfig(std::ofstream& newFile)
{
	// order matters.
	configVariables.handleNewConfig(newFile);
	ttlBoard.handleNewConfig(newFile);
	dacBoards.handleNewConfig(newFile);
	ddsBoards.handleNewConfig(newFile);
	picoScrew.handleNewConfig(newFile);
	/*for (auto& agilent : agilents)
	{
		agilent.handleNewConfig(newFile);
	}
	topBottomTek.handleNewConfig(newFile);
	eoAxialTek.handleNewConfig(newFile);*/
}


void AuxiliaryWindow::handleSaveConfig(std::ofstream& saveFile)
{
	// order matters.
	configVariables.handleSaveConfig(saveFile);
	ttlBoard.handleSaveConfig(saveFile);
	dacBoards.handleSaveConfig(saveFile);
	ddsBoards.handleSaveConfig(saveFile);
	picoScrew.handleSaveConfig(saveFile);
	//for (auto& agilent : agilents)
	//{
	//	agilent.handleSavingConfig(saveFile, mainWindowFriend->getProfileSettings().categoryPath,
	//		mainWindowFriend->getRunInfo());
	//}
	//topBottomTek.handleSaveConfig(saveFile);
	//eoAxialTek.handleSaveConfig(saveFile);
}

void AuxiliaryWindow::handleOpeningConfig(std::ifstream& configFile, int versionMajor, int versionMinor)
{
	ttlBoard.prepareForce();
	dacBoards.prepareForce();
	ddsBoards.prepareForce();

	configVariables.handleOpenConfig(configFile, versionMajor, versionMinor);
	ttlBoard.handleOpenConfig(configFile, versionMajor, versionMinor);
	dacBoards.handleOpenConfig(configFile, versionMajor, versionMinor, &ttlBoard);
	ddsBoards.handleOpenConfig(configFile, versionMajor, versionMinor);
	picoScrew.handleOpenConfig(configFile);

}


UINT AuxiliaryWindow::getNumberOfDacs()
{
	return dacBoards.getNumberOfDacs();
}


std::array<std::array<std::string, 8>, 8> AuxiliaryWindow::getTtlNames()
{
	return ttlBoard.getAllNames();
}


std::array<std::string, 32> AuxiliaryWindow::getDacNames()
{
	return dacBoards.getAllNames();
}


void AuxiliaryWindow::drawVariables(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	if (id == IDC_GLOBAL_VARS_LISTVIEW)
	{
		globalVariables.handleDraw(pNMHDR, pResult, mainWindowFriend->getRgbs());
	}
	else
	{
		configVariables.handleDraw(pNMHDR, pResult, mainWindowFriend->getRgbs());
	}
}


void AuxiliaryWindow::ConfigVarsDblClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	std::vector<Script*> scriptList;
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		configVariables.updateVariableInfo(scriptList, mainWindowFriend, this, &ttlBoard, &dacBoards, &edacBoards, &ddsBoards);
	}
	catch (Error& exception)
	{
		sendErr("Variables Double Click Handler : " + exception.whatStr() + "\r\n");
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::ConfigVarsRClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		configVariables.deleteVariable();
	}
	catch (Error& exception)
	{
		sendErr("Variables Right Click Handler : " + exception.whatStr() + "\r\n");
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}

std::vector<variableType> AuxiliaryWindow::getAllVariables()
{
	std::vector<variableType> vars = configVariables.getEverything();
	std::vector<variableType> vars2 = globalVariables.getEverything();
	vars.insert(vars.end(), vars2.begin(), vars2.end());
	return vars;
}


void AuxiliaryWindow::GlobalVarDblClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	std::vector<Script*> scriptList;
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		globalVariables.updateVariableInfo(scriptList, mainWindowFriend, this, &ttlBoard, &dacBoards, &edacBoards, &ddsBoards);
	}
	catch (Error& exception)
	{
		sendErr("Global Variables Double Click Handler : " + exception.whatStr() + "\r\n");
	}
}


void AuxiliaryWindow::GlobalVarRClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		globalVariables.deleteVariable();
	}
	catch (Error& exception)
	{
		sendErr("Global Variables Right Click Handler : " + exception.whatStr() + "\r\n");
	}
}



void AuxiliaryWindow::ConfigVarsColumnClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		configVariables.handleColumnClick(pNotifyStruct, result);
	}
	catch (Error& exception)
	{
		sendErr("Handling config variable listview click : " + exception.whatStr() + "\r\n");
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::clearVariables()
{
	configVariables.clearVariables();
}


void AuxiliaryWindow::addVariable(std::string name, bool constant, double value, int item)
{
	variableType var;
	var.name = name;
	var.constant = constant;
	var.ranges.push_back({ value, 0, 1, false, true });
	configVariables.addConfigVariable(var, item);
}


void AuxiliaryWindow::passCommonCommand(UINT id)
{
	try
	{
		commonFunctions::handleCommonMessage(id, this, mainWindowFriend, scriptingWindowFriend, cameraWindowFriend, this);
	}
	catch (Error& err)
	{
		// catch any extra errors that handleCommonMessage doesn't explicitly handle.
		errBox(err.what());
	}
}

void AuxiliaryWindow::loadFriends(MainWindow* mainWin, ScriptingWindow* scriptWin, CameraWindow* camWin)
{
	mainWindowFriend = mainWin;
	scriptingWindowFriend = scriptWin;
	cameraWindowFriend = camWin;
}


void AuxiliaryWindow::passRoundToDac()
{
	dacBoards.handleRoundToDac(menu);
}


//void AuxiliaryWindow::handleTektronicsButtons(UINT id)
//{
//	if (id >= TOP_ON_OFF && id <= BOTTOM_FSK)
//	{
//		topBottomTek.handleButtons(id - TOP_ON_OFF);
//	}
//	if (id >= EO_ON_OFF && id <= AXIAL_FSK)
//	{
//		eoAxialTek.handleButtons(id - EO_ON_OFF);
//	}
//	mainWindowFriend->updateConfigurationSavedStatus(false);
//}


void AuxiliaryWindow::handleEnter()
{
	errBox("Hello, there!");
}


void AuxiliaryWindow::setVariablesActiveState(bool activeState)
{
	configVariables.setActive(activeState);
}


UINT AuxiliaryWindow::getTotalVariationNumber()
{
	return configVariables.getTotalVariationNumber();
}


void AuxiliaryWindow::OnSize(UINT nType, int cx, int cy)
{
	SetRedraw(false);
	
	ttlBoard.rearrange(cx, cy, getFonts());
	dacBoards.rearrange(cx, cy, getFonts());
	ddsBoards.rearrange(cx, cy, getFonts());

	configVariables.rearrange(cx, cy, getFonts());
	globalVariables.rearrange(cx, cy, getFonts());

	statusBox.rearrange(cx, cy, getFonts());
	SetRedraw();
	RedrawWindow();
}


fontMap AuxiliaryWindow::getFonts()
{
	return mainWindowFriend->getFonts();
}


void AuxiliaryWindow::sendErr(std::string msg)
{
	mainWindowFriend->getComm()->sendError(msg);
}


void AuxiliaryWindow::sendStatus(std::string msg)
{
	mainWindowFriend->getComm()->sendStatus(msg);
}


void AuxiliaryWindow::zeroDacs()
{
	try
	{
		dacBoards.resetDacEvents();
		ttlBoard.resetTtlEvents();

		dacBoards.zeroDACValues();
		dacBoards.setDACsSeq();
		ttlBoard.forceTtl(0, 0, -1);

		sendStatus("Zero'd DACs.\r\n");
	}
	catch (Error& exception)
	{
		sendStatus("Failed to Zero DACs!!!\r\n");
		sendErr(exception.what());
	}
}

void AuxiliaryWindow::SetDacsMOT()
{
	try
	{
		dacBoards.resetDacEvents();
		ttlBoard.resetTtlEvents();

		dacBoards.setMOTValues(&globalVariables);
		dacBoards.setDACsSeq();
		ttlBoard.forceTtl(0, 0, -1);

		sendStatus("DACs set to blue MOT values.\r\n");
	}
	catch (Error& exception)
	{
		sendStatus("Failed to set DACs to blue MOT values!!!\r\n");
		sendErr(exception.what());
	}
}


void AuxiliaryWindow::zeroTtls()
{
	try
	{
		ttlBoard.zeroBoard();
		sendStatus("Zero'd TTLs.\r\n");
	}
	catch (Error& exception)
	{
		sendStatus("Failed to Zero TTLs!!!\r\n");
		sendErr(exception.what());
	}
}


void AuxiliaryWindow::loadMotSettings(MasterThreadInput* input)
{
	try
	{
		sendStatus("Loading MOT Configuration...\r\n");
		input->quiet = true;
		input->ttls = &ttlBoard;
		input->dacs = &dacBoards;
		input->edacs = &edacBoards;
		input->ddss = &ddsBoards;
		input->globalControl = &globalVariables;
		input->comm = mainWindowFriend->getComm();
		input->settings = { 0,0,0 };
		input->debugOptions = { 0, 0, 0, 0, 0, 0, 0, "", 0, 0, 0 };
		// don't get configuration variables. The MOT shouldn't depend on config variables.
		input->variables = globalVariables.getEverything();
		// Only set it once, clearly.
		input->repetitionNumber = 1;
		input->masterScriptAddress = MOT_ROUTINE_ADDRESS;
		//input->rsg = &RhodeSchwarzGenerator;
		////input->intensityAgilentNumber = -1;
		//input->topBottomTek = &topBottomTek;
		//input->eoAxialTek = &eoAxialTek;
		input->runMaster = true;
		input->runNiawg = false;
	}
	catch (Error& exception)
	{
		sendStatus(": " + exception.whatStr() + " " + exception.whatStr() + "\r\n");
	}
}


// Gets called after alt-f4 or X button is pressed.
void AuxiliaryWindow::OnCancel()
{
	passCommonCommand(ID_FILE_MY_EXIT);
}


void AuxiliaryWindow::fillMasterThreadInput(MasterThreadInput* input)
{
	input->ttls = &ttlBoard;
	input->dacs = &dacBoards;
	input->edacs = &edacBoards;
	input->picoScrew = &picoScrew;
	input->ddss = &ddsBoards;
	input->globalControl = &globalVariables;

	// load the variables. This little loop is for letting configuration variables overwrite the globals.
	std::vector<variableType> configVars = configVariables.getEverything();
	std::vector<variableType> globals = globalVariables.getEverything();
	std::vector<variableType> experimentVars = configVars;
	for (auto& globalVar : globals)
	{
		globalVar.overwritten = false;
		bool nameExists = false;
		for (auto& configVar : experimentVars)
		{
			if (configVar.name == globalVar.name)
			{
				globalVar.overwritten = true;
				configVar.overwritten = true;
			}
		}
		if (!globalVar.overwritten)
		{
			experimentVars.push_back(globalVar);
		}
	}
	input->variables = experimentVars;
	globalVariables.setUsages(globals);
	/*input->rsg = &RhodeSchwarzGenerator;
	for (auto& agilent : agilents)
	{
		input->agilents.push_back(&agilent);
	}
	topBottomTek.getSettings();
	eoAxialTek.getSettings();
	input->topBottomTek = &topBottomTek;
	input->eoAxialTek = &eoAxialTek;*/
}


void AuxiliaryWindow::changeBoxColor(systemInfo<char> colors)
{
	statusBox.changeColor(colors);
}


void AuxiliaryWindow::handleAbort()
{
	ttlBoard.unshadeTtls();
	dacBoards.unshadeDacs();
	//ddsBoards.unshadeDDSs();
}


void AuxiliaryWindow::handleMasterConfigSave(std::stringstream& configStream)
{
	// save info
	/// ttls
	for (UINT ttlRowInc = 0; ttlRowInc < ttlBoard.getTtlBoardSize().first; ttlRowInc++)
	{
		for (UINT ttlNumberInc = 0; ttlNumberInc < ttlBoard.getTtlBoardSize().second; ttlNumberInc++)
		{
			std::string name = ttlBoard.getName(ttlRowInc, ttlNumberInc);
			if (name == "")
			{
				// then no name has been set, so create the default name.
				switch (ttlRowInc)
				{
				case 0:
					name = "A";
					break;
				case 1:
					name = "B";
					break;
				case 2:
					name = "C";
					break;
				case 3:
					name = "D";
					break;
				}
				name += str(ttlNumberInc);
			}
			configStream << name << "\n";

			configStream << ttlBoard.getDefaultTtl(ttlRowInc, ttlNumberInc) << "\n";
		}
	}
	// DAC Names
	for (UINT dacInc = 0; dacInc < dacBoards.getNumberOfDacs(); dacInc++)
	{
		std::string name = dacBoards.getName(dacInc);
		std::pair<double, double> minMax = dacBoards.getDacRange(dacInc);
		if (name == "")
		{
			// then the name hasn't been set, so create the default name
			name = "Dac" + str(dacInc);
		}
		configStream << name << "\n";
		configStream << minMax.first << " - " << minMax.second << "\n";
		configStream << dacBoards.getDefaultValue(dacInc) << "\n";
	}

	// DDS Names
	for (UINT ddsInc = 0; ddsInc < ddsBoards.getNumberOfDDSs(); ddsInc++)
	{
		std::string name = ddsBoards.getName(ddsInc);
		std::pair<double, double> minMaxFreq = ddsBoards.getDDSFreqRange(ddsInc);
		std::pair<double, double> minMaxAmp = ddsBoards.getDDSAmpRange(ddsInc);
		if (name == "")
		{
			// then the name hasn't been set, so create the default name
			name = "DDS" + str(ddsInc);
		}
		std::array<double, 2> defaultVals = ddsBoards.getDefaultValue(ddsInc);
		configStream << name << "\n";
		configStream << minMaxFreq.first << " - " << minMaxFreq.second << "\n";
		configStream << defaultVals[0] << "\n";
		configStream << minMaxAmp.first << " - " << minMaxAmp.second << "\n";
		configStream << defaultVals[1] << "\n";
	}

	// Number of Variables
	configStream << globalVariables.getCurrentNumberOfVariables() << "\n";
	/// Variables
	for (UINT varInc : range(globalVariables.getCurrentNumberOfVariables()))
	{
		variableType info = globalVariables.getVariableInfo(varInc);
		configStream << info.name << " ";
		configStream << info.ranges.front().initialValue << "\n";
		// all globals are constants, no need to output anything else.
	}
}


void AuxiliaryWindow::handleMasterConfigOpen(std::stringstream& configStream, double version)
{
	ttlBoard.resetTtlEvents();
	ttlBoard.prepareForce();
	dacBoards.resetDacEvents();
	dacBoards.prepareForce();
	ddsBoards.resetDDSEvents();
	ddsBoards.prepareForce();
	// save info
	for (UINT ttlRowInc : range(ttlBoard.getTtlBoardSize().first))
	{
		for (UINT ttlNumberInc : range(ttlBoard.getTtlBoardSize().second))
		{
			std::string name;
			std::string statusString;
			bool status;
			configStream >> name;
			configStream >> statusString;
			try
			{
				// should actually be zero or one, but just just convert to bool
				status = std::stoi(statusString);
			}
			catch (std::invalid_argument&)
			{
				thrower("ERROR: Failed to load one of the default ttl values!");
			}

			ttlBoard.setName(ttlRowInc, ttlNumberInc, name, toolTips, this);
			//ttlBoard.forceTtl(ttlRowInc, ttlNumberInc, status);
			ttlBoard.updateDefaultTtl(ttlRowInc, ttlNumberInc, status);
		}
	}

	// getting dacs.
	for (UINT dacInc : range(dacBoards.getNumberOfDacs()))
	{
		std::string name;
		std::string defaultValueString;
		double defaultValue;
		std::string minString;
		std::string maxString;
		double min;
		double max;
		configStream >> name;
		if (version >= 1.2)
		{
			configStream >> minString;
			std::string trash;
			configStream >> trash;
			if (trash != "-")
			{
				thrower("ERROR: Expected \"-\" in config file between min and max values!");
			}
			configStream >> maxString;
		}
		configStream >> defaultValueString;
		try
		{
			defaultValue = std::stod(defaultValueString);
			if (version >= 1.2)
			{
				min = std::stod(minString);
				max = std::stod(maxString);
			}
			else
			{
				min = -10;
				max = 10;
			}
		}
		catch (std::invalid_argument&)
		{
			thrower("ERROR: Failed to load one of the default DAC values!");
		}
		dacBoards.setName(dacInc, name, toolTips, this);
		dacBoards.setMinMax(dacInc, min, max);
		dacBoards.prepareDacForceChange(dacInc, defaultValue);
		dacBoards.setDefaultValue(dacInc, defaultValue);
	}
	// getting ddss.
	for (UINT ddsInc : range(ddsBoards.getNumberOfDDSs()))
	{
		std::string name;
		std::string defaultFreqString;
		std::string defaultAmpString;
		double defaultFreq;
		double defaultAmp;
		std::string minFreqString;
		std::string maxFreqString;
		std::string minAmpString;
		std::string maxAmpString;
		double minFreq;
		double maxFreq;
		double minAmp;
		double maxAmp;
		configStream >> name;
		configStream >> minFreqString;
		std::string trash;
		configStream >> trash;
		if (trash != "-")
		{
			thrower("ERROR: Expected \"-\" in config file between min and max freq values!");
		}
		configStream >> maxFreqString;
		configStream >> defaultFreqString;
		configStream >> minAmpString;
		configStream >> trash;
		if (trash != "-")
		{
			thrower("ERROR: Expected \"-\" in config file between min and max freq values!");
		}
		configStream >> maxAmpString;
		configStream >> defaultAmpString;
		try
		{
			defaultFreq = std::stod(defaultFreqString);
			minFreq = std::stod(minFreqString);
			maxFreq = std::stod(maxFreqString);

			defaultAmp = std::stod(maxAmpString);
			minAmp = std::stod(minAmpString);
			maxAmp = std::stod(maxAmpString);
		}
		catch (std::invalid_argument&)
		{
			thrower("ERROR: Failed to load one of the default DDS values!");
		}
		ddsBoards.setName(ddsInc, name, toolTips, this);
		ddsBoards.setFreqMinMax(ddsInc, minFreq, maxFreq);
		ddsBoards.setAmpMinMax(ddsInc, minAmp, maxAmp);
		//ddsBoards.prepareDDSForceChange(ddsInc, defaultFreq);
		ddsBoards.setDefaultValue(ddsInc, { defaultFreq, defaultAmp });
	}
	// variables.
	if (version >= 1.1)
	{
		int varNum;
		configStream >> varNum;
		if (varNum < 0 || varNum > 1000)
		{
			int answer = promptBox("ERROR: variable number retrieved from file appears suspicious. The number is "
				+ str(varNum) + ". Is this accurate?", MB_YESNO);
			if (answer == IDNO)
			{
				// don't try to load anything.
				varNum = 0;
				return;
			}
		}
		// Number of Variables
		globalVariables.clearVariables();
		for (int varInc = 0; varInc < varNum; varInc++)
		{
			variableType tempVar;
			tempVar.constant = true;
			tempVar.overwritten = false;
			tempVar.active = false;
			double value;
			configStream >> tempVar.name;
			configStream >> value;
			tempVar.ranges.push_back({ value, value, 0, false, true });
			globalVariables.addGlobalVariable(tempVar, varInc);
		}
	}
	variableType tempVar;
	tempVar.name = "";
	globalVariables.addGlobalVariable(tempVar, -1);
}


void AuxiliaryWindow::SetDacs()
{
	// have the dac values change
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		sendStatus("----------------------\r\n");
		dacBoards.resetDacEvents();
		ttlBoard.resetTtlEvents();
		sendStatus("Setting Dacs...\r\n");

		dacBoards.handleButtonPress();
		//dacBoards.setDACs();
		dacBoards.setDACsSeq();
		//ddsBoards.setDDSsAmpFreq();
		ttlBoard.forceTtl(0, 0, -1);

		sendStatus("Finished Setting Dacs.\r\n");
	}
	catch (Error& exception)
	{
		errBox(exception.what());
		sendStatus(": " + exception.whatStr() + "\r\n");
		sendErr(exception.what());
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}

void AuxiliaryWindow::SetPicoMotor()
{
	try
	{
		sendStatus("Setting Pico Motors..\r\n");
		picoScrew.handleButtonPress(configVariables.getEverything());
		sendStatus("Finished Setting Pico Motors.\r\n");
	}
	catch (Error& err)
	{
		errBox(err.what());
		sendStatus(": " + err.whatStr() + "\r\n");
		sendErr(err.what());
	}
}

void AuxiliaryWindow::SetPicoMotorHome()
{
	try
	{
		sendStatus("Setting Pico Motor's Home..\r\n");
		picoScrew.setHome(configVariables.getEverything());
		sendStatus("Finished Setting Pico Motor's Home.\r\n");
	}
	catch(Error& err)
	{
		errBox(err.what());
		sendStatus(": " + err.whatStr() + "\r\n");
		sendErr(err.what());
	}
}

void AuxiliaryWindow::SetDDSs()
{
	// have the dds values change
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		sendStatus("----------------------\r\n");
		/*dacBoards.resetDacEvents();
		ttlBoard.resetTtlEvents();*/
		sendStatus("Setting DDSs...\r\n");

		ddsBoards.handleButtonPress();
		ddsBoards.setDDSsAmpFreq();
		ttlBoard.forceTtl(0, 0, -1);

		sendStatus("Finished Setting DDSs.\r\n");
	}
	catch (Error& exception)
	{
		errBox(exception.what());
		sendStatus(": " + exception.whatStr() + "\r\n");
		sendErr(exception.what());
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}

void AuxiliaryWindow::lockDDSs()
{
	// have the dds values change
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		sendStatus("----------------------\r\n");

		sendStatus("Locking DDS PLLs...\r\n");

		ddsBoards.lockPLLs();

		sendStatus("Finished locking PLLs.\r\n");
	}
	catch (Error& exception)
	{
		errBox(exception.what());
		sendStatus(": " + exception.whatStr() + "\r\n");
		sendErr(exception.what());
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::DacEditChange(UINT id)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		dacBoards.handleEditChange(id - ID_DAC_FIRST_EDIT);
	}
	catch (Error& err)
	{
		sendErr(err.what());
	}
}


void AuxiliaryWindow::handleTtlPush(UINT id)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus(false);
		ttlBoard.handleTTLPress(id);
	}
	catch (Error& exception)
	{
		sendErr("TTL Press Handler Failed: " + exception.whatStr() + "\r\n");
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::handlTtlHoldPush()
{
	try
	{
		ttlBoard.handleHoldPress();
	}
	catch (Error& exception)
	{
		sendErr("TTL Hold Handler Failed: " + exception.whatStr() + "\r\n");
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::ViewOrChangeTTLNames()
{
	mainWindowFriend->updateConfigurationSavedStatus(false);
	ttlInputStruct input;
	input.ttls = &ttlBoard;
	input.toolTips = toolTips;
	TtlSettingsDialog dialog(&input, IDD_VIEW_AND_CHANGE_TTL_NAMES);
	dialog.DoModal();
}


void AuxiliaryWindow::ViewOrChangeDACNames()
{
	mainWindowFriend->updateConfigurationSavedStatus(false);
	dacInputStruct input;
	input.dacs = &dacBoards;
	input.toolTips = toolTips;
	DacSettingsDialog dialog(&input, IDD_VIEW_AND_CHANGE_DAC_NAMES);
	dialog.DoModal();
}


void AuxiliaryWindow::Exit()
{
	EndDialog(0);
}


HBRUSH AuxiliaryWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	brushMap brushes = mainWindowFriend->getBrushes();
	rgbMap rgbs = mainWindowFriend->getRgbs();
	HBRUSH result;
	//for (auto& ag : agilents)
	//{
	//	result = ag.handleColorMessage(pWnd, brushes, rgbs, pDC);
	//	if (result != NULL)
	//	{
	//		return result;
	//	}
	//}
	result = ttlBoard.handleColorMessage(pWnd, brushes, rgbs, pDC);
	if (result != NULL)
	{
		return result;
	}
	result = dacBoards.handleColorMessage(pWnd, brushes, rgbs, pDC);
	//if (result != NULL)
	//{
	//	return result;
	//}
	//result = topBottomTek.handleColorMessage(pWnd, brushes, rgbs, pDC);
	//if (result != NULL)
	//{
	//	return result;
	//}
	//result = eoAxialTek.handleColorMessage(pWnd, brushes, rgbs, pDC);
	if (result != NULL)
	{
		return result;
	}
	result = *statusBox.handleColoring(pWnd->GetDlgCtrlID(), pDC, brushes, rgbs);
	if (result != NULL)
	{
		return result;
	}

	// default colors
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
	{
		pDC->SetTextColor(rgbs["theme foreground"]);
		pDC->SetBkColor(rgbs["theme BG0"]);
		return *brushes["theme BG0"];
	}
	case CTLCOLOR_EDIT:
	{
		pDC->SetTextColor(rgbs["theme foreground"]);
		pDC->SetBkColor(rgbs["theme input"]);
		return *brushes["theme input"];
	}
	case CTLCOLOR_LISTBOX:
	{
		pDC->SetTextColor(rgbs["theme foreground"]);
		pDC->SetBkColor(rgbs["theme BG1"]);
		return *brushes["theme BG1"];
	}
	default:
		return *brushes["theme BG1"];
	}
}


BOOL AuxiliaryWindow::PreTranslateMessage(MSG* pMsg)
{
	for (UINT toolTipInc = 0; toolTipInc < toolTips.size(); toolTipInc++)
	{
		toolTips[toolTipInc]->RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL AuxiliaryWindow::OnInitDialog()
{
	// don't redraw until the first OnSize.
	SetRedraw(false);

	int id = 4000;
	POINT controlLocation{ 0, 0 };
	try
	{
		statusBox.initialize(controlLocation, id, this, 480, toolTips);
		ttlBoard.initialize(controlLocation, toolTips, this, id);
		dacBoards.initialize(controlLocation, toolTips, this, id);
		ddsBoards.initialize(controlLocation, toolTips, this, id);
		picoScrew.initialize(controlLocation, toolTips, this, id);

		POINT statusLoc = { 960, 0 };

		controlLocation = POINT{ 1240, 0 };
		globalVariables.initialize(controlLocation, toolTips, this, id, "GLOBAL VARIABLES",
			mainWindowFriend->getRgbs(), IDC_GLOBAL_VARS_LISTVIEW);
		configVariables.initialize(controlLocation, toolTips, this, id, "CONFIGURATION VARIABLES",
			mainWindowFriend->getRgbs(), IDC_CONFIG_VARS_LISTVIEW);
		configVariables.setActive(false);

	}
	catch (Error& exeption)
	{
		errBox(exeption.what());
	}
	menu.LoadMenu(IDR_MAIN_MENU);
	SetMenu(&menu);
	return TRUE;
}


std::string AuxiliaryWindow::getSystemStatusMsg()
{
	// controls are done. Report the initialization status...
	std::string msg;
	msg += " >>> TTL System <<<\n";
	if (!DIO_SAFEMODE)
	{
		msg += "Code System is active!\n";
		msg += ttlBoard.getSystemInfo() + "\n";
	}
	else
	{
		msg += "Code System is disabled! Enable in \"constants.h\"\n";
	}


	msg += "\n>>> DAC System <<<\n";
	if (!DAQMX_SAFEMODE)
	{
		msg += "Code System is Active!\n";
		msg += dacBoards.getDacSystemInfo() + "\n";
	}
	else
	{
		msg += "Code System is disabled! Enable in \"constants.h\"\n";
	}

	//msg += ">>>>>> VISA Devices <<<<<<<\n\n";
	//msg += "Tektronics 1: " + topBottomTek.queryIdentity() + "\n";
	//msg += "Tektronics 2: " + eoAxialTek.queryIdentity() + "\n";
	//msg += "\n\n>>> Agilents <<<\n";
	//msg += "Code System is Active!\n";
	//for (auto& agilent : agilents)
	//{
	//	msg += agilent.getName() + ": " + agilent.getDeviceIdentity();
	//}
	//msg += "\n>>> GPIB System <<<\n";
	//msg += "Code System is Active!\n";
	//msg += "RSG: " + RhodeSchwarzGenerator.getIdentity() + "\n";
	return msg;
}

