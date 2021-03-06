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

#ifndef VRML_H
#define VRML_H

int write_to_vrml_file (const char * filename, nmb_PlaneSelection planes,
                        GLdouble minColor [3], GLdouble maxColor [3]);

/**	This routine finds the normal to the surface at the given grid
 * point.  It does this by averaging the normals with the four 4-connected
 * points in the grid (one away in x or y.)
 *	This routine takes into account the current stride in x and y
 * between tesselated points and grid points.
 *
 * x and y are the indices of the point.  dx and dy are the distances between
 * points in the X and Y directions.  I think dz is the Z scale.
 *
 *	This routine returns 0 on success and -1 on failure.
 */
int vrml_compute_plane_normal(const BCPlane *plane, int x,int y,
                              double dx,double dy,double dz, GLfloat Normal[3]);

#endif  // VRML_H
