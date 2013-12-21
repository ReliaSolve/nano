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

#ifndef NMB_TYPES_H
#define NMB_TYPES_H

//------------------------------------------------------------------
//   This header contains definitions for architecture-dependent
// types, to make sure size of data type is consistent over a 
// network connection.

#include <vrpn_Types.h>


typedef float PointType [3];


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
  USER_REGION_MODE = (17),
  USER_CROSS_SECTION_MODE = (18),
  NUM_USER_MODES = (19)
};  // user_mode
  // values are fixed because they must correspond to literals in Tcl/Tk

enum AFMStyle {
  SHARP,
  BLUNT,
  SWEEP,
  SEWING,
  FORCECURVE
};  // style


#endif  // NMB_TYPES_H
