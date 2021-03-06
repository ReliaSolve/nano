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

#ifndef CORRESPONDENCE_H
#define CORRESPONDENCE_H

#include <nmb_Types.h>  // for vrpn_bool
#include <BCPlane.h>
#include <nmb_Image.h>

class corr_point_t {
  public:
    corr_point_t(double xp=0.0, double yp=0.0, double radiusp=5.0);
    corr_point_t(double xp, double yp, double zp, double radiusp);
    vrpn_bool is2D;
    double x, y, z, radius;
};

class corr_curve_t {
  public:
    corr_curve_t(int n) { n_pnts = n; curve_pnt = new corr_point_t[n];};
    int n_pnts;
    corr_point_t *curve_pnt;
};

class Correspondence {
  public:
    Correspondence();
    Correspondence(int num_spaces, int max_points);
    void init(int num_spaces, int max_points);
    void clear();
    int addPoint(corr_point_t &p);
    int unpaired_fluoro_addPoint(corr_point_t &p); // new
    int addPoint(corr_point_t *p);
    int unpaired_fluoro_addPoint(corr_point_t *p); // new
	int addCurve(corr_curve_t &c);
    int setPoint(int spaceIndex, int pntIndex, const corr_point_t &p);
	int unpaired_fluoro_setPoint(int spaceIndex, int pntIndex, const corr_point_t &p); // new
    int setPoint(int pntIndex, corr_point_t *p);
	int unpaired_fluoro_setPoint(int pntIndex, corr_point_t *p); // new
    int deletePoint(int pntIndex);
	int unpaired_fluoro_deletePoint(int pntIndex); // new
    vrpn_bool findNearestPoint(int spaceIndex, double x, double y,
        double scaleX, double scaleY, int *pntIndex);
	vrpn_bool unpaired_fluoro_findNearestPoint(int spaceIndex, double x, double y,
        double scaleX, double scaleY, int *pntIndex); // new
    int numPoints() const {return num_points;};
	int unpaired_fluoro_numPoints() const {return unpaired_fluoro_num_points;};// new
    int maxPoints() const {return max_points;};
	int unpaired_fluoro_maxPoints() const {return unpaired_fluoro_max_points;};// new
    int numSpaces() const {return num_spaces;};
	int unpaired_fluoro_numSpaces() const {return unpaired_fluoro_num_spaces;};// new
    int getPoint(int spaceIndex, int pntIndex, corr_point_t *pnt) const;
	int unpaired_fluoro_getPoint(int spaceIndex, int pntIndex, corr_point_t *pnt) const; // new
    int setValuesFromPlane(int spaceIdx, BCPlane *p);
    int setValuesFromImage(int spaceIdx, nmb_Image *im);
    int scalePoints(int spaceIndex, double sx, double sy, double sz);
    Correspondence &operator = (const Correspondence &c);
    vrpn_bool equals(const Correspondence &c);
    void print();

  private:
    unsigned int num_points;
    unsigned int max_points;
    unsigned int num_spaces;
    corr_point_t **pnts;

	unsigned int unpaired_fluoro_num_points; // new
    unsigned int unpaired_fluoro_max_points; // new
    unsigned int unpaired_fluoro_num_spaces; // new
    corr_point_t **unpaired_fluoro_pnts; // new
};

#endif
