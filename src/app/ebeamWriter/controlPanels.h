/* The nanoManipulator and its source code have been released under the
 * Boost software license when nanoManipulator, Inc. ceased operations on
 * January 1, 2014.  At this point, the message below from 3rdTech (who
 * sublicensed from nanoManipulator, Inc.) was superceded.
 * Since that time, the code can be used according to the following
 * license.  Support for this system is now through the NIH/NIBIB
 * National Research Resource at cismm.org.

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef CONTROLPANELS_H
#define CONTROLPANELS_H

#include <Tcl_Linkvar.h>
#include "patternEditor.h"
#include "nmm_Microscope_SEM_Remote.h"
#include "nmr_RegistrationUI.h"
#include "nmb_Image.h"
#include "nmm_EDAX.h"
#include "patternFile.h"

// This class manages variables linked to the control panel components of
// the GUI
class ControlPanels {
 public:
   ControlPanels(PatternEditor *pe,
                 nmm_Microscope_SEM_Remote *sem,
				 nmr_RegistrationUI *aligner);
   ~ControlPanels();

   void setImageList(nmb_ImageList *data);
   nmb_ListOfStrings *imageNameList();

   void updateMinimumDoses();
   void setWidthValue(int value);
 protected:
   // callback stuff
   void setupCallbacks();

   static void handle_openImageFileName_change(const char *new_value, void *ud);
   static void handle_bufferImageFileName_change(const char *new_value, 
                                                 void *ud);
   static void handle_saveImageFileName_change(const char *new_value,
                                                 void *ud);
   static void handle_saveImageName_change(const char *new_value, void *ud);
   static void handle_openPatternFileName_change(const char *new_value, 
                                                 void *ud);
   static void handle_savePatternFileName_change(const char *new_value,
                                                 void *ud);

   static void handle_lineWidth_nm_change(double new_value, void *ud);
   static void handle_widthValue_change(int new_value, void *ud);
   static void handle_line_exposure_change(double new_value, void *ud);
   static void handle_area_exposure_change(double new_value, void *ud);
   static void handle_drawingTool_change(int new_value, void *ud);
   static void handle_undoPoint_change(int new_value, void *ud);
   static void handle_undoShape_change(int new_value, void *ud);
   static void handle_clearPattern_change(int new_value, void *ud);
   static void handle_clearPatternConfirm_change(int new_value, void *ud);
   static void handle_addTestGrid_change(int new_value, void *ud);
   static void handle_addFocusTest_change(int new_value, void *ud);
   static void handle_canvasImage_change(const char *new_value, void *ud);
   static void handle_patternColorChanged_change(int new_value, void *ud);


   static void handle_imageColorChanged_change(int new_value, void *ud);
   static void handle_imageOpacity_change(double new_value, void *ud);
   static void handle_imageMagnification_change(double new_value, void *ud);
   static void handle_hideOtherImages_change(int new_value, void *ud);
   static void handle_enableImageDisplay_change(int new_value, void *ud);
   static void handle_currentImage_change(const char *new_value, void *ud);
   void updateCurrentImageControls();

   static void handle_semWindowOpen_change(int new_value, void *ud);
   static void handle_semAcquireImagePushed_change(int new_value, void *ud);
   static void handle_semAcquireContinuousChecked_change(
               int new_value, void *ud);
   static void handle_semPixelIntegrationTime_change(int new_value, void *ud);
   static void handle_semInterPixelDelayTime_change(int new_value, void *ud);
   static void handle_semResolution_change(int new_value, void *ud);
   static void handle_semAcquisitionMagnification_change(int new_value, 
                    void *ud);
   static void handle_semBeamBlankEnable_change(int new_value, void *ud);
   static void handle_semHorizRetraceDelay_change(int new_value, void *ud);
   static void handle_semVertRetraceDelay_change(int new_value, void *ud);
   static void handle_semDACParams_change(int new_value, void *ud);
   static void handle_semExternalScanControlEnable_change(
                                                 int new_value, void *ud);

   static void handle_semExposureMagnification_change(int new_value, void *ud);
   static void handle_semDotSpacing_change(double new_value, void *ud);
   static void handle_semLineSpacing_change(double new_value, void *ud);
   static void handle_semBeamCurrent_change(double new_value, void *ud);
   static void handle_semBeamExposePushed_change(int new_value, void *ud);
   static void handle_semDoTimingTest_change(int new_value, void *ud);
   static void handle_semPointReportEnable_change(int new_value, void *ud);

   static void handle_sem_change(void *ud,
                        const nmm_Microscope_SEM_ChangeHandlerData &info);
   void handleSEMChange(const nmm_Microscope_SEM_ChangeHandlerData &info);

   bool patternInsideScanRange();
   void setLineWidth();

   // list of all images available for display
   Tclvar_list_of_strings *d_imageNames;

   // file menu
   Tclvar_list_of_strings *d_bufferImageFormatList;
   Tclvar_list_of_strings *d_saveImageFormatList;
   Tclvar_string d_openImageFileName;
   Tclvar_string d_bufferImageFileName;
   Tclvar_string d_bufferImageFormat;
   Tclvar_string d_saveImageFileName;
   Tclvar_string d_saveImageFileType;
   Tclvar_string d_saveImageName;
   Tclvar_string d_openPatternFileName;
   Tclvar_string d_savePatternFileName;

   // Tcl variables linked to control panels
   // drawing parameters:
   Tclvar_float d_lineWidth1_nm;
   Tclvar_float d_lineWidth2_nm;
   Tclvar_float d_lineWidth3_nm;
   Tclvar_int d_widthValue;
   Tclvar_float d_line_exposure_pCoulombs_per_cm;
   Tclvar_float d_area_exposure_uCoulombs_per_square_cm;
   Tclvar_int d_drawingTool;
   Tclvar_int d_clearPattern;
   Tclvar_int d_clearPatternConfirm;
   Tclvar_int d_undoShape;
   Tclvar_int d_undoPoint;
   Tclvar_int d_addTestGrid;
   Tclvar_int d_addFocusTest;
   Tclvar_string d_canvasImage;

   Tclvar_int d_patternColorChanged;
   Tclvar_int d_patternRed;
   Tclvar_int d_patternGreen;
   Tclvar_int d_patternBlue;

   // display parameters:
   Tclvar_int d_imageColorChanged;
   Tclvar_int d_imageRed;
   Tclvar_int d_imageGreen;
   Tclvar_int d_imageBlue;
   Tclvar_float d_imageOpacity;
   Tclvar_float d_imageMagnification;
   Tclvar_int d_hideOtherImages;
   Tclvar_int d_enableImageDisplay;
   Tclvar_string d_currentImage;

   // SEM
   Tclvar_int d_semWindowOpen;
   Tclvar_int d_semAcquireImagePushed;
   Tclvar_int d_semAcquireContinuousChecked;
   Tclvar_int d_semPixelIntegrationTime_nsec;
   Tclvar_int d_semInterPixelDelayTime_nsec;
   Tclvar_int d_semResolution;
   Tclvar_int d_semAcquisitionMagnification; // for a 10 cm wide display
   Tclvar_int d_semOverwriteOldData;
   Tclvar_int d_semBeamBlankEnable;
   Tclvar_int d_semHorizRetraceDelay_nsec;
   Tclvar_int d_semVertRetraceDelay_nsec;
   Tclvar_int d_semXDACGain, d_semXDACOffset;
   Tclvar_int d_semYDACGain, d_semYDACOffset;
   Tclvar_int d_semZADCGain, d_semZADCOffset;
   Tclvar_int d_semExternalScanControlEnable;
   Tclvar_string d_semDataBuffer;
   Tclvar_list_of_strings *d_semBufferImageNames;

   // Beam Control
   Tclvar_int d_semExposureMagnification; // for a 10 cm wide display
/*
   two cases:
    1. thin lines (thinner than line spacing):
       linear exposure, dot spacing, current --> dwell time

    2. fat lines (thicker than line spacing) or polygons:
       area exposure, dot spacing, line spacing, current --> dwell time
*/
   Tclvar_float d_semDotSpacing_nm;
   Tclvar_float d_semLineSpacing_nm;
   Tclvar_float d_semBeamCurrent_picoAmps;
   Tclvar_int d_semBeamExposePushed;
   Tclvar_int d_semBeamExposeEnabled;
   Tclvar_int d_semDoTimingTest;
   Tclvar_int d_semPointReportEnable;
   Tclvar_string d_semExposureStatus;
   Tclvar_string d_semMinLinExposure;
   Tclvar_string d_semMinAreaExposure;
   double d_semMinLinExposure_pCoul_per_cm;
   double d_semMinAreaExposure_uCoul_per_sq_cm;

   double d_minAllowedDotSpacing;
   double d_minAllowedLineSpacing;
   double d_minAllowedBeamCurrent;

   PatternEditor *d_patternEditor;
   PatternFile d_patternFile;
   nmm_Microscope_SEM_Remote *d_SEM;
   nmb_ImageList *d_imageList;
   int imageCount[EDAX_NUM_SCAN_MATRICES]; // a count of the number 
                    // of images being stored at each resolution
   char *d_currentLiveSEMImageName;

   static int s_numImageFileFormats;
   static char *s_imageFileFormatNames[];
   bool d_disableCommandsToSEM;
   bool d_disableDisplaySettingCallbacks;

   nmr_RegistrationUI *d_aligner;
};

#endif
