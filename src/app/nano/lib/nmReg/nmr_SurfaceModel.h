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

#ifndef NMR_SURFACEMODEL_H
#define NMR_SURFACEMODEL_H

#include "nmg_HeightField.h"

class nmr_SurfaceModel {
 public:
  // solve for the minimal t such that 
  // (x,y,z) + t*(vx,vy,vz) lies in the surface
  virtual vrpn_bool surfaceRayIntersection(double x, double y, double z,
                                      double vx, double vy, double vz,
                                      double &t) = 0;
};

// plane defined by the equation: nx*x + ny*y + nz*z + d = 0
class nmr_SurfaceModelPlane : public nmr_SurfaceModel {
 public:
  nmr_SurfaceModelPlane(double nx=0, double ny=0, double nz=1, double d=0):
    d_nx(nx), d_ny(ny), d_nz(nz), d_d(d) {}
  virtual vrpn_bool surfaceRayIntersection(double x, double y, double z,
                                      double vx, double vy, double vz, 
                                      double &t)
  {
    // nx*x + ny*y + nz*z + d = 0
    // nx*(x+t*vx) + ny*(y+t*vy) + nz*(z+t*vz) + d = 0
    // t*(vx*nx + vy*ny + vz*nz) = -(nx*x + ny*y + nz*z + d)
    t = -(d_nx*x + d_ny*y + d_nz*z + d_d)/(vx*d_nx + vy*d_ny + vz*d_nz);
    return vrpn_TRUE;
  }
 private:
  double d_nx, d_ny, d_nz, d_d;
};

class nmr_SurfaceModelHeightField : public nmr_SurfaceModel {
public:
	nmr_SurfaceModelHeightField(nmb_Image *heightValues,
		double minX, double minY, double maxX, double maxY, double zScale);
	~nmr_SurfaceModelHeightField();
	virtual vrpn_bool surfaceRayIntersection(double x, double y, double z,
                                      double vx, double vy, double vz, 
                                      double &t);
private:
	nmg_HeightField *d_heightField;
};

#endif
