#ifndef NMR_OBJECTIVEMI_EMMA_H
#define NMR_OBJECTIVEMI_EMMA_H

#include "nmb_Image.h"
#include "nmr_Objective.h"

typedef enum {NMR_RANDOM, NMR_REGULAR, NMR_JITTERED}
              nmr_SamplePositionMode;
typedef enum {NMR_NO_SELECT, NMR_GRADIENT_SELECT, 
              NMR_REF_FEATURE_DISTANCE_SELECT}
              nmr_SampleRejectionCriterion;

class nmr_ObjectiveMI_EMMA : public nmr_Objective{
  public:
    nmr_ObjectiveMI_EMMA();
    ~nmr_ObjectiveMI_EMMA();

    /* ************** 
    functions that affect the number and meaning of parameters of the
    objective function
    ***************** */

    /// choose between 2D reference or 2.5D reference image
    virtual void setDimensionMode(nmr_DimensionMode mode);
    virtual nmr_DimensionMode getDimensionMode();

    /* **************
    functions that affect the value of the objective function and its gradient
    ***************** */

    /// set/get how big the samples are - this affects how noisy the 
    /// objective function value is
    /// return value is 0 if successful or -1 if out of memory 
    int setSampleSizes(int sizeA, int sizeB);
    void getSampleSizes(int &sizeA, int &sizeB);

    /// set whether to choose samples randomly, on a regular grid
    /// or randomly with a gradient magnitude-based sample rejection criterion
    void setSamplePositionMode(nmr_SamplePositionMode mode);
    void setSampleRejectionCriterion(nmr_SampleRejectionCriterion crit);
    void setMinSampleSqrGradientMagnitude(double mag);
    void setRefFeaturePoints(int numPnts, double *x, double *y);

    /// set/get gaussian standard deviations for constructing Parzen
    /// window estimates of density 

    void setCovariance(double sigmaRefRef, double sigmaTestTest);
    void getCovariance(double &sigmaRefRef, double &sigmaTestTest);

    void setTestVariance(double sigma);
    void getTestVariance(double &sigma);
    void setRefVariance(double sigma);
    void getRefVariance(double &sigma);

    /// set images 
    virtual void setReferenceValueImage(nmb_Image *ref);
    virtual void setTestValueImage(nmb_Image *test);
    virtual void setReferenceZImage(nmb_Image *ref_z);

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
    virtual double value(double *testFromReferenceTransform);

    /// get gradient vector
    virtual void gradient(double *testFromReferenceTransform, double *gradMI);

    /// since we can share some of the computation between value and
    /// gradient computations, if you need both you should call this 
    /// function
    virtual void valueAndGradient(double *testFromReferenceTransform, 
                double &valueMI, double *gradMI);

    /* *************
    functions that are useful for determining the optimal variance parameters

    These are essentially a set of 3 objective functions and it is left to
    the user to decide what optimization method to use and how optimal the 
    parameters should be.
    In each case, the optimal variance is the one that minimizes the 
    associated entropy estimate.
    The test and reference entropies only depend on those images but the
    the cross entropy depends on the last used transformation so it doesn't
    really make to optimize this until the transformation is near the optimum
    (which should be where the transformation starts anyway so this shouldn't
     be a problem)
    **************** */

    // either make sure you have called value, gradient, or valueAndGradient
    // or you'll have to call this function before doing the test or reference
    // entropy calculations
    // returns -1 if either the test or reference images haven't been set
    int buildSampleSets(double *transform = NULL);

    // for test image intensity histogram:
    // ----------------------------------
    // compute gradient of test image entropy with respect to sigma
    // return values:
    //  0 = successful
    // -1 = error, need to call one of buildSampleSets, value, gradient, or
    // valueAndGradient
    // [EXPENSIVE]
    int computeTestEntropyGradient(double &dH_dsigma_test);

    // compute entropy of test image, the test variance should be
    // chosen so that this is minimized
    //  0 = successful
    // -1 = error, need to call one of buildSampleSets, value, gradient, or
    // valueAndGradient
    // [EXPENSIVE]
    int testEntropy(double &entropy);

    // for reference image intensity histogram:
    // ----------------------------------------
    // compute gradient of ref image entropy with respect to sigma
    // return values:
    //  0 = successful
    // -1 = error, need to call one of buildSampleSets, value, gradient, or
    // valueAndGradient
    // [EXPENSIVE]
    int computeRefEntropyGradient(double &dH_dsigma_ref);

    // compute entropy of reference image, the reference variance should be
    // chosen so that this is minimized
    //  0 = successful
    // -1 = error, need to call one of buildSampleSets, value, gradient, or
    // valueAndGradient
    // [EXPENSIVE]
    int refEntropy(double &entropy);

    // for (test, reference) joint histogram:
    // -------------------------------------
    // compute gradient of cross-entropy with respect to sigma
    // return values:
    //  0 = successful
    // -1 = error, need to call one of buildSampleSets, value, gradient, or
    // valueAndGradient
    // [EXPENSIVE]
    int computeCrossEntropyGradient(double &dHc_dsigma_ref,
                                    double &dHc_dsigma_test);

    // cross-entropy computed using the new values for sigmaRef and sigmaTest
    // (cross-entropy should be minimized as we step along)
    // return values:
    //   0 = successful
    // -1 = error, need to call one of buildSampleSets, value, gradient, or
    // valueAndGradient
    // [EXPENSIVE]
    int crossEntropy(double &entropy);

    // for debugging:
    // the transformation (passed in as a 16 element
    // array of doubles as used by nmb_TransformMatrix44)
    // that takes points in the reference image (possibly augmented by
    // a height value if a z value image has been set)
    // this transformation should be in terms of normalized image units where
    // the range for x and y is [0,1] over the extents of an image
    int getJointHistogram(nmb_Image *histogram,double *transform,vrpn_bool blur,
              vrpn_bool setRefScale, float min_ref, float max_ref,
              vrpn_bool setTestScale, float min_test, float max_test);

  protected:

    // if the dimension mode is set for a 2.5D reference image then
    // a z value for each corresponding (x,y) position should be supplied in
    // the optional ref_z argument
    // returns  0 if successful and 
    //         -1 if ref_z value does not agree with dimension mode
    // [EXPENSIVE]
    int buildSampleA(nmb_Image *ref, nmb_Image *test, 
                     nmb_Image *grad_x_test, 
                     nmb_Image *grad_y_test, 
                     double *transform,
                 nmr_SamplePositionMode samplePositionMode = NMR_RANDOM,
                 nmr_SampleRejectionCriterion sampleRejectCrit = NMR_NO_SELECT,
                     double minSqrGradientMagnitude = 0.0,
                     nmb_Image *ref_z = NULL);

    int buildSampleB(nmb_Image *ref, nmb_Image *test, 
                     nmb_Image *grad_x_test, 
                     nmb_Image *grad_y_test,
                     double *transform,
                 nmr_SamplePositionMode samplePositionMode = NMR_RANDOM,
                 nmr_SampleRejectionCriterion sampleRejectCrit = NMR_NO_SELECT,
                     double minSqrGradientMagnitude = 0.0,
                     nmb_Image *ref_z = NULL);

    // fills in the joint histogram and blurs it with the current
    // covariance matrix - the resolution is determined by the
    // resolution of the image passed in and min and max values are
    // optional
    int buildJointHistogram(nmb_Image *ref, nmb_Image *test, 
                            nmb_Image *ref_z,
                            nmb_Image *histogram, double *transform,
                            vrpn_bool blur = vrpn_FALSE,
                            vrpn_bool setRefScale = VRPN_FALSE,
                            float min_ref = 0.0, float max_ref = 0.0,
                            vrpn_bool setTestScale = VRPN_FALSE,
                           float min_test = 0.0, float max_test = 0.0);

    // compute gradient of mutual information from last sample to optimize T
    // return values:
    //   0 = successful
    //  -1 = error, need to call buildSampleA and buildSampleB 
    //       after transformation is set
    // [EXPENSIVE]
    int computeTransformationGradient(double *dIdT);

    // compute the mutual information from last sample
    // return values:
    //   0 = successful
    //  -1 = error, need to call buildSampleA and buildSampleB 
    //       after transformation is set
    // [EXPENSIVE]
    int computeMutualInformation(double &mutualInfo);

    /* *******************
    // helper subroutines:
     *********************/

    // helper for computeTransformationGradient()
    int computeTransformationGradient_HeightfieldRef(double *dIdT);

    // helper for buildSampleA and buildSampleB
    int buildSampleHelper(nmb_Image *ref, nmb_Image *test,
                     nmb_Image *grad_x_test,
                     nmb_Image *grad_y_test,
                     double *transform,
                     nmr_SamplePositionMode samplePosMode,
                     nmr_SampleRejectionCriterion sampleRejCrit,
                     nmb_Image *ref_z,
       int numPoints, double *x_ref, double *y_ref, double *z_ref,
       double *x_test, double *y_test, double *val_ref, double *val_test,
       double *dtest_dx, double *dtest_dy, double minSqrGradientMagnitude);

    // inline helper functions
    double transform_x(double x, double y, double *T) 
      {  return T[0]*x + T[4]*y + T[12];  }
    double transform_y(double x, double y, double *T)
      {  return T[1]*x + T[5]*y + T[13];  }
    double transform_x(double x, double y, double z, double *T)
      {  return T[0]*x + T[4]*y + T[8]*z + T[12];  }
    double transform_y(double x, double y, double z, double *T)
      {  return T[1]*x + T[5]*y + T[9]*z + T[13];  }

    void convertTransformToPixelUnits(double *T1, double *T2);
    void convertTransformToImageUnits(double *T1, double *T2);

    double distanceToNearestFeaturePoint(double x, double y);

    // allocation/deallocation reference for the x,y,z, and intensity arrays
    // for the A and B samples
    double *d_workspace;

    // has buildSampleA been called since transformation matrix changed?
    vrpn_bool d_sampleA_acquired; 
    
    int d_sizeA; // size of sample A
    // x,y,z and intensity values for sample A:
    double *d_Ax_ref, *d_Ay_ref, *d_Az_ref, *d_Ax_test, *d_Ay_test;
    double *d_refValuesA, *d_testValuesA;
    double *d_dTestValueA_dx, *d_dTestValueA_dy;

    // has buildSampleB been called since transformation matrix changed?
    vrpn_bool d_sampleB_acquired;
    
    int d_sizeB; // size of sample B
    // x,y,z and intensity values for sample B:
    double *d_Bx_ref, *d_By_ref, *d_Bz_ref, *d_Bx_test, *d_By_test;
    double *d_refValuesB, *d_testValuesB;
    double *d_dTestValueB_dx, *d_dTestValueB_dy;

    // representation for sqrt of covariance matrix (assumed to be diagonal)
    double d_sigmaRefRef, d_sigmaTestTest;

    // standard deviation for Parzen windowing of test image intensity 
    // distribution
    double d_sigmaTest;

    // standard deviation for Parzen windowing of ref image intensity
    // distribution
    double d_sigmaRef;

    // does reference image correspond to a simple 2D projection or a 
    // height field?
    nmr_DimensionMode d_dimensionMode;

    // how to pick samples
    nmr_SamplePositionMode d_samplePositionMode;
    nmr_SampleRejectionCriterion d_sampleRejectionCriterion;
    double d_minSampleSqrGradientMagnitude;
    int d_numRefFeaturePoints;
    double *d_xFeature;
    double *d_yFeature;
    double d_maxDistanceToFeaturePoint;

    // images
    nmb_Image *d_testValue;
    nmb_Image *d_gradX_test, *d_gradY_test;
    nmb_Image *d_refValue;
    nmb_Image *d_refZ;

};

#endif