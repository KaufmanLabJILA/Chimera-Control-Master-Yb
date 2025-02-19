#pragma once
#include "constants.h"
#include "VariableStructures.h"
#include "Control.h"
#include "Windows.h"
#include "afxcview.h"
#include <vector>
#include <string>
#include <unordered_map>

class MainWindow;
class AuxiliaryWindow;
class Script;
class AuxiliaryWindow;
class DacSystem;
class EDacSystem;
class DioSystem;
class DDSSystem;

class VariableSystem
{
	public:
		UINT getTotalVariationNumber();
		void handleNewConfig( std::ofstream& newFile );
		void handleSaveConfig(std::ofstream& saveFile);
		void handleOpenConfig(std::ifstream& openFile, int versionMajor, int versionMinor );
		void handleDraw(NMHDR* pNMHDR, LRESULT* pResult, rgbMap rgbs);
		void updateVariableInfo( std::vector<Script*> scripts, MainWindow* mainWin, AuxiliaryWindow* auxWin,
								 DioSystem* ttls, DacSystem* dacs, EDacSystem* edacs, DDSSystem* ddss);
		void deleteVariable();
		void initialize( POINT& pos, cToolTips& toolTips, AuxiliaryWindow* master, int& id, std::string title,
						 rgbMap rgbs, UINT listviewId );
		void addConfigVariable(variableType var, UINT item);
		void addGlobalVariable( variableType var, UINT item );
		void handleColumnClick(NMHDR * pNotifyStruct, LRESULT * result);
		void addVariableDimension();
		void removeVariableDimension();
		variableType getVariableInfo(int varNumber);
		std::vector<variableType> getAllConstants();
		std::vector<variableType> getAllVariables();
		std::vector<variableType> getEverything();
		unsigned int getCurrentNumberOfVariables();
		void clearVariables();
		INT_PTR handleColorMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, brushMap brushes);
		void setVariationRangeNumber(int num, USHORT dimNumber);
		void rearrange(UINT width, UINT height, fontMap fonts);
		void setActive(bool active);
		void setUsages(std::vector<variableType> vars);
		void updateVariationNumber( );
		void setRangeInclusivity( UINT rangeNum, bool leftBorder, bool inclusive, UINT column );
		/// used to be in KeyHandler
		static void generateKey( std::vector<variableType>& variables, bool randomizeVariablesOption );
		static std::vector<double> getKeyValues( std::vector<variableType> variables );
		void reorderVariableDimensions( );

	private:
		USHORT preRangeColumns = 3;
		// Only 2 gui elements.
		Control<CStatic> variablesHeader;
		Control<CListCtrl> variablesListview;
		// most important member, holds the settings for all current variables. Might change to have an outer vector 
		// for each scan dimension, like so?
		//std::vector<std::vector<variable>> currentVariables;
		std::vector<variableType> currentVariables;
		// number of variations that the variables will go through.
		UINT currentVariations;

		// holds the number of variable ranges. Not sure why this is necessary, could probablty get this info from 
		// currentVariables member.
		USHORT variableRanges;
		// tells the class whether it was initialized as the global variable control or the configuration control.
		bool isGlobal;
		// number of dimensions to the variable scans. Unusual to do more than 2.
		USHORT scanDimensions;
};




