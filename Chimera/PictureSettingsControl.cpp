#include "stdafx.h"
#include "PictureSettingsControl.h"
// #include "Andor.h"
#include "qcmos.h"
#include "CameraSettingsControl.h"
#include "CameraWindow.h"
#include "Commctrl.h"
#include <boost/lexical_cast.hpp>
#include "Constants.h"

// void PictureSettingsControl::initialize(cameraPositions& pos, CWnd* parent, int& id)
// {
// 	picsPerRepetitionUnofficial = 1;

// 	// introducing things row by row
// 	/// Set Picture Options
// 	UINT count = 0;

// 	/// IMAGES PER REP CONTROL OVERRIDE

// 	picsPerRepLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + 200, pos.seriesPos.y + 25 };
// 	picsPerRepLabel.videoPos = { -1,-1,-1,-1 };
// 	picsPerRepLabel.amPos = { -1,-1,-1,-1 };
// 	picsPerRepLabel.Create("Images Per Repetition:", NORM_STATIC_OPTIONS, picsPerRepLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);

// 	picsPerRepToggle.seriesPos = { pos.seriesPos.x + 200, pos.seriesPos.y, pos.seriesPos.x + 220, pos.seriesPos.y + 25 };
// 	picsPerRepToggle.videoPos = { -1,-1,-1,-1 };
// 	picsPerRepToggle.amPos = { -1,-1,-1,-1 };
// 	picsPerRepToggle.Create("", NORM_CHECK_OPTIONS, picsPerRepLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
// 	picsPerRepToggle.SetCheck(0);

// 	picsPerRepEdit.seriesPos = { pos.seriesPos.x + 220, pos.seriesPos.y, pos.seriesPos.x + 480, pos.seriesPos.y + 25 };
// 	picsPerRepEdit.videoPos = { -1,-1,-1,-1 };
// 	picsPerRepEdit.amPos = { -1,-1,-1,-1 };
// 	picsPerRepEdit.Create(NORM_EDIT_OPTIONS, picsPerRepToggle.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
// 	picsPerRepEdit.SetWindowTextA("");

// 	pos.seriesPos.y += 25;
// 	pos.amPos.y += 25;
// 	pos.videoPos.y += 25;

// 	/// Picture Numbers
// 	pictureLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + 100, pos.seriesPos.y + 20 };
// 	pictureLabel.amPos = { pos.amPos.x, pos.amPos.y, pos.amPos.x + 100,	pos.amPos.y + 20 };
// 	pictureLabel.videoPos = { pos.videoPos.x, pos.videoPos.y, pos.videoPos.x + 100,	pos.videoPos.y + 20 };
// 	pictureLabel.Create("Picture #:", NORM_STATIC_OPTIONS, pictureLabel.seriesPos, parent,
// 		PICTURE_SETTINGS_ID_START + count++);

// 	for (int picInc = 0; picInc < 4; picInc++)
// 	{
// 		pictureNumbers[picInc].seriesPos = { pos.seriesPos.x + 100 + 95 * picInc, pos.seriesPos.y,
// 			pos.seriesPos.x + 100 + 95 * (picInc + 1), pos.seriesPos.y + 20 };
// 		pictureNumbers[picInc].amPos = { pos.amPos.x + 100 + 95 * picInc, pos.amPos.y, pos.amPos.x + 100 + 95 * (picInc + 1),
// 			pos.amPos.y + 20 };
// 		pictureNumbers[picInc].videoPos = { pos.videoPos.x + 100 + 95 * picInc, pos.videoPos.y,
// 			pos.videoPos.x + 100 + 95 * (picInc + 1), pos.videoPos.y + 20 };
// 		pictureNumbers[picInc].Create(cstr(picInc + 1), NORM_STATIC_OPTIONS,
// 			pictureNumbers[picInc].seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
// 	}
// 	pos.seriesPos.y += 20;
// 	pos.amPos.y += 20;
// 	pos.videoPos.y += 20;

// 	/// Total picture number
// 	totalPicNumberLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + 100, pos.seriesPos.y + 20 };
// 	totalPicNumberLabel.amPos = { pos.amPos.x, pos.amPos.y, pos.amPos.x + 100, pos.amPos.y + 20 };
// 	totalPicNumberLabel.videoPos = { pos.videoPos.x, pos.videoPos.y, pos.videoPos.x + 100, pos.videoPos.y + 20 };
// 	totalPicNumberLabel.Create("Total Picture #", NORM_STATIC_OPTIONS, totalPicNumberLabel.seriesPos, parent,
// 		PICTURE_SETTINGS_ID_START + count++);
// 	totalPicNumberLabel.fontType = SmallFont;
// 	for (int picInc = 0; picInc < 4; picInc++)
// 	{
// 		totalNumberChoice[picInc].seriesPos = { pos.seriesPos.x + 100 + 95 * picInc, pos.seriesPos.y,
// 			pos.seriesPos.x + 100 + 95 * (picInc + 1), pos.seriesPos.y + 20 };
// 		totalNumberChoice[picInc].amPos = { pos.amPos.x + 100 + 95 * picInc, pos.amPos.y, pos.amPos.x + 100 + 95 * (picInc + 1),
// 			pos.amPos.y + 20 };
// 		totalNumberChoice[picInc].videoPos = { pos.videoPos.x + 100 + 95 * picInc, pos.videoPos.y, pos.videoPos.x + 100 + 95 * (picInc + 1),
// 			pos.videoPos.y + 20 };
// 		if (picInc == 0)
// 		{
// 			// first of group
// 			totalNumberChoice[picInc].Create("", NORM_RADIO_OPTIONS | WS_GROUP, totalNumberChoice[picInc].seriesPos,
// 				parent, PICTURE_SETTINGS_ID_START + count++);
// 			totalNumberChoice[picInc].SetCheck(1);
// 		}
// 		else
// 		{
// 			// members of group.
// 			totalNumberChoice[picInc].Create("", NORM_RADIO_OPTIONS, totalNumberChoice[picInc].seriesPos, parent,
// 				PICTURE_SETTINGS_ID_START + count++);
// 		}
// 	}
// 	pos.seriesPos.y += 20;
// 	pos.amPos.y += 20;
// 	pos.videoPos.y += 20;

// 	/// Exposure Times
// 	exposureLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + 100, pos.seriesPos.y + 20 };
// 	exposureLabel.amPos = { pos.amPos.x, pos.amPos.y, pos.amPos.x + 100, pos.amPos.y + 20 };
// 	exposureLabel.videoPos = { pos.videoPos.x, pos.videoPos.y, pos.videoPos.x + 100, pos.videoPos.y + 20 };
// 	exposureLabel.Create("Exposure (ms):", NORM_STATIC_OPTIONS, exposureLabel.seriesPos, parent,
// 		PICTURE_SETTINGS_ID_START + count++);
// 	exposureLabel.fontType = SmallFont;
// 	exposureTimesUnofficial.resize(4);

// 	for (int picInc = 0; picInc < 4; picInc++)
// 	{
// 		exposureEdits[picInc].seriesPos = { pos.seriesPos.x + 100 + 95 * picInc, pos.seriesPos.y,
// 			pos.seriesPos.x + 100 + 95 * (picInc + 1), pos.seriesPos.y + 20 };
// 		exposureEdits[picInc].amPos = { pos.amPos.x + 100 + 95 * picInc, pos.amPos.y,
// 			pos.amPos.x + 100 + 95 * (picInc + 1), pos.amPos.y + 20 };
// 		exposureEdits[picInc].videoPos = { pos.videoPos.x + 100 + 95 * picInc, pos.videoPos.y,
// 			pos.videoPos.x + 100 + 95 * (picInc + 1), pos.videoPos.y + 20 };
// 		// first of group
// 		exposureEdits[picInc].Create(NORM_EDIT_OPTIONS, exposureEdits[picInc].seriesPos, parent,
// 			PICTURE_SETTINGS_ID_START + count++);
// 		exposureEdits[picInc].SetWindowTextA("26.0");
// 		exposureTimesUnofficial[picInc] = 26 / 1000.0f;
// 	}
// 	pos.seriesPos.y += 20;
// 	pos.amPos.y += 20;
// 	pos.videoPos.y += 20;

// 	/// Thresholds
// 	thresholdLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + 100, pos.seriesPos.y + 20 };
// 	thresholdLabel.amPos = { pos.amPos.x, pos.amPos.y, pos.amPos.x + 100, pos.amPos.y + 20 };
// 	thresholdLabel.videoPos = { pos.videoPos.x, pos.videoPos.y, pos.videoPos.x + 100, pos.videoPos.y + 20 };
// 	thresholdLabel.Create("Threshold (cts)", NORM_STATIC_OPTIONS, thresholdLabel.seriesPos, parent,
// 		PICTURE_SETTINGS_ID_START + count++);
// 	thresholdLabel.fontType = SmallFont;
// 	for (int picInc = 0; picInc < 4; picInc++)
// 	{
// 		thresholdEdits[picInc].seriesPos = { pos.seriesPos.x + 100 + 95 * picInc, pos.seriesPos.y,
// 			pos.seriesPos.x + 100 + 95 * (picInc + 1), pos.seriesPos.y + 20 };
// 		thresholdEdits[picInc].amPos = { pos.amPos.x + 100 + 95 * picInc, pos.amPos.y,
// 			pos.amPos.x + 100 + 95 * (picInc + 1), pos.amPos.y + 20 };
// 		thresholdEdits[picInc].videoPos = { pos.videoPos.x + 100 + 95 * picInc, pos.videoPos.y,
// 			pos.videoPos.x + 100 + 95 * (picInc + 1), pos.videoPos.y + 20 };
// 		// first of group
// 		thresholdEdits[picInc].Create(NORM_EDIT_OPTIONS, thresholdEdits[picInc].seriesPos, parent,
// 			PICTURE_SETTINGS_ID_START + count++);
// 		thresholdEdits[picInc].SetWindowTextA("100");
// 		thresholds[picInc] = 100;
// 	}
// 	pos.seriesPos.y += 20;
// 	pos.amPos.y += 20;
// 	pos.videoPos.y += 20;

// 	/// Yellow --> Blue Color
// 	colormapLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + 100, pos.seriesPos.y + 20 };
// 	colormapLabel.amPos = { pos.amPos.x, pos.amPos.y, pos.amPos.x + 100, pos.amPos.y + 20 };
// 	colormapLabel.videoPos = { pos.videoPos.x, pos.videoPos.y, pos.videoPos.x + 100, pos.videoPos.y + 20 };
// 	colormapLabel.Create("Virida", NORM_STATIC_OPTIONS, colormapLabel.seriesPos, parent,
// 		PICTURE_SETTINGS_ID_START + count++);


// 	/// The radio buttons
// 	for (int picInc = 0; picInc < 4; picInc++)
// 	{
// 		colormapCombos[picInc].seriesPos = { pos.seriesPos.x + 100 + 95 * picInc, pos.seriesPos.y,
// 			pos.seriesPos.x + 100 + 95 * (picInc + 1), pos.seriesPos.y + 100 };
// 		colormapCombos[picInc].amPos = { pos.amPos.x + 100 + 95 * picInc, pos.amPos.y,
// 			pos.amPos.x + 100 + 95 * (picInc + 1), pos.amPos.y + 100 };
// 		colormapCombos[picInc].videoPos = { pos.videoPos.x + 100 + 95 * picInc, pos.videoPos.y,
// 			pos.videoPos.x + 100 + 95 * (picInc + 1), pos.videoPos.y + 100 };
// 		colormapCombos[picInc].Create(NORM_COMBO_OPTIONS, colormapCombos[picInc].seriesPos, parent,
// 			PICTURE_SETTINGS_ID_START + count++);
// 		colormapCombos[picInc].AddString("Dark Verida");
// 		colormapCombos[picInc].AddString("Inferno");
// 		colormapCombos[picInc].AddString("Black & White");
// 		colormapCombos[picInc].SetCurSel(0);
// 		colors[picInc] = 2;
// 	}
// 	pos.seriesPos.y += 20;
// 	pos.amPos.y += 20;
// 	pos.videoPos.y += 20;
// 	// above, the total number was set to 1.
// 	enablePictureControls(0);
// 	disablePictureControls(1);
// 	disablePictureControls(2);
// 	disablePictureControls(3);
// }

void PictureSettingsControl::initialize(cameraPositions& pos, CWnd* parent, int& id)
{
    picsPerRepetitionUnofficial = 1;
	selectedDisplayCount = picsPerRepetitionUnofficial;

    // introducing things row by row
    /// Set Picture Options
    UINT count = 0;

    // Adjust the height increment for the new layout
    const int rowHeight = 25;
    const int colWidth = 120;

    /// IMAGES PER REP CONTROL OVERRIDE
    picsPerRepLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + colWidth, pos.seriesPos.y + rowHeight };
    picsPerRepLabel.videoPos = { -1, -1, -1, -1 };
    picsPerRepLabel.amPos = { -1, -1, -1, -1 };
    picsPerRepLabel.Create("Images Per Repetition:", NORM_STATIC_OPTIONS, picsPerRepLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);

    picsPerRepToggle.seriesPos = { pos.seriesPos.x + colWidth, pos.seriesPos.y, pos.seriesPos.x + colWidth + 20, pos.seriesPos.y + rowHeight };
    picsPerRepToggle.videoPos = { -1, -1, -1, -1 };
    picsPerRepToggle.amPos = { -1, -1, -1, -1 };
    picsPerRepToggle.Create("", NORM_CHECK_OPTIONS, picsPerRepLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
    picsPerRepToggle.SetCheck(0);

    picsPerRepEdit.seriesPos = { pos.seriesPos.x + colWidth + 20, pos.seriesPos.y, pos.seriesPos.x + colWidth + 260, pos.seriesPos.y + rowHeight };
    picsPerRepEdit.videoPos = { -1, -1, -1, -1 };
    picsPerRepEdit.amPos = { -1, -1, -1, -1 };
    picsPerRepEdit.Create(NORM_EDIT_OPTIONS, picsPerRepToggle.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
    picsPerRepEdit.SetWindowTextA("");

    pos.seriesPos.y += rowHeight;
    pos.amPos.y += rowHeight;
    pos.videoPos.y += rowHeight;

    /// Picture Numbers
    pictureLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + colWidth, pos.seriesPos.y + rowHeight };
    pictureLabel.amPos = { pos.amPos.x, pos.amPos.y, pos.amPos.x + colWidth, pos.amPos.y + rowHeight };
    pictureLabel.videoPos = { pos.videoPos.x, pos.videoPos.y, pos.videoPos.x + colWidth, pos.videoPos.y + rowHeight };
    pictureLabel.Create("Picture #:", NORM_STATIC_OPTIONS, pictureLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);

    for (int picInc = 0; picInc < totalMaxPictures; picInc++) // Change to 4 pictures
    {
        pictureNumbers[picInc].seriesPos = { pos.seriesPos.x, pos.seriesPos.y + rowHeight * (picInc + 1), pos.seriesPos.x + colWidth, pos.seriesPos.y + rowHeight * (picInc + 1) + rowHeight };
        pictureNumbers[picInc].amPos = { pos.amPos.x, pos.amPos.y + rowHeight * (picInc + 1), pos.amPos.x + colWidth, pos.amPos.y + rowHeight * (picInc + 1) + rowHeight };
        pictureNumbers[picInc].videoPos = { pos.videoPos.x, pos.videoPos.y + rowHeight * (picInc + 1), pos.videoPos.x + colWidth, pos.videoPos.y + rowHeight * (picInc + 1) + rowHeight };
        pictureNumbers[picInc].Create(cstr(picInc + 1), NORM_STATIC_OPTIONS, pictureNumbers[picInc].seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
    }

    /// Total picture number
    totalPicNumberLabel.seriesPos = { pos.seriesPos.x + colWidth, pos.seriesPos.y, pos.seriesPos.x + 2 * colWidth, pos.seriesPos.y + rowHeight };
    totalPicNumberLabel.amPos = { pos.amPos.x + colWidth, pos.amPos.y, pos.amPos.x + 2 * colWidth, pos.amPos.y + rowHeight };
    totalPicNumberLabel.videoPos = { pos.videoPos.x + colWidth, pos.videoPos.y, pos.videoPos.x + 2 * colWidth, pos.videoPos.y + rowHeight };
    totalPicNumberLabel.Create("Total Picture #", NORM_STATIC_OPTIONS, totalPicNumberLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
    totalPicNumberLabel.fontType = SmallFont;

    for (int picInc = 0; picInc < totalMaxPictures; picInc++) // Change to 4 pictures
    {
        totalNumberChoice[picInc].seriesPos = { pos.seriesPos.x + colWidth, pos.seriesPos.y + rowHeight * (picInc + 1), pos.seriesPos.x + 2 * colWidth, pos.seriesPos.y + rowHeight * (picInc + 1) + rowHeight };
        totalNumberChoice[picInc].amPos = { pos.amPos.x + colWidth, pos.amPos.y + rowHeight * (picInc + 1), pos.amPos.x + 2 * colWidth, pos.amPos.y + rowHeight * (picInc + 1) + rowHeight };
        totalNumberChoice[picInc].videoPos = { pos.videoPos.x + colWidth, pos.videoPos.y + rowHeight * (picInc + 1), pos.videoPos.x + 2 * colWidth, pos.videoPos.y + rowHeight * (picInc + 1) + rowHeight };
        if (picInc == 0)
        {
            // first of group
            totalNumberChoice[picInc].Create("", NORM_RADIO_OPTIONS | WS_GROUP, totalNumberChoice[picInc].seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
            totalNumberChoice[picInc].SetCheck(1);
        }
        else
        {
            // members of group.
            totalNumberChoice[picInc].Create("", NORM_RADIO_OPTIONS, totalNumberChoice[picInc].seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
        }
    }

    /// Exposure Times
    exposureLabel.seriesPos = { pos.seriesPos.x + 2 * colWidth, pos.seriesPos.y, pos.seriesPos.x + 3 * colWidth, pos.seriesPos.y + rowHeight };
    exposureLabel.amPos = { pos.amPos.x + 2 * colWidth, pos.amPos.y, pos.amPos.x + 3 * colWidth, pos.amPos.y + rowHeight };
    exposureLabel.videoPos = { pos.videoPos.x + 2 * colWidth, pos.videoPos.y, pos.videoPos.x + 3 * colWidth, pos.videoPos.y + rowHeight };
    exposureLabel.Create("Exposure (ms):", NORM_STATIC_OPTIONS, exposureLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
    exposureLabel.fontType = SmallFont;
    exposureTimesUnofficial.resize(totalMaxPictures);

    for (int picInc = 0; picInc < totalMaxPictures; picInc++) // Change to 4 pictures
    {
        exposureEdits[picInc].seriesPos = { pos.seriesPos.x + 2 * colWidth, pos.seriesPos.y + rowHeight * (picInc + 1), pos.seriesPos.x + 3 * colWidth, pos.seriesPos.y + rowHeight * (picInc + 1) + rowHeight };
        exposureEdits[picInc].amPos = { pos.amPos.x + 2 * colWidth, pos.amPos.y + rowHeight * (picInc + 1), pos.amPos.x + 3 * colWidth, pos.amPos.y + rowHeight * (picInc + 1) + rowHeight };
        exposureEdits[picInc].videoPos = { pos.videoPos.x + 2 * colWidth, pos.videoPos.y + rowHeight * (picInc + 1), pos.videoPos.x + 3 * colWidth, pos.videoPos.y + rowHeight * (picInc + 1) + rowHeight };
        exposureEdits[picInc].Create(NORM_EDIT_OPTIONS, exposureEdits[picInc].seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
        exposureEdits[picInc].SetWindowTextA("26.0");
        exposureTimesUnofficial[picInc] = 26 / 1000.0f;
    }

    /// Thresholds
    thresholdLabel.seriesPos = { pos.seriesPos.x + 3 * colWidth, pos.seriesPos.y, pos.seriesPos.x + 4 * colWidth, pos.seriesPos.y + rowHeight };
    thresholdLabel.amPos = { pos.amPos.x + 3 * colWidth, pos.amPos.y, pos.amPos.x + 4 * colWidth, pos.amPos.y + rowHeight };
    thresholdLabel.videoPos = { pos.videoPos.x + 3 * colWidth, pos.videoPos.y, pos.videoPos.x + 4 * colWidth, pos.videoPos.y + rowHeight };
    thresholdLabel.Create("Thresholds:", NORM_STATIC_OPTIONS, thresholdLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
    thresholdLabel.fontType = SmallFont;

    for (int picInc = 0; picInc < totalMaxPictures; picInc++) // Change to 4 pictures
    {
        thresholdEdits[picInc].seriesPos = { pos.seriesPos.x + 3 * colWidth, pos.seriesPos.y + rowHeight * (picInc + 1), pos.seriesPos.x + 4 * colWidth, pos.seriesPos.y + rowHeight * (picInc + 1) + rowHeight };
        thresholdEdits[picInc].amPos = { pos.amPos.x + 3 * colWidth, pos.amPos.y + rowHeight * (picInc + 1), pos.amPos.x + 4 * colWidth, pos.amPos.y + rowHeight * (picInc + 1) + rowHeight };
        thresholdEdits[picInc].videoPos = { pos.videoPos.x + 3 * colWidth, pos.videoPos.y + rowHeight * (picInc + 1), pos.videoPos.x + 4 * colWidth, pos.videoPos.y + rowHeight * (picInc + 1) + rowHeight };
        thresholdEdits[picInc].Create(NORM_EDIT_OPTIONS, thresholdEdits[picInc].seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
        thresholdEdits[picInc].SetWindowTextA("15");
        thresholds[picInc] = 15;
    }

//    // Colormap Label
// 	colormapLabel.seriesPos = { pos.seriesPos.x, pos.seriesPos.y, pos.seriesPos.x + colWidth, pos.seriesPos.y + rowHeight };
// 	colormapLabel.amPos = { pos.amPos.x, pos.amPos.y, pos.amPos.x + colWidth, pos.amPos.y + rowHeight };
// 	colormapLabel.videoPos = { pos.videoPos.x, pos.videoPos.y, pos.videoPos.x + colWidth, pos.videoPos.y + rowHeight };
// 	colormapLabel.Create("Virida", NORM_STATIC_OPTIONS, colormapLabel.seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);

// 	// Update positions for the next row
// 	pos.seriesPos.y += rowHeight;
// 	pos.amPos.y += rowHeight;
// 	pos.videoPos.y += rowHeight;

// 	// Loop for 4 pictures, creating individual combo boxes for each picture
// 	for (int picInc = 0; picInc < 4; picInc++) {

// 		// Colormap combo box for each picture row
// 		colormapCombos[picInc].seriesPos = { pos.seriesPos.x + 4*colWidth, pos.seriesPos.y + rowHeight * picInc, pos.seriesPos.x + colWidth * 5, pos.seriesPos.y + rowHeight + rowHeight * picInc };
// 		colormapCombos[picInc].amPos = { pos.amPos.x + 4*colWidth, pos.amPos.y + rowHeight * picInc, pos.amPos.x + colWidth * 5, pos.amPos.y + rowHeight + rowHeight * picInc };
// 		colormapCombos[picInc].videoPos = { pos.videoPos.x + 4*colWidth, pos.videoPos.y + rowHeight * picInc, pos.videoPos.x + colWidth * 5, pos.videoPos.y + rowHeight + rowHeight * picInc };
// 		colormapCombos[picInc].Create(NORM_COMBO_OPTIONS, colormapCombos[picInc].seriesPos, parent, PICTURE_SETTINGS_ID_START + count++);
// 		colormapCombos[picInc].AddString("Dark Verida");
// 		colormapCombos[picInc].AddString("Inferno");
// 		colormapCombos[picInc].AddString("Black & White");
// 		colormapCombos[picInc].SetCurSel(0);
// 		colors[picInc] = 2;
// 	}

    // Update pos.y to prevent overlap

	/// Total picture number
	int displayCount = 0;
    displayLabel.seriesPos = { pos.seriesPos.x + 4*colWidth, pos.seriesPos.y, pos.seriesPos.x + 5 * colWidth, pos.seriesPos.y + rowHeight };
    displayLabel.amPos = { pos.amPos.x + 4*colWidth, pos.amPos.y, pos.amPos.x + 5 * colWidth, pos.amPos.y + rowHeight };
    displayLabel.videoPos = { pos.videoPos.x + 4*colWidth, pos.videoPos.y, pos.videoPos.x + 5 * colWidth, pos.videoPos.y + rowHeight };
    displayLabel.Create("Display?", NORM_STATIC_OPTIONS, displayLabel.seriesPos, parent, DISPLAY_SETTINGS_ID_START + displayCount++);
    displayLabel.fontType = SmallFont;

    for (int picInc = 0; picInc < totalMaxPictures; picInc++) // Change to 4 pictures
    {
        displayChoice[picInc].seriesPos = { pos.seriesPos.x + 4*colWidth, pos.seriesPos.y + rowHeight * (picInc + 1), pos.seriesPos.x + 5 * colWidth, pos.seriesPos.y + rowHeight * (picInc + 1) + rowHeight };
        displayChoice[picInc].amPos = { pos.amPos.x + 4*colWidth, pos.amPos.y + rowHeight * (picInc + 1), pos.amPos.x + 5 * colWidth, pos.amPos.y + rowHeight * (picInc + 1) + rowHeight };
        displayChoice[picInc].videoPos = { pos.videoPos.x + 4*colWidth, pos.videoPos.y + rowHeight * (picInc + 1), pos.videoPos.x + 5 * colWidth, pos.videoPos.y + rowHeight * (picInc + 1) + rowHeight };
        if (picInc == 0)
        {
            // first of group
            displayChoice[picInc].Create("", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, displayChoice[picInc].seriesPos, parent, DISPLAY_SETTINGS_ID_START + displayCount++);
            displayChoice[picInc].SetCheck(1);
        }
        else
        {
            // members of group.
            displayChoice[picInc].Create("", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, displayChoice[picInc].seriesPos, parent, DISPLAY_SETTINGS_ID_START + displayCount++);
        }


    }

    pos.seriesPos.y += rowHeight * (totalMaxPictures+1); // Adjust based on total rows created
    pos.amPos.y += rowHeight * (totalMaxPictures+1);
    pos.videoPos.y += rowHeight * (totalMaxPictures+1);
}


// Event handler to update the number of selected checkboxes
void PictureSettingsControl::OnDisplayCheckboxClicked(int picInc, qcmosCamera* qcmosObj)
{
    if (displayChoice[picInc].GetCheck() == 1) {
        // If checkbox is checked, increase count
        selectedDisplayCount++;
    } else {
        // If checkbox is unchecked, decrease count
        selectedDisplayCount--;
    }

    // Limit the number of selected checkboxes to 4
    if (selectedDisplayCount > 4) {
        // If more than 4 are selected, uncheck the current box
        displayChoice[picInc].SetCheck(0);
        selectedDisplayCount--;  // Revert the count
    }
	int idx = 0;
	for (int i = 0; i<totalMaxPictures; i++)
	{
		if (displayChoice[i].GetCheck() == 1)
		{
			qcmosObj->picturesToDraw[idx] = i;
			idx++;
		}
	}
}



void PictureSettingsControl::handleNewConfig(std::ofstream& newFile)
{
	newFile << "PICTURE_SETTINGS\n";
	newFile << 1 << "\n";
	for (auto color : colors)
	{
		newFile << 0 << " ";
	}
	newFile << "\n";
	for (auto exposure : exposureTimesUnofficial)
	{
		// in seconds
		newFile << 0.025 << " ";
	}
	newFile << "\n";
	for (auto threshold : thresholds)
	{
		newFile << 200 << " ";
	}
	newFile << "\n";
	newFile << "END_PICTURE_SETTINGS\n";
}


void PictureSettingsControl::handleSaveConfig(std::ofstream& saveFile)
{
	saveFile << "PICTURE_SETTINGS\n";
	saveFile << picsPerRepManual << "\n";
	saveFile << picsPerRepetitionUnofficial << "\n";
	for (auto color : colors)
	{
		saveFile << color << " ";
	}
	saveFile << "\n";
	for (auto exposure : exposureTimesUnofficial)
	{
		saveFile << exposure << " ";
	}
	saveFile << "\n";
	for (auto threshold : thresholds)
	{
		saveFile << threshold << " ";
	}
	saveFile << "\n";
	saveFile << "END_PICTURE_SETTINGS\n";
}


void PictureSettingsControl::handleOpenConfig(std::ifstream& openFile, int versionMajor, int versionMinor, qcmosCamera* qcmosObj)
{
	ProfileSystem::checkDelimiterLine(openFile, "PICTURE_SETTINGS");
	UINT picsPerRep;
	if (versionMajor > 2) {
		openFile >> picsPerRepManual;
		openFile >> picsPerRep;
		picsPerRepEdit.SetWindowTextA(cstr(picsPerRep));
		picsPerRepToggle.SetCheck(picsPerRepManual);
		setUnofficialPicsPerRep(picsPerRep, qcmosObj);
	}
	else {
		openFile >> picsPerRep;
		setUnofficialPicsPerRep(picsPerRep, qcmosObj);
	}

	std::array<int, totalMaxPictures> fileThresholds;
	for (auto& color : colors)
	{
		openFile >> color;
	}
	for (auto& exposure : exposureTimesUnofficial)
	{
		openFile >> exposure;
	}
	for (auto& threshold : fileThresholds)
	{
		openFile >> threshold;
	}
	setExposureTimes(qcmosObj);
	setThresholds(fileThresholds);
	ProfileSystem::checkDelimiterLine(openFile, "END_PICTURE_SETTINGS");
}


void PictureSettingsControl::disablePictureControls(int pic)
{
	if (pic > totalMaxPictures-1)
	{
		return;
	}
	exposureEdits[pic].EnableWindow(0);
	thresholdEdits[pic].EnableWindow(0);
	//colormapCombos[pic].EnableWindow(0);
}


void PictureSettingsControl::enablePictureControls(int pic)
{
	if (pic > totalMaxPictures-1)
	{
		return;
	}
	exposureEdits[pic].EnableWindow();
	thresholdEdits[pic].EnableWindow();
	//colormapCombos[pic].EnableWindow();
}


CBrush* PictureSettingsControl::colorControls(int id, CDC* colorer, brushMap brushes, rgbMap rgbs)
{
	/// Exposures
	if (id >= exposureEdits.front().GetDlgCtrlID() && id <= exposureEdits.back().GetDlgCtrlID())
	{
		int picNum = id - exposureEdits.front().GetDlgCtrlID();
		if (!exposureEdits[picNum].IsWindowEnabled())
		{
			return NULL;
		}
		colorer->SetTextColor(rgbs["theme foreground"]);
		//TCHAR textEdit[256];
		CString text;
		exposureEdits[picNum].GetWindowTextA(text);
		double exposure;
		try
		{
			exposure = std::stof(str(text));// / 1000.0f;
			double dif = std::fabs(exposure / 1000.0 - exposureTimesUnofficial[picNum]);
			if (dif < 0.000000001)
			{
				// good.
				colorer->SetBkColor(rgbs["theme green"]);
				// catch change of color and redraw window.
				if (exposureEdits[picNum].colorState != 0)
				{
					exposureEdits[picNum].colorState = 0;
					exposureEdits[picNum].RedrawWindow();
				}
				return brushes["theme green"];
			}
		}
		catch (std::exception&)
		{
			// don't do anything with it.
		}
		colorer->SetBkColor(rgbs["Red"]);
		// catch change of color and redraw window.
		if (exposureEdits[picNum].colorState != 1)
		{
			exposureEdits[picNum].colorState = 1;
			exposureEdits[picNum].RedrawWindow();
		}
		return brushes["Red"];
	}
	/// Thresholds
	else if (id >= thresholdEdits.front().GetDlgCtrlID() && id <= thresholdEdits.back().GetDlgCtrlID())
	{
		int picNum = id - thresholdEdits.front().GetDlgCtrlID();
		if (!thresholdEdits[picNum].IsWindowEnabled())
		{
			return NULL;
		}
		colorer->SetTextColor(RGB(255, 255, 255));
		CString text;
		thresholdEdits[picNum].GetWindowTextA(text);
		int threshold;
		try
		{
			threshold = std::stoi(str(text));
			double dif = std::fabs(threshold - thresholds[picNum]);
			if (dif < 0.000000001)
			{
				// good.
				colorer->SetBkColor(rgbs["theme green"]);
				// catch change of color and redraw window.
				if (thresholdEdits[picNum].colorState != 0)
				{
					thresholdEdits[picNum].colorState = 0;
					thresholdEdits[picNum].RedrawWindow();
				}
				return brushes["theme green"];
			}
		}
		catch (std::exception&)
		{
			// don't do anything with it.
		}
		colorer->SetBkColor(rgbs["Red"]);
		// catch change of color and redraw window.
		if (exposureEdits[picNum].colorState != 1)
		{
			exposureEdits[picNum].colorState = 1;
			exposureEdits[picNum].RedrawWindow();
		}
		return brushes["Red"];
	}
	else
	{
		return NULL;
	}
}


UINT PictureSettingsControl::getPicsPerRepetition()
{
	CString txt;
	picsPerRepEdit.GetWindowTextA(txt);
	if (picsPerRepManual)
	{
		try
		{
			picsPerRepetitionUnofficial = boost::lexical_cast<UINT>(txt);
		}
		catch (boost::bad_lexical_cast& err)
		{
			thrower("Failed to convert pics per repetition to unsigned int!");
		}
		picsPerRepManual = true;
	}
	else
	{
		picsPerRepManual = false;
	}
	return picsPerRepetitionUnofficial;
}


void PictureSettingsControl::setUnofficialPicsPerRep(UINT picNum, qcmosCamera* qcmosObj)
{
	if (picsPerRepManual) {
		picsPerRepetitionUnofficial = getPicsPerRepetition();
		picNum = 1;
	}
	else {
		picsPerRepetitionUnofficial = picNum;
		picsPerRepEdit.SetWindowTextA(cstr(picNum));
	}
	for (int i = 0; i < picsPerRepetitionUnofficial; ++i)
	{
		if (i < 4)
		{
			if (displayChoice[i].GetCheck() == 1)
			{
				//
			}
			else
			{   if (selectedDisplayCount <= 4)
				{
					displayChoice[i].SetCheck(1);
					selectedDisplayCount += 1;
					qcmosObj->picturesToDraw = {0,1,2,3};
				}
			}
			
		}
	}
	for (int i = picsPerRepetitionUnofficial; i < totalMaxPictures; ++i)
	{
		if (displayChoice[i].GetCheck() == 1)
			{
				displayChoice[i].SetCheck(0);
				selectedDisplayCount -= 1;
			}
		
	}
	// not all settings are changed here, and some are used to recalculate totals.
	qcmosRunSettings settings = qcmosObj->getSettings();
	settings.picsPerRepetition = picsPerRepetitionUnofficial;
	settings.totalPicsInVariation = settings.picsPerRepetition  * settings.repetitionsPerVariation;
	if (settings.totalVariations * settings.totalPicsInVariation > INT_MAX)
	{
		thrower("ERROR: too many pictures to take! Maximum number of pictures possible is " + str(INT_MAX));
	}
	settings.totalPicsInExperiment = int(settings.totalVariations * settings.totalPicsInVariation);
	qcmosObj->setSettings(settings);
	for (UINT picInc = 0; picInc < totalMaxPictures; picInc++)
	{
		if (picInc < picNum)
		{
			enablePictureControls(picInc);
		}
		else
		{
			disablePictureControls(picInc);
		}
		if (picInc == picNum - 1)
		{
			totalNumberChoice[picInc].SetCheck(1);
		}
		else
		{
			totalNumberChoice[picInc].SetCheck(0);
		}
	}
}


void PictureSettingsControl::handleOptionChange(int id, qcmosCamera* qcmosObj)
{
	setPicsPerRepManual();
	if (id == picsPerRepToggle.GetDlgCtrlID()) {
		setUnofficialPicsPerRep(1, qcmosObj);
	}
	else if (id >= totalNumberChoice.front().GetDlgCtrlID() && id <= totalNumberChoice.back().GetDlgCtrlID())
	{
		int picNum = id - totalNumberChoice.front().GetDlgCtrlID();
		if (totalNumberChoice[picNum].GetCheck())
		{
			setUnofficialPicsPerRep(picNum + 1, qcmosObj);
		}
		// this message can weirdly get set after a configuration opens as well, it only means to set the number if the 
		// relevant button is now checked.
	}
	// else if (id >= colormapCombos[0].GetDlgCtrlID() && id <= colormapCombos[3].GetDlgCtrlID())
	// {
	// 	id -= colormapCombos[0].GetDlgCtrlID();
	// 	int color = colormapCombos[id].GetCurSel();
	// 	colors[id] = color;
	// }

	else if (id >= displayChoice[0].GetDlgCtrlID() && id <= displayChoice[totalMaxPictures-1].GetDlgCtrlID())
	{
		id -= displayChoice[0].GetDlgCtrlID();
		OnDisplayCheckboxClicked(id, qcmosObj);
    }
}


void PictureSettingsControl::setExposureTimes(qcmosCamera* qcmosObj)
{
	setExposureTimes(exposureTimesUnofficial, qcmosObj);
}


void PictureSettingsControl::setExposureTimes(std::vector<double>& times, qcmosCamera* qcmosObj)
{
	std::vector<double> exposuresToSet;
	exposuresToSet = times;
	if (picsPerRepManual) {
		exposuresToSet.resize(1);
	}
	else {
		exposuresToSet.resize(picsPerRepetitionUnofficial);
	}
	qcmosRunSettings settings = qcmosObj->getSettings();
	settings.exposureTimes = exposuresToSet;
	qcmosObj->setSettings(settings);
	// try to set this time.
	qcmosObj->setExposures();
	// now check actual times.
	try { parentSettingsControl->checkTimings(exposuresToSet); }
	catch (std::runtime_error&) { throw; }

	for (UINT exposureInc = 0; exposureInc < exposuresToSet.size(); exposureInc++)
	{
		exposureTimesUnofficial[exposureInc] = exposuresToSet[exposureInc];
	}

	if (exposureTimesUnofficial.size() <= 0)
	{
		// this shouldn't happen
		thrower("ERROR: reached bad location where exposure times was of zero size, but this should have been detected earlier in the "
			"code.");
	}
	// now output things.
	for (int exposureInc = 0; exposureInc < totalMaxPictures; exposureInc++)
	{
		exposureEdits[exposureInc].SetWindowTextA(cstr(this->exposureTimesUnofficial[exposureInc] * 1000));
	}
}



std::vector<double> PictureSettingsControl::getUsedExposureTimes()
{
	updateSettings();
	std::vector<double> usedTimes;
	usedTimes = exposureTimesUnofficial;

	if (picsPerRepManual) {
		usedTimes.resize(1); //Only use first exposure time if many images.
	}
	else {
		usedTimes.resize(picsPerRepetitionUnofficial);
	}

	return usedTimes;
}

/*
 * modifies values for exposures, accumlation time, kinetic cycle time as the qcmosObj camera reports them.
 */
void PictureSettingsControl::confirmAcquisitionTimings()
{
	std::vector<double> usedExposures;
	usedExposures = exposureTimesUnofficial;
	usedExposures.resize(picsPerRepetitionUnofficial);
	try
	{
		parentSettingsControl->checkTimings(usedExposures);
	}
	catch (std::runtime_error)
	{
		throw;
	}
	for (UINT exposureInc = 0; exposureInc < usedExposures.size(); exposureInc++)
	{
		exposureTimesUnofficial[exposureInc] = usedExposures[exposureInc];
	}
}

/**/
std::array<int, totalMaxPictures> PictureSettingsControl::getThresholds()
{
	updateSettings();
	return thresholds;
}

void PictureSettingsControl::setThresholds(std::array<int, totalMaxPictures> newThresholds)
{
	thresholds = newThresholds;
	for (UINT thresholdInc = 0; thresholdInc < thresholds.size(); thresholdInc++)
	{
		thresholdEdits[thresholdInc].SetWindowTextA(cstr(thresholds[thresholdInc]));
	}
}

void PictureSettingsControl::setPicsPerRepManual() {
	picsPerRepManual = picsPerRepToggle.GetCheck();
}

//void PictureSettingsControl::setPicturesPerExperiment(UINT pics, qcmosCamera* qcmosObj)
//{
//	if (pics > 4)
//	{
//		return;
//	}
//	picsPerRepetitionUnofficial = pics;
//	qcmosRunSettings settings = qcmosObj->getSettings();
//	settings.picsPerRepetition = picsPerRepetitionUnofficial;
//	settings.totalPicsInVariation = settings.picsPerRepetition  * settings.repetitionsPerVariation;
//	if (settings.totalVariations * settings.totalPicsInVariation > INT_MAX)
//	{
//		thrower( "ERROR: Trying to take too many pictures! Maximum picture number is " + str( INT_MAX ) );
//	}
//	settings.totalPicsInExperiment = int(settings.totalVariations * settings.totalPicsInVariation);
//	for (UINT picInc = 0; picInc < 4; picInc++)
//	{
//		if (picInc == pics - 1)
//		{
//			totalNumberChoice[picInc].SetCheck(1);
//		}
//		else
//		{
//			totalNumberChoice[picInc].SetCheck(0);
//		}
//
//		if (picInc < picsPerRepetitionUnofficial)
//		{
//			enablePictureControls(picInc);
//		}
//		else
//		{
//			disablePictureControls(picInc);
//		}
//	}
//}


/*
*/
std::array<int, 4> PictureSettingsControl::getPictureColors()
{
	return colors;
}


void PictureSettingsControl::updateSettings()
{
	// grab the thresholds
	for (int thresholdInc = 0; thresholdInc < 4; thresholdInc++)
	{
		CString textEdit;
		thresholdEdits[thresholdInc].GetWindowTextA(textEdit);
		int threshold;
		try
		{
			threshold = std::stoi(str(textEdit));
			thresholds[thresholdInc] = threshold;
		}
		catch (std::invalid_argument)
		{
			errBox("ERROR: failed to convert threshold number " + str(thresholdInc + 1) + " to an integer.");
		}
		thresholdEdits[thresholdInc].RedrawWindow();
	}
	// grab the exposures.
	for (int exposureInc = 0; exposureInc < 4; exposureInc++)
	{
		CString textEdit;
		exposureEdits[exposureInc].GetWindowTextA(textEdit);
		float exposure;
		try
		{
			exposure = std::stof(str(textEdit));
			exposureTimesUnofficial[exposureInc] = exposure / 1000.0f;
		}
		catch (std::invalid_argument)
		{
			errBox("ERROR: failed to convert exposure number " + str(exposureInc + 1) + " to an integer.");
		}
		// refresh for new color
		exposureEdits[exposureInc].RedrawWindow();
	}
	/// set the exposure times via qcmosObj
	//setExposureTimes( qcmosObj );
}


void PictureSettingsControl::rearrange(std::string cameraMode, std::string triggerMode, int width, int height,
	fontMap fonts)
{
	totalPicNumberLabel.rearrange(cameraMode, triggerMode, width, height, fonts);
	pictureLabel.rearrange(cameraMode, triggerMode, width, height, fonts);
	exposureLabel.rearrange(cameraMode, triggerMode, width, height, fonts);
	thresholdLabel.rearrange(cameraMode, triggerMode, width, height, fonts);
	displayLabel.rearrange(cameraMode, triggerMode, width, height, fonts);
	displayLabel.rearrange(cameraMode, triggerMode, width, height, fonts);
	colormapLabel.rearrange(cameraMode, triggerMode, width, height, fonts);
	picsPerRepLabel.rearrange(cameraMode, triggerMode, width, height, fonts);
	picsPerRepToggle.rearrange(cameraMode, triggerMode, width, height, fonts);
	picsPerRepEdit.rearrange(cameraMode, triggerMode, width, height, fonts);

	for (auto& control : pictureNumbers)
	{
		control.rearrange(cameraMode, triggerMode, width, height, fonts);
	}
	for (auto& control : totalNumberChoice)
	{
		control.rearrange(cameraMode, triggerMode, width, height, fonts);
	}
	for (auto& control : exposureEdits)
	{
		control.rearrange(cameraMode, triggerMode, width, height, fonts);
	}
	for (auto& control : thresholdEdits)
	{
		control.rearrange(cameraMode, triggerMode, width, height, fonts);
	}
	for (auto& control : colormapCombos)
	{
		control.rearrange(cameraMode, triggerMode, width, height, fonts);
	}
	for (auto& control : displayChoice)
	{
		control.rearrange(cameraMode, triggerMode, width, height, fonts);
	}
}