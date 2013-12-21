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

#ifndef MINIT_H
#define MINIT_H


int x_init (char * argv []);
int reset_phantom (void);
int peripheral_init (vrpn_Connection *, const char * handTrackerName,
                     const char * headTrackerName, const char * bdboxName,
                     char * magellanName);
  ///< Calls phantom_init(), sets up head tracker, button box, magellan.
  ///< Takes lots of parameters so we don't have to share globals from
  ///< microscape.c, cutting down on (evil!) use of microscape.h
  ///< which causes circular depenencies which are hard to understand and
  ///< maintain.

int phantom_init (vrpn_Connection *, const char * handTrackerName);
  ///< Sets up phantom.  Exposed so that the phantom connection can be
  ///< torn down and reestablished if we're replaying user input.
int teardown_phantom
             (nm_MouseInteractor ** mousePhantomServer,
              vrpn_ForceDevice_Remote ** forceDevice,
              vrpn_Button_Remote ** phantButton,
              vrpn_Tracker_Remote ** vrpnHandTracker);
  ///< Unregisters callbacks, deletes objects, sets pointers to NULL.


#endif  // MINIT_H


