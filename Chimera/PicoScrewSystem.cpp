#include "stdafx.h"
#include "PicoScrewSystem.h"
#include "AuxiliaryWindow.h"
// for other ni stuff
#include "nidaqmx2.h"

PicoScrewSystem::PicoScrewSystem()
{
    if (PICOSCREW_SAFEMODE) {
		return;
	}
	//newp_usb_SetLogging(true);
	newp_usb_init_system();
	// If the devices were not opened successfully
	if (!npUSB.OpenDevices(0, true)) {
		thrower("\n***** Error:  Could not open the devices. *****\n\n"
			"Please make sure that the devices are powered on, \n"
			"connected to the PC, and that the drivers are properly installed.\n\n");
		
	}

	// Get the device table
	std::map<std::string, int> deviceTable = npUSB.GetDeviceTable();

	// If there are no open instruments
	if (deviceTable.size() == 0) {
		newp_usb_uninit_system();
		thrower("No devices discovered.\n\n");
		return;
	}
	if (deviceTable.find(PICOSCREW_KEY) == deviceTable.end()) {
		// somehow if called newp_usb_init_system() on the top, the deviceTable will be an map with empty key
		// and if not calling newp_usb_init_system(), will see weird break of the program during starting. 
		//thrower("Error in finding the desired PicoScrew device: " + str(deviceKey) +
		//	"Please make sure that the devices are powered on, \n"
		//	"connected to the PC, and that the drivers are properly installed.\n\n");
	}
}

void PicoScrewSystem::initialize(POINT& pos, cToolTips& toolTips, AuxiliaryWindow* master, int& id)
{
	// title
	picoScrewHistory = {0,0,0,0};
	commandHistory = {0,0,0,0};
	hystP = {1,1,1,1};
	// hystN = {1.1349,1.1214,1,1}; Old Mount
	hystN = {1,1,1,1};
	picoScrewTitle.sPos = { pos.x, pos.y += 25, pos.x + 400, pos.y += 25 };
	picoScrewTitle.Create("PICO SCREW CONTROL", WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER, picoScrewTitle.sPos, master, id++);
	picoScrewTitle.fontType = HeadingFont;
	// 
	picoScrewSetButton.sPos = { pos.x, pos.y, pos.x + 400, pos.y += 25 };
	picoScrewSetButton.Create("PROGRAM SCREWS", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		picoScrewSetButton.sPos, master, ID_PICO_SET_BUTTON);
	picoScrewSetButton.setToolTip("Press this button to attempt force all Pico Screw values to the values currently recorded in the"
		" edits below.", toolTips, master);
    
    //For now, no enable control to make things simple.
    // picoScrewControl.sPos = {pos.x + 420, pos.y, pos.x + 540, pos.y += 25 };
	// picoScrewControl.Create( "Control?", NORM_CHECK_OPTIONS, picoScrewControl.sPos, master, ID_CONTROL_PIOCOSCREW);
	// picoScrewControl.EnableWindow( true );

	pos.y += 0;

	// Motor Labels
	motor0Title.sPos = { pos.x, pos.y, pos.x + 100, pos.y + 25 };
	motor0Title.Create("Motor 1", WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER, motor0Title.sPos, master, id++);
	motor0Title.fontType = HeadingFont;
	motor1Title.sPos = { pos.x + 100, pos.y, pos.x + 200, pos.y + 25 };
	motor1Title.Create("Motor 2", WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER, motor1Title.sPos, master, id++);
	motor1Title.fontType = HeadingFont;
	motor2Title.sPos = { pos.x + 200, pos.y, pos.x + 300, pos.y + 25 };
	motor2Title.Create("Motor 3", WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER, motor2Title.sPos, master, id++);
	motor2Title.fontType = HeadingFont;
    motor3Title.sPos = { pos.x + 300, pos.y, pos.x + 400, pos.y += 25 };
	motor3Title.Create("Motor 4", WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER, motor3Title.sPos, master, id++);
	motor3Title.fontType = HeadingFont;

    for (int picoInc = 0; picoInc < PICOSCREW_NUM; picoInc++)
        {
            motorValues[picoInc].sPos = { pos.x + picoInc * 100, pos.y, pos.x + 100 + picoInc * 100,
                                                    pos.y + 25 };
            motorValues[picoInc].colorState = 0;
            motorValues[picoInc].Create(WS_CHILD | WS_VISIBLE | WS_BORDER, motorValues[picoInc].sPos,
                master, id++);
            motorValues[picoInc].SetWindowText("0");

        }
	picoScrewControlButton.sPos = { pos.x, pos.y +=25, pos.x + 400 , pos.y += 25 };
	picoScrewControlButton.Create( "Enable pico screw control during the experiment?", NORM_CHECK_OPTIONS, picoScrewControlButton.sPos, 
									  master, id++ );
	picoScrewHome.sPos = { pos.x, pos.y, pos.x + 400 , pos.y += 25 };
	picoScrewHome.Create("SET HOME", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		picoScrewHome.sPos, master, ID_PICO_HOME_SET_BUTTON);

}

void PicoScrewSystem::handleOpenConfig(std::ifstream& openFile)
{
	ProfileSystem::checkDelimiterLine(openFile, "PICOSCREW");
	UINT ddsInc = 0;
	for (UINT picoInc = 0; picoInc < PICOSCREW_NUM; picoInc++)
	{
		std::string motorValueStr;
		openFile >> motorValueStr;
		motorValues[picoInc].SetWindowTextA(cstr(motorValueStr));
		picoScrewHistory[picoInc] = std::stoi(motorValueStr);
		commandHistory[picoInc] = std::stoi(motorValueStr);
	}
	ProfileSystem::checkDelimiterLine(openFile, "END_PICOSCREW");
}


void PicoScrewSystem::handleNewConfig(std::ofstream& newFile)
{
	newFile << "PICOSCREW\n";
	for (UINT picoInc = 0; picoInc < PICOSCREW_NUM; picoInc++)
	{
		CString motorValue;
		std::string motorValueStr;
		motorValues[picoInc].GetWindowTextA(motorValue);
		motorValueStr = std::string(motorValue);
		newFile << motorValueStr << " ";
	}
	newFile << "\nEND_PICOSCREW\n";
}


void PicoScrewSystem::handleSaveConfig(std::ofstream& saveFile)
{
	saveFile << "PICOSCREW\n";
	for (UINT picoInc = 0; picoInc < PICOSCREW_NUM; picoInc++)
	{
		CString motorValue;
		std::string motorValueStr;
		motorValues[picoInc].GetWindowTextA(motorValue);
		motorValueStr = std::string(motorValue);
		saveFile << motorValueStr << " ";
	}
	saveFile << "\nEND_PICOSCREW\n";
}


void PicoScrewSystem::rearrange(UINT width, UINT height, fontMap fonts)
{
	picoScrewTitle.rearrange( width, height, fonts);
	picoScrewSetButton.rearrange(width, height, fonts);
	motor0Title.rearrange(width, height, fonts);
	motor1Title.rearrange( width, height, fonts);
	motor2Title.rearrange( width, height, fonts);
	motor3Title.rearrange(width, height, fonts);
	for (auto& motorValue : motorValues)
	{
		motorValue.rearrange(width, height, fonts);
	}
	picoScrewControlButton.rearrange(width, height, fonts);
	picoScrewHome.rearrange(width, height, fonts);
}

void PicoScrewSystem::handleButtonPress(std::vector<variableType>& variables)
{
	//Vanilla Version
	for (UINT picoInc = 0; picoInc < motorValues.size(); picoInc++)
	{
		CString motorValue;
		std::string motorValueStr;
		motorValues[picoInc].GetWindowTextA(motorValue);
		motorValueStr = std::string(motorValue);
		int motorValueInt;
		motorValueInt = std::stoi(motorValueStr);
		moveTo(picoInc, motorValueInt);
		waitMotionDone(picoInc);
	}
	// std::string opt = "set";
	// handlePicoScrewCommand(command, variables);
	// setPicoScrewCommandForm(command);
	// interpretKey(variables, opt);
	// handleVariationProgramming(0);

}

void PicoScrewSystem::moveTo(UINT channel, int position)
{
	int tempPosition = position;
	if (position > 2147483647 || position < int(-2147483647)) {
		thrower("Position value out of allowable range for PicoScrew. Set value is: " + str(position));
	}
	std::string strChan = str(channel + 1);

	if (commandHistory[channel]>position)
	{
		position = picoScrewHistory[channel]+(position-commandHistory[channel])*hystN[channel];
		position = int(position);
	}
	else if (commandHistory[channel]<position)
	{
		position = picoScrewHistory[channel]+(position-commandHistory[channel])*hystP[channel];
		position = int(position);
	}
	else
	{
		position = picoScrewHistory[channel];
	}

	std::string cmd = strChan + "PA" + str(position);
	if (PICOSCREW_SAFEMODE) {
		return;
	}
	int nStatus = npUSB.Write(PICOSCREW_KEY, cmd);
	if (nStatus != 0) {
		thrower("Error:  Device Write Error Code = " + str(nStatus) + ". *****\n\n");
	}
	picoScrewHistory[channel] = position;
	commandHistory[channel] = tempPosition;
	
	
}

void PicoScrewSystem::waitMotionDone(UINT channel)
{
	std::string done = "2";
	while(done != "1")
	{
		std::string strChan = str(channel + 1);
		int nStatus = npUSB.Write(PICOSCREW_KEY,strChan + "MD?");
		char szBuffer[NewportUSB::m_knMaxBufferLength];
		unsigned long lBytesRead = 0;
		nStatus = npUSB.Read(PICOSCREW_KEY, szBuffer, NewportUSB::m_knMaxBufferLength, &lBytesRead);
		done = std::string(szBuffer);
	}
}
	

void PicoScrewSystem::resetPicoScrewEvents()
{
	picoScrewCommandFormList.clear();
	picoScrewCommandList.clear();
}

void PicoScrewSystem::handleVariationProgramming(UINT variationInc)
{
	PicoScrewCommand tempCommand;
	tempCommand = picoScrewCommandList[variationInc][0];
	moveTo(0, tempCommand.motor0Value);
	waitMotionDone(0);
	moveTo(1, tempCommand.motor1Value);
	waitMotionDone(1);
	moveTo(2, tempCommand.motor2Value);
	waitMotionDone(2);
	moveTo(3, tempCommand.motor3Value);
	waitMotionDone(3);

}

void PicoScrewSystem::setPicoScrewCommandForm( PicoScrewCommandForm command )
{
	picoScrewCommandFormList.push_back(command);
	// you need to set up a corresponding trigger to tell the dacs to change the output at the correct time. 
	// This is done later on interpretation of ramps etc.
}

void PicoScrewSystem::handlePicoScrewCommand( PicoScrewCommandForm command, std::vector<variableType>& vars)
{

	CString motor0Value;
	std::string motor0ValueStr;
	CString motor1Value;
	std::string motor1ValueStr;
	CString motor2Value;
	std::string motor2ValueStr;
	CString motor3Value;
	std::string motor3ValueStr;
	motorValues[0].GetWindowTextA(motor0Value);
	motor0ValueStr = str(motor0Value);
	command.motor0Value = motor0ValueStr;
	command.motor0Value.assertValid(vars);
	motorValues[1].GetWindowTextA(motor1Value);
	motor1ValueStr = str(motor1Value);
	command.motor1Value = motor1ValueStr;
	command.motor1Value.assertValid(vars);
	motorValues[2].GetWindowTextA(motor2Value);
	motor2ValueStr = str(motor2Value);
	command.motor2Value = motor2ValueStr;
	command.motor2Value.assertValid(vars);
	motorValues[3].GetWindowTextA(motor3Value);
	motor3ValueStr = str(motor3Value);
	command.motor3Value = motor3ValueStr;
	command.motor3Value.assertValid(vars);
	setPicoScrewCommandForm(command);

    
}

void PicoScrewSystem::interpretKey( std::vector<variableType>& variables, std::string option)
{
	UINT variations;
	variations = 0;
	if (option == "exp")
	{

		variations = variables.front( ).keyValues.size( );
	}

	if (variations == 0)
	{
		variations = 1;
	}
	/// imporantly, this sizes the relevant structures.
	picoScrewCommandList = std::vector<std::vector<PicoScrewCommand>> (variations);
	for (UINT variationInc = 0; variationInc < variations; variationInc++)
	{
		for (UINT eventInc = 0; eventInc < picoScrewCommandFormList.size(); eventInc++)
		{
			PicoScrewCommand tempEvent;
			tempEvent.motor0Value = int(picoScrewCommandFormList[eventInc].motor0Value.evaluate( variables, variationInc ));
            tempEvent.motor1Value = int(picoScrewCommandFormList[eventInc].motor1Value.evaluate( variables, variationInc ));
			tempEvent.motor2Value = int(picoScrewCommandFormList[eventInc].motor2Value.evaluate( variables, variationInc ));
			tempEvent.motor3Value = int(picoScrewCommandFormList[eventInc].motor3Value.evaluate( variables, variationInc ));
			picoScrewCommandList[variationInc].push_back(tempEvent);
		}
	}
}

void PicoScrewSystem::setHome(std::vector<variableType>& variables)
{
	//Vanilla Version
	for (UINT picoInc = 0; picoInc < motorValues.size(); picoInc++)
	{
		CString motorValue;
		std::string motorValueStr;
		motorValues[picoInc].GetWindowTextA(motorValue);
		motorValueStr = std::string(motorValue);
		int motorValueInt;
		motorValueInt = std::stoi(motorValueStr);
		setHomePosition(picoInc, motorValueInt);
		Sleep(1000);
	}
}

void PicoScrewSystem::setHomePosition(UINT channel, int position)
{
	if (position > 2147483647 || position < int(-2147483647)) {
		thrower("Position value out of allowable range for PicoScrew. Set value is: " + str(position));
	}
	std::string strChan = str(channel + 1);

	std::string cmd = strChan + "DH";
	if (PICOSCREW_SAFEMODE) {
		return;
	}
	int nStatus = npUSB.Write(PICOSCREW_KEY, cmd);
	if (nStatus != 0) {
		thrower("Error:  Device Write Error Code = " + str(nStatus) + ". *****\n\n");
	}
	motorValues[channel].SetWindowText("0");
	picoScrewHistory[channel] = 0;
	commandHistory[channel] = 0;
}

