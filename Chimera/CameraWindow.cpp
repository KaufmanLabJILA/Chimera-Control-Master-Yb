#include "stdafx.h"
#include "commonFunctions.h"
#include "CameraSettingsControl.h"
#include "PlottingInfo.h"
#include "AuxiliaryWindow.h"
#include "CameraWindow.h"
#include "realTimePlotterInput.h"
#include "MasterThreadInput.h"
#include "ATMCD32D.H"
#include "dcamapi4.h"
#include "dcamprop.h"
#include <numeric>
#include "cnpy.h"

CameraWindow::CameraWindow() : CDialog(),
CameraSettings(&qcmos),
dataHandler(DATA_SAVE_LOCATION)
{

	/// test the plotter quickly
	std::vector<double> data(100);
	int count = 0;
	for (auto& dat : data)
	{
		dat = -(count - 50)*(count - 50);
		count++;
	}
	//	plotter.sendData(data);
};


IMPLEMENT_DYNAMIC(CameraWindow, CDialog)


BEGIN_MESSAGE_MAP(CameraWindow, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_VSCROLL()

	ON_COMMAND_RANGE(MENU_ID_RANGE_BEGIN, MENU_ID_RANGE_END, &CameraWindow::passCommandsAndSettings)
	//ON_COMMAND(ID_RUNMENU_RUNCAMERA, &CameraWindow::passPictureSettings)
	ON_COMMAND_RANGE(PICTURE_SETTINGS_ID_START, PICTURE_SETTINGS_ID_END, &CameraWindow::passPictureSettings)
	ON_COMMAND_RANGE(DISPLAY_SETTINGS_ID_START, DISPLAY_SETTINGS_ID_END, &CameraWindow::passPictureSettings)
	//ON_CONTROL_RANGE( CBN_SELENDOK, PICTURE_SETTINGS_ID_START, PICTURE_SETTINGS_ID_END, 
					  //&CameraWindow::passPictureSettings )
	// these ids all go to the same function.
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PICTURE_1_MIN_EDIT, IDC_PICTURE_1_MIN_EDIT, &CameraWindow::handlePictureEditChange)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PICTURE_1_MAX_EDIT, IDC_PICTURE_1_MAX_EDIT, &CameraWindow::handlePictureEditChange)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PICTURE_2_MIN_EDIT, IDC_PICTURE_2_MIN_EDIT, &CameraWindow::handlePictureEditChange)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PICTURE_2_MAX_EDIT, IDC_PICTURE_2_MAX_EDIT, &CameraWindow::handlePictureEditChange)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PICTURE_3_MIN_EDIT, IDC_PICTURE_3_MIN_EDIT, &CameraWindow::handlePictureEditChange)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PICTURE_3_MAX_EDIT, IDC_PICTURE_3_MAX_EDIT, &CameraWindow::handlePictureEditChange)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PICTURE_4_MIN_EDIT, IDC_PICTURE_4_MIN_EDIT, &CameraWindow::handlePictureEditChange)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PICTURE_4_MAX_EDIT, IDC_PICTURE_4_MAX_EDIT, &CameraWindow::handlePictureEditChange)
	// 
	ON_COMMAND(IDC_SET_EM_GAIN_BUTTON, &CameraWindow::setEmGain)
	ON_COMMAND(IDC_SET_TEMPERATURE_BUTTON, &CameraWindow::passSetTemperaturePress)
	ON_COMMAND(IDC_SET_TEMPERATURE_OFF_BUTTON, &CameraWindow::passSetTemperatureOffPress)
	ON_COMMAND(IDOK, &CameraWindow::catchEnter)
	ON_COMMAND(IDC_SET_ANALYSIS_LOCATIONS, &CameraWindow::passManualSetAnalysisLocations)
	ON_COMMAND(IDC_SET_GRID_CORNER, &CameraWindow::passSetGridCorner)

	ON_CBN_SELENDOK(IDC_TRIGGER_COMBO, &CameraWindow::passTrigger)
	ON_CBN_SELENDOK(IDC_COOLER_COMBO, &CameraWindow::passCooler)
	ON_CBN_SELENDOK(IDC_FAN_COMBO, &CameraWindow::passFan)
	ON_CBN_SELENDOK(IDC_CAMERA_MODE_COMBO, &CameraWindow::passCameraMode)

	ON_REGISTERED_MESSAGE(eCameraFinishMessageID, &CameraWindow::onCameraFinish)
	ON_REGISTERED_MESSAGE(eCameraProgressMessageID, &CameraWindow::onCameraProgress)

	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()

	ON_NOTIFY(NM_RCLICK, IDC_PLOTTING_LISTVIEW, &CameraWindow::listViewRClick)
	ON_NOTIFY(NM_DBLCLK, IDC_PLOTTING_LISTVIEW, &CameraWindow::handleDblClick)
END_MESSAGE_MAP()


std::string CameraWindow::getSystemStatusString()
{
	std::string statusStr;
	statusStr = "\n\n>>> Hamamatsu Camera <<<\n";
	if (!HAM_SAFEMODE)
	{
		statusStr += "Code System is Active!\n";
		statusStr += qcmos.getSystemInfo();
	}
	else
	{
		statusStr += "Code System is disabled! Enable in \"constants.h\"\n";
	}
	return statusStr;
}


void CameraWindow::handleNewConfig(std::ofstream& newFile)
{
	CameraSettings.handleNewConfig(newFile);
	pics.handleNewConfig(newFile);
	analysisHandler.handleNewConfig(newFile);
}


void CameraWindow::handleSaveConfig(std::ofstream& saveFile)
{
	CameraSettings.handleSaveConfig(saveFile);
	pics.handleSaveConfig(saveFile);
	analysisHandler.handleSaveConfig(saveFile);
}


void CameraWindow::handleOpeningConfig(std::ifstream& configFile, int versionMajor, int versionMinor)
{
	// I could and perhaps should further subdivide this up.
	CameraSettings.handleOpenConfig(configFile, versionMajor, versionMinor);
	pics.handleOpenConfig(configFile, versionMajor, versionMinor);
	analysisHandler.handleOpenConfig(configFile, versionMajor, versionMinor);
	if (CameraSettings.getSettings().picsPerRepetition == 1 || CameraSettings.getPicsPerRepManual())
	{
		pics.setSinglePicture(this, CameraSettings.readImageParameters(this));
	}
	else
	{
		pics.setMultiplePictures(this, CameraSettings.readImageParameters(this),
			CameraSettings.getSettings().picsPerRepetition);
	}
	pics.resetPictureStorage();
	std::array<int, 4> nums = CameraSettings.getPaletteNumbers();
	pics.setPalletes(nums);

	CRect rect;
	GetWindowRect(&rect);
	OnSize(0, rect.right - rect.left, rect.bottom - rect.top);
}


void CameraWindow::loadFriends(MainWindow* mainWin, ScriptingWindow* scriptWin, AuxiliaryWindow* masterWin)
{
	mainWindowFriend = mainWin;
	scriptingWindowFriend = scriptWin;
	auxWindowFriend = masterWin;
}


void CameraWindow::passManualSetAnalysisLocations()
{
	analysisHandler.onManualButtonPushed();
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::passSetGridCorner()
{
	analysisHandler.onCornerButtonPushed();
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::catchEnter()
{
	// the default handling is to close the window, so I need to catch it.
	errBox("Hello there!");
}


void CameraWindow::passAlwaysShowGrid()
{
	if (alwaysShowGrid)
	{
		alwaysShowGrid = false;
		menu.CheckMenuItem(ID_PICTURES_ALWAYSSHOWGRID, MF_UNCHECKED);
	}
	else
	{
		alwaysShowGrid = true;
		menu.CheckMenuItem(ID_PICTURES_ALWAYSSHOWGRID, MF_CHECKED);
	}
	CDC* dc = GetDC();
	pics.setAlwaysShowGrid(alwaysShowGrid, dc);
	ReleaseDC(dc);
	pics.setSpecialGreaterThanMax(specialGreaterThanMax);
}


void CameraWindow::passCameraMode()
{
	CameraSettings.handleModeChange(this);
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::abortCameraRun()
{
	int32 status;
	qcmos.queryStatus(status);

	if (HAM_SAFEMODE)
	{
		// simulate as if you needed to abort.
		status = DCAMCAP_STATUS_BUSY;
	}
	if (status == DCAMCAP_STATUS_BUSY)
	{
		
		//
		pauseThread();
		threadInput.signaler.notify_all();
		qcmos.pauseThread();
		qcmos.onFinish();
		qcmos.abortAcquisition();
		timer.setTimerDisplay("Aborted");
		qcmos.setIsRunningState(false);
		// close the plotting thread.
		atomCrunchThreadActive = false;
		// Wait until plotting thread is complete.
		WaitForSingleObject(plotThreadHandle, INFINITE);
		//plotThreadAborting = false;
		// camera is no longer running.
		try
		{
			dataHandler.closeFile();
		}
		catch (Error& err)
		{
			mainWindowFriend->getComm()->sendError(err.what());
		}

		if (!mainWindowFriend->masterIsRunning()) {

		}
		else if (qcmos.getSettings().cameraMode != "Continuous Single Scans Mode")
		{
			int answer = promptBox("Acquisition Aborted. Delete Data file (data_" + str(dataHandler.getDataFileNumber())
				+ ".h5) for this run?", MB_YESNO);
			if (answer == IDYES)
			{
				try
				{
					dataHandler.deleteFile(mainWindowFriend->getComm());
				}
				catch (Error& err)
				{
					mainWindowFriend->getComm()->sendError(err.what());
				}
			}
		}
		plotThreadAborting = false;
	}
	else if (status == DCAMCAP_STATUS_READY)
	{
		qcmos.setIsRunningState(false);
	}
}


bool CameraWindow::cameraIsRunning()
{
	return qcmos.isRunning();
}


void CameraWindow::handlePictureEditChange(UINT id)
{
	try
	{
		pics.handleEditChange(id);
	}
	catch (Error& err)
	{
		// these errors seem more deserving of an error box.
		errBox(err.what());
	}
}

//Make a thread instead of on camera progress
unsigned __stdcall CameraWindow::frameGrabberThread(void* voidPtr)
{
	frameGrabberThreadInput* input = (frameGrabberThreadInput*)voidPtr;
	std::unique_lock<std::mutex> lock(input->runMutex);
	int safeModeCount = 0;
	long pictureNumber;
	if(!HAM_SAFEMODE)
	{
		while (true)
		{
			input->signaler.wait(lock, [input, &safeModeCount]() { return input->spuriousWakeupHandler;});
			try
			{
				qcmosRunSettings currentSettings = input->qcmos->getSettings();
				pictureNumber = input->qcmos->frameIndexQueue.pop();
				if (pictureNumber % 2 == 1)
				{
					input->cameraWindow->mainThreadStartTimes.push_back(std::chrono::high_resolution_clock::now());
				}
				input->qcmos->updatePictureNumber(pictureNumber);
				if (pictureNumber <= currentSettings.totalPicsInExperiment && pictureNumber != -1)
				{
					std::vector<std::vector<long>> picData;
					try
					{
						picData = input->qcmos->acquireImageData();
					}
					catch (Error& err)
					{
						input->cameraWindow->mainWindowFriend->getComm()->sendError(err.what());
						return NULL;
					}
					if (pictureNumber % 2 == 1)
					{
						input->cameraWindow->imageGrabTimes.push_back(std::chrono::high_resolution_clock::now());
					}
					{
						std::lock_guard<std::mutex> locker(input->cameraWindow->plotLock);
						std::lock_guard<std::mutex> locker2(input->cameraWindow->imageLock);
						// TODO: add check to check if this is needed.
						//211227: this check is never initializing the queue.
						//if (imageQueue.size() == 0) {
						//}
						//else {
						//	imageQueue.push_back(picData[(pictureNumber - 1) % currentSettings.picsPerRepetition]);
						//}
						input->cameraWindow->imageQueue.push_back(picData[(pictureNumber - 1) % currentSettings.picsPerRepetition]);
					}
					
			

					CDC* drawer = input->cameraWindow->GetDC();
					try
					{
						if (input->cameraWindow->plotThreadAborting == false) {
							if (input->cameraWindow->realTimePic)
							{
								std::pair<int, int> minMax;
								// draw the most recent pic.
								minMax = input->cameraWindow->stats.update(picData.back(), pictureNumber % currentSettings.picsPerRepetition, input->cameraWindow->selectedPixel,
									currentSettings.imageSettings.width, currentSettings.imageSettings.height,
									pictureNumber / currentSettings.picsPerRepetition,
									currentSettings.totalPicsInExperiment / currentSettings.picsPerRepetition);

								input->cameraWindow->pics.drawPicture(drawer, pictureNumber % currentSettings.picsPerRepetition, picData.back(), minMax, input->qcmos->picturesToDraw);

								input->cameraWindow->timer.update(pictureNumber / currentSettings.picsPerRepetition, currentSettings.repetitionsPerVariation,
									currentSettings.totalVariations, currentSettings.picsPerRepetition);
							}
							else if (input->cameraWindow->CameraSettings.getPicsPerRepManual())
							{
								std::pair<int, int> minMax;
								// draw the most recent pic.
								minMax = input->cameraWindow->stats.update(picData.back(), 0, input->cameraWindow->selectedPixel,
									currentSettings.imageSettings.width, currentSettings.imageSettings.height,
									pictureNumber / currentSettings.picsPerRepetition,
									currentSettings.totalPicsInExperiment / currentSettings.picsPerRepetition);

								input->cameraWindow->pics.drawPicture(drawer, 0, picData.back(), minMax, input->qcmos->picturesToDraw);

								input->cameraWindow->timer.update(pictureNumber / currentSettings.picsPerRepetition, currentSettings.repetitionsPerVariation,
									currentSettings.totalVariations, currentSettings.picsPerRepetition);
							}
							else if (pictureNumber % currentSettings.picsPerRepetition == 0)
							{

								int counter = 0;
								for (auto data : picData)
								{
									if (data.size() == 0) {//TODO: remove, better fix involving reading all images at end of each rep, rather than during.
										int size = currentSettings.imageSettings.width * currentSettings.imageSettings.height;
										data.resize(size, 0);
									}
									std::pair<int, int> minMax;
									minMax = input->cameraWindow->stats.update(data, counter, input->cameraWindow->selectedPixel, currentSettings.imageSettings.width,
										currentSettings.imageSettings.height, pictureNumber / currentSettings.picsPerRepetition,
										currentSettings.totalPicsInExperiment / currentSettings.picsPerRepetition);

									input->cameraWindow->pics.drawPicture(drawer, counter, data, minMax, input->qcmos->picturesToDraw);
									input->cameraWindow->pics.drawDongles(drawer, input->cameraWindow->selectedPixel, input->cameraWindow->analysisHandler.getAnalysisLocs(), input->cameraWindow->analysisHandler.getAtomGrid());
									counter++;
								}
								input->cameraWindow->timer.update(pictureNumber / currentSettings.picsPerRepetition, currentSettings.repetitionsPerVariation,
									currentSettings.totalVariations, currentSettings.picsPerRepetition);
							}
						}

					}
					catch (Error& err)
					{
						input->cameraWindow->mainWindowFriend->getComm()->sendError(err.what());
					}

					input->cameraWindow->ReleaseDC(drawer);

					// write the data to the file.
					if (currentSettings.cameraMode != "Continuous Single Scans Mode" && picData.size() != 0)
					{
						try
						{
							if (input->cameraWindow->CameraSettings.getPicsPerRepManual()) {
								input->cameraWindow->dataHandler.writePic(pictureNumber, picData[(pictureNumber - 1) % 1],
									currentSettings.imageSettings);
							}
							else {
								input->cameraWindow->dataHandler.writePic(pictureNumber, picData[(pictureNumber - 1) % currentSettings.picsPerRepetition],
									currentSettings.imageSettings);
							}

						}
						catch (Error& err)
						{
							input->cameraWindow->mainWindowFriend->getComm()->sendError(err.what());
						}
					}

				}
				if (pictureNumber == currentSettings.totalPicsInExperiment)
				{
					input->comm->sendCameraFin();
					//input->cameraWindow->dataHandler.closeFile();
				}
			}
			catch (Error& err)
			{
				input->cameraWindow->mainWindowFriend->getComm()->sendError(err.what());
			}
			
		}
		
		
			
	}
}


LRESULT CameraWindow::onCameraProgress(WPARAM wParam, LPARAM lParam)
{
	UINT pictureNumber = lParam;
	if (pictureNumber % 2 == 1)
	{
		mainThreadStartTimes.push_back(std::chrono::high_resolution_clock::now());
	}
	if (lParam == 0)
	{
		// ???
		return NULL;
	}
	qcmosRunSettings currentSettings = qcmos.getSettings();
	if (lParam == -1)
	{
		pictureNumber = currentSettings.totalPicsInExperiment;
	}

	// need to call this before acquireImageData().
	qcmos.updatePictureNumber(pictureNumber);
	std::vector<std::vector<long>> picData;
	try
	{
		picData = qcmos.acquireImageData();
	}
	catch (Error& err)
	{
		mainWindowFriend->getComm()->sendError(err.what());
		return NULL;
	}

	if (pictureNumber % 2 == 1)
	{
		imageGrabTimes.push_back(std::chrono::high_resolution_clock::now());
	}
	//
	{
		std::lock_guard<std::mutex> locker(plotLock);
		std::lock_guard<std::mutex> locker2(imageLock);
		// TODO: add check to check if this is needed.
		//211227: this check is never initializing the queue.
		//if (imageQueue.size() == 0) {
		//}
		//else {
		//	imageQueue.push_back(picData[(pictureNumber - 1) % currentSettings.picsPerRepetition]);
		//}
		imageQueue.push_back(picData[(pictureNumber - 1) % currentSettings.picsPerRepetition]);
	}

	CDC* drawer = GetDC();
	try
	{
		if (plotThreadAborting == false) {
			if (realTimePic)
			{
				std::pair<int, int> minMax;
				// draw the most recent pic.
				minMax = stats.update(picData.back(), pictureNumber % currentSettings.picsPerRepetition, selectedPixel,
					currentSettings.imageSettings.width, currentSettings.imageSettings.height,
					pictureNumber / currentSettings.picsPerRepetition,
					currentSettings.totalPicsInExperiment / currentSettings.picsPerRepetition);

				pics.drawPicture(drawer, pictureNumber % currentSettings.picsPerRepetition, picData.back(), minMax, qcmos.picturesToDraw);

				timer.update(pictureNumber / currentSettings.picsPerRepetition, currentSettings.repetitionsPerVariation,
					currentSettings.totalVariations, currentSettings.picsPerRepetition);
			}
			else if (CameraSettings.getPicsPerRepManual())
			{
				std::pair<int, int> minMax;
				// draw the most recent pic.
				minMax = stats.update(picData.back(), 0, selectedPixel,
					currentSettings.imageSettings.width, currentSettings.imageSettings.height,
					pictureNumber / currentSettings.picsPerRepetition,
					currentSettings.totalPicsInExperiment / currentSettings.picsPerRepetition);

				pics.drawPicture(drawer, 0, picData.back(), minMax, qcmos.picturesToDraw);

				timer.update(pictureNumber / currentSettings.picsPerRepetition, currentSettings.repetitionsPerVariation,
					currentSettings.totalVariations, currentSettings.picsPerRepetition);
			}
			else if (pictureNumber % currentSettings.picsPerRepetition == 0)
			{

				int counter = 0;
				for (auto data : picData)
				{
					if (data.size() == 0) {//TODO: remove, better fix involving reading all images at end of each rep, rather than during.
						int size = currentSettings.imageSettings.width * currentSettings.imageSettings.height;
						data.resize(size, 0);
					}
					std::pair<int, int> minMax;
					minMax = stats.update(data, counter, selectedPixel, currentSettings.imageSettings.width,
						currentSettings.imageSettings.height, pictureNumber / currentSettings.picsPerRepetition,
						currentSettings.totalPicsInExperiment / currentSettings.picsPerRepetition);

					pics.drawPicture(drawer, counter, data, minMax, qcmos.picturesToDraw);
					pics.drawDongles(drawer, selectedPixel, analysisHandler.getAnalysisLocs(), analysisHandler.getAtomGrid());
					counter++;
				}
				timer.update(pictureNumber / currentSettings.picsPerRepetition, currentSettings.repetitionsPerVariation,
					currentSettings.totalVariations, currentSettings.picsPerRepetition);
			}
		}

	}
	catch (Error& err)
	{
		mainWindowFriend->getComm()->sendError(err.what());
	}

	ReleaseDC(drawer);

	// write the data to the file.
	if (currentSettings.cameraMode != "Continuous Single Scans Mode" && picData.size() != 0)
	{
		try
		{
			if (CameraSettings.getPicsPerRepManual()) {
				dataHandler.writePic(pictureNumber, picData[(pictureNumber - 1) % 1],
					currentSettings.imageSettings);
			}
			else {
				dataHandler.writePic(pictureNumber, picData[(pictureNumber - 1) % currentSettings.picsPerRepetition],
					currentSettings.imageSettings);
			}

		}
		catch (Error& err)
		{
			mainWindowFriend->getComm()->sendError(err.what());
		}
	}
	return 0;
}


void CameraWindow::wakeRearranger()
{
	std::unique_lock<std::mutex> lock(rearrangerLock);
	rearrangerConditionVariable.notify_all();
}


void CameraWindow::handleSpecialLessThanMinSelection()
{
	if (specialLessThanMin)
	{
		specialLessThanMin = false;
		menu.CheckMenuItem(ID_PICTURES_LESS_THAN_MIN_SPECIAL, MF_UNCHECKED);
	}
	else
	{
		specialLessThanMin = true;
		menu.CheckMenuItem(ID_PICTURES_LESS_THAN_MIN_SPECIAL, MF_CHECKED);
	}
	pics.setSpecialLessThanMin(specialLessThanMin);
}


void CameraWindow::handleSpecialGreaterThanMaxSelection()
{
	if (specialGreaterThanMax)
	{
		specialGreaterThanMax = false;
		menu.CheckMenuItem(ID_PICTURES_GREATER_THAN_MAX_SPECIAL, MF_UNCHECKED);
	}
	else
	{
		specialGreaterThanMax = true;
		menu.CheckMenuItem(ID_PICTURES_GREATER_THAN_MAX_SPECIAL, MF_CHECKED);
	}
	pics.setSpecialGreaterThanMax(specialGreaterThanMax);
}


void CameraWindow::handleAutoscaleSelection()
{
	if (autoScalePictureData)
	{
		autoScalePictureData = false;
		menu.CheckMenuItem(ID_PICTURES_AUTOSCALEPICTURES, MF_UNCHECKED);
	}
	else
	{
		autoScalePictureData = true;
		menu.CheckMenuItem(ID_PICTURES_AUTOSCALEPICTURES, MF_CHECKED);
	}
	pics.setAutoScalePicturesOption(autoScalePictureData);
}

void CameraWindow::pauseThread()
{
	// andor should not be taking images anymore at this point.
	threadInput.spuriousWakeupHandler = false;
}

LRESULT CameraWindow::onCameraFinish(WPARAM wParam, LPARAM lParam)
{
	// write all processed images - TODO: could do this in real time, but need to work out how to signal using events
	qcmosRunSettings currentSettings = qcmos.getSettings();
	if (lParam == -1)
	{
		UINT pictureNumber = currentSettings.totalPicsInExperiment;
	}
	UINT picNum = 1;
	for (auto & atomArray : atomArrayQueue) {
		if (currentSettings.cameraMode != "Continuous Single Scans Mode" && atomArray.size() != 0)
		{
			try
			{
				std::vector<UINT8> dataArray(atomArray.size()); //Dumb hacky fix.
				for (size_t i = 0; i < atomArray.size(); i++)
				{
					dataArray[i] = atomArray[i];
				}
				dataHandler.writeAtomArray(picNum, dataArray);
				picNum++;
			}
			catch (Error& err)
			{
				mainWindowFriend->getComm()->sendError(err.what());
				break;
			}
		};
	}
	try
	{
		dataHandler.logTweezerOffsets(xOffsetAutoQueue, yOffsetAutoQueue);
	}
	catch (Error& err)
	{
		mainWindowFriend->getComm()->sendError(err.what());
	}

	// notify the andor object that it is done.
	qcmos.onFinish();
	qcmos.pauseThread();
	pauseThread();
	if (alerts.soundIsToBePlayed())
	{
		alerts.playSound();
	}
	dataHandler.closeFile();
	mainWindowFriend->getComm()->sendColorBox(Camera, 'B');
	mainWindowFriend->getComm()->sendStatus("Camera has finished taking pictures and is no longer running.\r\n");
	CameraSettings.cameraIsOn(false);
	mainWindowFriend->handleFinish();
	plotThreadActive = false;
	atomCrunchThreadActive = false;
	/*
	std::vector<double> imageToMainTime, mainToGrabTime, grabToCrunchTime, crunchCrunchingTime;
	for ( auto inc : range( imageTimes.size( ) ) )
	{
		imageToMainTime.push_back( std::chrono::duration<double>( mainThreadStartTimes[inc]
																  - imageTimes[inc] ).count( ) );
		mainToGrabTime.push_back( std::chrono::duration<double>( imageGrabTimes[inc]
																 - mainThreadStartTimes[inc] ).count( ) );
		grabToCrunchTime.push_back( std::chrono::duration<double>( crunchSeesTimes[inc]
																   - imageGrabTimes[inc] ).count( ) );
		crunchCrunchingTime.push_back( std::chrono::duration<double>( crunchFinTimes[inc]
																	  - crunchSeesTimes[inc] ).count( ) );
	}
	*/
	// rearranger thread handles these right now.
	//imageTimes.clear();
	//imageGrabTimes.clear();
	mainThreadStartTimes.clear();
	crunchFinTimes.clear();
	crunchSeesTimes.clear();
	/*
	std::ofstream dataFile( TIMING_OUTPUT_LOCATION + "CamTimeLog.txt" );
	if ( !dataFile.is_open( ) )
	{
		errBox( "ERROR: data file failed to open for rearrangement log!" );
	}
	dataFile << "imageToCamera "
		<< "grabTime "
		<< "grabToCrunch "
		<< "crunchingTime\n";
	for ( auto count : range( imageToMainTime.size( ) ) )
	{
		dataFile << imageToMainTime[count] << " "
			<< mainToGrabTime[count] << " "
			<< grabToCrunchTime[count] << " "
			<< crunchCrunchingTime[count] << "\n";
	}
	dataFile.close( );
	*/
	//Sleep( 5000 );
	mainWindowFriend->stopRearranger();
	wakeRearranger();
	//abort camera here
	qcmos.abortAcquisition();
	return 0;
}


void CameraWindow::startCamera()
{
	mainWindowFriend->getComm()->sendColorBox(Camera, 'Y');
	// turn some buttons off.
	CameraSettings.cameraIsOn(true);
	//
	CDC* dc = GetDC();
	pics.refreshBackgrounds(dc);
	ReleaseDC(dc);
	stats.reset();
	// I used to initialize the data logger here.
	analysisHandler.updateDataSetNumberEdit(dataHandler.getNextFileNumber() - 1);
	double minKineticTime;
	threadInput.spuriousWakeupHandler = true;
	threadInput.signaler.notify_all();
	qcmos.armCamera(this, minKineticTime);
	// CameraSettings.updateMinKineticCycleTime(minKineticTime);
	mainWindowFriend->getComm()->sendColorBox(Camera, 'G');
}


bool CameraWindow::getCameraStatus()
{
	return qcmos.isRunning();
}


void CameraWindow::handleDblClick(NMHDR* info, LRESULT* lResult)
{
	analysisHandler.handleDoubleClick(&mainWindowFriend->getFonts(), CameraSettings.getSettings().picsPerRepetition);
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::listViewRClick(NMHDR* info, LRESULT* lResult)
{
	analysisHandler.handleRClick();
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::OnLButtonUp(UINT stuff, CPoint loc)
{
	alerts.stopSound();
}


// pics looks up the location itself.
void CameraWindow::OnRButtonUp(UINT stuff, CPoint clickLocation)
{
	alerts.stopSound();
	CDC* dc = GetDC();
	try
	{
		if (analysisHandler.buttonClicked())
		{
			coordinate loc = pics.handleRClick(clickLocation);
			if (loc.row != -1)
			{
				analysisHandler.handlePictureClick(loc);
				pics.redrawPictures(dc, selectedPixel, analysisHandler.getAnalysisLocs(),
					analysisHandler.getAtomGrid(), qcmos.picturesToDraw);
			}
		}
		else
		{
			coordinate box = pics.handleRClick(clickLocation);
			if (box.row != -1)
			{
				selectedPixel = box;
				pics.redrawPictures(dc, selectedPixel, analysisHandler.getAnalysisLocs(),
					analysisHandler.getAtomGrid() , qcmos.picturesToDraw);
			}
		}
	}
	catch (Error& err)
	{
		if (err.whatBare() != "not found")
		{
			mainWindowFriend->getComm()->sendError(err.what());
		}
	}
	ReleaseDC(dc);
}


/*
 *
 */
void CameraWindow::passSetTemperaturePress()
{
	try
	{
		CameraSettings.handleSetTemperaturePress();
	}
	catch (Error& err)
	{
		mainWindowFriend->getComm()->sendError(err.what());
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}

void CameraWindow::passSetTemperatureOffPress()
{
	try
	{
		CameraSettings.handleSetTemperatureOffPress();
	}
	catch (Error& err)
	{
		mainWindowFriend->getComm()->sendError(err.what());
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}

/*
 *
 */
void CameraWindow::OnTimer(UINT_PTR id)
{
	CameraSettings.handleTimer();
}


/*
 *
 */
void CameraWindow::passTrigger()
{
	CameraSettings.handleTriggerControl(this);
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::passCooler()
{	
	//This property is not added to the configuration settings
	CameraSettings.handleCoolerControl(this);
	//mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::passFan()
{
	//This property is not added to the configuration settings
	CameraSettings.handleFanControl(this);
	// mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::passPictureSettings(UINT id)
{
	handlePictureSettings(id);
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::handlePictureSettings(UINT id)
{
	selectedPixel = { 0,0 };
	CameraSettings.handlePictureSettings(id, &qcmos);
	if (CameraSettings.getSettings().picsPerRepetition == 1 || CameraSettings.getPicsPerRepManual())
	{
		pics.setSinglePicture(this, CameraSettings.readImageParameters(this));
	}
	else
	{
		pics.setMultiplePictures(this, CameraSettings.readImageParameters(this),
			CameraSettings.getSettings().picsPerRepetition);
	}
	pics.resetPictureStorage();
	std::array<int, 4> nums = CameraSettings.getPaletteNumbers();
	pics.setPalletes(nums);

	CRect rect;
	GetWindowRect(&rect);
	OnSize(0, rect.right - rect.left, rect.bottom - rect.top);
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


BOOL CameraWindow::PreTranslateMessage(MSG* pMsg)
{
	for (UINT toolTipInc = 0; toolTipInc < tooltips.size(); toolTipInc++)
	{
		tooltips[toolTipInc]->RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CameraWindow::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* scrollbar)
{
	pics.handleScroll(nSBCode, nPos, scrollbar);
}


void CameraWindow::OnSize(UINT nType, int cx, int cy)
{
	SetRedraw(false);
	qcmosRunSettings settings = CameraSettings.getSettings();
	stats.rearrange(settings.cameraMode, settings.triggerMode, cx, cy, mainWindowFriend->getFonts());
	CameraSettings.rearrange(settings.cameraMode, settings.triggerMode, cx, cy, mainWindowFriend->getFonts());
	box.rearrange(cx, cy, mainWindowFriend->getFonts());
	pics.rearrange(settings.cameraMode, settings.triggerMode, cx, cy, mainWindowFriend->getFonts());
	alerts.rearrange(settings.cameraMode, settings.triggerMode, cx, cy, mainWindowFriend->getFonts());
	analysisHandler.rearrange(settings.cameraMode, settings.triggerMode, cx, cy, mainWindowFriend->getFonts());
	pics.setParameters(CameraSettings.readImageParameters(this));
	CDC* dc = GetDC();
	try
	{
		pics.redrawPictures(dc, selectedPixel, analysisHandler.getAnalysisLocs(), analysisHandler.getAtomGrid(), qcmos.picturesToDraw);
	}
	catch (Error& err)
	{
		mainWindowFriend->getComm()->sendError(err.what());
	}
	ReleaseDC(dc);
	timer.rearrange(settings.cameraMode, settings.triggerMode, cx, cy, mainWindowFriend->getFonts());
	SetRedraw();
	RedrawWindow();
}


void CameraWindow::setEmGain()
{
	try
	{
		CameraSettings.setEmGain(&qcmos);
	}
	catch (Error& exception)
	{
		errBox(exception.what());
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void CameraWindow::handleMasterConfigSave(std::stringstream& configStream)
{
	imageParameters settings = CameraSettings.getSettings().imageSettings;
	configStream << settings.left << " " << settings.right << " " << settings.horizontalBinning << " ";
	configStream << settings.bottom << " " << settings.top << " " << settings.verticalBinning << "\n";
}


void CameraWindow::handleMasterConfigOpen(std::stringstream& configStream, double version)
{
	mainWindowFriend->updateConfigurationSavedStatus(false);
	imageParameters settings = CameraSettings.getSettings().imageSettings;
	selectedPixel = { 0,0 };
	std::string tempStr;
	try
	{
		configStream >> tempStr;
		settings.left = std::stol(tempStr);
		configStream >> tempStr;
		settings.right = std::stol(tempStr);
		configStream >> tempStr;
		settings.horizontalBinning = std::stol(tempStr);
		configStream >> tempStr;
		settings.bottom = std::stol(tempStr);
		configStream >> tempStr;
		settings.top = std::stol(tempStr);
		configStream >> tempStr;
		settings.verticalBinning = std::stol(tempStr);
		settings.width = (settings.right - settings.left + 1) / settings.horizontalBinning;
		settings.height = (settings.bottom - settings.top + 1) / settings.verticalBinning;

		CameraSettings.setImageParameters(settings, this);
		pics.setParameters(settings);
		redrawPictures(true);
	}
	catch (std::invalid_argument&)
	{
		thrower("ERROR: Bad value seen in master configueration file while attempting to load camera settings!");
	}
}


DataLogger* CameraWindow::getLogger()
{
	return &dataHandler;
}

void CameraWindow::prepareCamera(ExperimentInput& input)
{
	if (qcmos.isRunning())
	{
		thrower("Camera is already running! Please Abort to restart.\r\n");
	}
	if (analysisHandler.getLocationSettingStatus())
	{
		thrower("Please finish selecting analysis points before starting the camera!\r\n");
	}
	// make sure it's idle.
	try
	{
		int32 status;
		qcmos.queryStatus(status);
		if (HAM_SAFEMODE)
		{
			thrower("DRV_IDLE");
		}
	}
	catch (Error& exception)
	{
		if (exception.whatBare() != str(DCAMCAP_STATUS_READY))
		{
			throw;
		}
	}

	CDC* dc = GetDC();
	pics.refreshBackgrounds(dc);
	ReleaseDC(dc);
	// I used to mandate use of a button to change image parameters. Now I don't have the button and just always 
	// update at this point.
	readImageParameters();
	//
	CameraSettings.updateRunSettingsFromPicSettings();
	CameraSettings.updatePassivelySetSettings();
	pics.setNumberPicturesActive(CameraSettings.getSettings().picsPerRepetition, CameraSettings.getPicsPerRepManual());
	// this is a bit awkward at the moment.

	CameraSettings.setRepsPerVariation(mainWindowFriend->getRepNumber());
	UINT varNumber = auxWindowFriend->getTotalVariationNumber();
	if (varNumber == 0)
	{
		// this means that the user isn't varying anything, so effectively this should be 1.
		varNumber = 1;
	}
	CameraSettings.setVariationNumber(varNumber);

	// biggest check here, camera settings includes a lot of things.
	CameraSettings.checkIfReady();
	input.camSettings = CameraSettings.getSettings();
	/// start the camera.
	qcmos.setSettings(input.camSettings);

	//get some info about andor settings
	float vss;
	float hss;
	float pgain;
	int numhspeeds;
	float fkvss;
	float fkexp;
	// compare to the Andor et functions to see which indices you should read
// 	qcmos.getVSSpeed(ANDOR_VSS_INDEX, &vss);
// 	qcmos.getHSSpeed(0, 0, ANDOR_HSS_INDEX, &hss);
// 	qcmos.getPreAmpGain(ANDOR_PREAMP_INDEX, pgain);
// 	qcmos.getNumberHSSpeeds(0, 0, &numhspeeds);
// 	qcmos.getFKVShiftSpeedF(ANDOR_VSS_INDEX, &fkvss);
// 	qcmos.getFKExposureTime(&fkexp);
// 	mainWindowFriend->getComm()->sendStatus("Vertical shift speed = " + str(vss) + " us\nHorizontal shift speed = " + str(hss) + " MHz\n"
// 		"Pre-amp gain = " + str(pgain) + "\n Fast Kinetics vertical shift speed = " + str(fkvss) + "us \n Fast Kinetics exposure time = " + str(fkexp) + "s \n\r");
}


void CameraWindow::startAtomCruncher(ExperimentInput& input)
{
	UINT atomCruncherID;
	atomCruncherThreadHandle = (HANDLE)_beginthreadex(0, 0, CameraWindow::atomCruncherProcedure,
		(void*)input.cruncherInput, 0, &atomCruncherID);
}


bool CameraWindow::wantsAutoPause()
{
	return alerts.wantsAutoPause();
}


void CameraWindow::preparePlotter(ExperimentInput& input)
{
	/// start the plotting thread.
	plotThreadActive = true;
	plotThreadAborting = false;
	imageQueue.clear();
	plotterAtomQueue.clear();
	input.plotterInput = new realTimePlotterInput;
	input.plotterInput->aborting = &plotThreadAborting;
	input.plotterInput->active = &plotThreadActive;
	input.plotterInput->imageQueue = &plotterPictureQueue;
	input.plotterInput->imageShape = CameraSettings.getSettings().imageSettings;
	input.plotterInput->picsPerVariation = mainWindowFriend->getRepNumber() * CameraSettings.getSettings().picsPerRepetition;
	input.plotterInput->variations = auxWindowFriend->getTotalVariationNumber();
	input.plotterInput->picsPerRep = CameraSettings.getSettings().picsPerRepetition;
	input.plotterInput->alertThreshold = alerts.getAlertThreshold();
	input.plotterInput->wantAlerts = alerts.alertsAreToBeUsed();
	input.plotterInput->comm = mainWindowFriend->getComm();
	input.plotterInput->plotLock = &plotLock;
	input.plotterInput->numberOfRunsToAverage = 5;
	input.plotterInput->plottingFrequency = analysisHandler.getPlotFreq();
	if (input.masterInput)
	{
		input.plotterInput->key = VariableSystem::getKeyValues(input.masterInput->variables);
	}
	else
	{
		std::vector<double> dummyKey;
		// make a large dummy array to be used. In principle if the users uses a plotter without a master thread for
		// a long time this could crash... 
		dummyKey.resize(1000);
		input.plotterInput->key = dummyKey;
	}
	input.plotterInput->atomQueue = &plotterAtomQueue;
	analysisHandler.fillPlotThreadInput(input.plotterInput);
}


void CameraWindow::startPlotterThread(ExperimentInput& input)
{
	UINT plottingThreadID;
	if ((input.plotterInput->atomGridInfo.topLeftCorner == coordinate(0, 0)
		&& input.plotterInput->analysisLocations.size() == 0) || input.plotterInput->plotInfo.size() == 0)
	{
		plotThreadActive = false;
	}
	else
	{
		if (input.camSettings.totalPicsInExperiment * input.plotterInput->analysisLocations.size()
			/ input.plotterInput->plottingFrequency > 1000)
		{
			infoBox("Warning: The number of pictures * points to analyze in the experiment is very large,"
				" and the plotting period is fairly small. Consider increasing the plotting period. ");
		}
		// start the plotting thread
		plotThreadActive = true;
		plotThreadHandle = (HANDLE)_beginthreadex(0, 0, DataAnalysisControl::plotterProcedure, (void*)input.plotterInput,
			0, &plottingThreadID);
	}
}


qcmosRunSettings CameraWindow::getRunSettings()
{
	return qcmos.getSettings();
}


void CameraWindow::prepareAtomCruncher(ExperimentInput& input)
{
	input.cruncherInput = new atomCruncher;
	input.cruncherInput->plotterActive = plotThreadActive;
	input.cruncherInput->imageDims = CameraSettings.getSettings().imageSettings;
	atomCrunchThreadActive = true;
	input.cruncherInput->plotterNeedsImages = input.plotterInput->needsCounts;
	input.cruncherInput->cruncherThreadActive = &atomCrunchThreadActive;
	skipNext = false;
	input.cruncherInput->skipNext = &skipNext;
	input.cruncherInput->imageQueue = &imageQueue;
	try {
		//load masks from .npy file
		cnpy::NpyArray arrMasks = cnpy::npy_load(MASKS_FILE_LOCATION);
		input.cruncherInput->masks = arrMasks.as_vec<int16>(); //load masks as a flattened list of longs (row major), passing by pointer to first element.
		input.cruncherInput->nMask = arrMasks.shape[0];
		input.cruncherInput->maskWidX = arrMasks.shape[2];
		input.cruncherInput->maskWidY = arrMasks.shape[1]; //Get np array dimensions

		cnpy::NpyArray arrCrops = cnpy::npy_load(MASKS_CROP_FILE_LOCATION);
		input.cruncherInput->masksCropOriginal = arrCrops.as_vec<int16>(); //This is flattened column major automatically for no goddamn reason.
		input.cruncherInput->masksCrop = arrCrops.as_vec<int16>(); //This is flattened column major automatically for no goddamn reason.
		cnpy::NpyArray arrBGImg = cnpy::npy_load(BG_IMAGE_FILE_LOCATION);
		input.cruncherInput->bgImg = arrBGImg.as_vec<long>(); //row major

		cnpy::NpyArray arrSubpixelMasks = cnpy::npy_load(SUBPIXELMASKS_FILE_LOCATION);
		input.cruncherInput->subpixelMasks = arrSubpixelMasks.as_vec<int16>();
		input.cruncherInput->nSubpixel = arrSubpixelMasks.shape[0];

		if (!AUTOALIGN_SAFEMODE) {//This is not used in Yb AS2411
			if ((arrSubpixelMasks.shape[2] != input.cruncherInput->imageDims.width) || (arrSubpixelMasks.shape[1] != input.cruncherInput->imageDims.height))
			{
				thrower("Subpixel mask dimensions do not match image dimensions.");
			}
			std::copy(
				input.cruncherInput->subpixelMasks.begin() + (arrSubpixelMasks.shape[1] * arrSubpixelMasks.shape[2]) * (arrSubpixelMasks.shape[0] / 2),
				input.cruncherInput->subpixelMasks.begin() + (arrSubpixelMasks.shape[1] * arrSubpixelMasks.shape[2]) * (arrSubpixelMasks.shape[0] / 2 + 1),
				std::back_inserter(input.cruncherInput->subpixelMaskSingle)
			); //copy central mask for convenience
		}
	}
	catch (const std::runtime_error& e) {
		errBox(e.what());
	}

	// options
	if (input.masterInput)
	{
		input.cruncherInput->gmoog = input.masterInput->gmoog; //TODO: make sure this is right.
		//input.cruncherInput->rearrangerActive = input.masterInput->rearrangeInfo.active; //211229 - rearrangeInfo depreciated.
		input.cruncherInput->rearrangerActive = input.masterInput->gmoog->rearrangerActive; //Now set rearranger active depending on gmoog settings.
		input.cruncherInput->autoTweezerOffsetActive = input.masterInput->gmoog->autoTweezerOffsetActive;
		if (input.cruncherInput->autoTweezerOffsetActive) //Ensure that first image in experiment has updated mask locations.
		{
			input.cruncherInput->offsetMasks((input.cruncherInput->gmoog->xPixelOffsetAuto), (input.cruncherInput->gmoog->yPixelOffsetAuto));
		}
	}
	else
	{
		input.cruncherInput->rearrangerActive = false;
		input.cruncherInput->autoTweezerOffsetActive = false;
	}
	atomCrunchThreadActive = input.cruncherInput->rearrangerActive;
	// locks
	input.cruncherInput->imageLock = &imageLock;
	input.cruncherInput->plotLock = &plotLock;
	input.cruncherInput->rearrangerLock = &rearrangerLock;
	// what the thread fills.
	plotterPictureQueue.clear();
	input.cruncherInput->plotterImageQueue = &plotterPictureQueue;
	plotterAtomQueue.clear();
	input.cruncherInput->plotterAtomQueue = &plotterAtomQueue;
	rearrangerAtomQueue.clear();
	input.cruncherInput->rearrangerAtomQueue = &rearrangerAtomQueue;
	atomArrayQueue.clear();
	input.cruncherInput->atomArrayQueue = &atomArrayQueue;
	xOffsetAutoQueue.clear();
	input.cruncherInput->xOffsetAutoQueue = &xOffsetAutoQueue;
	yOffsetAutoQueue.clear();
	input.cruncherInput->yOffsetAutoQueue = &yOffsetAutoQueue;

	input.cruncherInput->thresholds = CameraSettings.getThresholds();
	input.cruncherInput->picsPerRep = CameraSettings.getSettings().picsPerRepetition;
	input.cruncherInput->gridInfo = analysisHandler.getAtomGrid();
	input.cruncherInput->catchPicTime = &crunchSeesTimes;
	input.cruncherInput->finTime = &crunchFinTimes;
	input.cruncherInput->atomThresholdForSkip = mainWindowFriend->getMainOptions().atomThresholdForSkip;
	input.cruncherInput->rearrangerConditionWatcher = &rearrangerConditionVariable;
}

// should consider modifying so that it can use an array of locations. At the moment doesn't.
//UINT __stdcall CameraWindow::atomCruncherProcedure(void* inputPtr)
//{
//	atomCruncher* input = (atomCruncher*)inputPtr; 
//	std::vector<long> monitoredPixelIndecies;
//
//	if ( input->gridInfo.topLeftCorner == coordinate( 0, 0 ) )
//	{
//		return 0;
//	}
//	
//	for ( auto columnInc : range( input->gridInfo.width ) )
//	{
//		for ( auto rowInc : range( input->gridInfo.height ) )
//		{
//			ULONG pixelRow = (input->gridInfo.topLeftCorner.row - 1) + rowInc * input->gridInfo.pixelSpacing;
//			ULONG pixelColumn = (input->gridInfo.topLeftCorner.column - 1) + columnInc * input->gridInfo.pixelSpacing;
//			if ( pixelRow >= input->imageDims.height || pixelColumn >= input->imageDims.width )
//			{
//				errBox( "ERROR: Grid appears to include pixels outside the image frame! Not allowed, seen by atom "
//						"cruncher thread" );
//				return 0;
//			}
//			int index = ((input->imageDims.height - 1 - pixelRow) * input->imageDims.width + pixelColumn);
//			if ( index >= input->imageDims.width * input->imageDims.height )
//			{
//				// shouldn't happen after I finish debugging.
//				errBox( "ERROR: Math error! Somehow, the pixel indexes appear within bounds, but the calculated index"
//						" is larger than the image is!" );
//				return 0;
//			}
//			monitoredPixelIndecies.push_back( index );
//		}
//	}
//	UINT imageCount = 0;
//	// loop watching the image queue.
//	while (*input->cruncherThreadActive || input->imageQueue->size() != 0)
//	{
//		// if no images wait until images. Should probably change to be event based.
//		if (input->imageQueue->size() == 0)
//		{
//			continue;
//		}
//		if ( imageCount % 2 == 0 )
//		{
//			input->catchPicTime->push_back( std::chrono::high_resolution_clock::now( ) );
//		}
//		// only contains the counts for the pixels being monitored.
//		std::vector<long> tempImagePixels( monitoredPixelIndecies.size( ) );
//		// only contains the boolean true/false of whether an atom passed a threshold or not. 
//		std::vector<bool> tempAtomArray( monitoredPixelIndecies.size() );
//		UINT count = 0;
//		// scope for the lock_guard. I want to free the lock as soon as possible, so add extra small scope.
//		{
//			std::lock_guard<std::mutex> locker( *input->imageLock );				
//			for ( auto pixelIndex : monitoredPixelIndecies )
//			{
//				tempImagePixels[count] = (*input->imageQueue)[0][pixelIndex];
//				count++;
//			}
//		}
//		count = 0;
//		for ( auto& pix : tempImagePixels )
//		{
//			if ( pix >= input->thresholds[imageCount % input->picsPerRep] )
//			{
//				tempAtomArray[count] = true;
//			}
//			count++;
//		}
//		if ( input->rearrangerActive )
//		{
//			// copies the array if first pic of rep. Only looks at first picture because its rearranging. Could change
//			// if we need to do funny experiments, just need to change rearranger handling.
//			if ( imageCount % input->picsPerRep == 0 )
//			{
//				{
//					std::lock_guard<std::mutex> locker( *input->rearrangerLock ); // When a lock_guard object is created, it attempts to take ownership of the mutex it is given. When control leaves the scope in which the lock_guard object was created, the lock_guard is destructed and the mutex is released.
//
//					(*input->rearrangerAtomQueue).push_back( tempAtomArray ); //put atom array in rearrange queue
//					input->rearrangerConditionWatcher->notify_all( );
//				}
//				input->finTime->push_back( std::chrono::high_resolution_clock::now( ) );
//			}
//		}
//		if (input->plotterActive)
//		{
//			// copies the array. Right now I'm assuming that the thread always needs atoms, which is not a good 
//			// assumption.
//			(*input->plotterAtomQueue).push_back(tempAtomArray);
//
//			if (input->plotterNeedsImages)
//			{
//				(*input->plotterImageQueue).push_back(tempImagePixels);
//			}
//		}
//		// if last picture of repetition
//		if ( imageCount % input->picsPerRep == input->picsPerRep - 1 )
//		{
//			UINT numAtoms = std::accumulate( tempAtomArray.begin( ), tempAtomArray.end( ), 0 );
//			if ( numAtoms >= input->atomThresholdForSkip )
//			{
//				*input->skipNext = true;
//			}
//			else
//			{
//				*input->skipNext = false;
//			}
//		}
//		imageCount++;
//		std::lock_guard<std::mutex> locker( *input->imageLock );
//		(*input->imageQueue).erase((*input->imageQueue).begin());		
//	}
//	return 0;
//}
UINT __stdcall CameraWindow::atomCruncherProcedure(void* inputPtr)
{
	atomCruncher* input = (atomCruncher*)inputPtr;
	std::vector<long> monitoredAtomIndecies;

	UINT imageCount = 0;
	// loop watching the image queue.
	while (*input->cruncherThreadActive || input->imageQueue->size() != 0)
	{
		// if no images wait until images. Should probably change to be event based.
		if (input->imageQueue->size() == 0)
		{
			continue;
		}
		if (imageCount % input->picsPerRep == 0)
		{
			input->catchPicTime->push_back(std::chrono::high_resolution_clock::now());
		}
		// only contains the counts for the ROIs being monitored.
		std::vector<long> tempImageROIs(input->nMask);
		// only contains the boolean true/false of whether an atom passed a threshold or not. 
		std::vector<bool> tempAtomArray(input->nMask);
		UINT count = 0;
		// scope for the lock_guard. I want to free the lock as soon as possible, so add extra small scope.
		{
			std::lock_guard<std::mutex> locker(*input->imageLock);
			for (size_t imask = 0; imask < input->nMask; imask++)
			{
				tempImageROIs[imask] = 0;
				//iterate from min to max x, y values in each mask
				for (size_t iy = 0; iy < input->maskWidY; iy++)
				{
					for (size_t ix = 0; ix < input->maskWidX; ix++)
					{
						//select appropriate pixel in image and mask and take product. Also subtracting background in this step - doing it all at once to avoid saving image twice, but also want to keep raw image for plotting.
						size_t indPixImg = (ix + input->masksCrop[4*imask + 2]) + (input->imageDims.width)*(iy + input->masksCrop[4*imask]);
						//size_t indPixImg = (ix + input->masksCrop[imask + 2 * input->nMask]) + (input->imageDims.width)*(iy + input->masksCrop[imask]); //column major indexing
						size_t indPixMask = (input->maskWidX)*(input->maskWidY)*imask + ix + iy * (input->maskWidX);
						try {

							tempImageROIs[imask] += ((*input->imageQueue)[0][indPixImg] - input->bgImg[indPixImg]) * (input->masks[indPixMask]);
						}

						catch (...) {
							//TODO: work                                                                                                                                                                                                                                                                                                                                                                                          out why this is failing intermittently in debug mode.
						}
					}
				}
			}
		}
		{
			size_t iroi = 0;
			input->nAtom = 0; //Keep track of how many atoms are loaded.
			for (auto& roi : tempImageROIs)
			{
				if (roi / 100 >= input->thresholds[imageCount % input->picsPerRep]) //factor of 100 due to normalization of masks.
				{
					tempAtomArray[iroi] = true;
					input->nAtom++;
				}
				else
				{
					tempAtomArray[iroi] = false;
				}
				iroi++;
			}
			//size_t iroi = 0;
			//input->nAtom = 16; //Keep track of how many atoms are loaded.
			//for (auto& roi : tempImageROIs)
			//{
			//	if (iroi % 2 == 0) //factor of 100 due to normalization of masks.
			//	{
			//		tempAtomArray[iroi] = true;
			//	}
			//	iroi++;
			//}
		}



		(*input->atomArrayQueue).push_back(tempAtomArray); //save processed image

		if (input->rearrangerActive)
		{
			// copies the array if first pic of rep. Only looks at first picture because its rearranging. Could change
			// if we need to do funny experiments, just need to change rearranger handling.
			if (imageCount % input->picsPerRep == 0)
			{
				{
					std::lock_guard<std::mutex> locker2(*input->imageLock);
					std::lock_guard<std::mutex> locker(*input->rearrangerLock); // When a lock_guard object is created, it attempts to take ownership of the mutex it is given. When control leaves the scope in which the lock_guard object was created, the lock_guard is destructed and the mutex is released.

					(*input->rearrangerAtomQueue).push_back(tempAtomArray); //put atom array in rearrange queue
					input->rearrangerConditionWatcher->notify_all();

					if (input->nAtom >= 0)//input->gmoog->targetNumber)
					{
						//try
						//{
							//REARRANGE
							//moveSequence moveseq = input->getRearrangeMoves();
							MessageSender ms;
							input->gmoog->writeOff(ms); //Important to start with load tones off, so that every tone has explicit settings.
							input->gmoog->writeLoad(ms); //Write load settings so that tweezers can be reset immediately after moves.
							input->gmoog->writeRearrangeMoves(input->getRearrangeMoves(input->gmoog->rearrangeMode), ms);
							input->gmoog->writeTerminator(ms);
							input->gmoog->send(ms);
						//}
						//catch (Error& exception)
						//{
						//	/*input->comm->sendError(exception.what());*/
						//	//TODO: add handling for reporting rearrangement errors.
						//}
					}
					//else if (input->nAtom == 0)//when there are no atom
					//{
					//	try
					//	{
					//		//REARRANGE
					//		//moveSequence moveseq = input->getRearrangeMoves();
					//		MessageSender ms;
					//		input->gmoog->writeOff(ms); //Important to start with load tones off, so that every tone has explicit settings.
					//		input->gmoog->writeZerotone(input->getRearrangeMoves(input->gmoog->rearrangeMode), ms);
					//		input->gmoog->writeTerminator(ms);
					//		input->gmoog->send(ms);
					//	}
					//	catch (Error& exception)
					//	{
					//		/*input->comm->sendError(exception.what());*/
					//		//TODO: add handling for reporting rearrangement errors.
					//	}
					//}
					(*input->rearrangerAtomQueue).clear(); //clear the rearrange queue once we've completed or ignored the rearrangement.

				}
				input->finTime->push_back(std::chrono::high_resolution_clock::now());
			}
		}
		if (imageCount % input->picsPerRep == 0) //Just always run this and store the measured values on first image. Control whether this gets applied in gmoog. input->autoTweezerOffsetActive
		{
			//if (input->nAtom >= 100) //enforce enough atoms for decent single shot signal.
			if (!AUTOALIGN_SAFEMODE) //always check, due to jumps by 1 lattice spacing. //Not used in Yb
			{
				std::lock_guard<std::mutex> locker(*input->imageLock);
				input->getTweezerOffset(&(input->gmoog->xPixelOffsetAuto), &(input->gmoog->yPixelOffsetAuto), &(input->gmoog->subpixelIndexOffsetAuto));
				input->gmoog->updateXYOffsetAuto();
				input->offsetMasks((input->gmoog->xPixelOffsetAuto), (input->gmoog->yPixelOffsetAuto));
			}
			(*input->xOffsetAutoQueue).push_back(input->gmoog->xOffsetAuto); //Always store value, so that offset list is up to date with reps.
			(*input->yOffsetAutoQueue).push_back(input->gmoog->yOffsetAuto);
		}
		if (input->plotterActive)
		{
			// copies the array. Right now I'm assuming that the thread always needs atoms, which is not a good 
			// assumption.
			(*input->plotterAtomQueue).push_back(tempAtomArray);

			if (input->plotterNeedsImages)
			{
				(*input->plotterImageQueue).push_back(tempImageROIs);
			}
		}
		// if last picture of repetition
		if (imageCount % input->picsPerRep == input->picsPerRep - 1)
		{
			UINT numAtoms = std::accumulate(tempAtomArray.begin(), tempAtomArray.end(), 0);
			if (numAtoms >= input->atomThresholdForSkip)
			{
				*input->skipNext = true;
			}
			else
			{
				*input->skipNext = false;
			}
		}
		imageCount++;
		std::lock_guard<std::mutex> locker(*input->imageLock);
		(*input->imageQueue).erase((*input->imageQueue).begin());
	}
	return 0;
}

std::string CameraWindow::getStartMessage()
{
	// get selected plots
	std::vector<std::string> plots = analysisHandler.getActivePlotList();
	imageParameters currentImageParameters = CameraSettings.readImageParameters(this);
	bool errCheck = false;
	for (UINT plotInc = 0; plotInc < plots.size(); plotInc++)
	{
		PlottingInfo tempInfoCheck(PLOT_FILES_SAVE_LOCATION + "\\" + plots[plotInc] + ".plot");
		if (tempInfoCheck.getPicNumber() != CameraSettings.getSettings().picsPerRepetition)
		{
			thrower("ERROR: one of the plots selected, " + plots[plotInc] + ", is not built for the currently "
				"selected number of pictures per experiment. Please revise either the current setting or the plot"
				" file.");
		}
		tempInfoCheck.setGroups(analysisHandler.getAnalysisLocs());
		std::vector<std::pair<UINT, UINT>> plotLocations = tempInfoCheck.getAllPixelLocations();
	}
	std::string dialogMsg;
	dialogMsg = "Starting Parameters:\r\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\r\n";
	dialogMsg += "Current Camera Temperature Setting: " + str(
		CameraSettings.getSettings().temperatureSetting) + "\r\n";
	dialogMsg += "Exposure Times: ";
	for (auto& time : CameraSettings.getSettings().exposureTimes)
	{
		dialogMsg += str(time * 1000) + ", ";
	}
	dialogMsg += "\r\n";
	dialogMsg += "Image Settings: " + str(currentImageParameters.left) + " - " + str(currentImageParameters.right) + ", "
		+ str(currentImageParameters.bottom) + " - " + str(currentImageParameters.top) + "\r\n";
	dialogMsg += "\r\n";
	dialogMsg += "Kintetic Cycle Time: " + str(CameraSettings.getSettings().kineticCycleTime) + "\r\n";
	dialogMsg += "Pictures per Repetition: " + str(CameraSettings.getSettings().picsPerRepetition) + "\r\n";
	dialogMsg += "Pictures per Variation: " + str(CameraSettings.getSettings().totalPicsInVariation) + "\r\n";
	dialogMsg += "Variations per Experiment: " + str(CameraSettings.getSettings().totalVariations) + "\r\n";
	dialogMsg += "Total Pictures per Experiment: " + str(CameraSettings.getSettings().totalPicsInExperiment) + "\r\n";
	dialogMsg += "Real-Time Atom Detection Thresholds: ";

	for (auto& threshold : CameraSettings.getThresholds())
	{
		dialogMsg += str(threshold) + ", ";
	}

	dialogMsg += "\r\n";
	dialogMsg += "Current Plotting Options: \r\n";

	for (UINT plotInc = 0; plotInc < plots.size(); plotInc++)
	{
		dialogMsg += "\t" + plots[plotInc] + "\r\n";
	}

	return dialogMsg;
}


void CameraWindow::fillMasterThreadInput(MasterThreadInput* input)
{
	input->atomQueueForRearrangement = &rearrangerAtomQueue;
	input->rearrangerLock = &rearrangerLock;
	input->andorsImageTimes = &imageTimes;
	input->grabTimes = &imageGrabTimes;
	input->conditionVariableForRearrangement = &rearrangerConditionVariable;
}


void CameraWindow::setTimerText(std::string timerText)
{
	timer.setTimerDisplay(timerText);
}


void CameraWindow::OnCancel()
{
	try
	{
		passCommonCommand(ID_FILE_MY_EXIT);
	}
	catch (Error& exception)
	{
		errBox(exception.what());
	}
}


cToolTips CameraWindow::getToolTips()
{
	return tooltips;
}


BOOL CameraWindow::OnInitDialog()
{
	// don't redraw until the first OnSize.
	SetRedraw(false);
	qcmos.initializeClass(mainWindowFriend->getComm(), &imageTimes);
	cameraPositions positions;
	// all of the initialization functions increment and use the id, so by the end it will be 3000 + # of controls.
	int id = 3000;
	positions.sPos = { 0, 0 };
	box.initialize(positions.sPos, id, this, 480, tooltips);
	positions.videoPos = positions.amPos = positions.seriesPos = positions.sPos;
	alerts.alertMainThread(0);
	alerts.initialize(positions, this, false, id, tooltips);
	analysisHandler.initialize(positions, id, this, tooltips, false, mainWindowFriend->getRgbs());
	CameraSettings.initialize(positions, id, this, tooltips);
	POINT position = { 480, 0 };
	stats.initialize(position, this, id, tooltips);
	positions.sPos = { 757, 0 };
	timer.initialize(positions, this, false, id, tooltips, mainWindowFriend->getRgbs());
	position = { 757, 40 };
	pics.initialize(position, this, id, tooltips, mainWindowFriend->getBrushes()["Dark Green"]);
	//
	pics.setSinglePicture(this, CameraSettings.readImageParameters(this));
	qcmos.setSettings(CameraSettings.getSettings());

	// load the menu
	menu.LoadMenu(IDR_MAIN_MENU);
	SetMenu(&menu);

	// final steps
	//ShowWindow( SW_MAXIMIZE );
	SetTimer(NULL, 1000, NULL);

	CRect rect;
	GetWindowRect(&rect);
	OnSize(0, rect.right - rect.left, rect.bottom - rect.top);
	

	//startFrameGrabberThread
	Communicator* comm = mainWindowFriend->getComm();
	threadInput.comm = comm;
	threadInput.qcmos = &qcmos;
	threadInput.cameraWindow = this;
	threadInput.spuriousWakeupHandler = false;
	// begin the camera wait thread.
	_beginthreadex(NULL, 0, &CameraWindow::frameGrabberThread, &threadInput, 0, &frameGrabberThreadID);
	return TRUE;
}


void CameraWindow::redrawPictures(bool andGrid)
{
	CDC* dc = GetDC();
	try
	{
		pics.refreshBackgrounds(dc);
		if (andGrid)
		{
			pics.drawGrids(dc);
		}
		ReleaseDC(dc);
	}
	catch (Error& err)
	{
		ReleaseDC(dc);
		mainWindowFriend->getComm()->sendError(err.what());
	}
	// currently don't attempt to redraw previous picture data.
}


HBRUSH CameraWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	brushMap brushes = mainWindowFriend->getBrushes();
	rgbMap rgbs = mainWindowFriend->getRgbs();
	HBRUSH result;
	int num = pWnd->GetDlgCtrlID();

	result = *CameraSettings.handleColor(num, pDC, mainWindowFriend->getBrushes(), mainWindowFriend->getRgbs());
	if (result) { return result; }

	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
	{
		CBrush* result = box.handleColoring(num, pDC, brushes, rgbs);
		if (result)
		{
			return *result;
		}
		else
		{
			pDC->SetTextColor(rgbs["theme foreground"]);
			pDC->SetBkColor(rgbs["theme BG1"]);
			return *brushes["theme BG1"];
		}
	}
	case CTLCOLOR_EDIT:
	{
		pDC->SetTextColor(rgbs["theme blue"]);
		pDC->SetBkColor(rgbs["theme input"]);
		return *brushes["theme input"];
	}
	case CTLCOLOR_LISTBOX:
	{
		pDC->SetTextColor(rgbs["theme foreground"]);
		pDC->SetBkColor(rgbs["theme input"]);
		return *brushes["theme input"];
	}
	default:
	{
		return *brushes["theme BG2"];
	}
	}
}


std::atomic<bool>* CameraWindow::getSkipNextAtomic()
{
	return &skipNext;
}


void CameraWindow::stopPlotter()
{
	plotThreadAborting = true;
}

void CameraWindow::passCommandsAndSettings(UINT id) {
	passPictureSettings(id);
	passCommonCommand(id);
}

void CameraWindow::passCommonCommand(UINT id)
{
	try
	{
		commonFunctions::handleCommonMessage(id, this, mainWindowFriend, scriptingWindowFriend, this,
			auxWindowFriend);
	}
	catch (Error& err)
	{
		// catch any extra errors that handleCommonMessage doesn't explicitly handle.
		errBox(err.what());
	}
}


// this is typically a little redundant to call, but can use to make sure things are set to off.
void CameraWindow::assertOff()
{
	CameraSettings.cameraIsOn(false);
	plotThreadActive = false;
	atomCrunchThreadActive = false;
}


void CameraWindow::readImageParameters()
{
	selectedPixel = { 0,0 };
	try
	{
		redrawPictures(false);
		imageParameters parameters = CameraSettings.readImageParameters(this);
		pics.setParameters(parameters);
	}
	catch (Error& exception)
	{
		//errBox( "Error!" );
		Communicator* comm = mainWindowFriend->getComm();
		comm->sendColorBox(Camera, 'R');
		comm->sendError(exception.whatStr() + "\r\n");
	}
	CDC* dc = GetDC();
	pics.drawGrids(dc);
	ReleaseDC(dc);
}

void CameraWindow::changeBoxColor(systemInfo<char> colors)
{
	box.changeColor(colors);
}