#/* The nanoManipulator and its source code have been released under the
# * Boost software license when nanoManipulator, Inc. ceased operations on
# * January 1, 2014.  At this point, the message below from 3rdTech (who
# * sublicensed from nanoManipulator, Inc.) was superceded.
# * Since that time, the code can be used according to the following
# * license.  Support for this system is now through the NIH/NIBIB
# * National Research Resource at cismm.org.
#
#Boost Software License - Version 1.0 - August 17th, 2003
#
#Permission is hereby granted, free of charge, to any person or organization
#obtaining a copy of the software and accompanying documentation covered by
#this license (the "Software") to use, reproduce, display, distribute,
#execute, and transmit the Software, and to prepare derivative works of the
#Software, and to permit third-parties to whom the Software is furnished to
#do so, all subject to the following:
#
#The copyright notices in the Software and this entire statement, including
#the above license grant, this restriction and the following disclaimer,
#must be included in all copies of the Software, in whole or in part, and
#all derivative works of the Software, unless such copies or derivative
#works are solely in the form of machine-executable object code generated by
#a source language processor.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
#SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
#FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
#ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#DEALINGS IN THE SOFTWARE.
#*/

#/*===3rdtech===
#  Copyright (c) 2000 by 3rdTech, Inc.
#  All Rights Reserved.
#
#  This file may not be distributed without the permission of 
#  3rdTech, Inc. 
#  ===3rdtech===*/


#################################    
#
# This script brings up sliders to control the mapping of values
# into colors on the surface.  A colorbar is displayed which shows the
# current colormap being applied to the surface four sliders control
# the mapping of data values to color.  On the right are the upper and
# lower bounds on the colormap: which data points are represented by
# the maximum and minimum values in the colormap. These controls can
# be used to clamp values above or below a certain point in the data.
# On the left are the upper and lower bounds of the applied colormap,
# these arrows indicate the range of the colormap that is in use. They
# are used to limit the applied color to a subrange of the complete
# colormap. For example they enable the user to apply only the red to
# white range of a blackbody colormap to the data.  

# The entire file is contained in this function, so we can
# call this fcn more than once to create different colormap controls. 
proc colormap_controls {cm_frame cm_arr_name plane_varname plane_label plane_list} {
    # These globals are shared between colormap widgets. 
    global colorMapNames inputPlaneNames
    global triangle_width triangle_height
    global box_width box_height
    global canvas_lower_bound canvas_upper_bound image_heightf
    global $cm_arr_name 

    # all data and widget info specific to this colormap 
    # is stored in this associative array. 
    upvar #0 $cm_arr_name cmData

# store the name of our parent frame. 
set cmData(topframe) $cm_frame

# Frame for the canvas. Must be packed first for side-by-side layout. 
pack [frame $cmData(topframe).cframe] -side right

# Make a frame to hold the pull-down menus that select from the lists
frame $cmData(topframe).pickframe

pack $cmData(topframe).pickframe -side top -fill both

# Pop-up menu to choose which plane is displayed as a color map.
generic_optionmenu $cmData(topframe).pickframe.colormap_plane \
        $plane_varname $plane_label $plane_list 

if { ![info exists colorMapNames] } {set colorMapNames {none }}

# create the unique image for the first menu entry
image create photo "${cm_arr_name}(cm_image_none)" 

# Change the colormap menu from words to images. 
# Images are exported by the C code. 
proc show_colormap_images {win cm_name nm el op} {
    global colorMapNames
    global $cm_name

    set m [$win component popupMenu] 
    set i 0
    # Special handling for colormap 0, it's different for
    # different colormap interfaces. 
    # All other colormaps are the same for all colormap windows. 
    foreach my_name $colorMapNames {
        if { $i == 0 } {
            #set name ${cm_name}(cm_image_none)
            #puts $name
          $m entryconfigure $i -image ${cm_name}(cm_image_none) -columnbreak 1
        } else {
            $m entryconfigure $i -image cm_image_$my_name -columnbreak 1
        }
        incr i
    }
    # Handle a really long list of colormaps. 
    set j [expr $i/12]
    if { $j > 0 } {
        for {set k 0} {$k < $i} { incr k} {
            if {[expr $k % $j]} {
                $m entryconfigure $k -columnbreak 0
            }
        }
    }
}

trace variable colorMapNames w [list show_colormap_images $cmData(topframe).pickframe.colormap $cm_arr_name]

generic_optionmenu $cmData(topframe).pickframe.colormap \
	${cm_arr_name}(color_map) \
	"Colormap" colorMapNames
# This doesn't actually set the width, but it prevents the width
# from being too large
$cmData(topframe).pickframe.colormap configure -width 30

iwidgets::Labeledwidget::alignlabels \
	$cmData(topframe).pickframe.colormap_plane \
	$cmData(topframe).pickframe.colormap
proc cm_autoscale {cm_name} {
    upvar #0 $cm_name cmData
    # resetting the color plane forces color_m*_limit to be reset. 
    set cmData(color_comes_from) $cmData(color_comes_from)
    adjust_color_min_max $cm_name autoscale autoscale autoscale
}

# Reset the colormap sliders:
button $cmData(topframe).pickframe.autoscale -text "Auto-Scale"  \
        -command [list cm_autoscale $cm_arr_name]

pack $cmData(topframe).pickframe.colormap_plane \
	$cmData(topframe).pickframe.colormap \
        $cmData(topframe).pickframe.autoscale -side top -anchor nw -fill x

# frame for buttons. Don't -fill, so they won't grow too big, but will be aligned. 
frame $cmData(topframe).childsite
pack $cmData(topframe).childsite -side top -anchor nw

#set these so we can see do " wishx <mainwin.tcl" and test interface
set cmData(color_min) 0
set cmData(color_max) 1.0

#
# The color_*_limit variables are the minimum and maximum data values
# in the current colorplane:
set cmData(color_limit_min) 0
set cmData(color_limit_max) 1
trace variable ${cm_arr_name}(color_limit_min) w [list adjust_color_min_max $cm_arr_name]
trace variable ${cm_arr_name}(color_limit_max) w [list adjust_color_min_max $cm_arr_name]

# If the limits change, this indicates a new context, so 
# we should reset the widget to it's default state. 
proc adjust_color_min_max {cm_name name element op} {
    upvar #0 $cm_name cmData
    set cmData(d_min_changed) 1
    set cmData(d_min_value) 0
    set cmData(d_max_changed) 1
    set cmData(d_max_value) 1.0
    set cmData(c_min_changed) 0
    set cmData(c_min_value) $cmData(color_limit_min)
    set cmData(c_max_changed) 0
    set cmData(c_max_value) $cmData(color_limit_max)
}

set triangle_width 10
set triangle_height 8
set box_width 6
set box_height 8

set canvas_lower_bound 9
set canvas_upper_bound 201

set image_x 200 
set image_y 201
set image_width 32
set image_height 192
set image_heightf 192.0

set cmData(canvas) [canvas $cmData(topframe).cframe.canvas -background LemonChiffon1 \
        -height [expr $canvas_upper_bound + $canvas_lower_bound]]
pack $cmData(canvas) 

# The data_x, data_y_* variables represent the (x,y) coordinates of the 
# data triangles in canvas coordinates:
set cmData(data_x) [expr $image_x - $image_width - 10]
set cmData(data_y_low) $image_y
set cmData(data_y_high) [expr $image_y - $image_height]

# The color_x, color_y_* variables represent the (x,y) coordinates of the 
# color triangles in canvas coordinates:
set cmData(color_x) [expr $image_x + 10]
set cmData(color_y_low) $image_y
set cmData(color_y_high) [expr $image_y - $image_height]

#
# cmData contains the values displayed in the data text boxes.
# The values range from 0 - 1. The *_changed variables are used
# to indicate whether the values were changed by interactively sliding
# the triangle, or by typing in the textbox.
set cmData(d_min_value) 0
set cmData(d_max_value) 1
set cmData(d_min_changed) 0
set cmData(d_max_changed) 0

#
# cmData contains the values displayed in the color text boxes.
# The values range from the minimum data value to the maximum data value. 
# contained in the current colorplane as scaled by the cmData triangle range.
# (see below). The *_changed variables are used
# to indicate whether the values were changed by interactively sliding
# the triangle, by typing in the textbox, or because the data triangles
# changed.
set cmData(c_min_value) 0
set cmData(c_max_value) 1
set cmData(c_min_changed) 0
set cmData(c_max_changed) 0

# The cmData(c_min_value) and cmData(c_max_value) are related to the  
# cmData(d_min_value) and cmData(d_max_value) through a scaling operation. 
# For example the cmData(c_max_value)
# cmData(c_max_value) = (colorplane data range)/(data arrows range) *  cmData(color_max) +
#                      ((cmData(color_min) * cmData(d_max_value) - (cmData(color_max) * cmData(d_min_value))/
#                      (data arrows range)


# Draws a triangle pointing right:
proc left_triangle { x y } {
    global triangle_width triangle_height
    return [list [expr $x + $triangle_width] $y \
	    [expr $x - $triangle_width] [expr $y + $triangle_height] \
	    [expr $x - $triangle_width] [expr $y - $triangle_height]]
}

# Draws a triangle pointing left:
proc right_triangle { x y } {
    global triangle_width triangle_height
    return [list [expr $x - $triangle_width] $y \
	    [expr $x + $triangle_width] [expr $y + $triangle_height] \
	    [expr $x + $triangle_width] [expr $y - $triangle_height]]
}
proc draw_line { x y1 y2 } {
    return [list  $x [expr $y2] $x [expr $y1] ]
}

proc draw_rectangle { x y1 y2 } {
    global box_width box_height

    return [list \
	    [expr $x - $box_width]\
	    [expr $y1 + ($y2 - $y1)/2 - $box_height] \
	    [expr $x  + $box_width]\
	    [expr $y1 + ($y2 - $y1)/2 + $box_height] \
	    ]
}
### Line - drawn first so it is below box and tris
eval $cmData(canvas) create line [draw_line $cmData(data_x) $cmData(data_y_low) $cmData(data_y_high) ] \
	-width 3 -fill blue4 -tags data_line
###
### The box to drag both Data selectors.
### Drawn first so triangles get dragged if there is overlap. 
###
eval $cmData(canvas) create rectangle \
	[draw_rectangle $cmData(data_x) $cmData(data_y_low) $cmData(data_y_high)]\
	-fill blue4 -outline blue4 -tags data_line_box
$cmData(canvas) bind data_line_box <B1-Motion>\
	[list colormap_set_data_line $cm_arr_name $cmData(canvas) %y]

###
### The triangle and entry widget for the Data Min selector
### the motion of the triangle is restricted to up/down.
###
eval $cmData(canvas) create polygon [left_triangle $cmData(data_x) $cmData(data_y_low)] \
	-fill blue4 -tags data_min_tri
$cmData(canvas) bind data_min_tri <B1-Motion>\
	[list colormap_set_data_min $cm_arr_name $cmData(canvas) %y]
generic_entry $cmData(topframe).data_min_entry ${cm_arr_name}(d_min_value) "Data Min" real
trace variable ${cm_arr_name}(d_min_value) w [list adjust_data_min $cm_arr_name $cmData(canvas)]
$cmData(canvas) create window [expr $cmData(data_x) - $triangle_width] $cmData(data_y_low)\
	-anchor e -window $cmData(topframe).data_min_entry -tags data_min_entry

###
### The triangle and entry widget for the Data Max selector
### the motion of the triangle is restricted to up/down.
###
eval $cmData(canvas) create polygon [left_triangle $cmData(data_x) $cmData(data_y_high)]\
	-fill blue4 -tags data_max_tri
$cmData(canvas) bind data_max_tri <B1-Motion>\
	[list colormap_set_data_max $cm_arr_name $cmData(canvas) %y]
generic_entry $cmData(topframe).data_max_entry ${cm_arr_name}(d_max_value) "Data Max" real
trace variable ${cm_arr_name}(d_max_value) w [list adjust_data_max $cm_arr_name $cmData(canvas)]
$cmData(canvas) create window [expr $cmData(data_x) - $triangle_width] $cmData(data_y_high)\
	-anchor e -window $cmData(topframe).data_max_entry -tags data_max_entry


### Line - drawn first so it is below box and tris
eval $cmData(canvas) create line [draw_line $cmData(color_x) $cmData(color_y_low) $cmData(color_y_high)]\
	-width 3 -fill blue4 -tags color_line

###
### The box to drag both Color selectors.
### Drawn first so triangles get dragged if there is overlap. 
###
eval $cmData(canvas) create rectangle \
	[draw_rectangle $cmData(color_x) $cmData(color_y_low) $cmData(color_y_high)] \
	-fill blue4 -outline blue4 -tags color_line_box
$cmData(canvas) bind color_line_box <B1-Motion> \
	[list colormap_set_color_line $cm_arr_name $cmData(canvas) %y]
###
### The triangle and entry widget for the Color Min selector
### the motion of the triangle is restricted to up/down.
###
eval $cmData(canvas) create polygon [right_triangle $cmData(color_x) $cmData(color_y_low)]\
	-fill blue4 -tags color_min_tri
$cmData(canvas) bind color_min_tri <B1-Motion> \
	[list colormap_set_color_min $cm_arr_name $cmData(canvas) %y]
generic_entry $cmData(topframe).color_min_entry ${cm_arr_name}(c_min_value) "Color Min" real
trace variable ${cm_arr_name}(c_min_value) w [list adjust_color_min $cm_arr_name $cmData(canvas)]
$cmData(canvas) create window [expr $cmData(color_x) + $triangle_width] $cmData(color_y_low)\
	-anchor w -window $cmData(topframe).color_min_entry -tags color_min_entry

###
### The triangle and entry widget for the Color Max selector
### the motion of the triangle is restricted to up/down.
###
eval $cmData(canvas) create polygon [right_triangle $cmData(color_x) $cmData(color_y_high)]\
	-fill blue4 -tags color_max_tri
$cmData(canvas) bind color_max_tri <B1-Motion> \
	[list colormap_set_color_max $cm_arr_name $cmData(canvas) %y]
generic_entry $cmData(topframe).color_max_entry ${cm_arr_name}(c_max_value) "Color Max" real
trace variable ${cm_arr_name}(c_max_value) w [list adjust_color_max $cm_arr_name $cmData(canvas)]
$cmData(canvas) create window [expr $cmData(color_x) + $triangle_width] $cmData(color_y_high)\
	-anchor w -window $cmData(topframe).color_max_entry -tags color_max_entry

set imh [image create photo "${cm_arr_name}(colormap_image)" ]
#-height $image_height -width $image_width]
$cmData(canvas) create image $image_x $image_y -anchor se -image $imh

## This function gets called when the user moves the data_min triangle
## It makes sure the triangle is w/in the bounds of the canvas and
## transforms the value from canvas coordinates to between 0-1.
## It then sets the data variables cmData(data_min), which is used in unix,
## and cmData(d_min_value/changed) which are used to display in tcl.
## Once cmData(d_min_value) is set a callback is activated. See adjust_data_min.
proc colormap_set_data_min { cm_name win y } {
    global canvas_lower_bound canvas_upper_bound image_heightf
    upvar #0 $cm_name cmData

    set y1 $y
    if { $y1 <= $canvas_lower_bound } {
	set y1 $canvas_lower_bound
    } elseif { $y1 >= $canvas_upper_bound } {
	set y1 $canvas_upper_bound
    }

    set cmData(data_min) [expr 1.0 - (($y1 - $canvas_lower_bound)/$image_heightf)]
    set cmData(d_min_changed) 1
    set cmData(d_min_value) $cmData(data_min)
}

## This function gets called whenever cmData(d_min_value) is changed,
## either by moving the data_min triangle, by typing into the textbox,
## or by pressing the autoscale button. 
## In all cases the triangle, line, and box icons are repositioned.
proc adjust_data_min { cm_name win name element op } {
    global canvas_lower_bound canvas_upper_bound triangle_width
    global image_heightf
    upvar #0 $cm_name cmData

    ## Makes sure the values are w/in range:
    if { $cmData(d_min_value) < 0 } {
	set cmData(d_min_value) 0
    } elseif { $cmData(d_min_value) > 1.0 } {
	set cmData(d_min_value) 1.0
    }

    ## Changes the cmData(c_max_value) variable to correctly represent
    ## the new data range. This is used when the data_min/max are
    ## set so that only a subrange of the colormap is applied to the
    ## data, in that case the cmData(c_*_value) displayed are no longer
    ## w/in the range of the min/max data points in the colorplane
    ## but actually extend outside the colorplane range.
    ## if $cmData(d_min_changed) ==2 then the values have already been set.
    if { $cmData(d_min_changed) != 2 } {
	set cmData(c_min_changed) 2
	set cmData(c_min_value) [scale_color_min $cmData(color_limit_max) $cmData(color_limit_min) \
		$cmData(color_min) $cmData(d_max_value) $cmData(d_min_value)]
    }
    ##
    ## Calculate where to position arrow and box:
    ##
    set y1 [expr 1.0 - $cmData(d_min_value)]
    set y1 [expr $y1 * ($canvas_upper_bound - $canvas_lower_bound)]
    set y1 [expr $y1 + $canvas_lower_bound]
    
    if { $y1 < $canvas_lower_bound } {
	set y1 $canvas_lower_bound
    } elseif { $y1 > $canvas_upper_bound } {
	set y1 $canvas_upper_bound
    }
    set cmData(data_min) [expr 1.0 - (($y1 - $canvas_lower_bound)/$image_heightf)]
    set cmData(data_y_low) $y1
    
    ## reposition line:
    eval $win coords data_line [draw_line $cmData(data_x) $cmData(data_y_low)  $cmData(data_y_high)]
    
    ## reposition box:
    eval $win coords data_line_box [draw_rectangle $cmData(data_x) $cmData(data_y_low) $cmData(data_y_high)]

    ## reposition arrow:
    eval $win coords data_min_tri [left_triangle $cmData(data_x) $cmData(data_y_low)]
    $win coords data_min_entry [expr $cmData(data_x) - $triangle_width] $cmData(data_y_low)
}

## This function gets called when the user moves the data_max triangle
## It makes sure the triangle is w/in the bounds of the canvas and
## transforms the value from canvas coordinates to between 0-1.
## It then sets the data variables cmData(data_max), which is used in unix,
## and cmData(d_max_value/changed) which are used to display in tcl.
## Once cmData(d_max_value) is set a callback is activated. See adjust_data_max.
proc colormap_set_data_max { cm_name win y } {
    global canvas_lower_bound canvas_upper_bound
    global image_heightf
    upvar #0 $cm_name cmData

    set y1 $y
    if { $y1 < $canvas_lower_bound } {
	set y1 $canvas_lower_bound
    } elseif { $y1 > $canvas_upper_bound } {
	set y1 $canvas_upper_bound
    }
    set cmData(data_max) [expr 1.0 - (($y1 - $canvas_lower_bound)/$image_heightf)]
    set cmData(d_max_changed) 1
    set cmData(d_max_value) $cmData(data_max)
}

## This function gets called whenever cmData(d_max_value) is changed,
## either by moving the data_max triangle, by typing into the textbox,
## or by pressing the autoscale button. 
## In all cases the triangle, line, and box icons are repositioned.
proc adjust_data_max { cm_name win name element op } {
    global canvas_lower_bound canvas_upper_bound triangle_width
    global image_heightf
    upvar #0 $cm_name cmData

    ## Makes sure the number typed in is w/in range:
    if { $cmData(d_max_value) < 0 } {
	set cmData(d_max_value) 0.0
    } elseif { $cmData(d_max_value) > 1.0 } {
	set cmData(d_max_value) 1.0
    }

    ## Changes the cmData(c_max_value) variable to correctly represent
    ## the new data range. This is used when the data_min/max are
    ## set so that only a subrange of the colormap is applied to the
    ## data, in that case the cmData(c_*_value) displayed are no longer
    ## w/in the range of the min/max data points in the colorplane
    ## but actually extend outside the colorplane range.
    ## if $cmData(d_max_changed) ==2 then the values have already been set.
    if { $cmData(d_max_changed) != 2} {
	set cmData(c_max_changed) 2
	set cmData(c_max_value) [scale_color_max $cmData(color_limit_max) $cmData(color_limit_min) \
		$cmData(color_max) $cmData(d_max_value) $cmData(d_min_value)]
    }
    ##
    ## Calculate where to position arrow and box:
    ##
    set y1 [expr 1.0 - $cmData(d_max_value)]
    set y1 [expr $y1 * ($canvas_upper_bound - $canvas_lower_bound)]
    set y1 [expr $y1 + $canvas_lower_bound]
    
    if { $y1 < $canvas_lower_bound } {
	set y1 $canvas_lower_bound
    } elseif { $y1 > $canvas_upper_bound } {
	set y1 $canvas_upper_bound
    }
    set cmData(data_max) [expr 1.0 - (($y1 - $canvas_lower_bound)/$image_heightf)]
    set cmData(data_y_high) $y1
    
    ## reposition line:
    eval $win coords data_line [draw_line $cmData(data_x) $cmData(data_y_low)  $cmData(data_y_high)]
    
    ## reposition box:
    eval $win coords data_line_box [draw_rectangle $cmData(data_x) $cmData(data_y_low) $cmData(data_y_high)]

    ## reposition arrow:
    eval $win coords data_max_tri [left_triangle $cmData(data_x) $cmData(data_y_high)]
    $win coords data_max_entry [expr $cmData(data_x) - $triangle_width] $cmData(data_y_high)
}

## This function gets called when the box icon connecting the data triangles
## is moved. In that case both triangles move as a unit.
## Both callbacks get called as well, repositioning the icons and changing
## the cmData(c_*_values) displayed.
proc colormap_set_data_line { cm_name win y } {
    global canvas_lower_bound canvas_upper_bound
    global image_heightf
    upvar #0 $cm_name cmData

    set y1 $y
    if { $y1 < $canvas_lower_bound } {
	set y1 $canvas_lower_bound
    } elseif { $y1 > $canvas_upper_bound } {
	set y1 $canvas_upper_bound
    }

    set dif [expr (($cmData(data_y_high) - $y1) - ($y1 - $cmData(data_y_low)))/2]

    set data_y_low_tmp [expr $cmData(data_y_low) - $dif] 
    set data_y_high_tmp [expr $cmData(data_y_high) - $dif]
    set size [expr $data_y_low_tmp - $data_y_high_tmp]
    
    ## Checks that the values are w/in the range of the canvas
    ## allows for swapping of the min/max triangles.
    if { $data_y_high_tmp <= $canvas_lower_bound } {
	set cmData(data_y_high) $canvas_lower_bound
	set cmData(data_y_low) [expr $cmData(data_y_high) + $size]
    } elseif { $data_y_high_tmp >= $canvas_upper_bound } {
	set cmData(data_y_high) $canvas_upper_bound
	set cmData(data_y_low) [expr $cmData(data_y_high) - $size]
    } elseif { $data_y_low_tmp <= $canvas_lower_bound } {
	set cmData(data_y_low) $canvas_lower_bound
	set cmData(data_y_high) [expr $cmData(data_y_low) + $size]
    } elseif { $data_y_low_tmp >= $canvas_upper_bound } {
	set cmData(data_y_low) $canvas_upper_bound
	set cmData(data_y_high) [expr $cmData(data_y_low) - $size]
    } else {
	set cmData(data_y_low) $data_y_low_tmp
	set cmData(data_y_high) $data_y_high_tmp
    }

    set cmData(data_min) [expr 1.0 - ($cmData(data_y_low) - $canvas_lower_bound)/$image_heightf]
    set cmData(data_max) [expr 1.0 - ($cmData(data_y_high) - $canvas_lower_bound)/$image_heightf]


    ## Changes the cmData(c_*_value) variables to correctly represent
    ## the new data range. This is used when the data_min/max are
    ## set so that only a subrange of the colormap is applied to the
    ## data, in that case the cmData(c_*_value) displayed are no longer
    ## w/in the range of the min/max data points in the colorplane
    ## but actually extend outside the colorplane range.

    set cmData(c_max_changed) 1
    set cmData(c_max_value) [scale_color_max $cmData(color_limit_max) $cmData(color_limit_min) \
	    $cmData(color_max) $cmData(data_max) $cmData(data_min)]
    set cmData(c_min_changed) 1
    set cmData(c_min_value) [scale_color_min $cmData(color_limit_max) $cmData(color_limit_min) \
	    $cmData(color_min) $cmData(data_max) $cmData(data_min)]

    set cmData(d_min_changed) 2
    set cmData(d_min_value) $cmData(data_min)
    set cmData(d_max_changed) 2
    set cmData(d_max_value) $cmData(data_max)
}

## This function gets called when the user moves the color_min
## triangle. It makes sure the values are w/in the range of the canvas
## then converts from canvas coordinates to the min-max range
## of the colorplane (taking into account any scaling by the data triangles)
proc colormap_set_color_min { cm_name win y } {
    global canvas_lower_bound canvas_upper_bound image_heightf
    upvar #0 $cm_name cmData

    set y1 $y
    if { $y1 < $cmData(color_y_high) } {
	set y1 $cmData(color_y_high)
    } elseif { $y1 > $canvas_upper_bound } {
	set y1 $canvas_upper_bound
    }
    set cmData(color_min) [expr 1.0 - (($y1 - $canvas_lower_bound)/$image_heightf)]
    set cmData(color_y_low) $y1

    set cmData(c_min_changed) 1
    set cmData(c_min_value) [scale_color_min $cmData(color_limit_max) $cmData(color_limit_min) \
	    $cmData(color_min) $cmData(d_max_value) $cmData(d_min_value)]
}

## This function gets called whenever the cmData(c_min_value) value is 
## set, whether through the autoscale button, the slider, or by changing
## cmData(d_min_value). Depending on how the value changed different things
## need to be recalculated.
proc adjust_color_min { cm_name win name element op } {
    global canvas_lower_bound canvas_upper_bound triangle_width
    global image_heightf
    upvar #0 $cm_name cmData

    ## If the value changed only because cmData(d_min_value) changed,
    ## then it is not necessary to reposition the arrow icon. Otherwise,
    ## if the cmData(color_min) slider moved, or the text box was typed into, reposition the arrow:
    if { $cmData(c_min_changed) != 2 } {
	## == 0 means the textbox was typed into:
	if { $cmData(c_min_changed) == 0 } {
	    set y1 [expr ($cmData(c_min_value) - $cmData(color_limit_min))/ \
		    (0.0 +$cmData(color_limit_max) - $cmData(color_limit_min))]
	    set y1 [expr 1.0 - $y1]
	    set y1 [expr $y1 * ($canvas_upper_bound - $canvas_lower_bound)]
	    set y1 [expr $y1 + $canvas_lower_bound]
	    if { $y1 < $cmData(color_y_high) } {
		set y1 $cmData(color_y_high)
	    } elseif { $y1 > $canvas_upper_bound } {
		set y1 $canvas_upper_bound
	    }
	    set cmData(color_min) [expr 1.0 - (($y1 - $canvas_lower_bound)/$image_heightf)]
	    set cmData(color_y_low) $y1
	}

	## reposition line:
	eval $win coords color_line [draw_line $cmData(color_x) $cmData(color_y_low)  $cmData(color_y_high)]
	
	## reposition box:
	eval $win coords color_line_box \
		[draw_rectangle $cmData(color_x) $cmData(color_y_low) $cmData(color_y_high)]

	## reposition arrow:
	eval $win coords color_min_tri [right_triangle $cmData(color_x) $cmData(color_y_low)]
	$win coords color_min_entry [expr $cmData(color_x) + $triangle_width] $cmData(color_y_low)
    }
    set cmData(c_min_changed) 0
}

## This function gets called when the user moves the color_max
## triangle. It makes sure the values are w/in the range of the canvas
## then converts from canvas coordinates to the min-max range
## of the colorplane (taking into account any scaling by the data triangles)
proc colormap_set_color_max { cm_name win y } {
    global canvas_lower_bound canvas_upper_bound image_heightf 
    upvar #0 $cm_name cmData

    set y1 $y
    if { $y1 < $canvas_lower_bound } {
	set y1 $canvas_lower_bound
    } elseif { $y1 > $cmData(color_y_low) } {
	set y1 $cmData(color_y_low)
    }
    set cmData(color_y_high) $y1
    set cmData(color_max) [expr 1.0 - (($y1 - $canvas_lower_bound)/$image_heightf)]
    set cmData(c_max_changed) 1
    set cmData(c_max_value) [scale_color_max $cmData(color_limit_max) $cmData(color_limit_min) \
	    $cmData(color_max) $cmData(d_max_value) $cmData(d_min_value)]
}

## This function gets called whenever the cmData(c_max_value) value is 
## set, whether through the autoscale button, the slider, or by changing
## cmData(d_max_value). Depending on how the value changed different things
## need to be recalculated.
proc adjust_color_max { cm_name win name element op } {
    global canvas_lower_bound canvas_upper_bound triangle_width
    global image_heightf
    upvar #0 $cm_name cmData

    ## If the value changed only because cmData(d_max_value) changed,
    ## then it is not necessary to reposition the arrow icon. Otherwise,
    ## if the color_min slider moved, or the text box was typed into, reposition the arrow:
    if { $cmData(c_max_changed) != 2 } {
	## == 0 means the textbox was typed into:
	if { $cmData(c_max_changed) == 0 } {
            # force floating point. 
	    set y1 [expr ($cmData(c_max_value) - $cmData(color_limit_min))/ \
		    (0.0 +$cmData(color_limit_max) - $cmData(color_limit_min))]
	    set y1 [expr 1.0 - $y1]
	    set y1 [expr $y1 * ($canvas_upper_bound - $canvas_lower_bound)]
	    set y1 [expr $y1 + $canvas_lower_bound]
	    
	    if { $y1 < $canvas_lower_bound } {
		set y1 $canvas_lower_bound
	    } elseif { $y1 > $cmData(color_y_low) } {
		set y1 $cmData(color_y_low)
	    }
	    set cmData(color_max) [expr 1.0 - (($y1 - $canvas_lower_bound)/$image_heightf)]
	    set cmData(color_y_high) $y1
	} 
	
	## reposition line:
	eval $win coords color_line [draw_line $cmData(color_x) $cmData(color_y_low)  $cmData(color_y_high)]
	
	## reposition box:
	eval $win coords color_line_box \
		[draw_rectangle $cmData(color_x) $cmData(color_y_low) $cmData(color_y_high)]

	## reposition arrow:
	eval $win coords color_max_tri [right_triangle $cmData(color_x) $cmData(color_y_high)]
	$win coords color_max_entry [expr $cmData(color_x) + $triangle_width] $cmData(color_y_high)
    }
    set cmData(c_max_changed) 0
}

## This function gets called when the box icon for the color_min/max arrows
## is moved. It calculates the correct range and moves the arrows as a unit.
proc colormap_set_color_line { cm_name win y } {
    global canvas_lower_bound canvas_upper_bound
    global triangle_width triangle_height box_width box_height
    global image_heightf
    upvar #0 $cm_name cmData

    set y1 $y
    if { $y1 < $canvas_lower_bound } {
	set y1 $canvas_lower_bound
    } elseif { $y1 > $canvas_upper_bound } {
	set y1 $canvas_upper_bound
    }

    set dif [expr (($cmData(color_y_high) - $y1) - ($y1 - $cmData(color_y_low)))/2]

    set color_y_low_tmp [expr $cmData(color_y_low) - $dif] 
    set color_y_high_tmp [expr $cmData(color_y_high) - $dif]
    set size [expr $color_y_low_tmp - $color_y_high_tmp]
    
    if { $color_y_high_tmp < $canvas_lower_bound } {
	set cmData(color_y_high) $canvas_lower_bound
	set cmData(color_y_low) [expr $cmData(color_y_high) + $size]
    } elseif { $color_y_high_tmp > $canvas_upper_bound } {
    } elseif { $color_y_low_tmp < $canvas_lower_bound } {
    } elseif { $color_y_low_tmp > $canvas_upper_bound } {
	set cmData(color_y_low) $canvas_upper_bound
	set cmData(color_y_high) [expr $cmData(color_y_low) - $size]
    } else {
	set cmData(color_y_low) $color_y_low_tmp
	set cmData(color_y_high) $color_y_high_tmp
    }

    ## Set the minimum value displayed in the text box:
    set cmData(color_min) [expr 1.0 - ($cmData(color_y_low) - $canvas_lower_bound)/$image_heightf]

    ## Set the maximum value displayed in the text box:
    set cmData(color_max) [expr 1.0 - ($cmData(color_y_high) - $canvas_lower_bound)/$image_heightf]

    set cmData(c_max_changed) 1
    set cmData(c_max_value) [scale_color_max $cmData(color_limit_max) $cmData(color_limit_min) \
	    $cmData(color_max) $cmData(d_max_value) $cmData(d_min_value)]

    set cmData(c_min_changed) 1
    set cmData(c_min_value) [scale_color_min $cmData(color_limit_max) $cmData(color_limit_min) \
	    $cmData(color_min) $cmData(d_max_value) $cmData(d_min_value)]
}

## Changes the cmData(c_max_value) variable to correctly represent
## the new data range. This is used when the data_min/max are
## set so that only a subrange of the colormap is applied to the
## data, in that case the cmData(c_*_value) displayed are no longer
## w/in the range of the min/max data points in the colorplane
## but actually extend outside the colorplane range.
## if $cmData(d_max_changed) ==2 then the values have already been set.
proc scale_color_max { c_max_limit c_min_limit c_max d_max d_min} {
    if { $d_max != $d_min } {
	set tmp [expr (($c_max_limit - $c_min_limit) / (0.0 +$d_max - $d_min) * $c_max) + \
		((($c_min_limit * $d_max) - ($c_max_limit * $d_min))/(0.0 +$d_max - $d_min))]
    } else {
	## Only happens if the user wants to swap the min/max colors, and is temporary...
	set tmp [expr (($c_max_limit - $c_min_limit) / ($d_max - $d_min + .001) * $c_max) + \
		((($c_min_limit * $d_max) - ($c_max_limit * $d_min))/($d_max - $d_min + .001))]
    }
    return $tmp
}

## Changes the cmData(c_max_value) variable to correctly represent
## the new data range. This is used when the data_min/max are
## set so that only a subrange of the colormap is applied to the
## data, in that case the cmData(c_*_value) displayed are no longer
## w/in the range of the min/max data points in the colorplane
## but actually extend outside the colorplane range.
## if $cmData(d_min_changed) ==2 then the values have already been set.
proc scale_color_min { c_max_limit c_min_limit c_min d_max d_min} {
    if { $d_max != $d_min } {
	set tmp [expr (($c_max_limit - $c_min_limit) / (0.0 +$d_max - $d_min) * $c_min) + \
		((($c_min_limit * $d_max) - ($c_max_limit * $d_min))/(0.0 +$d_max - $d_min))]
    } else {
	## Only happens if the user wants to swap the min/max colors, and is temporary...
	set tmp [expr (($c_max_limit - $c_min_limit) / ($d_max - $d_min + .001) * $c_min) + \
		((($c_min_limit * $d_max) - ($c_max_limit * $d_min))/($d_max - $d_min + .001))]
    }
    return $tmp
}

} 
# end proc colormap_controls

##########
# end ColorMap
