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

/******************************************************************************\
@BCDebug
--------------------------------------------------------------------------------
inheritance: BCDebug
description: 
\******************************************************************************/

#ifndef BCDEBUG_H
#define BCDEBUG_H

#ifdef	_WIN32
#include <windows.h>
#include <winnt.h>
#define	sleep(x)	Sleep(1000*x)
#else
#include <unistd.h> // for sleep()
#endif
#include "BCString.h"


// if you add to this enumerated type be sure to modify BCDebug::initializeStaticVariables
typedef enum 
{
    UNIDENTIFIED_SUBSET = 0,

    GRID_CODE,
    PLANE_CODE,

    // please keep the following list alphabetized to avoid duplication when adding to it

     LAST_SUBSET = 100
} CodeSubset;


const long MAX_NUM_FUNCTIONS = 15; // arbitrary!


class BCDebug
{

  public:

    BCDebug(const BCString& function, const CodeSubset subset = UNIDENTIFIED_SUBSET);
    ~BCDebug();

    void warn(const BCString& label);

    void watch(const BCString& label, const int value);
    void watch(const BCString& label, const float value);
    void watch(const BCString& label, const double value);
    void watch(const BCString& label, const BCString& value);

    void delay(const long seconds);

    int yes();

    int on();
    inline void turnOn() {_on = 0; };
    inline void turnOff() {_on = 0; };

    int indent();
    void drawSeparator();

  protected:

    static int _charactersPerTab;
    static int _charactersPerLine;

    static int _on;

    static int _level;

    static int _timesInvoked;

    static int _unfinishedSubsetArray[LAST_SUBSET];

    typedef struct 
    {
	CodeSubset subset;
	BCString name;
    } Function;
    
    static Function _functionStack[MAX_NUM_FUNCTIONS];

    int _subset;

    void initializeStaticVariables();

  private:

};



#endif // BCDEBUG_H
