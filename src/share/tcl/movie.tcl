#Initial dialog which allows user to choose which plane
#of data to save.
iwidgets::dialog .save_sequence_dialog -title "Save sequence image" 
#-modality application

.save_sequence_dialog hide Help
.save_sequence_dialog hide Apply
.save_sequence_dialog buttonconfigure OK -text "Go" -command {
#    .save_sequence_dialog deactivate 1
    global num_images_so_far time_so_far base_time
    set num_images_so_far 0
    set time_so_far $base_time
    save_an_image
}

.save_sequence_dialog buttonconfigure Cancel -text "Done" -command {
    .save_sequence_dialog deactivate 0
    # Turn the screen information back on whatever happens
    set chart_junk 1
}

set sequenceImage_basefile ""
proc choose_basefile {} {
    global fileinfo screenImage_format sequenceImage_basefile
    set types { {"All files" *} 
    {"TIFF" ".tif" }
    {"PPM/PGM" ".pgm" } 
    {"PPM/PGM" ".ppm" } }

        # Set the file extension correctly
        set def_file_exten ".tif"
        foreach item $types {
            if { [string compare $screenImage_format [lindex $item 0]] == 0} {
                set def_file_exten [lindex $item 1]
            }
        }

    set filename [tk_getSaveFile -filetypes $types \
		-initialfile sequenceimage$def_file_exten \
                -initialdir $fileinfo(save_dir) \
                -title "Save sequence image"] 
    if {$filename != ""} {
	#puts "Save sequence image: $filename $screenImage_format"
        # Dialog checks for writeable directory, and asks about
        # replacing existing files. 
        set sequenceImage_basefile $filename
	set fileinfo(save_dir) [file dirname $filename]
    } else {
	# otherwise do nothing - user pressed cancel or didn't enter file name
    }
}

set win [.save_sequence_dialog childsite]
# Toggle to determine whether the screen labels and decorations
# are visible when the sequence shot is saved. 
set chart_junk 1
set screenImage_format_list {}
checkbutton $win.chart_junk -variable chart_junk \
	-text "Show screen text"
generic_optionmenu $win.screenImage_format screenImage_format \
	"Format for saved picture:" screenImage_format_list
pack $win.chart_junk $win.screenImage_format -anchor nw

set num_images 10
generic_entry $win.num_images num_images "Num images" integer
set time_step 1
generic_entry $win.time_step time_step "Time step (sec)" real
set base_time 0
set time_so_far 0
generic_entry $win.base_time base_time "Base time (sec)" numeric
pack $win.num_images $win.time_step $win.base_time -anchor nw

generic_entry $win.sequenceImage_basefile sequenceImage_basefile \
	"Base filename:" ""
$win.sequenceImage_basefile configure -width 20
button $win.get_file_name -text "Choose..." -command choose_basefile
pack $win.sequenceImage_basefile $win.get_file_name -side left


# Allow the user to save 
proc save_sequenceImage {} {
    global num_images_so_far time_so_far base_time
    set num_images_so_far 0
    set time_so_far $base_time
    # All activity is done in the button commands defined above.
    .save_sequence_dialog activate
}	

proc save_an_image {} {
    global screenImage_filename num_images time_step sequenceImage_basefile \
            num_images_so_far time_so_far set_stream_time set_stream_time_now

    set stream_replay_rate 0
    if { $num_images_so_far >= $num_images } return;

    set fdir [file dirname $sequenceImage_basefile]
    set froot [file rootname $sequenceImage_basefile]
    set fext [file extension $sequenceImage_basefile]
    set filename [file join $fdir ${froot}[format %04d $num_images_so_far]${fext}]
    puts "$filename "
    update idletasks

    # Setting this variable triggers a callback which saves the file.
    set screenImage_filename $filename

    set time_so_far [expr $time_so_far + $time_step]
    set set_stream_time $time_so_far
    set set_stream_time_now 1

    incr num_images_so_far
    after 500 save_an_image 
}