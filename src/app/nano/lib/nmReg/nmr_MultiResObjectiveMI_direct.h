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

#ifndef NMR_MULTIRESOBJECTIVEMI_DIRECT_H
#define NMR_MULTIRESOBJECTIVEMI_DIRECT_H

#include "nmr_ObjectiveMI_direct.h"

/* nmr_MultiResObjectiveMI_direct
  This class is a multiresolution version of nmr_ObjectiveMI_direct
*/
class nmr_MultiResObjectiveMI_direct {
  public:
    nmr_MultiResObjectiveMI_direct();
    nmr_MultiResObjectiveMI_direct(int numLevels, float *stddev);
    ~nmr_MultiResObjectiveMI_direct();

    int numLevels() {return d_numResolutionLevels;}
    void getBlurStdDev(float *stddev);
    int getLevelByScaleOrder(int order);

    /* **************
    functions that affect the number and meaning of parameters of the
    objective function
    ***************** */

    /// choose between 2D reference or 2.5D reference image
    void setDimensionMode(nmr_DimensionMode mode);
    nmr_DimensionMode getDimensionMode();

    /* **************
    functions that affect the value of the objective function and its gradient
    ***************** */

    /// set images, if a list is supplied then multiple versions of the
    /// image will be added to the list for the various resolutions
    void setReferenceValueImage(nmb_Image *ref, 
                                nmb_ImageList *monitorList = NULL);
    void setTestValueImage(nmb_Image *test,
                           nmb_ImageList *monitorList = NULL);
    void setReferenceZImage(nmb_Image *ref_z,
                            nmb_ImageList *monitorList = NULL);
 
    /* **************
    objective function value and gradient
    ***************** */

    /// these functions compute objective function value and gradient vector
    /// as a function of the transformation (passed in as a 16 element
    /// array of doubles as used by nmb_TransformMatrix44)
    /// that takes points in the reference image (possibly augmented by
    /// a height value if a z value image has been set)
    /// this transformation should be in terms of pixels

    /// objective function value
    double value(int level, double *testFromReferenceTransform);

    /// get gradient vector
    void gradient(int level, double *testFromReferenceTransform, 
                  double *gradMI);

    /// since we can share some of the computation between value and
    /// gradient computations, if you need both you should call this
    /// function
    void valueAndGradient(int level, double *testFromReferenceTransform,
                double &valueMI, double *gradMI);
	void getJointHistogramSize(int level, int &numX, int &numY);
	void getJointHistogramImage(int level, nmb_Image *image);

  protected:
    int d_numResolutionLevels;
    float *d_stddev;
    int *d_sortOrder; // gives the order of the scales from largest to smallest
    nmr_ObjectiveMI_direct *d_objectiveMI;
    static int s_defaultNumResolutionLevels;
    static float s_defaultStdDev[];
};

#endif
