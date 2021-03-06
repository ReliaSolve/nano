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

#ifndef PixelBuffer_Class
#define PixelBuffer_Class


#include <stdlib.h>
#include <string.h>


#ifndef bool
#define bool   char
#define true   1
#define false  0
#endif


class PixelBuffer
{
   protected:
      unsigned char *pixels;

      unsigned int nRows, nColumns;

      unsigned short nColors;

      bool valid;


      inline void Die(void)
      {
         if (pixels)
            delete [] pixels;

         nRows = nColumns = nColors = 0;

         pixels = NULL;

         valid = false;
      }

      inline void Copy(const PixelBuffer &p)
      {
         if ((valid = p.valid))
         {
            nRows = p.nRows;
            nColumns = p.nColumns;
            nColors = p.nColors;

            pixels = new unsigned char[nRows * nColumns * nColors];
            memcpy(pixels, p.pixels, nRows * nColumns * nColors);
         }
      }


   public:
      ~PixelBuffer(void) { Die(); }

      PixelBuffer(void) :
         pixels(NULL),
         nRows(0),
         nColumns(0),
         nColors(0),
         valid(false)
      { }

      PixelBuffer
      (
         const int            y,                // Image height
         const int            x,                // Image width
         const short          c,                // Image colors
         const unsigned char *p = NULL,         // Array of pixels to copy
         const bool           reverse = false   // Rows are in reverse order
         // Note: If you are grabbing the GL frame buffer, you'll need to use
         // align = true and reverse = true when you make a this call
      ) :
         nRows(y),
         nColumns(x),
         nColors(c),
         valid(true)
      {
         pixels = new unsigned char[y*x*c];

         if (p)
         {
            if (!reverse)
               memcpy(pixels, p, y*x*c);
            else
               for (unsigned int i = 0; i < nRows; i++)
                  memcpy(pixels + (y-1-i)*x*c, p + i*x*c, x*c);
         }
         else
            memset(pixels, 0, nRows * nColumns * nColors);
      }

      PixelBuffer(const PixelBuffer &p) { Copy(p); }

      inline unsigned int Rows(void) { return nRows; }

      inline unsigned int Columns(void) { return nColumns; }

      inline unsigned short Colors(void) { return nColors; }

      inline bool Valid(void) { return valid; }

      // This gives direct access to the data in the pixel array,
      // that's bad. But doing so makes this class fast, that's good.
      inline unsigned char &Pixel
      (
         const int   y = 0,
         const int   x = 0,
         const short c = 0
      )
      {
         return pixels[c + x*nColors + y*nColors*nColumns];
      }

      // This gives direct access to the data in the pixel array,
      // that's bad. But doing so makes this class fast, that's good.
      inline const unsigned char &Pixel
      (
         const int   y = 0,
         const int   x = 0,
         const short c = 0
      ) const
      {
         return pixels[c + x*nColors + y*nColors*nColumns];
      }

      // This gives a bilinearly interpolated value from the pixel array,
      // input coordinates should be in the range [0..1) but x,y values will
      // be clamped from above
      inline unsigned char interpPixel
      (
         const double   y = 0.0,
         const double   x = 0.0,
         const short c = 0
      )
      {
	 unsigned char c00,c01,c10,c11;
	 unsigned char result;
	 double y_r = y*nRows, x_r = x*nColumns; // scale by image size
	 unsigned int x0_i = (unsigned int)x_r, y0_i = (unsigned int)y_r;	// truncate
	 unsigned int x1_i = x0_i+1, y1_i = y0_i+1;
	 double a,b;
	 if (x0_i > nColumns-1 || x1_i > nColumns-1) {
	     x0_i = nColumns-1;
	     x1_i = nColumns-1;
	     a = 1.0;
	 }
	 else a = (double)x1_i - x_r;
	 if (y0_i > nRows-1 || y1_i > nRows-1) {
	     y0_i = nRows-1;
	     y1_i = nRows-1;
	     b = 1.0;
	 }
	 else b = (double)y1_i - y_r;

	 c00 = pixels[c + x0_i*nColors + y0_i*nColors*nColumns];
	 c01 = pixels[c + x0_i*nColors + y1_i*nColors*nColumns];
	 c10 = pixels[c + x1_i*nColors + y0_i*nColors*nColumns];
	 c11 = pixels[c + x1_i*nColors + y1_i*nColors*nColumns];
	 result = (unsigned char)(c00*a*b + c01*a*(1-b) + 
			c10*(1-a)*b + c11*(1-a)*(1-b));
	 return result;
      }


      inline PixelBuffer &operator=(const PixelBuffer &p)
      {
         if (this != &p)
         {
            Die();
            Copy(p);
         }

         return *this;
      }
};


#endif
