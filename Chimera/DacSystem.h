#pragma once
#include <array>
#include <string>
#include <unordered_map>
#include <iomanip>

#include "Control.h"
#include "VariableSystem.h"
#include "DioSystem.h"
#include "miscellaneousCommonFunctions.h"

#include "nidaqmx2.h"
#include "DacStructures.h"

/**
 * The DacSystem is meant to be a constant class but it currently doesn't actually prevent the user from making 
 * multiple copies of the object. This class is based off of the DAC.bas module in the original VB6 code, of course 
 * adapted for this gui in controlling the relevant controls and handling changes more directly.
 */
class DacSystem
{
	public:
		DacSystem();
		void handleNewConfig( std::ofstream& newFile );
		void handleSaveConfig(std::ofstream& saveFile);
		void handleOpenConfig(std::ifstream& openFile, int versionMajor, int versionMinor, DioSystem* ttls);
		void abort();
		void initialize( POINT& pos, cToolTips& toolTips, AuxiliaryWindow* master, int& id );
		std::string getDacSequenceMessage(UINT variation );
		void handleButtonPress();
		void setDacCommandForm( DacCommandForm command );
		void setForceDacEvent( int line, double val, UINT variation );
		void handleRoundToDac(CMenu& menu);
		void setDacStatusNoForceOut(std::array<double, 32> status);
		void prepareDacForceChange(int line, double voltage);
		void stopDacs();
		void interpretKey( std::vector<variableType>& variables, std::string& warnings );
		void organizeDacCommands(UINT variation);
		void makeFinalDataFormat(UINT variation );
		void formatDacForFPGA(UINT variation);
		void writeDacs( UINT variation, bool loadSkip );
		void setDACs();
		void setDACsSeq();
		void zeroDACValues();
		void setMOTValues(VariableSystem* globalVariables);
		void setTweezerServo();
		void startDacs();
		void configureClocks( UINT variation, bool loadSkip );
		void setDefaultValue(UINT dacNum, double val);
		double getDefaultValue(UINT dacNum);

		unsigned int getNumberSnapshots(UINT variation );
		void checkTimingsWork(UINT variation );
		void setName(int dacNumber, std::string name, cToolTips& toolTips, AuxiliaryWindow* master);
		std::string getName(int dacNumber);
		std::array<std::string, 32> getAllNames();
		std::string getErrorMessage(int errorCode);
		ULONG getNumberEvents(UINT variation );
		void handleDacScriptCommand( DacCommandForm command, std::string name, std::vector<UINT>& dacShadeLocations, 
									 std::vector<variableType>& vars, DioSystem* ttls );
		std::string getDacSystemInfo();
		int getDacIdentifier(std::string name);
		double getDacValue(int dacNumber);
		unsigned int getNumberOfDacs();
		std::pair<double, double> getDacRange(int dacNumber);
		void setMinMax(int dacNumber, double min, double max);
		void shadeDacs(std::vector<unsigned int>& dacShadeLocations);
		void unshadeDacs();
		void rearrange(UINT width, UINT height, fontMap fonts);
		bool isValidDACName(std::string name);
		HBRUSH handleColorMessage(CWnd* hwnd, brushMap brushes, rgbMap rgbs, CDC* cDC);
		void resetDacEvents();
		std::array<double, 32> getDacStatus();
		std::array<double, 32> getFinalSnapshot();
		void checkValuesAgainstLimits(UINT variation );
		void prepareForce();
		double roundToDacResolution(double);
		void findLoadSkipSnapshots( double time, std::vector<variableType>& variables, UINT variation );
		void handleEditChange( UINT dacNumber );
	private:
		Control<CStatic> dacTitle;
		Control<CStatic> dac0Title;
		Control<CStatic> dac1Title;
		Control<CButton> dacSetButton;
		Control<CButton> dacSetMOTButton;
		Control<CButton> zeroDacs;
		std::array<Control<CStatic>, 32> dacLabels;
		std::array<Control<CEdit>, 32> breakoutBoardEdits;
		std::array<double, 32> dacValues;
		std::array<std::string, 32> dacNames;
		std::array<double, 32> dacMinVals;
		std::array<double, 32> dacMaxVals;
		std::array<double, 32> defaultVals;
		const double dacResolution;
		std::vector<DacCommandForm> dacCommandFormList;
		// the first vector is for each variation.
		std::vector<std::vector<DacCommand>> dacCommandList;
		std::vector<std::vector<DacSnapshot>> dacSnapshots, loadSkipDacSnapshots;
		std::vector<std::array<std::vector<double>, 2>> finalFormatDacData, loadSkipDacFinalFormat;
		std::vector<std::vector<DacChannelSnapshot>> finalDacSnapshots;
		//std::vector<std::vector<char[DAC_LEN_BYTE_BUF]>> seqDac1Data;

		//Zynq tcp connection
		ZynqTCP zynq_tcp;

		std::pair<USHORT, USHORT> dacTriggerLine;

		double dacTriggerTime;
		bool roundToDacPrecision;

		// task for DACboard0 (tasks are a national instruments DAQmx thing)
		TaskHandle staticDac0 = 0;
		// task for DACboard1
		TaskHandle staticDac1 = 0;
		// task for DACboard2
		TaskHandle staticDac2 = 0;
		/// digital in lines not used at the moment.
		TaskHandle digitalDac_0_00 = 0;
		TaskHandle digitalDac_0_01 = 0;
		
		/// My wrappers for all of the daqmx functions that I use currently. If I needed to use another function, I'd 
		/// create another wrapper.
		// note that DAQ stands for Data Aquisition (software). It's not a typo!
		void daqCreateTask( const char* taskName, TaskHandle& handle );
		void daqCreateAOVoltageChan( TaskHandle taskHandle, const char physicalChannel[], 
									 const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, 
									 const char customScaleName[] );
		void daqCreateDIChan( TaskHandle taskHandle, const char lines[], const char nameToAssignToLines[], 
							  int32 lineGrouping );
		void daqStopTask( TaskHandle handle );
		void daqConfigSampleClkTiming( TaskHandle taskHandle, const char source[], float64 rate, int32 activeEdge, 
								  int32 sampleMode, uInt64 sampsPerChan );
		void daqWriteAnalogF64( TaskHandle handle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, 
								bool32 dataLayout, const float64 writeArray[], int32 *sampsPerChanWritten);
		void daqStartTask( TaskHandle handle );
};
