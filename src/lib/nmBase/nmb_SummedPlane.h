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


#ifndef _NMB_SUMMEDPLANE_H
#define _NMB_SUMMEDPLANE_H

#include "nmb_CalculatedPlane.h"
#include "nmb_Dataset.h"

#ifdef _WIN32
// turns off warning C4290: C++ Exception Specification ignored
#pragma warning( push )
#pragma warning( disable : 4290 )
#endif


/**
   A class representing a data plane computed from the weighted sum of 
   two other planes.  The formula used is:  
     output_plane = first_plane + ( scale * second_plane )
   To do the subtraction of two planes, the scale should be -1.
*/
class nmb_SummedPlane 
  : public nmb_CalculatedPlane
{
public:
  // Constructor
  nmb_SummedPlane( const char* inputPlaneName1,
		   const char* inputPlaneName2,
		   double scale,
		   const char* outputPlaneName,
		   nmb_Dataset* dataset 
		   // Dataset to which this plane will be added.
		   )
    throw( nmb_CalculatedPlaneCreationException );
  
  virtual ~nmb_SummedPlane( );

  // Accessor.  Returns true if this calc'd plane depend on
  // (is calculated from) the specified plane.
  virtual bool dependsOnPlane( const BCPlane* const plane );
  virtual bool dependsOnPlane( const char* planeName );

  // Packs up and sends across the connection all the data
  // necessary for the other end to recreate this calculated 
  // plane.
  void sendCalculatedPlane( vrpn_Connection* conn, vrpn_int32 senderID,
			    vrpn_int32 synchCalculatedPlaneMessageType ) const;
  
protected:

  BCPlane* sourcePlane1;
  BCPlane* sourcePlane2;
  double scale;

  // create a new plane according to the data from vrpn
  static nmb_CalculatedPlane*
  _handle_PlaneSynch( vrpn_HANDLERPARAM p, nmb_Dataset* dataset )
    throw( nmb_CalculatedPlaneCreationException );
  friend class nmb_CalculatedPlane;

  // Update the calculated plane for changes in the source plane
  static void sourcePlaneChangeCallback( BCPlane* plane, int x, int y,
					 void* userdata );
  
  // non-static member function to handle changes in the source plane
  void _handleSourcePlaneChange( int x, int y );

private:
	nmb_SummedPlane( ) : nmb_CalculatedPlane( "", NULL )
	{ }

}; // end class nmb_SummedPlane


#ifdef _WIN32
#pragma warning( pop )
#endif

#endif // _NMB_SUMMEDPLANE_H
