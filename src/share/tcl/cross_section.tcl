#/*===3rdtech===
#  Copyright (c) 2000 by 3rdTech, Inc.
#  All Rights Reserved.
#
#  This file may not be distributed without the permission of 
#  3rdTech, Inc. 
#  ===3rdtech===*/

#################################################################
# ----------------------------------------------------------------------
set xswidget(main) [create_closing_toplevel cross_section \
        "Cross Section Analysis"]
# We can show the window with show.cross_section procedure.
set xswidget(display_data) [create_closing_toplevel cross_section_data \
        "Cross Section Data"]


# ----------------------------------------------------------------------
set xswidget(cntrl) [frame $xswidget(main).cntrl]
pack $xswidget(cntrl) -side left -expand yes -fill both

# ----------------------------------------------------------------------
# This could easily be "graph" instead of "stripchart", but they are 
# essentially the same, and options are already set up for stripchart. 
set xswidget(stripchart) [stripchart $xswidget(main).sc -title "" ]
pack $xswidget(stripchart) -side left -expand yes -fill both

$xswidget(stripchart) yaxis configure -title ""
# x2 and y2 axis are used for second cross section. 
$xswidget(stripchart) x2axis configure -hide no
$xswidget(stripchart) y2axis configure -hide no

# Hide the legend - we'll display legend-like info elsewhere. 
$xswidget(stripchart) legend configure -hide yes

# Cross sections graphed vs path length only.

#dummy axis used only for markers
$xswidget(stripchart) axis create marker_axis -min 0 -max 1

# Vertical markers in graph are handles used to take measurements. 
$xswidget(stripchart) marker create line -name meas1 -xor yes -linewidth 2 \
        -coords ".2 0 .2 1" -outline red -bindtags meas -mapx marker_axis
$xswidget(stripchart) marker create line -name meas2 -xor yes -linewidth 2 \
        -coords ".5 0 .5 1" -outline yellow -bindtags meas -mapx marker_axis
$xswidget(stripchart) marker create line -name meas3 -xor yes -linewidth 2 \
        -coords ".7 0 .7 1" -outline blue -bindtags meas -mapx marker_axis
# Bind to mouse button 1 so they can be dragged and display data. 
$xswidget(stripchart) marker bind meas <B1-Motion> {
    set x [$xswidget(stripchart) axis invtransform marker_axis %x]
    if {$x < 0 } { set x 0 }
    if {$x > 1 } { set x 1 }
    catch { %W marker configure [%W marker get current] -coords "$x 0 $x 1" }
    show_xs_values [%W marker get current]
}

set xswidget(data0) [frame $xswidget(display_data).f0]
set xswidget(data1) [frame $xswidget(display_data).f1]
pack $xswidget(data0) $xswidget(data1) \
        -side top -expand yes -fill both
set xsect(data0_init) 0
set xsect(data1_init) 0

proc show_xs_values { marker {name ""} {el ""} {op ""} } {
    global xsect xswidget

  for { set id 1 } { $id > -1 } { incr id -1 } {
    if { $xsect(data${id}_init) == 0 } { continue }

    set gridrow 1
    
    # Find the proper index into the data vectors to display.
    set vecname "xs$id"
    upvar ${vecname}_Path pathvec

     # take number of data points / range of data points.
    set factor [expr [${vecname}_Path length] -1.0]

    set coords [$xswidget(stripchart) marker cget meas1 -coords]
    # To find index, find dist of line from first data point, * factor
    set index1 [expr [lindex $coords 0] * $factor ]
    #puts "$id [lindex $coords 0] $index1"

    set coords [$xswidget(stripchart) marker cget meas2 -coords]
    set index2 [expr [lindex $coords 0] * $factor ]
    
    set coords [$xswidget(stripchart) marker cget meas3 -coords]
    set index3 [expr [lindex $coords 0] * $factor ]
    
    #puts  "$marker $index1"
    foreach datavec [xs_get_sorted_names $id] {
        if { $datavec != "${vecname}_Path" } {
            upvar $datavec dvec
            $xswidget(data$id).data_m1_$gridrow configure -text \
                    "[format %.3f $dvec($index1)]"
            $xswidget(data$id).data_m2_$gridrow configure -text \
                    "[format %.3f $dvec($index2)]"
            $xswidget(data$id).diff12_$gridrow configure -text \
                    "[format %.3f [expr $dvec($index1) - $dvec($index2)]]"
            $xswidget(data$id).data_m3_$gridrow configure -text \
                    "[format %.3f $dvec($index3)]"
            $xswidget(data$id).diff23_$gridrow configure -text \
                    "[format %.3f [expr $dvec($index2) - $dvec($index3)]]"
            incr gridrow
        }
    }
  }
}

# Display new data as it arrives from C code.
trace variable xs_data_update w [list show_xs_values 0]

# ----------------------------------------------------------------------
frame $xswidget(cntrl).point_frame
pack $xswidget(cntrl).point_frame -side right -expand yes -fill both

set xswidget(point_frame0) [frame $xswidget(cntrl).point_frame.f0]
pack $xswidget(point_frame0) -side top -expand yes -fill both
set xswidget(point_frame1) [frame $xswidget(cntrl).point_frame.f1]
pack $xswidget(point_frame1) -side top -expand yes -fill both

#checkbutton $xswidget(cntrl).active -text "Active" -variable xsect(active)
#pack $xswidget(cntrl).active -side top

generic_radiobox $xswidget(cntrl).type xs_snap_to_45 "Type" { "Free" "Constr 45" }
pack $xswidget(cntrl).type -side top -expand yes -fill x
generic_radiobox $xswidget(cntrl).width xs_vary_width "Width" { "Full" "Variable" }
pack $xswidget(cntrl).width -side top -expand yes -fill x
#button $xswidget(cntrl).update -text "Update" -command { 
#    show_xs_values 0 
#}
#pack $xswidget(cntrl).update -side top

set xswidget(del_frame) [frame $xswidget(cntrl).del_frame]
pack $xswidget(del_frame) -side top -expand yes -fill both

for { set index 0 } { $index <2 } { incr index } {

    # Create controls for this cross-section
    label $xswidget(del_frame).l$index -text "[expr $index + 1]:"
    button $xswidget(del_frame).d$index -text "Reset" \
            -command "set xs_clear_$index 1"
    button $xswidget(del_frame).s$index -text "Save" \
            -command "save_xs_dialog $index"
    grid $xswidget(del_frame).l$index \
            $xswidget(del_frame).d$index \
            $xswidget(del_frame).s$index -sticky ew -row $index
    
    set win $xswidget(point_frame$index)
    # labels for controlling this cross section
    grid x x [label $win.l1 -text "Active"] \
            [label $win.l2 -text "Inv"] \
            [label $win.l3 -text "Use Y"] \
            [label $win.l4 -text "Min"] \
            [label $win.l5 -text "Max"] 
    # labels for data from this cross section
    grid x x [label $xswidget(data$index).l1 -text "Red"] \
            [label $xswidget(data$index).l2 -text "Diff"] \
            [label $xswidget(data$index).l3 -text "Yellow"] \
            [label $xswidget(data$index).l4 -text "Diff"] \
            [label $xswidget(data$index).l5 -text "Blue"] 
    grid columnconfigure $xswidget(data$index) [list 2 3 4 5 6] -minsize 50
}

# Make the min/max entries change axis display.
# BUG in BLT. If both min/max are set, then if one of the min/max
# values is set to "", it is supposed to auto-scale, but it
# seems to set one or the other to MIN/MAX_DOUBLE.
# SO, we work around it by setting both limits to "", then setting the
# relevant limit, and NOT setting the other unless it is not ""

proc xs_axis_min_change { minvar maxvar axis_name } {
    global xswidget
    upvar #0 $minvar minvarval
    upvar #0 $maxvar maxvarval

    $xswidget(stripchart) axis configure $axis_name -min "" -max ""
    $xswidget(stripchart) axis configure $axis_name -min "$minvarval" 
    if { $maxvarval != "" } {
        $xswidget(stripchart) axis configure $axis_name  -max "$maxvarval"
    }
}
proc xs_axis_max_change { maxvar minvar axis_name } {
    global xswidget
    upvar #0 $minvar minvarval
    upvar #0 $maxvar maxvarval

    $xswidget(stripchart) axis configure $axis_name -min "" -max ""
    $xswidget(stripchart) axis configure $axis_name -max "$maxvarval"
    if { $minvarval != "" } {
        $xswidget(stripchart) axis configure $axis_name -min "$minvarval"
    }
}

proc xs_get_sorted_names { index } {
    global inputPlaneNames
    set vecname "xs$index"
    set vec_names [info globals "${vecname}_*"]
    set name_list ""
    # Create the graphs of this cross-section
    foreach name $inputPlaneNames {
        if { [string equal $name "none"] } { continue; }
        # Replace spaces and "-" (bad for variable names)
        set name [string map {" " _ "-" _ } $name]

        set i [lsearch $vec_names "*${name}*"]
        if { $i == -1 } { continue; }
        lappend name_list [lindex $vec_names $i]
    }
    return $name_list
}
 
proc create_new_cross_section { index } {
    global xsect xswidget inputPlaneNames
    set gridrow 1
    set vecname "xs$index"

    #puts "create_new_cross_section $vecname $index "

    # trace the path vector so data point displayed correctly
    # Doesn't work - I don't know why. 
    #trace variable ${vecname}_Path w "show_xs_values 0"
    
    set win $xswidget(point_frame$index)

    set id2 0
        
    foreach datavec [xs_get_sorted_names $index] {
        # Title strips off the leading "xs0_"
        $xswidget(stripchart) element create $datavec \
                -xdata ${vecname}_Path -ydata $datavec \
                -color [xs_color $index $id2] \
                -label [string range $datavec 4 end]
        $xswidget(stripchart) axis create $datavec \
                -limitscolor [xs_color $index $id2] \
                -limitsformat \"%4.4g\" \
                -title [string range $datavec 4 end]
        $xswidget(stripchart) element configure $datavec \
                -mapy $datavec
        if { $index == 1 } {
            $xswidget(stripchart) element configure $datavec \
                    -mapx x2
        }
        
        # Creates some controls for an element in the stripchart. 
        # A checkbox turns the graph of the element on and off.
        # A checkbox to invert the Y axis of the element
        # A button to let this element use the main Y axis.
        # Entries allow the min and max of the axis to be set. 
        
        frame $win.line$gridrow -width 20 -height 3 \
                -bg [xs_color $index $id2]
        scan $datavec xs%d_%s bogus labelname
        label $win.label$gridrow -text "$labelname"
        
        # When un-checked, stop graphing this data, stop displaying
        # element in the legend, hide it's axis
        set xswidget($datavec-active) 1
    checkbutton $win.active$gridrow  -variable xswidget($datavec-active) \
	-command "
	if { \$xswidget($datavec-active) } {
          \$xswidget(stripchart) element configure $datavec -hide no \
                  -label [string range $datavec 4 end]
          \$xswidget(stripchart) axis configure $datavec -hide no \
                  -limitsformat \"%4.4g\"
        } else {
          \$xswidget(stripchart) element configure $datavec -hide yes -label \"\"
          \$xswidget(stripchart) axis configure $datavec -hide yes \
                  -limitsformat \"\"
        }"
    radiobutton $win.getaxis$gridrow -variable xswidget(getaxis$index) \
            -value $gridrow \
            -command "
            if { \$xswidget($datavec-active) } { 
                \$xswidget(stripchart) y[expr $index ? 2:\"\"]axis use $datavec 
            }"
            # activate if first row. 
            if { $id2 == 0 } { 
                $win.getaxis$gridrow select 
                $xswidget(stripchart) y[expr $index ? 2:\"\"]axis use $datavec
            }
            
    checkbutton $win.invert$gridrow -variable xswidget($datavec-invert) \
	-command "
	if { \$xswidget($datavec-invert) } {
          \$xswidget(stripchart) axis configure $datavec -descending yes
        } else {
          \$xswidget(stripchart) axis configure $datavec -descending no
        }"

        set xsect(${datavec}_min) ""
        set xsect(${datavec}_max) ""
            generic_entry $win.max$gridrow xsect(${datavec}_max) "" real \
                  "xs_axis_max_change xsect(${datavec}_max) \
                  xsect(${datavec}_min) $datavec"
            generic_entry $win.min$gridrow xsect(${datavec}_min) "" real \
                  "xs_axis_min_change xsect(${datavec}_min) \
                  xsect(${datavec}_max) $datavec"
            
            grid $win.line$gridrow $win.label$gridrow $win.active$gridrow \
                    $win.invert$gridrow \
                    $win.getaxis$gridrow \
                    $win.min$gridrow $win.max$gridrow \
                    -row $gridrow -sticky w
            
            # Now create some widgets to display data for this xs
            frame $xswidget(data$index).line$gridrow -width 20 -height 3 \
                    -bg [xs_color $index $id2]
            label $xswidget(data$index).label$gridrow -text "$labelname"
            label $xswidget(data$index).data_m1_$gridrow 
            label $xswidget(data$index).data_m2_$gridrow 
            label $xswidget(data$index).data_m3_$gridrow 
            label $xswidget(data$index).diff12_$gridrow 
            label $xswidget(data$index).diff23_$gridrow 

            grid $xswidget(data$index).line$gridrow \
                    $xswidget(data$index).label$gridrow \
                    $xswidget(data$index).data_m1_$gridrow \
                    $xswidget(data$index).diff12_$gridrow \
                    $xswidget(data$index).data_m2_$gridrow \
                    $xswidget(data$index).diff23_$gridrow \
                    $xswidget(data$index).data_m3_$gridrow \
                    -sticky e

        incr id2
        incr gridrow
            
    }
        
    set xsect(data${index}_init) 1
}

# Clear all graphing and controls for this cross section
proc clear_cross_section { index } {
    global xsect xswidget
    set gridrow 1
    set vecname "xs$index"

    #puts "clear_cross_section $vecname $index "

    if { $xsect(data${index}_init) == 0 } { return; }
    set win $xswidget(point_frame$index)

    # Clear the graphs of this cross-section
    foreach datavec  [xs_get_sorted_names $index] {
        $xswidget(stripchart) element delete $datavec 
        # for some reason, the limits don't go away when you delete
        $xswidget(stripchart) axis configure $datavec -limitsformat ""
        $xswidget(stripchart) axis delete $datavec 
        
        # Clear controls for an element in the stripchart. 
        
        destroy $win.line$gridrow $win.label$gridrow $win.active$gridrow \
                $win.invert$gridrow \
                $win.getaxis$gridrow \
                $win.max$gridrow $win.min$gridrow 
        # Get rid of traces on the generic_entry global variables. 
        # prevents errors when xs is created again. 
        unset xsect(${datavec}_min) 
        unset xsect(${datavec}_max) 
        
        # Now clear some widgets that display data for this xs
        destroy $xswidget(data$index).line$gridrow \
                $xswidget(data$index).label$gridrow \
                $xswidget(data$index).data_m1_$gridrow \
                $xswidget(data$index).diff12_$gridrow \
                $xswidget(data$index).data_m2_$gridrow \
                $xswidget(data$index).diff23_$gridrow \
                $xswidget(data$index).data_m3_$gridrow 
        
        incr gridrow
        
    }    
    set xsect(data${index}_init) 0
}

# Get data from xs vectors and put it in readable text format
proc save_cross_section { index } {
    global xsect xswidget
    set vecname "xs$index"
    if { $xsect(data${index}_init) == 0 } { return ""; }
    set vec_list [xs_get_sorted_names $index] 
    global ${vecname}_Path
 
    set out_text "Point No.\tDist"
    foreach datavec $vec_list {
        append out_text "\t[string range $datavec 4 end]"
    }
    append out_text "\n"

    
    set factor [${vecname}_Path length]
    for {set i 0} { $i < $factor } { incr i } {
        append out_text "$i\t[set ${vecname}_Path($i)]"
        foreach datavec $vec_list {
            upvar #0 $datavec v
            append out_text "\t$v($i)"
        } 
        append out_text "\n"
    }
    
    return $out_text;
}

# Allow the user to save data from a single cross section to a file
proc save_xs_dialog { which } {
    global xsect xswidget fileinfo tcl_platform

    set out_text [save_cross_section $which]
    if { $out_text == "" } { return; }
    set types { {"All files" *} }
    set filename [tk_getSaveFile -filetypes $types \
            -initialfile "cross_section[expr $which + 1].txt" \
            -initialdir $fileinfo(save_dir)\
            -title "Save Cross Section"]
    if {$filename != ""} {
        # Dialog checks for writeable directory, and asks about
        # replacing existing files. 
        set xsfile [open $filename w]
        puts -nonewline $xsfile $out_text
        flush $xsfile
        close $xsfile
        set fileinfo(save_dir) [file dirname $filename]
        if {$tcl_platform(platform) == "unix"} {
            exec unix_to_dos $filename
        }
        
    } 
    # otherwise do nothing - user pressed cancel or didn't enter file name
}	


# Change controls and data display when the input planes change. 
proc xs_change_input_planes { nm el op } {
    global inputPlaneNames
    puts "xs Changed plane names $inputPlaneNames"

}
#trace variable inputPlaneNames w xs_change_input_planes

# return one of several unique colors, based on an integer index or two. 
# OK, I know it's not unique, but it's close...
proc xs_color {index index2} {
    set r 0
    set g 0
    set b 0
    set index [expr $index%2]
    set index2 [expr $index2%6]
    if {$index ==0 } {
    switch -- $index2 {
	0 {
	    #red
	    set r 255
	    set g 0
	    set b 0
	}
	1 {
	    #dark red
	    set r [expr int(255*0.7)]
	    set g 0
	    set b 0
	}
	2 {
	    #yellow
	    set r 255
	    set g 255
	    set b 0
	}
	3 {
	    #dark yellow
	    set r [expr int(255*0.7)]
	    set g [expr int(255*0.7)]
	    set b 0
	}
	4 {
	    #orange
	    set r [expr int(255*1)]
	    set g [expr int(180*1)]
	    set b 0
	}
	5 {
	    #dark orange
	    set r [expr int(255*0.7)]
	    set g [expr int(180*0.7)]
	    set b 0
	}
    } 
    } else {
    switch -- $index2 {
    
	0 {
	    #cyan
	    set r 0
	    set g [expr int(255*1)]
	    set b [expr int(255*1)]
	}
	1 {
	    #cyan
	    set r 0
	    set g [expr int(255*0.7)]
	    set b [expr int(255*0.7)]
	}
	2 {
	    # green
	    set r 0
	    set g [expr int(255*1)]
	    set b 0
	}
	3 {
	    # green
	    set r 0
	    set g [expr int(255*0.7)]
	    set b 0
	}
	4 {
	    # purple
	    set r [expr int(255*1)]
	    set g 0
	    set b [expr int(255*1)]
	}
	5 {
	    # purple
	    set r [expr int(255*0.7)]
	    set g 0
	    set b [expr int(255*0.7)]
	}
	6 {
	    #blue isn't very visible on black background
	    set r 0
	    set g 0
	    set b [expr int(255*1)]
	}
	default {
	    set r [expr int(200*1)]
	    set g [expr int(200*1)]
	    set b [expr int(200*1)]
	}
    }
    }
    set color [format "#%2.2x%2.2x%2.2x" $r $g $b]
    
    
    return $color
}


#vector s1_Path s1_Topography s1_Z_Piezo 
#s1_Path set { 0 1 2 3 }
#s1_Topography set { 3 4 2 1 }
#s1_Z_Piezo set { 9 4.5 5 3 }
#create_new_cross_section s1 1

#vector s2_Path s2_Topography s2_Z_Piezo 
#s2_Path set { 0 1.2 2.8 4.5 }
#s2_Topography set { .3 4.2 4.3 3.8 }
#s2_Z_Piezo set { 2 3 4 5.4 }
#create_new_cross_section s2 2



