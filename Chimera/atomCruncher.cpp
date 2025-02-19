#include "stdafx.h"
#include "atomCruncher.h"

void atomCruncher::getTweezerOffset(int* xOffPixels, int* yOffPixels, int* indexSubpixelMask) {
	///modifies input pointers to pass newly computed offset values in units of pixels and subpixel mask offset. Also updates masksCrop appropriately.

	//First find brightest pixel There is probably a more efficient way of doing this.
	//shift masks w.r.t. image and see at which shifts the summed pixel*mask value is maximized (shift +- 2 pixels)

	std::vector<int> macroSignals;

	int indPixImg;
	int indPixMask;

	for (int yShift = -2; yShift < 3; yShift++) //TODO: for now range is just hard coded, 
	{
		for (int xShift = -2; xShift < 3; xShift++)
		{
			int integratedSignal = 0;
			//iterate over whole image - just throw out boundary when shifting.
			for (int iy = 0; iy < imageDims.height - abs(yShift); iy++)
			{
				for (int ix = 0; ix < imageDims.width - abs(xShift); ix++)
				{
					//select appropriate pixel in image and mask and take product. Also subtracting background in this step - doing it all at once to avoid saving image twice, but also want to keep raw image for plotting.

					if (xShift >= 0 && yShift >= 0) // Handle indexing for negative mask roll.
					{
						indPixImg = (ix + xShift) + (iy + yShift) * imageDims.width; //positive roll of masks means starting with offset on image indexing.
						indPixMask = (ix)+(iy)* imageDims.width;
					}
					else if (xShift >= 0 && yShift < 0)
					{
						indPixImg = (ix + xShift) + (iy)* imageDims.width;
						indPixMask = (ix)+(iy - yShift) * imageDims.width; //negative roll of masks means starting with offset on mask indexing.
					}
					else if (xShift < 0 && yShift >= 0)
					{
						indPixImg = (ix)+(iy + yShift) * imageDims.width;
						indPixMask = (ix - xShift) + (iy)* imageDims.width;
					}
					else if (xShift < 0 && yShift < 0)
					{
						indPixImg = (ix)+(iy)* imageDims.width;
						indPixMask = (ix - xShift) + (iy - yShift) * imageDims.width;
					}
					//int tmp = (*imageQueue)[0].size();
					integratedSignal += ((*imageQueue)[0][indPixImg] - bgImg[indPixImg]) * (subpixelMaskSingle[indPixMask]);
				}
			}
			macroSignals.push_back(integratedSignal);
		}
	}

	int maxSignalIndex = std::max_element(macroSignals.begin(), macroSignals.end()) - macroSignals.begin(); //find brightest signal
	*xOffPixels = maxSignalIndex % 5 - 2;
	*yOffPixels = maxSignalIndex / 5 - 2;

	//Recenter on next lattice site in lock range if getting close to the edge.
	//if (*xOffPixels == 2)
	//{
	//	*xOffPixels = -1;
	//}
	//else if (*xOffPixels == -2)
	//{
	//	*xOffPixels = 1;
	//}

	//if (*yOffPixels == 2)
	//{
	//	*yOffPixels = -1;
	//}
	//else if (*yOffPixels == -2)
	//{
	//	*yOffPixels = 1;
	//}

	std::vector<int> subpixelSignals;
	//Now given maximum pixel, repeat procedure for subpixel masks.
	for (int i = 0; i < nSubpixel; i++)
	{
		int integratedSignal = 0;
		//iterate over whole image - just throw out boundary when shifting.
		for (int iy = 0; iy < imageDims.height - abs(*yOffPixels); iy++)
		{
			for (int ix = 0; ix < imageDims.width - abs(*xOffPixels); ix++)
			{
				//select appropriate pixel in image and mask and take product. Also subtracting background in this step - doing it all at once to avoid saving image twice, but also want to keep raw image for plotting.

				if (*xOffPixels >= 0 && *yOffPixels >= 0) // Handle indexing for negative mask roll.
				{
					indPixImg = (ix + *xOffPixels) + (iy + *yOffPixels) * imageDims.width; //positive roll of masks means starting with offset on image indexing.
					indPixMask = (ix)+(iy)* imageDims.width + i * imageDims.width * imageDims.height;
				}
				else if (*xOffPixels >= 0 && yOffPixels < 0)
				{
					indPixImg = (ix + *xOffPixels) + (iy)* imageDims.width;
					indPixMask = (ix)+(iy - *yOffPixels) * imageDims.width + i * imageDims.width * imageDims.height; //negative roll of masks means starting with offset on mask indexing.
				}
				else if (*xOffPixels < 0 && *yOffPixels >= 0)
				{
					indPixImg = (ix)+(iy + *yOffPixels) * imageDims.width;
					indPixMask = (ix - *xOffPixels) + (iy)* imageDims.width + i * imageDims.width * imageDims.height;
				}
				else if (*xOffPixels < 0 && *yOffPixels < 0)
				{
					indPixImg = (ix)+(iy)* imageDims.width;
					indPixMask = (ix - *xOffPixels) + (iy - *yOffPixels) * imageDims.width + i * imageDims.width * imageDims.height;
				}

				integratedSignal += ((*imageQueue)[0][indPixImg] - bgImg[indPixImg]) * (subpixelMasks[indPixMask]);
			}
		}
		subpixelSignals.push_back(integratedSignal);
	}
	*indexSubpixelMask = std::max_element(subpixelSignals.begin(), subpixelSignals.end()) - subpixelSignals.begin(); //find brightest signal
}

void atomCruncher::offsetMasks(int xOffPixels, int yOffPixels) {
	///update masksCrop to offset masks by integer number of pixels.
	for (size_t i = 0; i < masksCrop.size() / 2; i++)
	{
		masksCrop[i] = masksCropOriginal[i] + yOffPixels;
	}
	for (size_t i = masksCrop.size() / 2; i < masksCrop.size(); i++)
	{
		masksCrop[i] = masksCropOriginal[i] + xOffPixels;
	}

	//size_t indPixImg = (ix + input->masksCrop[imask + 2 * input->nMask]) + (input->imageDims.width)*(iy + input->masksCrop[imask]); //column major indexing
}

void atomCruncher::filterAtomQueue(void) {
	//exclude atoms that were not on a target site.
	for (int i = 0; i < positions.size(); i++)
	{
		(*rearrangerAtomQueue)[0][i] = (*rearrangerAtomQueue)[0][i] * (positions)[i];
	}
}

void atomCruncher::scrunchX(moveSequence& moveseq, bool centered = false) {
	int iy = 0; //tone index (tot 48)
	int iyl = 0; //tweezer (mask) index ("1"s in inity)
	int wx = gmoog->nTweezerX; //number of tweezers (masks) -- refers initx
	for (auto const& channelBoolY : positionsY)
	{
		if (channelBoolY) //if there is a tweezer (mask) at that tone ("1" in inity)
		{
			moveSingle single;
			int ix = 0; //tone index (tot 48)
			int ixl = 0; //tweezer (mask) index ("1"s in initx)
			std::vector<int> ixs = {}; //array for tone indices that appear in initx
			for (auto const& channelBoolX : positionsX)
			{
				if (channelBoolX) //if there is a tweezer (mask) at that tone ("1" in initx)
				{
					if ((*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx]) { //if there is an atom loaded at that tone
						single.startAOX.push_back(ix); //Place tweezers on all atoms in row
						(*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx] = 0; //remove atom from pickup location in "atom-position tracker"
					}
					ixl++;
					ixs.push_back(ix);
				}
				ix++;
			}
			single.startAOY.push_back(iy); // Single tone in y
			single.endAOY.push_back(iy); // y does not move
			moveseq.moves.push_back(single);

			int nGap = 0; // gap in tweezers, counting from 1th tweezer (and last tweezer if centered) as defined in initx
			int nAtomsInRow = moveseq.moves.back().nx(); // number of loaded atoms in a row
			if (centered)
			{
				nGap = wx / 2 - gmoog->scrunchSpacing * (nAtomsInRow / 2);
				nGap = (nGap < 0) ? 0 : nGap;
			}
			for (int ix2 = 0; ix2 < nAtomsInRow; ix2++) //for an atom loaded
			{
				if (nGap + (gmoog->scrunchSpacing) * ix2 >= wx)
				{
					moveseq.moves.back().endAOX.push_back(-2); // remove atom from higher frequency side
				}
				else
				{
					moveseq.moves.back().endAOX.push_back(nGap + ixs[(gmoog->scrunchSpacing) * ix2]); // final tone in x
					(*rearrangerAtomQueue)[0][(wx - nGap) - (gmoog->scrunchSpacing) * ix2 - 1 + iyl * wx] = 1; // place atom in dropoff location in "atom-position tracker"
				}
			};
			iyl++;
		}
		iy++;
	}
}

void atomCruncher::moveUpX(moveSequence& moveseq) {
	int iy = 0; //tone index (tot 48)
	int iyl = 0; //tweezer (mask) index ("1"s in inity)
	int wx = gmoog->nTweezerX; //number of tweezers (masks) -- refers initx
	for (auto const& channelBoolY : positionsY)
	{
		if (channelBoolY) //if there is a tweezer (mask) at that tone ("1" in inity)
		{
			moveSingle single;
			int ix = 0; //tone index (tot 48)
			int ixl = 0; //tweezer (mask) index ("1"s in initx)
			std::vector<int> ixs = {}; //array for tone indices that appear in initx
			for (auto const& channelBoolX : positionsX)
			{
				if (channelBoolX) //if there is a tweezer (mask) at that tone ("1" in initx)
				{
					if ((*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx]) { //if there is an atom loaded at that tone
						single.startAOX.push_back(ix); //Place tweezers on all atoms in row
						(*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx] = 0; //remove atom from pickup location in "atom-position tracker"
					}
					ixl++;
					ixs.push_back(ix);
				}
				ix++;
			}
			single.startAOY.push_back(iy); // Single tone in y
			single.endAOY.push_back(iy); // y does not move
			moveseq.moves.push_back(single);

			int nAtomsInRow = moveseq.moves.back().nx(); // number of loaded atoms in a row
			
			for (int ix2 = 0; ix2 < nAtomsInRow; ix2++) //for an atom loaded
			{
				
				moveseq.moves.back().endAOX.push_back(ixs[ix2+1]); // final tone in x
				(*rearrangerAtomQueue)[0][wx - (ix2+1) - 1 + iyl * wx] = 1; // place atom in dropoff location in "atom-position tracker"
				
			};
			iyl++;
		}
		iy++;
	}
}

void atomCruncher::scrunchXTarget(moveSequence& moveseq) {
	UINT tx = (positionsX.size()); //number of possible tones (48)
	UINT ty = (positionsY.size()); //number of possible tones (48)
	int iy = 0; //tone index
	int iyl = 0; //tweezer (mask) index
	int wx = gmoog->nTweezerX; //number of tweezers (masks) -- refers initx
	for (auto const& channelBoolY : positionsY)
	{
		if (channelBoolY) //if there is a tweezer (mask) at that tone ("1" in inity)
		{
			moveSingle single;
			int ix = 0; //tone index
			int ixl = 0; //tweezer (mask) index
			int numxTarget = 0; //numer of atoms in target config in the current y row
			std::vector<int> ixs = {}; //array for tone indices that appear in initx
			for (auto const& channelBoolX : positionsX)
			{
				if (targetPositionsTemp[ix + tx * iy]) //if the current x tone is in the final target confirg
				{
					numxTarget++; 
				}
				if (channelBoolX) //if there is a tweezer (mask) at that tone ("1" in initx)
				{
					if ((*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx]) { //if there is an atom loaded at that tone
						single.startAOX.push_back(ix); //Place tweezers on all atoms in row
						(*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx] = 0; //remove atom from pickup location
					}
					ixl++;
					ixs.push_back(ix);
				}
				ix++;
			}
			single.startAOY.push_back(iy); // Single tone in y
			single.endAOY.push_back(iy); // y does not move
			moveseq.moves.push_back(single);

			int nAtomsInRow = moveseq.moves.back().nx(); //number of loaded atoms
			int ixTarget = 0; //target site index (target sites are labelled by one index -- it is a flattened 48x48 array)
			
			for (int ix2 = 0; ix2 < nAtomsInRow; ix2++) // for each loaded atom
			{
				if (numxTarget > 0) // if there are remianing target sites to be filled in this y row
				{
					while (targetPositionsTemp[ixTarget + tx * iy] == 0) { ixTarget++; } // iterate to next target site
					moveseq.moves.back().endAOX.push_back(ixTarget); // Move tweezer to target site
					targetPositionsTemp[ixTarget + tx * iy] = 0; // remove target site
					numxTarget--; // update the number of target sites remaining to be filled in this y row
					for (int ix3 = 0; ix3 < wx; ix3++)
					{
						if (ixs[ix3] == ixTarget) { //find which mask index the target site corresponds to
							(*rearrangerAtomQueue)[0][wx - ix3 - 1 + iyl * wx] = 1; // place atom in dropoff location in "atom-position tracker"
						} 
					}
				}
				else {
					moveseq.moves.back().endAOX.push_back(-2); // remove atom from higher frequency side
				}
			};
			iyl++;
		}
		iy++;
	}
}

void atomCruncher::scrunchXTargetKeep(moveSequence& moveseq) {
	UINT tx = (positionsX.size()); //number of possible tones (48)
	UINT ty = (positionsY.size()); //number of possible tones (48)
	int iy = 0; //tone index
	int iyl = 0; //tweezer (mask) index
	int wx = gmoog->nTweezerX; //number of tweezers (masks) -- refers initx
	for (auto const& channelBoolY : positionsY)
	{
		if (channelBoolY) //if there is a tweezer (mask) at that tone ("1" in inity)
		{
			moveSingle single;
			int ix = 0; //tone index
			int ixl = 0; //tweezer (mask) index
			int numxTarget = 0; //numer of atoms in target config in the current y row
			std::vector<int> ixs = {}; //array for tone indices that appear in initx
			for (auto const& channelBoolX : positionsX)
			{
				if (targetPositionsTemp[ix + tx * iy]) //if the current x tone is in the final target confirg
				{
					numxTarget++;
				}
				if (channelBoolX) //if there is a tweezer (mask) at that tone ("1" in initx)
				{
					if ((*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx]) { //if there is an atom loaded at that tone
						single.startAOX.push_back(ix); //Place tweezers on all atoms in row
						(*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx] = 0; //remove atom from pickup location
					}
					ixl++;
					ixs.push_back(ix);
				}
				ix++;
			}
			single.startAOY.push_back(iy); // Single tone in y
			single.endAOY.push_back(iy); // y does not move
			moveseq.moves.push_back(single);

			int nAtomsInRow = moveseq.moves.back().nx(); //number of loaded atoms
			int ixTarget = 0; //target site index (target sites are labelled by one index -- it is a flattened 48x48 array)

			for (int ix2 = 0; ix2 < nAtomsInRow; ix2++) // for each loaded atom
			{
				if (ix2 < (nAtomsInRow - numxTarget)/2)
				{
					moveseq.moves.back().endAOX.push_back(ixs[ix2]); // move unwanted atom to lower freq side
				}
				else if (numxTarget > 0) // if there are remianing target sites to be filled in this y row
				{
					while (targetPositionsTemp[ixTarget + tx * iy] == 0) { ixTarget++; } // iterate to next target site
					moveseq.moves.back().endAOX.push_back(ixTarget); // Move tweezer to target site
					targetPositionsTemp[ixTarget + tx * iy] = 0; // remove target site
					numxTarget--; // update the number of target sites remaining to be filled in this y row
					for (int ix3 = 0; ix3 < wx; ix3++)
					{
						if (ixs[ix3] == ixTarget) { //find which mask index the target site corresponds to
							(*rearrangerAtomQueue)[0][wx - ix3 - 1 + iyl * wx] = 1; // place atom in dropoff location in "atom-position tracker"
						}
					}
				}
				else {
					moveseq.moves.back().endAOX.push_back(ixs[wx-1] - (nAtomsInRow - ix2) + 1); // move unwanted atom to higher freq side
				}
			};
			iyl++;
		}
		iy++;
	}
}

void atomCruncher::scrunchY(moveSequence& moveseq, bool centered = false) { // NOT READY YET
	int ix = 0; //tone index (tot 48)
	int ixl = 0; //tweezer (mask) index ("1"s in initx)
	int wx = gmoog->nTweezerX; //number of tweezers (masks) -- refers initx
	int wy = gmoog->nTweezerY; //number of tweezers (masks) -- refers inity
	for (auto const& channelBoolX : positionsX)
	{
		if (channelBoolX) //if there is a tweezer (mask) at that tone ("1" in initx)
		{
			moveSingle single;
			int iy = 0; //tone index (tot 48)
			int iyl = 0; //tweezer (mask) index ("1"s in inity)
			std::vector<int> iys = {}; //array for tone indices that appear in inity
			for (auto const& channelBoolY : positionsY)
			{
				if (channelBoolY) //if there is a tweezer (mask) at that tone ("1" in inity)
				{
					if ((*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx]) { //if there is an atom loaded at that tone
						single.startAOY.push_back(iy); //Place tweezers on all atoms in row
						(*rearrangerAtomQueue)[0][wx - ixl - 1 + iyl * wx] = 0; //remove atom from pickup location in "atom-position tracker"
					}
					iyl++;
					iys.push_back(iy);
				}
				iy++;
			}
			single.startAOX.push_back(ix); //Single tone in x
			single.endAOX.push_back(ix); //y does not move
			moveseq.moves.push_back(single);

			int nGap = 0; // gap in tweezers, counting from 1th tweezer (and last tweezer if centered) as defined in inity
			int nAtomsInCol = moveseq.moves.back().ny(); // number of loaded atoms in a column
			if (centered)
			{
				nGap = wy / 2 - gmoog->scrunchSpacing * (nAtomsInCol / 2);
				nGap = (nGap < 0) ? 0 : nGap;
			}
			for (int iy2 = 0; iy2 < nAtomsInCol; iy2++) //for an atom loaded
			{
				if (nGap + (gmoog->scrunchSpacing) * iy2 >= wy)
				{
					moveseq.moves.back().endAOY.push_back(-2); // remove atom from higher frequency side
				}
				else
				{
					moveseq.moves.back().endAOY.push_back(nGap + iys[(gmoog->scrunchSpacing) * iy2]); // final tone in x
					(*rearrangerAtomQueue)[0][(wx + nGap * wx) - ixl - 1 + (gmoog->scrunchSpacing) *iy2 * wx] = 1; // place atom in dropoff location in "atom-position tracker"	

				}
			};
			ixl++;
		}
		ix++;
	}
}

void atomCruncher::feedBack(moveSequence& moveseq, bool centered = false) {
	int iy = 0;
	int iyl = 0;
	int nx = gmoog->nTweezerX;
	std::vector<int> ixs = {};
	for (auto const& channelBoolY : positionsY)
	{
		if (channelBoolY) //If first step, choose the rows with load tweezers. If not first step, choose the rows that atoms were scrunched to.
		{
			moveSingle single;
			int ix = 0;
			int ixl = 0;
			std::vector<int> ixs = {};
			for (auto const& channelBoolX : positionsX)
			{
				if (channelBoolX)
				{
					if ((*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx]) {
						single.startAOX.push_back(ix); //Place tweezers on all atoms in row
						(*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx] = 0; //remove atom from pickup location
					}
					else {
						single.startAOX.push_back(-ixl-1);
					}
					ixl++;
					ixs.push_back(ix);
				}
				ix++;
			}
			single.startAOY.push_back(iy); //Single tone in y
			single.endAOY.push_back(iy); //y does not move
			moveseq.moves.push_back(single);

			int nGap = 0;
			int nAtomsInRow = moveseq.moves.back().nx();
			if (centered)
			{
				nGap = positionsX.size() / 2 - gmoog->scrunchSpacing * (nAtomsInRow / 2);
				nGap = (nGap < 0) ? 0 : nGap;
			}
			for (int ix2 = 0; ix2 < nAtomsInRow; ix2++)
			{
				if (nGap + (gmoog->scrunchSpacing) * ix2 >= positionsX.size())
				{
					moveseq.moves.back().endAOX.push_back(-2); // remove atom from higher frequency side
				}
				else
				{
					moveseq.moves.back().endAOX.push_back(nGap + ixs[ix2]);
					(*rearrangerAtomQueue)[0][ix2 + iyl * nx] = 1; // place atom in dropoff location
				}
			};
			iyl++;
		}
		iy++;
	}
}

void atomCruncher::LightShifting(moveSequence& moveseq, bool centered = false) {
	int iy = 0;
	int iyl = 0;
	int nx = gmoog->nTweezerX;
	std::vector<int> ixs = {};
	for (auto const& channelBoolY : positionsY)
	{
		if (channelBoolY) //If first step, choose the rows with load tweezers. If not first step, choose the rows that atoms were scrunched to.
		{
			moveSingle single;
			int ix = 0;
			int ixl = 0;
			std::vector<int> ixs = {};
			for (auto const& channelBoolX : positionsX)
			{
				if (channelBoolX)
				{
					if ((*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx]) {
						single.startAOX.push_back(ix); //Place tweezers on all atoms in row
						(*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx] = 0; //remove atom from pickup location
					}
					ixl++;
					ixs.push_back(ix);
				}
				ix++;
			}
			single.startAOY.push_back(iy); //Single tone in y
			single.endAOY.push_back(iy); //y does not move
			moveseq.moves.push_back(single);

			iyl++;
		}
		iy++;
	}
}

void atomCruncher::scrunchYFixedLength(moveSequence& moveseq, int nPerColumn, bool centered = false) {
	int ix = 0;
	for (auto const& channelBoolX : positionsX)
	{
		if (channelBoolX) //If first step, choose the rows with load tweezers. If not first step, choose the rows that atoms were scrunched to.
		{
			moveSingle single;
			int iy = 0;
			for (auto const& channelBoolY : positionsY)
			{
				if (channelBoolY && (*rearrangerAtomQueue)[0][ix + (positionsX.size())*iy])
				{
					single.startAOY.push_back(iy); //Place tweezers on all atoms in row
					(*rearrangerAtomQueue)[0][ix + (positionsX.size())*iy] = 0; //remove atom from pickup location
				}
				iy++;
			}
			single.startAOX.push_back(ix); //Single tone in x
			single.endAOX.push_back(ix); //x does not move
			moveseq.moves.push_back(single);

			int nAtomsInRow = moveseq.moves.back().ny();
			int nGap = 0;
			if (centered)
			{
				nGap = positionsY.size() / 2 - gmoog->scrunchSpacing * (nPerColumn / 2);
				nGap = (nGap < 0) ? 0 : nGap;
			}
			for (int iy2 = 0; iy2 < nAtomsInRow; iy2++)
			{
				if (iy2 < nAtomsInRow - nPerColumn)
				{
					moveseq.moves.back().endAOY.push_back(-1); // remove atom from lower frequency side
				}
				else if (nGap + (gmoog->scrunchSpacing) * (iy2 - (nAtomsInRow - nPerColumn)) >= positionsY.size())
				{
					moveseq.moves.back().endAOY.push_back(-2); // remove atom from higher frequency side
				}
				else
				{
					moveseq.moves.back().endAOY.push_back(nGap + (gmoog->scrunchSpacing) * (iy2 - (nAtomsInRow - nPerColumn))); //Bunch up tweezers, offset to compensate for removed atoms.
					(*rearrangerAtomQueue)[0][ix + (positionsX.size())*(nGap + iy2 * (gmoog->scrunchSpacing))] = 1; //place atom in dropoff location
				}
			};
		}
		ix++;
	}
}

int atomCruncher::equalizeX(moveSequence& moveseq) { // I'm broken still, don't use me yet
	///Equalize the number of atoms in each row. Returns target atoms/row

	// Calculate mean atom number per row.
	int nxMean = 0;
	int iy = 0;
	int iyl = 0;
	int nx = gmoog->nTweezerX;
	std::vector<int> iys = {};
	std::vector<int> nxList;
	for (auto const& channelBoolY : positionsY)
	{
		if (channelBoolY)
		{
			int nrow = 0;
			int ix = 0;
			int ixl = 0;
			iys.push_back(iy);
			for (auto const& channelBoolX : positionsX)
			{
				if (channelBoolX)
				{
					if ((*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx]) {
						nrow++;
					}
				}
			}
			nxMean += nrow;
			nxList.push_back(nx);
			iyl++;
		}
		iy++;
	}
	nxMean /= (gmoog->nTweezerY);

	
	for (auto& element : nxList) // nxList now contains excess atoms.
		element -= nxMean;

	//iterate through rows, and move to adjacent row as needed.
	int iyTweezer = 0; //tweezer index
	iy = 0;
	iyl = 0;
	for (auto const& channelBoolY : positionsY)
	{
		if (channelBoolY)
		{
			int ix = 0;
			int ixl = 0;
			if (iyTweezer >= gmoog->nTweezerY - 1)
			{
				break; //end on second last load row.
			}
			if (nxList[iyTweezer] > 0) // move excess atoms out of row
			{
				moveSingle single;
				for (auto const& channelBoolX : positionsX)
				{
					if (channelBoolX)
					{
						if ((*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx]
							&& !((*rearrangerAtomQueue)[0][nx - ixl - 1 + (iyl + 1) * nx])) // move only if site in adjacent loaded row is empty.
						{
							single.startAOX.push_back(ix); //select atoms in column to move
							single.endAOX.push_back(ix);
							(*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx] = 0;
							(*rearrangerAtomQueue)[0][nx - ixl - 1 + (iyl + 1) * nx] = 1; //remove atom from pickup location and place in target
							nxList[iyTweezer]--;
							nxList[iyTweezer + 1]++; //keep track of column atom numbers
						}
						if (nxList[iyTweezer] <= 0)
						{
							break; //stop if all excess atoms have been moved.
						}
						ixl++;
					}
					ix++;
				}
				single.startAOY.push_back(iy); //Single tone in y
				single.endAOY.push_back(iys[iyl + 1]); //y moves to next load row
				if (single.nx() > 0) { moveseq.moves.push_back(single); }
			}

			ix = 0;
			ixl = 0;

			if (nxList[iyTweezer] < 0) // pull missing atoms into column
			{
				moveSingle single;
				for (auto const& channelBoolX : positionsX)
				{
					if (channelBoolX)
					{
						if (!(*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx]
							&& (*rearrangerAtomQueue)[0][nx - ixl - 1 + (iyl + 1) * nx]) // move only if site in adjacent loaded column is full.
						{
							single.startAOX.push_back(ix); //select atoms in column to move
							single.endAOX.push_back(ix);
							(*rearrangerAtomQueue)[0][nx - ixl - 1 + iyl * nx] = 1;
							(*rearrangerAtomQueue)[0][nx - ixl - 1 + (iyl + 1) * nx] = 0; //remove atom from pickup location and place in target
							nxList[iyTweezer]++;
							nxList[iyTweezer + 1]--; //keep track of column atom numbers
						}
						if (nxList[iyTweezer] >= 0)
						{
							break; //stop if all excess atoms have been moved.
						}
						ixl++;
					}
					ix++;
				}
				single.startAOX.push_back(iys[iyl + 1]); //y moves from next load row
				single.endAOX.push_back(iy); //Single tone in y
				if (single.nx() > 0) { moveseq.moves.push_back(single); }
			}
			iyl++;
		}
		iy++;
	}
	return nxMean;
}

int atomCruncher::equalizeY(moveSequence& moveseq) {
	///Equalize the number of atoms in each column. Returns target atoms/column

	// Calculate mean atom number per column.
	int nxMean = 0;
	std::vector<int> nxList;
	for (auto const& coordX : positionCoordinatesX)
	{
		int nx = 0;
		for (auto const& coordY : positionCoordinatesY)
		{
			if ((*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY])
				nx++;
		}
		nxMean += nx;
		nxList.push_back(nx);
	}
	nxMean /= (gmoog->nTweezerX);
	for (auto& element : nxList) // nxList now contains excess atoms.
		element -= nxMean;

	//iterate through columns, and move to adjacent column as needed.
	int ixTweezer = 0; //tweezer index, can step by multiple lattice sites.
	for (auto const& coordX : positionCoordinatesX)
	{
		if (ixTweezer >= positionCoordinatesX.size() - 1)
		{
			break; //end on second last load column.
		}
		if (nxList[ixTweezer] > 0) // move excess atoms out of column
		{
			moveSingle single;
			for (auto const& coordY : positionCoordinatesY)
			{
				if ((*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY]
					&& !((*rearrangerAtomQueue)[0][positionCoordinatesX[ixTweezer + 1] + (positionsX.size())*coordY])) // move only if site in adjacent loaded column is empty.
				{
					single.startAOY.push_back(coordY); //select atoms in column to move
					single.endAOY.push_back(coordY);
					(*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY] = 0;
					(*rearrangerAtomQueue)[0][positionCoordinatesX[ixTweezer + 1] + (positionsX.size())*coordY] = 1; //remove atom from pickup location and place in target
					nxList[ixTweezer]--;
					nxList[ixTweezer + 1]++; //keep track of column atom numbers
				}
				if (nxList[ixTweezer] <= 0)
				{
					break; //stop if all excess atoms have been moved.
				}
			}
			single.startAOX.push_back(coordX); //Single tone in x
			single.endAOX.push_back(positionCoordinatesX[ixTweezer + 1]); //x moves to next load column
			if (single.ny() > 0) { moveseq.moves.push_back(single); }
		}
		if (nxList[ixTweezer] < 0) // pull missing atoms into column
		{
			moveSingle single;
			for (auto const& coordY : positionCoordinatesY)
			{
				if (!(*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY]
					&& ((*rearrangerAtomQueue)[0][positionCoordinatesX[ixTweezer + 1] + (positionsX.size())*coordY])) // move only if site in adjacent loaded column is full.
				{
					single.startAOY.push_back(coordY); //select atoms in column to move
					single.endAOY.push_back(coordY);
					(*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY] = 1;
					(*rearrangerAtomQueue)[0][positionCoordinatesX[ixTweezer + 1] + (positionsX.size())*coordY] = 0; //remove atom from pickup location and place in target
					nxList[ixTweezer]++;
					nxList[ixTweezer + 1]--; //keep track of column atom numbers
				}
				if (nxList[ixTweezer] >= 0)
				{
					break; //stop if all excess atoms have been moved.
				}
			}
			single.startAOX.push_back(positionCoordinatesX[ixTweezer + 1]); //x moves from next load column
			single.endAOX.push_back(coordX); //Single tone in x
			if (single.ny() > 0) { moveseq.moves.push_back(single); }
		}
		ixTweezer++;
	}
	return nxMean;
}

void atomCruncher::scrunchYTarget(moveSequence& moveseq, bool constantMoves = false) {
	UINT wx = (positionsX.size()); //For convenience, could remove.
	UINT wy = (positionsY.size());

	int ix = 0;
	for (auto const& channelBoolX : positionsX)
	{
		if (channelBoolX)
		{
			int nxTarget = 0;
			for (int iy = 0; iy < wy; iy++) //count number of target sites in column.
			{
				if (targetPositionsTemp[ix + wx * iy])
				{
					nxTarget++;
				}
			}

			moveSingle single;
			int iy = 0;
			for (auto const& channelBoolY : positionsY)
			{
				if (channelBoolY && (*rearrangerAtomQueue)[0][ix + wx * iy])
				{
					single.startAOY.push_back(iy); //Place tweezers on all atoms in column
					(*rearrangerAtomQueue)[0][ix + wx * iy] = 0; //remove atom from pickup location
				}
				iy++;
			}
			single.startAOX.push_back(ix); //Single tone in x
			single.endAOX.push_back(ix); //x does not move
			if (single.ny() > 0 || constantMoves) { moveseq.moves.push_back(single); }

			int nAtomsInRow = moveseq.moves.back().ny();
			int iyTarget = 0;
			for (int iy2 = 0; iy2 < nAtomsInRow; iy2++)
			{
				if (iy2 < (nAtomsInRow - nxTarget) / 2)
				{
					//moveseq.moves.back().endAOY.push_back(-1); // remove atom from lower frequency side
					moveseq.moves.back().endAOY.push_back(iy2);
				}
				else if (nxTarget > 0)
				{
					while (targetPositionsTemp[ix + wx * iyTarget] == 0) { iyTarget++; } //iterate to next target site
					moveseq.moves.back().endAOY.push_back(iyTarget); //Move tweezer to target site
					(*rearrangerAtomQueue)[0][ix + wx * iyTarget] = 1; //place atom in dropoff location
					targetPositionsTemp[ix + wx * iyTarget] = 0; //remove target site
					nxTarget--;
				}
				else
				{
					//moveseq.moves.back().endAOY.push_back(-2); // remove atom from higher frequency side
					moveseq.moves.back().endAOY.push_back(wy - (nAtomsInRow - iy2) - 2);
				}
			};
		}
		ix++;
	}
}


//void atomCruncher::scrunchYTarget(moveSequence& moveseq, bool constantMoves = false) {
//	UINT wx = (positionsX.size()); //For convenience, could remove.
//	UINT wy = (positionsY.size());
//
//	int ix = 0;
//	for (auto const& channelBoolX : positionsX)
//	{
//		if (channelBoolX)
//		{
//			int nxTarget = 0;
//			for (int iy = 0; iy < wy; iy++) //count number of target sites in column.
//			{
//				if (targetPositionsTemp[ix + wx * iy])
//				{
//					nxTarget++;
//				}
//			}
//
//			moveSingle single;
//			int iy = 0;
//			for (auto const& channelBoolY : positionsY)
//			{
//				if (channelBoolY && (*rearrangerAtomQueue)[0][ix + wx * iy])
//				{
//					single.startAOY.push_back(iy); //Place tweezers on all atoms in column
//					(*rearrangerAtomQueue)[0][ix + wx * iy] = 0; //remove atom from pickup location
//				}
//				iy++;
//			}
//			single.startAOX.push_back(ix); //Single tone in x
//			single.endAOX.push_back(ix); //x does not move
//			if (single.ny() > 0 || constantMoves) { moveseq.moves.push_back(single); }
//
//			int nAtomsInRow = moveseq.moves.back().ny();
//			int iyTarget = 0;
//			for (int iy2 = 0; iy2 < nAtomsInRow; iy2++)
//			{
//				if (iy2 < (nAtomsInRow - nxTarget) / 2)
//				{
//					//moveseq.moves.back().endAOY.push_back(-1); // remove atom from lower frequency side
//					moveseq.moves.back().endAOY.push_back(iy2);
//				}
//				else if (nxTarget > 0)
//				{
//					while (targetPositionsTemp[ix + wx * iyTarget] == 0) { iyTarget++; } //iterate to next target site
//					moveseq.moves.back().endAOY.push_back(iyTarget); //Move tweezer to target site
//					(*rearrangerAtomQueue)[0][ix + wx * iyTarget] = 1; //place atom in dropoff location
//					targetPositionsTemp[ix + wx * iyTarget] = 0; //remove target site
//					nxTarget--;
//				}
//				else
//				{
//					//moveseq.moves.back().endAOY.push_back(-2); // remove atom from higher frequency side
//					moveseq.moves.back().endAOY.push_back(wy - (nAtomsInRow - iy2) - 2);
//				}
//			};
//		}
//		ix++;
//	}
//}


void atomCruncher::enoughY(moveSequence& moveseq, bool constantMoves = false) {
	///Ensure the number of atoms in each column is sufficient for target pattern.

	std::vector<int> nxList;
	int ix = 0;
	for (auto const& channelBoolX : positionsX)
	{
		if (channelBoolX)
		{
			int iy = 0;
			int nx = 0;
			for (auto const& channelBoolY : positionsY)
			{
				if (channelBoolY && (*rearrangerAtomQueue)[0][ix + (positionsX.size())*iy])
				{
					nx++; //count atoms present
				}
				if (targetPositionsTemp[ix + (positionsX.size())*iy])
				{
					nx--; //remove atoms that are needed in target state
				}
				iy++;
			}
			nxList.push_back(nx); // nxList contains excess atoms.
		}
		ix++;
	}

	//iterate through columns, and move to adjacent column as needed.
	int ixTweezer = 0; //tweezer index, can step by multiple lattice sites.
	for (auto const& coordX : positionCoordinatesX)
	{
		if (ixTweezer >= positionCoordinatesX.size() - 1)
		{
			break; //end on second last load column.
		}
		if (nxList[ixTweezer] > 0) // move excess atoms out of column if next column needs atoms.
		{
			moveSingle single;
			for (auto const& coordY : positionCoordinatesY)
			{
				if ((*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY]
					&& !((*rearrangerAtomQueue)[0][positionCoordinatesX[ixTweezer + 1] + (positionsX.size())*coordY])
					&& nxList[ixTweezer + 1] < 0) // move only if site in adjacent loaded column is empty, and adjacent column needs atoms.
				{
					single.startAOY.push_back(coordY); //select atoms in column to move
					single.endAOY.push_back(coordY);
					(*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY] = 0;
					(*rearrangerAtomQueue)[0][positionCoordinatesX[ixTweezer + 1] + (positionsX.size())*coordY] = 1; //remove atom from pickup location and place in target
					nxList[ixTweezer]--;
					nxList[ixTweezer + 1]++; //keep track of column atom numbers
				}
				if (nxList[ixTweezer] <= 0)
				{
					break; //stop if all excess atoms have been moved.
				}
			}
			single.startAOX.push_back(coordX); //Single tone in x
			single.endAOX.push_back(positionCoordinatesX[ixTweezer + 1]); //x moves to next load column
			if (single.ny() > 0 || constantMoves) { moveseq.moves.push_back(single); } //If constant number of moves are needed, always add the move, even if it's empty.
		}
		else if (nxList[ixTweezer] < 0) // pull missing atoms into column.
		{
			moveSingle single;
			for (auto const& coordY : positionCoordinatesY)
			{
				if (!(*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY]
					&& ((*rearrangerAtomQueue)[0][positionCoordinatesX[ixTweezer + 1] + (positionsX.size())*coordY])) // move only if site in adjacent loaded column is full.
				{
					single.startAOY.push_back(coordY); //select atoms in column to move
					single.endAOY.push_back(coordY);
					(*rearrangerAtomQueue)[0][coordX + (positionsX.size())*coordY] = 1;
					(*rearrangerAtomQueue)[0][positionCoordinatesX[ixTweezer + 1] + (positionsX.size())*coordY] = 0; //remove atom from pickup location and place in target
					nxList[ixTweezer]++;
					nxList[ixTweezer + 1]--; //keep track of column atom numbers
				}
				if (nxList[ixTweezer] >= 0)
				{
					break; //stop if all excess atoms have been moved.
				}
			}
			single.startAOX.push_back(positionCoordinatesX[ixTweezer + 1]); //x moves from next load column
			single.endAOX.push_back(coordX); //Single tone in x
			if (single.ny() > 0 || constantMoves) { moveseq.moves.push_back(single); }
		}
		else if (constantMoves)
		{
			moveSingle single;
			single.startAOX.push_back(coordX); //dummy move to maintain constant move number.
			single.endAOX.push_back(coordX);
			moveseq.moves.push_back(single);
		}
		ixTweezer++;
	}
}

void atomCruncher::compressX(moveSequence& moveseq) {
	///Change the spacing between columns of target pattern by moving entire columns at a time.

	std::vector<int> positionCoordinatesXtmp = positionCoordinatesX; //make a version of positionCoordinates that can be modified.
	int iCenter = positionCoordinatesX.size() / 2; //central load column index
	int coordXCenter = positionCoordinatesX[iCenter];

	//iterate through columns, and move to adjacent column as needed.
	int coordX = 0;
	int coordXtarget = 0;
	for (int i = 1; i < positionCoordinatesX.size(); i++) //iterate through all but central column
	{
		if (i + iCenter < positionCoordinatesX.size())
		{
			coordX = positionCoordinatesX[i + iCenter];
		}
		else
		{
			coordX = positionCoordinatesX[2 * iCenter - (i + 1)];
		}
		coordXtarget = coordXCenter + ((coordX - coordXCenter)*(gmoog->scrunchSpacing)) / 3; //Assuming load is 3x lattice spacing for now.
		moveSingle single;
		for (int iy = 0; iy < positionsY.size(); iy++)
		{
			if ((*rearrangerAtomQueue)[0][coordX + (positionsX.size())*iy]
				&& gmoog->targetPositions[coordX + (positionsX.size())*iy]) // tweezers on all atoms that occupy target sites in column
			{
				single.startAOY.push_back(iy); //select atoms in column to move
				single.endAOY.push_back(iy);
				(*rearrangerAtomQueue)[0][coordXtarget + (positionsX.size())*iy] = 1;
				(*rearrangerAtomQueue)[0][coordX + (positionsX.size())*iy] = 0; //remove atom from pickup location and place in target
			}
		}
		single.startAOX.push_back(coordX);
		single.endAOX.push_back(coordXtarget); //move entire column
		if (single.ny() > 0) { moveseq.moves.push_back(single); }
	}
}

void  atomCruncher::filterReservoir(moveSequence& moveseq) {
	///Place tweezers on all positions to be kept
	moveSingle single;
	int ix = 0;
	for (auto const& channelBoolX : gmoog->filterPositionsX) {
		if (channelBoolX)
		{
			single.startAOX.push_back(ix);
			single.endAOX.push_back(ix);
		}
		ix++;
	}

	int iy = 0;
	for (auto const& channelBoolY : gmoog->filterPositionsY) {
		if (channelBoolY)
		{
			single.startAOY.push_back(iy);
			single.endAOY.push_back(iy);
		}
		iy++;
	}

	moveseq.moves.push_back(single);
}

void  atomCruncher::initArray(moveSequence& moveseq) {
	///Place tweezers on all positions to be kept
	moveSingle single;
	int ix = 0;
	for (auto const& channelBoolX : gmoog->initialPositionsX) {
		if (channelBoolX)
		{
			single.startAOX.push_back(ix);
			single.endAOX.push_back(ix);
		}
		ix++;
	}

	int iy = 0;
	for (auto const& channelBoolY : gmoog->initialPositionsY) {
		if (channelBoolY)
		{
			single.startAOY.push_back(iy);
			single.endAOY.push_back(iy);
		}
		iy++;
	}

	moveseq.moves.push_back(single);
}

moveSequence atomCruncher::getRearrangeMoves(std::string rearrangeType) {
	// atomCruncher contains gmoog and images. Generate moves based on these.
	//input->gmoog->initialPositionsX;
	//input->gmoog->initialPositionsY;
	//input->rearrangerAtomQueue[0];
	//input->gmoog->targetPositions;
	//input->gmoog->targetNumber;
	//input->nAtom;

	moveSequence moveseq;

	positions = gmoog->initialPositions;
	positionsX = gmoog->initialPositionsX;
	positionsY = gmoog->initialPositionsY;
	targetPositionsTemp = gmoog->targetPositions; //Make a copy of the target positions that can be modified.

	//define coordinate representation for convenience.
	positionCoordinatesX.clear();
	positionCoordinatesY.clear();
	{
		int ix = 0;
		for (auto const& channelBoolX : positionsX)
		{
			if (channelBoolX) {
				positionCoordinatesX.push_back(ix);
			}
			ix++;
		}
		int iy = 0;
		for (auto const& channelBoolY : positionsY)
		{
			if (channelBoolY) {
				positionCoordinatesY.push_back(iy);
			}
			iy++;
		}
	}

	//filterAtomQueue();

	if (rearrangeType == "scrunchx")
	{
		scrunchX(moveseq);
	}
	//else if (rearrangeType == "equalizex") {
	//	equalizeX(moveseq);
	//}
	else if (rearrangeType == "arbscrunchx") {
		scrunchXTarget(moveseq);
	}
	else if (rearrangeType == "filterarbscrunchx") {
		scrunchXTargetKeep(moveseq);
		filterReservoir(moveseq);
	}
	else if (rearrangeType == "arbscrunchxlls") {
		scrunchXTarget(moveseq);
		filterReservoir(moveseq);
	}
	else if (rearrangeType == "arbscrunchxllsthermal") {
		scrunchXTarget(moveseq);
		initArray(moveseq);
		filterReservoir(moveseq);
	}
	//else if (rearrangeType == "feedback")
	//{
	//	feedBack(moveseq);
	//}
	//else if (rearrangeType == "scrunchy")
	//{
	//	scrunchY(moveseq);
	//}
	//else if (rearrangeType == "lightshifting")
	//{
	//	LightShifting(moveseq);
	//}
	//else if (rearrangeType == "equalscrunchy")
	//{
	//	// Not updated from Sr code
	//	/*int nPerColumn = equalizeY(moveseq);
	//	scrunchYFixedLength(moveseq, nPerColumn);*/ 
	//}
	//else if (rearrangeType == "equalcenterscrunchy")
	//{
	//	// Not updated from Sr code
	//	/*int nPerColumn = equalizeY(moveseq);
	//	scrunchYFixedLength(moveseq, nPerColumn, true);*/
	//}
	//else if (rearrangeType == "equalcenterscrunchyx")
	//{
	//	// Not updated from Sr code
	//	/*int nPerColumn = equalizeY(moveseq);
	//	scrunchYFixedLength(moveseq, nPerColumn, true);

	//	int nGap = positionsY.size() / 2 - gmoog->scrunchSpacing * (nPerColumn / 2);
	//	for (int iy = 0; iy < positionsY.size(); iy++)
	//	{
	//		positionsY[iy] = false;
	//		if (iy >= nGap && iy % (gmoog->scrunchSpacing) == 0 && iy < nGap + (gmoog->scrunchSpacing) * nPerColumn)
	//		{
	//			positionsY[iy] = true;
	//		}
	//	}

	//	scrunchX(moveseq, true);*/
	//}
	//else if (rearrangeType == "arbscrunchy")
	//{
	//	// Not updated from Sr code
	//	/*enoughY(moveseq, true);
	//	scrunchYTarget(moveseq, true);*/
	//}
	//else if (rearrangeType == "arbscrunchycompressx")
	//{
	//	// Not updated from Sr code
	//	/*enoughY(moveseq);
	//	scrunchYTarget(moveseq);
	//	compressX(moveseq);*/
	//}
	//else if (rearrangeType == "filterarbscrunchy")
	//{
	//	// Not updated from Sr code
	//	/*enoughY(moveseq, true);
	//	scrunchYTarget(moveseq, true);
	//	filterReservoir(moveseq);*/
	//}
	//else if (rearrangeType == "filterarbscrunchycompressx")
	//{
	//	// Not updated from Sr code
	//	/*enoughY(moveseq, true);
	//	scrunchYTarget(moveseq, true);
	//	filterReservoir(moveseq);
	//	compressX(moveseq);*/
	//}
	//else if (rearrangeType == "arbequalscrunchy")
	//{
	//	// Not updated from Sr code
	//	//int nPerColumn = equalizeY(moveseq);
	//	//scrunchYTarget(moveseq);
	//}
	//else if (rearrangeType == "centerscrunchx")
	//{
	//	// Not updated from Sr code
	//	//scrunchX(moveseq, true);
	//}
	//else if (rearrangeType == "centerscrunchy")
	//{
	//	// Not updated from Sr code
	//	//scrunchY(moveseq, true);
	//}
	//else if (rearrangeType == "scrunchxy")
	//{
	//	// Not updated from Sr code
	//	//scrunchX(moveseq);

	//	////Update initial atom locations appropriately.
	//	//int nAtomsInRow = gmoog->nTweezerX;
	//	//int nGap = 0;
	//	//for (int ix = 0; ix < positionsX.size(); ix++)
	//	//{
	//	//	positionsX[ix] = false;
	//	//	if (ix >= nGap && ix % (gmoog->scrunchSpacing) == 0 && ix < nGap + (gmoog->scrunchSpacing) * nAtomsInRow)
	//	//	{
	//	//		positionsX[ix] = true;
	//	//	}
	//	//}

	//	//scrunchY(moveseq);
	//}
	//else if (rearrangeType == "scrunchyx")
	//{
	//// Not updated from Sr code
	//	/*scrunchY(moveseq);

	//	int nAtomsInRow = gmoog->nTweezerY;
	//	int nGap = 0;
	//	for (int iy = 0; iy < positionsY.size(); iy++)
	//	{
	//		positionsY[iy] = false;
	//		if (iy >= nGap && iy % (gmoog->scrunchSpacing) == 0 && iy < nGap + (gmoog->scrunchSpacing) * nAtomsInRow)
	//		{
	//			positionsY[iy] = true;
	//		}
	//	}

	//	scrunchX(moveseq);*/
	//}
	//else if (rearrangeType == "centerscrunchyx") {
	//// Not updated from Sr code
	//	/*scrunchY(moveseq, true);

	//	int nAtomsInRow = gmoog->nTweezerY;
	//	int nGap = positionsY.size() / 2 - gmoog->scrunchSpacing * (nAtomsInRow / 2);
	//	for (int iy = 0; iy < positionsY.size(); iy++)
	//	{
	//		positionsY[iy] = false;
	//		if (iy >= nGap && iy % (gmoog->scrunchSpacing) == 0 && iy < nGap + (gmoog->scrunchSpacing) * nAtomsInRow)
	//		{
	//			positionsY[iy] = true;
	//		}
	//	}

	//	scrunchX(moveseq, true);*/
	//}
	//else if (rearrangeType == "equaltetris")
	//{
	//// Not updated from Sr code
	//	//int nPerColumn = equalizeY(moveseq);
	//	//scrunchYFixedLength(moveseq, nPerColumn);

	//	//UINT wx = (positionsX.size()); //For convenience, could remove.
	//	//UINT wy = (positionsY.size());

	//	////std::copy(gmoog->targetPositions.begin(), gmoog->targetPositions.end(), targetPositionsTemp);

	//	//std::vector<UINT8> rearrangerAtomVect(((*rearrangerAtomQueue)[0]).size()); //Dumb hacky fix.
	//	//for (int i = 0; i < rearrangerAtomVect.size(); i++)
	//	//{
	//	//	rearrangerAtomVect[i] = (*rearrangerAtomQueue)[0][i];
	//	//}

	//	////std::vector<bool> targetColumn(positionsY.size(), false);

	//	//int iySource = (gmoog->scrunchSpacing)*(nPerColumn - 1); //Start taking atoms from top-most scrunched row, iterate towards bottom.
	//	//int nRowSource = sourceRowSum(iySource, rearrangerAtomVect); //number of atoms remaining in source row
	//	//int iyTarget = wy - 1;

	//	//while (iyTarget >= 0 && iySource >= 0) //iterate through target rows from the opposite side from scrunch. iyTarget must be int so that it can go negative for this condition to fail.
	//	//{
	//	//	int ixTarget = 0;
	//	//	int ixSource = 0;
	//	//	moveSingle single;

	//	//	while (ixTarget < wx) //iterate through all target positions in row
	//	//	{
	//	//		if (targetPositionsTemp[ixTarget + wx * iyTarget]) //if atom required at target
	//	//		{
	//	//			if (nRowSource < 1) //check if a source atom is available in current source row
	//	//			{
	//	//				iySource -= (gmoog->scrunchSpacing); //if no atoms, move to the next source row and reset available atoms in row
	//	//				if (iySource >= 0)
	//	//				{
	//	//					nRowSource = sourceRowSum(iySource, rearrangerAtomVect);
	//	//				}
	//	//				break;
	//	//			}
	//	//			while (!rearrangerAtomVect[ixSource + wx * iySource]) //find next atom in source row. This should be guaranteed due to counting nRowSource.
	//	//			{
	//	//				ixSource++;
	//	//			}
	//	//			single.startAOX.push_back(ixSource);
	//	//			rearrangerAtomVect[ixSource + wx * iySource] = 0; //erase used source position.
	//	//			nRowSource--; //remove an atom from the source row

	//	//			single.endAOX.push_back(ixTarget); //place tweezer at desired final location.
	//	//			targetPositionsTemp[ixTarget + wx * iyTarget] = 0; //erase filled target position.
	//	//		}
	//	//		ixTarget++; //iterate through target positions if no atom needed, or atom has been placed in target site.
	//	//		//if no source, this loops breaks, and continues from the previous target position.
	//	//	}
	//	//	if (iySource < 0) //TODO: work out why this is getting triggered. Should always have enough atoms.
	//	//	{
	//	//		break;
	//	//	}
	//	//	if (single.nx() > 0)
	//	//	{
	//	//		single.startAOY.push_back(iySource); //tweezers at desired source row
	//	//		single.endAOY.push_back(iyTarget); //tweezers at desired target row

	//	//		moveseq.moves.push_back(single); //add the move to the sequence, which either fully populates a target row, or fully depletes a source row.
	//	//	}
	//	//	if (ixTarget >= wx) //only iterate to next target row if entire row has been populated/checked.
	//	//	{
	//	//		iyTarget--;
	//	//	}
	//	//}
	//}
	//else if (rearrangeType == "equaltetris2")
	//{
	//// Not updated from Sr code
	//	//int nPerColumn = equalizeY(moveseq);
	//	//scrunchYFixedLength(moveseq, nPerColumn);

	//	//UINT wx = (positionsX.size()); //For convenience, could remove.
	//	//UINT wy = (positionsY.size());

	//	////std::copy(gmoog->targetPositions.begin(), gmoog->targetPositions.end(), targetPositionsTemp);
	//	//targetPositionsTemp = gmoog->targetPositions; //Make a copy of the target positions that can be modified.

	//	//std::vector<UINT8> rearrangerAtomVect(((*rearrangerAtomQueue)[0]).size()); //Dumb hacky fix.
	//	//for (int i = 0; i < rearrangerAtomVect.size(); i++)
	//	//{
	//	//	rearrangerAtomVect[i] = (*rearrangerAtomQueue)[0][i];
	//	//}

	//	////std::vector<bool> targetColumn(positionsY.size(), false);

	//	//int iySource = (gmoog->scrunchSpacing)*(nPerColumn - 1); //Start taking atoms from top-most scrunched row, iterate towards bottom.
	//	//int nRowSource = sourceRowSum(iySource, rearrangerAtomVect); //number of atoms remaining in source row
	//	//int iyTarget = wy - 1;

	//	//while (iyTarget >= 0 && iySource >= 0) //iterate through target rows from the opposite side from scrunch. iyTarget must be int so that it can go negative for this condition to fail.
	//	//{
	//	//	int ixTarget = 0;
	//	//	int ixSource = 0;
	//	//	moveSingle fromReservoir, scrunch, toTarget;

	//	//	while (ixTarget < wx) //iterate through all target positions in row
	//	//	{
	//	//		if (targetPositionsTemp[ixTarget + wx * iyTarget]) //if atom required at target
	//	//		{
	//	//			if (nRowSource < 1) //check if a source atom is available in current source row
	//	//			{
	//	//				iySource -= (gmoog->scrunchSpacing); //if no atoms, move to the next source row and reset available atoms in row
	//	//				if (iySource >= 0)
	//	//				{
	//	//					nRowSource = sourceRowSum(iySource, rearrangerAtomVect);
	//	//				}
	//	//				break;
	//	//			}
	//	//			while (!rearrangerAtomVect[ixSource + wx * iySource]) //find next atom in source row. This should be guaranteed due to counting nRowSource.
	//	//			{
	//	//				ixSource++;
	//	//			}
	//	//			fromReservoir.startAOX.push_back(ixSource);
	//	//			fromReservoir.endAOX.push_back(ixSource);
	//	//			scrunch.startAOX.push_back(ixSource);

	//	//			rearrangerAtomVect[ixSource + wx * iySource] = 0; //erase used source position.
	//	//			nRowSource--; //remove an atom from the source row

	//	//			scrunch.endAOX.push_back(ixTarget); //place tweezer at desired final location.
	//	//			toTarget.startAOX.push_back(ixTarget);
	//	//			toTarget.endAOX.push_back(ixTarget);

	//	//			targetPositionsTemp[ixTarget + wx * iyTarget] = 0; //erase filled target position.
	//	//		}
	//	//		ixTarget++; //iterate through target positions if no atom needed, or atom has been placed in target site.
	//	//		//if no source, this loops breaks, and continues from the previous target position.
	//	//	}
	//	//	if (iySource < 0) //TODO: work out why this is getting triggered. Should always have enough atoms.
	//	//	{
	//	//		break;
	//	//	}
	//	//	if (fromReservoir.nx() > 0)
	//	//	{
	//	//		fromReservoir.startAOY.push_back(iySource); //tweezers at desired source row
	//	//		fromReservoir.endAOY.push_back(iySource + (iyTarget - iySource) / 2); //tweezers at desired target row

	//	//		scrunch.startAOY.push_back(iySource + (iyTarget - iySource) / 2);
	//	//		scrunch.endAOY.push_back(iySource + (iyTarget - iySource) / 2);

	//	//		toTarget.startAOY.push_back(iySource + (iyTarget - iySource) / 2);
	//	//		toTarget.endAOY.push_back(iyTarget);

	//	//		moveseq.moves.push_back(fromReservoir);
	//	//		moveseq.moves.push_back(scrunch);
	//	//		moveseq.moves.push_back(toTarget); //add the move to the sequence, which either fully populates a target row, or fully depletes a source row.
	//	//	}
	//	//	if (ixTarget >= wx) //only iterate to next target row if entire row has been populated/checked.
	//	//	{
	//	//		iyTarget--;
	//	//	}
	//	//}
	//}
	//else if (rearrangeType == "tetris")
	//{
	//// Not updated from Sr code
	//	//UINT wx = (positionsX.size()); //For convenience, could remove.
	//	//UINT wy = (positionsY.size());

	//	////std::copy(gmoog->targetPositions.begin(), gmoog->targetPositions.end(), targetPositionsTemp);
	//	//targetPositionsTemp = gmoog->targetPositions; //Make a copy of the target positions that can be modified.

	//	//std::vector<UINT8> rearrangerAtomVect(((*rearrangerAtomQueue)[0]).size()); //Dumb hacky fix.
	//	//for (int i = 0; i < rearrangerAtomVect.size(); i++)
	//	//{
	//	//	rearrangerAtomVect[i] = (*rearrangerAtomQueue)[0][i];
	//	//}

	//	//UINT maxAtomRow = 0;
	//	//UINT numAtomRow = 0;
	//	//for (int iy = 0; iy < positionsY.size(); iy++) //Find which row has most atoms.
	//	//{
	//	//	numAtomRow = 0;
	//	//	for (int ix = 0; ix < positionsX.size(); ix++)
	//	//	{
	//	//		if (rearrangerAtomVect[ix + wx * iy])
	//	//		{
	//	//			numAtomRow++;
	//	//		}
	//	//	}
	//	//	if (numAtomRow > maxAtomRow)
	//	//	{
	//	//		maxAtomRow = numAtomRow;
	//	//	}
	//	//}

	//	////scrunchX(moveseq); //scrunch atoms to left side (index 0 side)
	//	//////// This is slightly modified scrunchx to leave sharp edge to draw from.
	//	//int iy = 0;
	//	//for (auto const& channelBoolY : positionsY)
	//	//{
	//	//	if (channelBoolY) //If first step, choose the rows with load tweezers. If not first step, choose the rows that atoms were scrunched to.
	//	//	{
	//	//		moveSingle single;
	//	//		int ix = 0;
	//	//		for (auto const& channelBoolX : positionsX)
	//	//		{
	//	//			if (channelBoolX && (*rearrangerAtomQueue)[0][ix + (positionsX.size())*iy])
	//	//			{
	//	//				single.startAOX.push_back(ix); //Place tweezers on all atoms in row
	//	//				(*rearrangerAtomQueue)[0][ix + (positionsX.size())*iy] = 0; //remove atom from pickup location
	//	//			}
	//	//			ix++;
	//	//		}
	//	//		single.startAOY.push_back(iy); //Single tone in y
	//	//		single.endAOY.push_back(iy); //y does not move
	//	//		moveseq.moves.push_back(single);

	//	//		int nAtomsInRow = moveseq.moves.back().nx();
	//	//		int nGap = (maxAtomRow - nAtomsInRow) * (gmoog->scrunchSpacing);
	//	//		//(positionsX.size() - 2 * nAtomsInRow) / 2;
	//	//		for (int ix2 = 0; ix2 < nAtomsInRow; ix2++)
	//	//		{
	//	//			moveseq.moves.back().endAOX.push_back(nGap + (gmoog->scrunchSpacing) * ix2); //Bunch up tweezers in center of row
	//	//			(*rearrangerAtomQueue)[0][nGap + (gmoog->scrunchSpacing) * ix2 + (positionsX.size())*iy] = 1; //place atom in dropoff location
	//	//		};
	//	//	}
	//	//	iy++;
	//	//}
	//	////////

	//	//for (int i = 0; i < rearrangerAtomVect.size(); i++) //Dumb hacky fix.
	//	//{
	//	//	rearrangerAtomVect[i] = (*rearrangerAtomQueue)[0][i];
	//	//}

	//	////std::vector<bool> targetColumn(positionsY.size(), false);

	//	//int ixSource = (gmoog->scrunchSpacing)*(maxAtomRow - 1); //Start taking atoms from right-most scrunched column, iterate towards left.
	//	//int nColumnSource = sourceColumnSum(ixSource, rearrangerAtomVect); //number of atoms remaining in source column
	//	//int ixTarget = wx - 1;

	//	//while (ixTarget >= 0 && ixSource >= 0) //iterate through target columns from the opposite side from scrunch. ixTarget must be int so that it can go negative for this condition to fail.
	//	//{
	//	//	int iyTarget = 0;
	//	//	int iySource = 0;
	//	//	moveSingle single;

	//	//	while (iyTarget < wy) //iterate through all target positions in column
	//	//	{
	//	//		if (targetPositionsTemp[ixTarget + wx * iyTarget]) //if atom required at target
	//	//		{
	//	//			if (nColumnSource < 1) //check if a source atom is available in current source column
	//	//			{
	//	//				ixSource -= (gmoog->scrunchSpacing); //if no atoms, move to the next source column and reset available atoms in column
	//	//				if (ixSource >= 0)
	//	//				{
	//	//					nColumnSource = sourceColumnSum(ixSource, rearrangerAtomVect);
	//	//				}
	//	//				break;
	//	//			}
	//	//			while (!rearrangerAtomVect[ixSource + wx * iySource]) //find next atom in source column. This should be guaranteed due to counting nColumnSource.
	//	//			{
	//	//				iySource++;
	//	//			}
	//	//			single.startAOY.push_back(iySource);
	//	//			rearrangerAtomVect[ixSource + wx * iySource] = 0; //erase used source position.
	//	//			nColumnSource--; //remove an atom from the source column

	//	//			single.endAOY.push_back(iyTarget); //place tweezer at desired final location.
	//	//			targetPositionsTemp[ixTarget + wx * iyTarget] = 0; //erase filled target position.
	//	//		}
	//	//		iyTarget++; //iterate through target positions if no atom needed, or atom has been placed in target site.
	//	//		//if no source, this loops breaks, and continues from the previous target position.
	//	//	}
	//	//	if (ixSource < 0) //TODO: work out why this is getting triggered. Should always have enough atoms.
	//	//	{
	//	//		break;
	//	//	}
	//	//	if (single.ny() > 0)
	//	//	{
	//	//		single.startAOX.push_back(ixSource); //tweezers at desired source column
	//	//		single.endAOX.push_back(ixTarget); //tweezers at desired target column

	//	//		moveseq.moves.push_back(single); //add the move to the sequence, which either fully populates a target column, or fully depletes a source column.
	//	//		//if (true) //TODO: remove
	//	//		//{
	//	//		//	break;
	//	//		//}
	//	//	}
	//	//	if (iyTarget >= wy) //only iterate to next target column if entire column has been populated/checked.
	//	//	{
	//	//		ixTarget--;
	//	//	}
	//	//}
	//}
	//else if (rearrangeType == "tetris2")
	//{
	//// Not updated from Sr code
	//	//UINT wx = (positionsX.size()); //For convenience, could remove.
	//	//UINT wy = (positionsY.size());

	//	////std::copy(gmoog->targetPositions.begin(), gmoog->targetPositions.end(), targetPositionsTemp);
	//	//targetPositionsTemp = gmoog->targetPositions; //Make a copy of the target positions that can be modified.

	//	//std::vector<UINT8> rearrangerAtomVect(((*rearrangerAtomQueue)[0]).size()); //Dumb hacky fix.
	//	//for (int i = 0; i < rearrangerAtomVect.size(); i++)
	//	//{
	//	//	rearrangerAtomVect[i] = (*rearrangerAtomQueue)[0][i];
	//	//}

	//	//UINT maxAtomRow = 0;
	//	//UINT numAtomRow = 0;
	//	//for (int iy = 0; iy < positionsY.size(); iy++) //Find which row has most atoms.
	//	//{
	//	//	numAtomRow = 0;
	//	//	for (int ix = 0; ix < positionsX.size(); ix++)
	//	//	{
	//	//		if (rearrangerAtomVect[ix + wx * iy])
	//	//		{
	//	//			numAtomRow++;
	//	//		}
	//	//	}
	//	//	if (numAtomRow > maxAtomRow)
	//	//	{
	//	//		maxAtomRow = numAtomRow;
	//	//	}
	//	//}

	//	////scrunchX(moveseq); //scrunch atoms to left side (index 0 side)
	//	//////// This is slightly modified scrunchx to leave sharp edge to draw from.
	//	//int iy = 0;
	//	//for (auto const& channelBoolY : positionsY)
	//	//{
	//	//	if (channelBoolY) //If first step, choose the rows with load tweezers. If not first step, choose the rows that atoms were scrunched to.
	//	//	{
	//	//		moveSingle single;
	//	//		int ix = 0;
	//	//		for (auto const& channelBoolX : positionsX)
	//	//		{
	//	//			if (channelBoolX && (*rearrangerAtomQueue)[0][ix + (positionsX.size())*iy])
	//	//			{
	//	//				single.startAOX.push_back(ix); //Place tweezers on all atoms in row
	//	//				(*rearrangerAtomQueue)[0][ix + (positionsX.size())*iy] = 0; //remove atom from pickup location
	//	//			}
	//	//			ix++;
	//	//		}
	//	//		single.startAOY.push_back(iy); //Single tone in y
	//	//		single.endAOY.push_back(iy); //y does not move
	//	//		moveseq.moves.push_back(single);

	//	//		int nAtomsInRow = moveseq.moves.back().nx();
	//	//		int nGap = (maxAtomRow - nAtomsInRow) * (gmoog->scrunchSpacing);
	//	//		//(positionsX.size() - 2 * nAtomsInRow) / 2;
	//	//		for (int ix2 = 0; ix2 < nAtomsInRow; ix2++)
	//	//		{
	//	//			moveseq.moves.back().endAOX.push_back(nGap + (gmoog->scrunchSpacing) * ix2); //Bunch up tweezers in center of row
	//	//			(*rearrangerAtomQueue)[0][nGap + (gmoog->scrunchSpacing) * ix2 + (positionsX.size())*iy] = 1; //place atom in dropoff location
	//	//		};
	//	//	}
	//	//	iy++;
	//	//}
	//	////////

	//	//for (int i = 0; i < rearrangerAtomVect.size(); i++) //Dumb hacky fix.
	//	//{
	//	//	rearrangerAtomVect[i] = (*rearrangerAtomQueue)[0][i];
	//	//}

	//	////std::vector<bool> targetColumn(positionsY.size(), false);

	//	//int ixSource = (gmoog->scrunchSpacing)*(maxAtomRow - 1); //Start taking atoms from right-most scrunched column, iterate towards left.
	//	//int nColumnSource = sourceColumnSum(ixSource, rearrangerAtomVect); //number of atoms remaining in source column
	//	//int ixTarget = wx - 1;

	//	//while (ixTarget >= 0 && ixSource >= 0) //iterate through target columns from the opposite side from scrunch. ixTarget must be int so that it can go negative for this condition to fail.
	//	//{
	//	//	int iyTarget = 0;
	//	//	int iySource = 0;
	//	//	moveSingle fromReservoir, scrunch, toTarget;

	//	//	while (iyTarget < wy) //iterate through all target positions in column
	//	//	{
	//	//		if (targetPositionsTemp[ixTarget + wx * iyTarget]) //if atom required at target
	//	//		{
	//	//			if (nColumnSource < 1) //check if a source atom is available in current source column
	//	//			{
	//	//				ixSource -= (gmoog->scrunchSpacing); //if no atoms, move to the next source column and reset available atoms in column
	//	//				if (ixSource >= 0)
	//	//				{
	//	//					nColumnSource = sourceColumnSum(ixSource, rearrangerAtomVect);
	//	//				}
	//	//				break;
	//	//			}
	//	//			while (!rearrangerAtomVect[ixSource + wx * iySource]) //find next atom in source column. This should be guaranteed due to counting nColumnSource.
	//	//			{
	//	//				iySource++;
	//	//			}
	//	//			fromReservoir.startAOY.push_back(iySource);
	//	//			fromReservoir.endAOY.push_back(iySource);
	//	//			scrunch.startAOY.push_back(iySource);

	//	//			rearrangerAtomVect[ixSource + wx * iySource] = 0; //erase used source position.
	//	//			nColumnSource--; //remove an atom from the source column

	//	//			scrunch.endAOY.push_back(iyTarget); //place tweezer at desired final location.
	//	//			toTarget.startAOY.push_back(iyTarget);
	//	//			toTarget.endAOY.push_back(iyTarget);

	//	//			targetPositionsTemp[ixTarget + wx * iyTarget] = 0; //erase filled target position.
	//	//		}
	//	//		iyTarget++; //iterate through target positions if no atom needed, or atom has been placed in target site.
	//	//		//if no source, this loops breaks, and continues from the previous target position.
	//	//	}
	//	//	if (ixSource < 0) //TODO: work out why this is getting triggered. Should always have enough atoms.
	//	//	{
	//	//		break;
	//	//	}
	//	//	if (fromReservoir.ny() > 0)
	//	//	{
	//	//		fromReservoir.startAOX.push_back(ixSource); //tweezers at desired source column
	//	//		fromReservoir.endAOX.push_back(ixSource + (ixTarget - ixSource) / 2); //tweezers half way to target column.
	//	//		scrunch.startAOX.push_back(ixSource + (ixTarget - ixSource) / 2); //scrunch occurs at half way point.
	//	//		scrunch.endAOX.push_back(ixSource + (ixTarget - ixSource) / 2);
	//	//		toTarget.startAOX.push_back(ixSource + (ixTarget - ixSource) / 2);
	//	//		toTarget.endAOX.push_back(ixTarget);


	//	//		moveseq.moves.push_back(fromReservoir); //add the move to the sequence, which either fully populates a target column, or fully depletes a source column.
	//	//		moveseq.moves.push_back(scrunch);
	//	//		moveseq.moves.push_back(toTarget);
	//	//	}
	//	//	if (iyTarget >= wy) //only iterate to next target column if entire column has been populated/checked.
	//	//	{
	//	//		ixTarget--;
	//	//	}
	//	//}
	//}
	else
	{
		thrower("Invalid rearrangement mode.");
	}

	return moveseq;
}

int atomCruncher::sourceRowSum(int iRow, std::vector<UINT8> rearrangerAtomVect)
{
	int nRowSource = 0; //number of atoms remaining in source column
	for (int ix = 0; ix < (positionsX.size()); ix++) //calling size() shouldn't be slow, but check if this is limiting.
	{
		if (rearrangerAtomVect[ix + (positionsX.size()) * iRow])
		{
			nRowSource++;
		}
	}

	return nRowSource;
}

int atomCruncher::sourceColumnSum(int iColumn, std::vector<UINT8> rearrangerAtomVect)
{
	int nColumnSource = 0; //number of atoms remaining in source column
	for (int iy = 0; iy < (positionsY.size()); iy++) //calling size() shouldn't be slow, but check if this is limiting.
	{
		if (rearrangerAtomVect[iColumn + (positionsX.size()) * iy])
		{
			nColumnSource++;
		}
	}

	return nColumnSource;
}