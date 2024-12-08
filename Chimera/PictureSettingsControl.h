#pragma once

#include "Control.h"
#include <array>
#include <vector>
#include "cameraPositions.h"
#include "Constants.h"
;
class qcmosCamera;
class CameraSettingsControl;


/*
 * This class handles all of the gui objects for assigning camera settings. It works closely with the Andor class
 * because it eventually needs to communicate all of these settings to the Andor class.
 */
class PictureSettingsControl
{
public:
	// must have parent. Enforced partially because both are singletons.
	PictureSettingsControl(CameraSettingsControl* parentObj) { parentSettingsControl = parentObj; }
	void handleNewConfig(std::ofstream& newFile);
	void handleSaveConfig(std::ofstream& saveFile);
	void handleOpenConfig(std::ifstream& openFile, int versionMajor, int versionMinor, qcmosCamera* qcmosObj);
	void initialize(cameraPositions& pos, CWnd* parent, int& id);
	void handleOptionChange(int id, qcmosCamera* qcmosObj);
	void disablePictureControls(int pic);
	void OnDisplayCheckboxClicked(int picInc, qcmosCamera* qcmosObj);
	void enablePictureControls(int pic);
	void setExposureTimes(std::vector<double>& times, qcmosCamera* qcmosObj);
	void setExposureTimes(qcmosCamera* qcmosObj);
	std::array<int, 4> getPictureColors();
	std::vector<double> getUsedExposureTimes();
	std::array<int, totalMaxPictures> getThresholds();
	CBrush* colorControls(int idNumber, CDC* colorer, brushMap brushes, rgbMap rgbs);
	void confirmAcquisitionTimings();
	//void setPicturesPerExperiment(UINT pics, qcmosCamera* qcmosObj);
	void setThresholds(std::array<int, totalMaxPictures> thresholds);
	void rearrange(std::string cameraMode, std::string triggerMode, int width, int height, fontMap fonts);
	UINT getPicsPerRepetition();
	void updateSettings();
	void setUnofficialPicsPerRep(UINT picNum, qcmosCamera* qcmosObj);
	void setPicsPerRepManual();
	BOOL picsPerRepManual;
	int selectedDisplayCount;

private:
	CameraSettingsControl* parentSettingsControl;
	std::array<int, 4> colors;
	std::vector<double> exposureTimesUnofficial;
	std::array<int, totalMaxPictures> thresholds;
	// This variable is used by this control and communicated to the andor object, but is not directly accessed
	// while the main camera control needs to figure out how many pictures per repetition there are.
	UINT picsPerRepetitionUnofficial;
	/// Grid of PictureOptions
	//Control<CButton> setPictureOptionsButton;		
	Control<CStatic> totalPicNumberLabel;
	Control<CStatic> pictureLabel;
	Control<CStatic> exposureLabel;
	Control<CStatic> thresholdLabel;
	Control<CStatic> colormapLabel;
	Control<CStatic> displayLabel;
	// 
	std::array<Control<CButton>, totalMaxPictures> totalNumberChoice;
	std::array<Control<CStatic>, totalMaxPictures> pictureNumbers;
	std::array<Control<CEdit>, totalMaxPictures> exposureEdits;
	std::array<Control<CEdit>, totalMaxPictures> thresholdEdits;
	std::array<Control<CComboBox>, 4> colormapCombos;
	std::array<Control<CButton>, totalMaxPictures> displayChoice;

	// Manual image/rep control
	Control<CStatic> picsPerRepLabel;
	Control<CButton> picsPerRepToggle;
	Control<CEdit> picsPerRepEdit;

	//std::array<Control<CButton>, 4> blackWhiteRadios;
	//std::array<Control<CButton>, 4> veridaRadios;
	//std::array<Control<CButton>, 4> infernoRadios;
};
