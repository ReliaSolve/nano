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

#ifndef EDGETABLE_H
#define EDGETABLE_H

class EdgeTableEntry {
 public:
  EdgeTableEntry(int ymax = 0, double xmin = 0.0, double deltaX = 1.0):
        d_yMax(ymax), d_xMin(xmin), d_deltaX(deltaX) {}
  EdgeTableEntry(const EdgeTableEntry &ete):
        d_yMax(ete.d_yMax), d_xMin(ete.d_xMin), d_deltaX(ete.d_deltaX){}

  int operator== (const EdgeTableEntry &ete) {
    return (d_yMax == ete.d_yMax &&
            d_xMin == ete.d_xMin &&
            d_deltaX == ete.d_deltaX);
  }

  int operator< (const EdgeTableEntry &ete) {
    if (d_xMin == ete.d_xMin) {
      return (d_deltaX < ete.d_deltaX);
    }
    return (d_xMin < ete.d_xMin);
  }
  int d_yMax;  // in units of d_area_inter_dot_dist_nm
  double d_xMin; // in nm
  double d_deltaX; // in nm per d_area_inter_dot_dist_nm
};

#endif
