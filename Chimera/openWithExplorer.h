#pragma once

#include "stdafx.h"

#include <string>
/*
* This function gets the name of a file to open, using the fancy windows dialog box.
* It returns the chosen filename if successful, it returns "" if the user canceled.
*/
std::string openWithExplorer(CWnd* parent, std::string extension);

std::string getFolderWithExplorer(CWnd* parent, std::string extension);

