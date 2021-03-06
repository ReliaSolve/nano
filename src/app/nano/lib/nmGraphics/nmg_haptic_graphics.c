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

#include "nmg_haptic_graphics.h"
#include <globjects.h>

#include <nmb_Types.h>
#include <nmb_Globals.h>
#include <nmb_String.h>
#include <nmb_Line.h>
#include <Tcl_Linkvar.h>
#include <Tcl_Netvar.h>
#include "..\..\interaction.h"


q_vec_type * fg_vertices;
int fg_xside = 0;
int fg_yside = 0;

nmg_haptic_graphics::nmg_haptic_graphics() {

    show_feel_plane = 0;
    show_feel_grid = 0;

}


void nmg_haptic_graphics::get_feel_plane() { 
    return ;
}

void nmg_haptic_graphics::set_show_feel_plane(int on) {

    show_feel_plane = on;
}


void nmg_haptic_graphics::set_show_feel_grid(int on) {

    show_feel_grid = on;
}

int nmg_haptic_graphics::get_show_feel_grid() {
    return show_feel_grid;
}

int nmg_haptic_graphics::get_show_feel_plane() {
    return show_feel_plane;
}

void nmg_haptic_graphics::do_show_feel_plane(int on) {
    enableFeelPlane(this,on);
    config_feelPlane_temp = on;
    showing_feel_plane = on;  
}

void nmg_haptic_graphics::do_show_feel_grid(int on) {
    enableFeelGrid(on);
    config_feelGrid_temp = on;
    showing_feel_grid = on;

}


void nmg_haptic_graphics::update_origin(q_vec_type * origin, q_vec_type * normal ) {
  q_vec_copy(*origin, fp_origin);
  q_vec_copy(*normal, fp_normal);

}

void nmg_haptic_graphics::setFeelPlane(q_vec_type origin, q_vec_type normal ) {
  q_vec_copy(fp_origin, origin);
  q_vec_copy(fp_normal, normal);

  q_vec_copy(fp_origin_j, origin);
  q_vec_copy(fp_normal_j, normal);

}

void nmg_haptic_graphics::setFeelGrid(int xside, int yside, 
                                      q_vec_type * vertices){

    fg_xside = xside;
    fg_yside = yside;
    fg_vertices = vertices;
    

}
