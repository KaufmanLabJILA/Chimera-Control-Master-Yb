#pragma once
#include "Control.h"
#include "commonTypes.h"


class DebugOptionsControl
{
	public:
		void handleNewConfig( std::ofstream& newFile );
		void handleSaveConfig(std::ofstream& saveFile);
		void handleOpenConfig(std::ifstream& openFile, double version);
		void initialize( int& idStart, POINT& loc, CWnd* parent, cToolTips& tooltips );
		void handleEvent(UINT id, MainWindow* comm);
		debugInfo getOptions();
		void setOptions(debugInfo options);
		void rearrange(int width, int height, fontMap fonts);

	private:
		Control<CStatic> header;
		Control<CButton> readProgress;
		Control<CButton> writeProgress;
		Control<CButton> correctionTimes;
		Control<CButton> niawgScript;
		Control<CButton> outputAgilentScript;
		Control<CButton> niawgMachineScript;
		Control<CButton> excessInfo;
		Control<CButton> showTtlsButton;
		Control<CButton> showDacsButton;
		Control<CStatic> pauseText;
		Control<CEdit> pauseEdit;

		debugInfo currentOptions;
};
