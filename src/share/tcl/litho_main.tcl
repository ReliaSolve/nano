#!/bin/sh
# the next line restarts using wishx (see man wish) \
	exec wish "$0" ${1+"$@"}
# This is a tcl/tk script to create the user interface to the
# ebeam lithography system.  
# It sets up:
#      a menu bar
#

# Import Itcl and Iwidgets, for the tabnotebook widget and others we
# will use. This import method is a BUG - we shouldn't have to import
# the itcl and itk namespaces, but at least we can use it...
catch { set auto_path [lappend auto_path $env(ITCL_LIBRARY)] }

package require Itcl 
namespace import itcl::*
package require Itk 
namespace import itk::*
package require Iwidgets

# Include the BLT package, which provides vectors, stripchart and
# graph widgets
package require BLT
namespace import blt::*
namespace import -force blt::tile::*

#set up some options for all the widgets
option add *background LemonChiffon1 startupFile
option add *highlightBackground LemonChiffon1 startupFile
option add *menu*background grey75 startupFile

# This needs to be made dependent on how big the font is on the screen.
catch { option add *font {helvetica -15 } startupFile}
catch { option add *Font {helvetica -15 } startupFile}

# where do I find supporting files?
# If the environment variable NM_TCL_DIR is not set, 
# check for NANO_ROOT. If that is not set, 
# assume files are in the current directory. Allows
# "wish litho_main.tcl" to bring up the interface.
if {[info exists env(NM_TCL_DIR)] } {
    set tcl_script_dir $env(NM_TCL_DIR)
} elseif {[info exists env(NANO_ROOT)]} {
    set tcl_script_dir [file join $env(NANO_ROOT) share tcl]
} else {
    set tcl_script_dir .
}
### ks
if {[string match "*wish*" [info nameofexecutable]] } {
    set tcl_script_dir .
}

#appearance variables

# vertical padding between floatscales - image and modify windows
set fspady 3

# List of available images
set imageNames {none }

#Make the window appear in the upper left corner
wm geometry . +0+0

set time_to_quit 0

# Give it a title
wm title . "Seegerizer"
wm iconbitmap . error
# contains definition of useful widgets for the 
# Tcl_Linkvar library.
source [file join ${tcl_script_dir} Tcl_Linkvar_widgets.tcl]

# for overall appearance of user interface
frame .main

# frames - overall layout of the screen
frame .menubar -relief raised -bd 4 

#set up window
pack .main .menubar -side top -fill x 

# menu bar
menu .menu -tearoff 0

#### FILE menu #############################
set filemenu .menu.file
menu $filemenu -tearoff 0
.menu add cascade -label "File" -menu $filemenu -underline 0

$filemenu add command -label "Quit" -underline 1 -command {
    if {[string match "*wish*" [info nameofexecutable]] } {
        # we ran the file using wish, for testing. Kill app.
        exit 0
    } else {
        # we ran using the program. Tell main app to exit. 
        set time_to_quit 1 
    }
}

#### SETUP menu #############################
set setupmenu .menu.setup
menu $setupmenu -tearoff 0
.menu add cascade -label "Setup" -menu $setupmenu -underline 0 -columnbreak 0

$setupmenu add command -label "Drawing" -underline 0 \
	-command "show.drawing_parameters"

$setupmenu add command -label "Display" -underline 0 \
        -command "show.display_parameters"

$setupmenu add command -label "Alignment" -underline 0 \
        -command "show.alignment_parameters"

#### SEM menu ###############################
set SEM_menu .menu.sem
menu $SEM_menu -tearoff 0
.menu add cascade -label "SEM" -menu $SEM_menu -underline 0 -columnbreak 0

$SEM_menu add command -label "Image Acquisition" -underline 0 \
        -command "show.sem_image_acquisition"

$SEM_menu add command -label "Expose Pattern" -underline 0 \
        -command "show.sem_beam_expose_control"


# This command attaches the menu to the main window
# and allows you to use "alt-KEY" to access the menus.
. config -menu .menu


# Invoke File.. Exit when destroying window for clean exit.
wm protocol . WM_DELETE_WINDOW {$filemenu invoke "Quit"}

######### Drawing Parameters Control Panel ###########################
set drawing_parameters_win \
         [create_closing_toplevel drawing_parameters "Drawing Parameters"]

set line_width_nm 0
set exposure_uCoulombs_per_square_cm 0
set drawing_tool 1
set clear_drawing 0

generic_entry $drawing_parameters_win.line_width line_width_nm \
    "line width (nm)" real
pack $drawing_parameters_win.line_width -anchor ne -padx 3 -pady 3
generic_entry $drawing_parameters_win.exposure \
    exposure_uCoulombs_per_square_cm "exposure (uCoul/cm^2)" real
pack $drawing_parameters_win.exposure -anchor ne -padx 3 -pady 3

frame $drawing_parameters_win.tool -bd 3 -relief groove
label $drawing_parameters_win.tool.tool_label -text "Tool:"
pack $drawing_parameters_win.tool.tool_label -side top
radiobutton $drawing_parameters_win.tool.polyline -variable drawing_tool \
      -value 1 -text "polyline" -justify left
radiobutton $drawing_parameters_win.tool.polygon -variable drawing_tool \
      -value 2 -text "polygon" -justify left

pack $drawing_parameters_win.tool -side left

pack $drawing_parameters_win.tool.polyline \
     $drawing_parameters_win.tool.polygon -side top

button $drawing_parameters_win.clear_drawing -text "Clear" -command \
    { set clear_drawing 1 }
pack $drawing_parameters_win.clear_drawing -side top

######### End of Drawing Parameters Control Panel ####################

######### Display Parameters Control Panel ###########################

#parameters with C counterparts (in fact, each image has its own set of these
# and the C code is responsible for swapping in the right values when you
# change the current image)
set image_color_changed 0
set image_r 255
set image_g 255
set image_b 120
set image_opacity 50.0
set hide_other_images 0
set enable_image_display 0
set current_image "none"

set display_parameters_win \
        [create_closing_toplevel display_parameters "Display Parameters"]
generic_optionmenu $display_parameters_win.current_image current_image \
        "Image" imageNames
pack $display_parameters_win.current_image -anchor nw -padx 3 -pady 3

set image_color [format #%02x%02x%02x $image_r $image_g $image_b]

proc set_image_color {} {
    global image_r image_g image_b image_color image_color_changed
    # Extract three component colors of image_color
    # and save into image_r g b
    scan $image_color #%02x%02x%02x image_r image_g image_b
    set image_color_changed 1
}

frame $display_parameters_win.image_color
button $display_parameters_win.image_color.set_color -text "Set image color" \
   -command {
     choose_color image_color "Choose image color" $display_parameters_win
     $display_parameters_win.image_color.colorsample configure -bg $image_color
     set_image_color
}

button $display_parameters_win.image_color.colorsample -relief groove -bd 2 \
   -bg $image_color -command {
           $display_parameters_win.image_color.set_color invoke
}

pack $display_parameters_win.image_color -side top -fill x
pack $display_parameters_win.image_color.set_color -side left -fill x
pack $display_parameters_win.image_color.colorsample -side left -fill x \
     -expand yes


generic_entry $display_parameters_win.image_opacity image_opacity \
     "Image opacity" real

pack $display_parameters_win.image_opacity -side top -fill x -padx 3 -pady 3

checkbutton $display_parameters_win.hide_others_check \
   -text "Hide Others" -variable hide_other_images
   
pack $display_parameters_win.hide_others_check -anchor nw -padx 3 -pady 3


checkbutton $display_parameters_win.enable_display_check \
   -text "Enabled" -variable enable_image_display

pack $display_parameters_win.enable_display_check -anchor nw -padx 3 -pady 3
######### End of Drawing Parameters Control Panel ####################

######### Alignment ##################################################
set clear_align_points 0
set alignment_needed 0
set source_image_name "none"
set target_image_name "none"
set align_window_open 0

set align_win \
        [create_closing_toplevel_with_notify alignment_parameters \
         align_window_open "Alignment"]

generic_optionmenu $align_win.source_select source_image_name \
        "Live" imageNames
pack $align_win.source_select -anchor nw -padx 3 -pady 3

generic_optionmenu $align_win.target_select target_image_name \
        "Static" imageNames
pack $align_win.target_select -anchor nw -padx 3 -pady 3

button $align_win.align_now -text "Align Now" -command \
    { set alignment_needed 1 }
pack $align_win.align_now

button $align_win.clear_points -text "Clear Points" -command \
    { set clear_align_points 1 }
pack $align_win.clear_points

######### End Alignment ##############################################

######### SEM Image Acquisition ######################################
global sem_window_open sem_acquire_image sem_acquire_continuous \
       sem_pixel_integration_time_nsec sem_inter_pixel_delay_time_nsec \
       sem_resolution

set sem_pixel_integration_time_nsec 0
set sem_inter_pixel_delay_time_nsec 0
set sem_resolution 1
set sem_acquire_continuous 0

set sem_win  \
   [create_closing_toplevel_with_notify sem_image_acquisition sem_window_open \
              "SEM Image Acquisition"]

# Button to start the scan
button $sem_win.acquire_image -text "Acquire Image" \
        -command { set sem_acquire_image 1 }
pack $sem_win.acquire_image -anchor w -padx 3 -pady 3

# Controls whether to scan continuously or not
checkbutton $sem_win.acquire_continuous \
        -text "Acquire Continuously" \
        -variable sem_acquire_continuous
pack $sem_win.acquire_continuous -anchor w -padx 3 -pady 3

frame $sem_win.res -bd 3 -relief groove
pack $sem_win.res -side top
label $sem_win.res.res_label -text "Resolution"
pack $sem_win.res.res_label -side top
radiobutton $sem_win.res.res1 -variable sem_resolution \
     -value 1 -text "50x64" -justify left -anchor nw
radiobutton $sem_win.res.res2 -variable sem_resolution \
     -value 2 -text "100x128" -justify left -anchor nw
radiobutton $sem_win.res.res3 -variable sem_resolution \
     -value 3 -text "200x256" -justify left -anchor nw
radiobutton $sem_win.res.res4 -variable sem_resolution \
     -value 4 -text "400x512" -justify left -anchor nw
radiobutton $sem_win.res.res5 -variable sem_resolution \
     -value 5 -text "800x1024" -justify left -anchor nw
radiobutton $sem_win.res.res6 -variable sem_resolution \
     -value 6 -text "1600x2048" -justify left -anchor nw
radiobutton $sem_win.res.res7 -variable sem_resolution \
     -value 7 -text "3200x4096" -justify left -anchor nw

pack $sem_win.res.res1 $sem_win.res.res2 \
     $sem_win.res.res3 $sem_win.res.res4 \
     $sem_win.res.res5 $sem_win.res.res6 \
     $sem_win.res.res7 -fill x

generic_entry $sem_win.pixel_integration_time_nsec \
    sem_pixel_integration_time_nsec \
    "Pixel Integration (nsec)" integer
pack $sem_win.pixel_integration_time_nsec -padx 3 -pady 3
generic_entry $sem_win.inter_pixel_delay_time_nsec \
    sem_inter_pixel_delay_time_nsec \
    "Inter Pixel Delay (nsec)" integer
pack $sem_win.inter_pixel_delay_time_nsec -padx 3 -pady 3
######### End Image Acquisition ###########################

######### Beam Control Panel ##############################
global sem_beam_width_nm sem_beam_current_uAmps \
       sem_beam_expose_now

set sem_beam_width_nm 200
set sem_beam_current_uAmps 1
set sem_beam_expose_now 0

set sem_win \
   [create_closing_toplevel sem_beam_expose_control "SEM Control"]

frame $sem_win.calibration -relief raised -bd 3

pack $sem_win.calibration -anchor w -padx 3 -pady 3

label $sem_win.calibration.calibration_label -text "Calibration Parameters"
pack $sem_win.calibration.calibration_label \
         -padx 3 -pady 3

generic_entry $sem_win.calibration.beam_width \
    sem_beam_width_nm \
    "Beam Width (nm)" real
pack $sem_win.calibration.beam_width -anchor w -padx 3 -pady 3

generic_entry $sem_win.calibration.beam_current \
    sem_beam_current_uAmps \
    "Beam Current (uAmps)" real
pack $sem_win.calibration.beam_current -anchor w -padx 3 -pady 3

button $sem_win.expose_now -text "EXPOSE" \
    -command { set sem_beam_expose_now 1 } -fg White -bg Red
pack $sem_win.expose_now -padx 3 -pady 3

######### End Beam ########################################

##----------------
# Setup window positions and geometries to be convenient and pleasant!
# We do this at the end so we can find out the requested size of
# all the widgets we created.
after idle {
    # Find out about the main window geometry.
    update idletasks
    # the root window, ".", seems to need special handling.
    #set width [winfo reqwidth .] Doesn't include borders.
    #set height [winfo reqheight .] Doesn't include the menu bar and title bar!

    scan [wm geometry .] %dx%d+%d+%d width height main_xpos main_ypos
    # wm rootx seems to tell us how big the border of the window is.
    set winborder [expr [winfo rootx .] - $main_xpos]
    set main_width [expr $width + 2* $winborder ]
    set main_height [expr $height + ([winfo rooty .] - $main_ypos) + \
           $winborder ]
#    puts " mainwin $main_width $main_height $width $height $main_xpos $main_ypos [wm geometry .]"

    # The display parameters window
    # Make the window appear on the left edge below the main window

    # Make the left strip all the same width - same as the image windows.
    set left_strip_width  [winfo reqwidth .display_parameters]

    # Keep track of where the next window should appear
    set next_left_pos [expr $main_ypos +$main_height]

    wm geometry .display_parameters +${main_xpos}+$next_left_pos

    wm geometry . 640x2

}

# puts the focus on the main window, instead of any other windows
# which have been created.
wm deiconify .