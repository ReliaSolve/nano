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

#ifndef NMR_REGISTRATION_IMPLUI_H
#define NMR_REGISTRATION_IMPLUI_H
/*===3rdtech===
  Copyright (c) 2001 by 3rdTech, Inc.
  All Rights Reserved.

  This file may not be distributed without the permission of 
  3rdTech, Inc. 
  ===3rdtech===*/

#include "nmb_Image.h"
#include "nmr_Registration_Interface.h"
#include "nmr_Registration_Impl.h"
#include "correspondenceEditor.h"

class nmb_ColorMap;
class nmr_Registration_Impl;

/// This class provides a graphical user-interface to the
/// nmr_Registration_Impl in order let the user edit correspondence 
/// information 
class nmr_Registration_ImplUI {
  public:
    nmr_Registration_ImplUI();

    ~nmr_Registration_ImplUI();
    void enable(vrpn_bool enable, vrpn_int32 window);
	void enableEdit(vrpn_bool enableAddAndDelete, vrpn_bool enableMove);
    void registerImages();
    void mainloop();

    void newScanline(nmr_ImageType whichImage,
                                vrpn_int32 row, nmb_Image *im);
	void clearImage(nmr_ImageType whichImage);

    void setFiducial(vrpn_int32 replace, vrpn_int32 num,
                     vrpn_float32 *x_src, vrpn_float32 *y_src,
                     vrpn_float32 *z_src, vrpn_float32 *x_tgt, 
                     vrpn_float32 *y_tgt, vrpn_float32 *z_tgt);
	void getFiducial(vrpn_int32 &num,
                     vrpn_float32 *x_src, vrpn_float32 *y_src,
                     vrpn_float32 *z_src, vrpn_float32 *x_tgt, 
                     vrpn_float32 *y_tgt, vrpn_float32 *z_tgt);
    void setColorMap(nmr_ImageType whichImage, nmb_ColorMap * cmap);
    void setColorMinMax(nmr_ImageType whichImage, 
                              vrpn_float64 dmin, vrpn_float64 dmax,
                              vrpn_float64 cmin, vrpn_float64 cmax);
    void setFiducialSpotTracker(nmr_ImageType whichImage, vrpn_int32 tracker);
    void setOptimizeSpotTrackerRadius(nmr_ImageType whichImage, vrpn_bool enable);
    void setSpotTrackerRadius(nmr_ImageType whichImage, vrpn_float64 radius);
    void setSpotTrackerPixelAccuracy(nmr_ImageType whichImage, vrpn_float64 accuracy);
    void setSpotTrackerRadiusAccuracy(nmr_ImageType whichImage, vrpn_float64 accuracy);
    void setImageOrientation(nmr_ImageType whichImage,
                              vrpn_bool flipX, vrpn_bool flipY);
    void getCorrespondence(Correspondence &c, int &srcIndex, int &tgtIndex);

    void registerCorrespondenceHandler(CorrespondenceCallback handler,void *ud);

	void setTopoIntensityThreshold(float intensity);
	void setProjIntensityThreshold(float intensity);
	float getIntensityValue(int x, int y);
	vector< vector< vector <float> > > readPixels();
	vector < vector <float> > getWidthHeight();

  protected:
    static int s_numImages;
    static char *s_imageWinNames[];
    static int s_sourceImageIndex, s_targetImageIndex;
    vrpn_bool d_sourceFlipX, d_sourceFlipY;
    vrpn_bool d_targetFlipX, d_targetFlipY;

    static void handle_registration_start_change(vrpn_int32 val, 
                                                 void *userdata);
    static void handle_ServerMessage( void *ud,
                         const nmr_ServerChangeHandlerData &info);

    CorrespondenceEditor d_ce; // ui to display landmarks and
                              // let user set them

};

#endif
