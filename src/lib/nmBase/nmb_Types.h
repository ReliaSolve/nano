#ifndef NMB_TYPES_H
#define NMB_TYPES_H

// MOVED from microscape.h

typedef float PointType [3];

/*
class PointType {
  float data[3];
 public:
  float& operator [] (int i) { return data[i]; }
  float operator [] (int i) const { return data[i]; }
  //  const float operator [] (int i) const { return data[i]; }
  operator const float * () const {
    float a[3] = {data[0], data[1], data[2]};
    return a;
  }
};
*/

enum UserMode {
  USER_FLY_MODE = (0),
  USER_GRAB_MODE = (1),
  USER_SCALE_UP_MODE = (2),
  USER_SCALE_DOWN_MODE = (3),
  USER_SERVO_MODE = (4),
  USER_PULSE_MODE = (5),
  USER_MEAS_MOVE_MODE = (6),
  USER_LINE_MODE = (7),
  USER_SWEEP_MODE = (8),
  USER_MEASURE_MODE = (9),
  USER_LIGHT_MODE = (10),
  USER_PLANE_MODE = (11),
  USER_PLANEL_MODE = (12),
  USER_BLUNT_TIP_MODE = (13),
  USER_COMB_MODE = (14),
  USER_CENTER_TEXTURE_MODE = (15),
  USER_SCANLINE_MODE = (16),
  NUM_USER_MODES = (17)
};  // user_mode
  // values are fixed because they must correspond to literals in Tcl/Tk

enum AFMStyle {
  SHARP,
  BLUNT,
  SWEEP,
  SEWING,
  FORCECURVE
};  // style


//------------------------------------------------------------------
//   This header contains definitions for architecture-dependent
// types, to make sure size of data type is consistent over a 
// network connection.
#include <vrpn_Types.h>

#endif  // NMB_TYPES_H
