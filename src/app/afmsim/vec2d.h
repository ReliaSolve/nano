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

/*==========================================================================
 *
 *  Copyright (C) 1997 Warren Robinett. All Rights Reserved.
 *
 *  File:       vec2d.h
 *  Content:    header file for class Vec2d
 *
 *  
 *  
 *
 *  See file skel.doc for documentation, including bibliography
 *
 ***************************************************************************/

#ifndef VEC2D_INCLUDED
#define VEC2D_INCLUDED


#if 0 // don't need this under C++
typedef int bool;
#define true 1
#define false 0
#endif


class Vec2d {
public:
	double x;
	double y;

	Vec2d() : x(0.), y(0.) {}						// default constructor: default = (0.,0.)
	Vec2d( double xx, double yy ) : x(xx), y(yy) {}

//	~Vec2d();										// destructor			--implicit OK
//	Vec2d operator=( Vec2d src& );					// assignment operator	--implicit OK
//	Vec2d Vec2d( Vec2d src& );						// copy constructor		--implicit OK
		// See p271 Stroustrup: "for types where the default copy constructor has the right
		// semantics, I prefer to rely on that default."
		// "I use a reference argument for the copy constructor because I must."

	// Only put functions in the class that modify the data members (eg +=).
	// Other functions that just produce new data values are defined outside the class.
	// See p267 Stroustrup.  
	Vec2d operator+=( Vec2d b )			{ x += b.x;     y += b.y;     return *this; }
	Vec2d operator-=( Vec2d b )			{ x -= b.x;     y -= b.y;     return *this; }

	Vec2d operator+=( double scalar )	{ x += scalar;  y += scalar;  return *this; }
	Vec2d operator-=( double scalar )	{ x -= scalar;  y -= scalar;  return *this; }
	Vec2d operator*=( double scalar )	{ x *= scalar;  y *= scalar;  return *this; }
	Vec2d operator/=( double scalar )	{ x /= scalar;  y /= scalar;  return *this; }

	Vec2d	rotate( double angle );
};

// non-member functions
Vec2d	operator-( Vec2d a);

Vec2d	operator+( Vec2d a, Vec2d b);
Vec2d	operator+( Vec2d a, double s);
Vec2d	operator+( double s, Vec2d b);

Vec2d	operator-( Vec2d a, Vec2d b);
Vec2d	operator-( Vec2d a, double s);
Vec2d	operator-( double s, Vec2d b);

Vec2d	operator*( Vec2d a, double s);
Vec2d	operator/( Vec2d a, double s);

bool	operator==( Vec2d a, Vec2d b);
bool	operator!=( Vec2d a, Vec2d b);
bool	operator<(  Vec2d a, Vec2d b);

double	dotProduct( Vec2d a, Vec2d b);


#endif  // VEC2D_INCLUDED
