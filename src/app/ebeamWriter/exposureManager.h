#ifndef EXPOSUREMANAGER_H
#define EXPOSUREMANAGER_H

#include "patternEditor.h"
#include "nmm_Microscope_SEM_Remote.h"

class ExposureManager {
 public:
  ExposureManager();

  void exposePattern(list<PatternShape> shapes, 
                     list<PatternPoint> dump_points,
                     nmm_Microscope_SEM_Remote *sem, int mag);
  void setExposure(double uCoul_per_cm);
  void setColumnParameters(double minDwellTime_sec,
                           double beamWidth_nm,
                           double current_picoAmps);
  int initShape(PatternShape &shape);
  vrpn_bool getNextPoint(PatternPoint &point, double &time);
  void convert_nm_to_DAC(const double x_nm, const double y_nm, 
                         int &xDAC, int &yDAC);

 private:
  // helper function for initShape and getNextPoint
  void initThickLineSegment(vrpn_bool firstSegment);
  void initThinLineSegment();

  // exposure-dependent parameters
  double d_exposure_uCoul_per_cm2;
  double d_area_dwell_time_sec;
  double d_line_dwell_time_sec;
  double d_area_inter_dot_dist_nm;
  double d_line_inter_dot_dist_nm;

  // exposure-independent parameters
  double d_min_exposure_per_point_uCoul_per_cm2;
  double d_min_dwell_time_sec;
  double d_nm_per_pixel;
  double d_beam_width_nm;
  double d_beam_current_picoAmps;
  int d_xSpan_DACunits;
  int d_ySpan_DACunits;
  double d_xSpan_nm;
  double d_ySpan_nm;

  // some parameters that need to be tuned
  double d_line_overlap_factor;
  double d_area_overlap_factor;

  // other
  PatternShape *d_currShape;
  list<PatternPoint>::iterator d_pointListPtr; 
  PatternPoint d_nextExposePoint;

  // additional state for doing thick polylines
  int d_currThickLineRow;
  int d_numThickLineRows;
  double d_segmentStartFirstRowX, d_segmentStartFirstRowY;
  double d_segmentStartLastRowX, d_segmentStartLastRowY;
  double d_segmentEndFirstRowX, d_segmentEndFirstRowY;
  double d_segmentEndLastRowX, d_segmentEndLastRowY;
  double d_halfWidth;
  double d_os_x0, d_os_y0, d_os_x1, d_os_y1;
};

#endif
