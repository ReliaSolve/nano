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

#ifndef URHEIGHTFIELD_H
#define URHEIGHTFIELD_H

#include "URender.h"
#include "nmb_Image.h"

class URHeightField : public URender{
private:
 public:
  URHeightField();
  virtual ~URHeightField();
  virtual int Render(void *userdata=NULL);
  void setSurface(nmb_Image *heightValues, double xmin, double ymin,
	  double xmax, double ymax, int stride = 1);
  void renderWithoutDisplayList(nmb_Image *heightValues,
	  double xmin, double ymin, double xmax, double ymax,
	  int stride = 1);
  void buildDisplayList(nmb_Image *heightValues, int stride);
  void setWorldFromObjectTransform(double *matrix);
  /// This is an alternative way to set the worldFromObject transformation
  /// and it sets the transformation so that the surface spans and is 
  /// bounded by the specified rectangle in world coordinates
  void setSurfaceRegion(double minX, double minY, double maxX, double maxY);
  void setTextureEnable(bool enable);

  int SetProjTextureAll(void *userdata=NULL);
  int SetTextureTransformAll(void *userdata=NULL);
 
 private:
   static int computeNormal(nmb_Image *heightValues, 
	   int x, int y, double normal[3]);

   GLuint d_displayListID;
   GLdouble d_worldFromObject[16];
   bool d_textureEnabled;
   double d_minX, d_minY, d_maxX, d_maxY;
};

#endif
