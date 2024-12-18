/*=============================================================================
  Copyright (C) 2013 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ApiController.h

  Description: Implementation file for the ApiController helper class that
               demonstrates how to implement a synchronous single image
               acquisition with VimbaCPP.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#ifndef AVT_VMBAPI_EXAMPLES_APICONTROLLER
#define AVT_VMBAPI_EXAMPLES_APICONTROLLER

#include <string>

#include "VimbaCPP/Include/VimbaCPP.h"

namespace AVT {
namespace VmbAPI {
namespace Examples {

class ApiController
{
  public:
    ApiController();
    ~ApiController();

    //
    // Starts the Vimba API and loads all transport layers
    //
    // Returns:
    //  An API status code
    //
    VmbErrorType    StartUp();
    
    //
    // Shuts down the API
    //
    void            ShutDown();

	VmbErrorType    openCamera(const std::string &rStrCameraID);
	VmbErrorType	GetAcquisitionStartFeature(AVT::VmbAPI::FeaturePtr & feature);
	VmbErrorType	GetAcquisitionStopFeature(AVT::VmbAPI::FeaturePtr & feature);
	VmbErrorType	AcquisitionStart();
	VmbErrorType	AcquisitionStop();
	VmbErrorType	setupAcquisition(const std::string &rStrCameraID, std::vector<FramePtr> &rpFrames);
	VmbErrorType    finishAcquisition();
	VmbErrorType	closeCamera();

    //
    // Gets all cameras known to Vimba
    //
    // Returns:
    //  A vector of camera shared pointers
    //
    CameraPtrVector GetCameraList();

    //
    // Translates Vimba error codes to readable error messages
    //
    // Parameters:
    //  [in]    eErr        The error code to be converted to string
    //
    // Returns:
    //  A descriptive string representation of the error code
    //
    std::string     ErrorCodeToMessage( VmbErrorType eErr ) const;
    
    //
    // Gets the version of the Vimba API
    //
    // Returns:
    //  The version as string
    //
    std::string     GetVersion() const;

  private:
    // A reference to our Vimba singleton
    VimbaSystem &m_system;
    // The currently streaming camera
    CameraPtr m_pCamera;

	AVT::VmbAPI::FeaturePtr m_AcquisitionStartFeature;
	AVT::VmbAPI::FeaturePtr m_AcquisitionStopFeature;
	VmbInt64_t nPLS;
	AVT::VmbAPI::FeaturePtr pFeature;
};

}}} // namespace AVT::VmbAPI::Examples

#endif
