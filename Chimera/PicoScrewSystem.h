#pragma once
#include <array>
#include <string>
#include <unordered_map>
#include "NewportUSB.h"

#include "Control.h"
#include "VariableSystem.h"
#include "DioSystem.h"
#include "miscellaneousCommonFunctions.h"

#include "nidaqmx2.h"
#include "Expression.h"
#include "commonTypes.h"

struct PicoScrewCommandForm
{
	Expression motor0Value;
    Expression motor1Value;
    Expression motor2Value;
    Expression motor3Value;
};

struct PicoScrewCommand
{
	int motor0Value;
    int motor1Value;
    int motor2Value;
    int motor3Value;
};


struct picoScrewSettings
{
	std::array<Expression, PICOSCREW_NUM> screwPos;
	bool ctrlScrew;
};

class PicoScrewSystem
{
public:
	// THIS CLASS IS NOT COPYABLE.

	PicoScrewSystem();
	void initialize( POINT& pos, cToolTips& toolTips, AuxiliaryWindow* master, int& id);
    void rearrange(UINT width, UINT height, fontMap fonts);
    void handleButtonPress(std::vector<variableType>& variables);
    void moveTo(UINT channel, int position);
    void setPicoScrewCommandForm( PicoScrewCommandForm command );
    void handlePicoScrewCommand( PicoScrewCommandForm command, std::vector<variableType>& vars);
    void interpretKey( std::vector<variableType>& variables, std::string option);
    void handleVariationProgramming(UINT variationInc);
    void handleNewConfig(std::ofstream& newFile);
    void handleSaveConfig(std::ofstream& saveFile);
    void handleOpenConfig(std::ifstream& openFile);
    void resetPicoScrewEvents();
    void waitMotionDone(UINT channel);
    void setHome(std::vector<variableType>& variables);
    void setHomePosition(UINT channel, int position);
    std::vector<std::vector<PicoScrewCommand>> picoScrewCommandList;
    std::vector<PicoScrewCommandForm> picoScrewCommandFormList;
    std::array<int,PICOSCREW_NUM> picoScrewHistory;
    std::array<int,PICOSCREW_NUM> commandHistory;
    PicoScrewCommandForm command;
    Control<CButton> picoScrewControlButton;
    std::array<double,PICOSCREW_NUM> hystP;
    std::array<double,PICOSCREW_NUM> hystN;
private:
    Control<CStatic> picoScrewTitle;
    Control<CStatic> motor0Title;
    Control<CStatic> motor1Title;
    Control<CStatic> motor2Title;
    Control<CStatic> motor3Title;
    Control<CButton> picoScrewSetButton;
    Control<CButton> picoScrewHome;
    std::array<Control<CEdit>, 4> motorValues;
    picoScrewSettings expSettings;
    NewportUSB npUSB;
};
