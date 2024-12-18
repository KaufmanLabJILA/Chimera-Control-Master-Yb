#pragma once

#include "Control.h"
#include "PictureSettingsControl.h"
#include "CameraImageDimensions.h"
// #include "Andor.h"
#include "qcmos.h"
#include "Constants.h"

struct cameraPositions;

/*
 * This large class maintains all of the settings & user interactions for said settings of the Andor camera. It more or
 * less contains the PictureSettingsControl Class, as this is meant to be the parent of such an object. It is distinct
 * but highly related to the Andor class, where the Andor class is the class that actually manages communications with
 * the camera and some base settings that the user does not change. Because of the close contact between this and the
 * andor class, this object is initialized with a pointer to the andor object.
 ***********************************************************************************************************************/
class CameraSettingsControl
{
public:
	CameraSettingsControl(qcmosCamera* friendInitializer);
	void setVariationNumber(UINT varNumber);
	void setRepsPerVariation(UINT repsPerVar);
	void updateRunSettingsFromPicSettings();
	CBrush* handleColor(int idNumber, CDC* colorer, brushMap brushes, rgbMap rgbs);
	void initialize(cameraPositions& pos, int& id, CWnd* parent, cToolTips& tooltips);
	void checkTimings(std::vector<double>& exposureTimes);
	void checkTimings(float& kineticCycleTime, float& accumulationTime, std::vector<double>& exposureTimes);
	imageParameters readImageParameters(CameraWindow* camWin);
	void updateMinKineticCycleTime(double time);
	void setEmGain(qcmosCamera* qcmosObj);
	void rearrange(std::string cameraMode, std::string triggerMode, int width, int height, fontMap fonts);
	void handlePictureSettings(UINT id, qcmosCamera* qcmosObj);
	void handleTriggerControl(CameraWindow* cameraWindow);
	void handleCoolerControl(CameraWindow* cameraWindow);
	void handleFanControl(CameraWindow* cameraWindow);
	std::array<int, 4> getPaletteNumbers();
	void handleSetTemperatureOffPress();
	void handleSetTemperaturePress();
	void handleTimer();
	void checkIfReady();
	void cameraIsOn(bool state);
	void handleModeChange(CameraWindow* cameraWindow);
	qcmosRunSettings getSettings();
	void setImageParameters(imageParameters newSettings, CameraWindow* camWin);
	std::array<int, totalMaxPictures> getThresholds();
	void updatePassivelySetSettings();
	void setRunSettings(qcmosRunSettings inputSettings);

	void handleOpenConfig(std::ifstream& configFile, int versionMajor, int versionMinor);
	void handleNewConfig(std::ofstream& newFile);
	void handleSaveConfig(std::ofstream& configFile);
	BOOL getPicsPerRepManual();

private:
	qcmosCamera* qcmosFriend;

	Control<CStatic> header;
	/// TODO
	// Accumulation Time
	Control<CStatic> accumulationCycleTimeLabel;
	Control<CEdit> accumulationCycleTimeEdit;
	// Accumulation Number
	Control<CStatic> accumulationNumberLabel;
	Control<CEdit> accumulationNumberEdit;

	// cameraMode
	Control<CComboBox> cameraModeCombo;
	// EM Gain
	Control<CButton> emGainButton;
	Control<CEdit> emGainEdit;
	Control<CStatic> emGainDisplay;
	// Trigger Mode
	Control<CComboBox> triggerCombo;
	// Cooler Mode
	Control<CComboBox> coolerCombo;
	// Cooler Mode
	Control<CComboBox> fanCombo;
	// Temperature
	Control<CButton> setTemperatureButton;
	Control<CButton> temperatureOffButton;
	Control<CEdit> temperatureEdit;
	Control<CStatic> temperatureDisplay;
	Control<CStatic> temperatureMsg;

	// Kinetic Cycle Time
	Control<CEdit> kineticCycleTimeEdit;
	Control<CStatic> kineticCycleTimeLabel;
	Control<CEdit> minKineticCycleTimeDisp;
	Control<CStatic> minKineticCycleTimeLabel;
	std::string currentControlColor;
	// two subclassed groups.
	ImageDimsControl imageDimensionsObj;
	PictureSettingsControl picSettingsObj;
	// the currently selected settings, not necessarily those being used to run the current
	// experiment.
	qcmosRunSettings runSettings;
};