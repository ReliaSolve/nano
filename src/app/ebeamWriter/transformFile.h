#ifndef TRANSFORMFILE_H
#define TRANSFORMFILE_H

#include "vrpn_Types.h"
#include "nmb_TransformMatrix44.h"
#include "list.h"

/*
format for transformation file (the matrix gives the world to image xform):
num_files <n>
file_name <filename_1>
t00 t01 t02 t03
t10 t11 t12 t13
t20 t21 t22 t23
t30 t31 t32 t33
file_name <filename_2>
t00 t01 t02 t03
t10 t11 t12 t13
t20 t21 t22 t23
t30 t31 t32 t33
...
file_name <filename_n)>
t00 t01 t02 t03
t10 t11 t12 t13
t20 t21 t22 t23
t30 t31 t32 t33
*/

class TransformFileEntry {
  public:
    TransformFileEntry()
    { fileName[0] = '\0'; }

    char fileName[256];
    static const int maxFileName;
    nmb_TransformMatrix44 transform;
    double acqDistX;
    double acqDistY;
};

/// This class represents a collection of 2D image coordinate systems for
/// a collection of images corresponding to the same 2D sample (or world)
/// The actual world coordinates are somewhat irrelevent except for the
/// concept of a world scale which may be specified in terms of the width
/// and height of an image in nano-meters. Image coordinates are assumed
/// to lie in [0,1]x[0,1]. For the sake of generality, a matrix argument
/// is assumed to be column-major order arrays of 16 elements representing
/// a 4x4 matrix.
class TransformFile {
  public:
    TransformFile();
    int load(char *filename);
    int save(char *filename);
    vrpn_bool lookupImageTransformByName(const char *name, double *matrix,
                                     double &acqDistX, double &acqDistY);

    /// sets the world->image transformation for an image with the name
    /// name_dst given an image->image
    /// transformation; in general the src image will be the live image
    /// from the microscope and the dst image will be some other image
    /// If the src image has not been created then this will return an
    /// error result of (-1) and if the dst image has not been created
    /// then it will be added to the list. If the function is successful, 
    /// the result is (0).
    int setImageTransform(const char *name_src, 
                          const char *name_dst, const double *matrix);

    /// sets the world->image transformation directly - we should only need
    /// to do this for the live image from the microscope and in this case
    /// we would typically just use the identity so that when a 
    /// calibration is applied, the world to live image transformation just
    /// becomes a scaling (although different scaling in x and y in general)
    /// other image transformations may also be initialized to the 
    /// identity using this function because if the name doesn't exist in the
    /// list then it will be added
    int setImageTransform(const char *name, const double *matrix,
                          double acqDistX, double acqDistY);

    /// lets you specify the width and height of an image in nm
    /// and adjusts the world->image transformation matrix by applying
    /// a scaling factor in x and y that gives the correct size in the world
    /// The same scaling factor is then applied to all transformations
    /// in the file so that the transformations between different images
    /// are preserved
    int applyCalibration(const char *name, double width_nm, double height_nm);

  protected:
    list<TransformFileEntry> data;
};

#endif
