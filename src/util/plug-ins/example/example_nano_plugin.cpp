//------------------------------------------------------------------------
// Example program showing how to write a plug-in for the UNC/3rdTech
// nanoManipulator program.  The plug-in architecture connects to a running
// nanoManipulator application and is able to read the data set that is
// currently mapped to the height channel, produce another image as a
// result, and also produce positioned text that the nanoManipulator
// will draw for the user.
//
// There are two locations in this example program that are bracketed
// by the following two lines:
// *** Application code replaces what comes below.
// *** Application code replaces what comes above.
// These sections are to be replaced by custom code that implements
// the function of the plug-in module.  They are both inside callback
// handlers for message from the imager server in the nanoManipulator.

// ToDo: Figure out how to handle more than one plug-in attachment at
// a time, where the plug-ins are returning images.  Figure out how to
// enable access to all data planes while telling the plug-in which is
// the one currently mapped to height.  This should be possible by
// sending multiple channels, perhaps.

#include <math.h>
#include <stdio.h>
#include <vrpn_Connection.h>
#include <vrpn_TempImager.h>
#include <vrpn_Tracker.h>
#include <vrpn_Text.h>

//------------------------------------------------------------------------
// This is the "magic name" of the device that returns values to
// the nanoManipulator program.  The "magic name" of the device
// on the nanoManipulator is "nM" and it is at whatever hostname
// the nanoManipulator is running on.
const char  return_name[] = "nMreturn";

//------------------------------------------------------------------------
// Global variables used because it is easier in a callback-based
// program to do it this way than to pass pointers around.  Either
// way would work, though.

// Print out information about what we're doing to stderr.
static	bool  g_verbose = true;

// Set this when it is time to quit.
static	bool  g_quit = false;

// Suffix to append to incoming channel name to form return channel name.
// Application code re-sets this in the description handler to match the
// function being performed.
char	    *g_name_suffix = "_imager_plugin";

//------------------------------------------------------------------------
// A TempImager client to read the image from the nanoManipulator.
// A TempImager Server to send an image back once we've modified it
// to suit, or filtered it, or done whatever we want to do with it.
// Be sure to wait until we know what the size is before actually
// creating it.  The connection object is global because it is needed
// by the callback routine.  Also, a text sender and tracker server to
// position and send text messages.

static	vrpn_TempImager_Remote	*g_imager_client = NULL;
static	vrpn_TempImager_Server	*g_imager_server = NULL;
static	vrpn_Connection		*g_connection = NULL;
static	vrpn_Text_Sender	*g_text_server = NULL;
static	vrpn_Tracker_Server	*g_tracker_server = NULL;

//------------------------------------------------------------------------
// A buffer to hold the data that comes from the other side.  Its size
// is unknown until we get the description message, so we allocate it
// at that point.  Also, store the number of rows and columns.  Also,
// store the outgoing channel index for sending data back.

static	vrpn_float32		*g_image_buffer = NULL;
static	vrpn_int32 g_nCols = 0;
static	vrpn_int32 g_nRows = 0;
static	int g_outgoing_channel = -1;

//------------------------------------------------------------------------
// The handlers are where the action happens in this program.  This first
// callback handles the report of the imager characteristics, it is
// guaranteed to be called before any region callbacks are called, and
// it creates the imager server that will send any resulting image back.
// If the the image description changes (size, for example, then
// this message will be called again.  If that happens in this example,
// we quit.  It is possible to tear down and reallocate everything to
// handle this.

void  handle_description_message(void *, const struct timeval)
{
  static bool got_message = false;
  if (g_verbose) {
    fprintf(stderr, "Got description message (%dx%d image)\n",
      g_imager_client->nCols(), g_imager_client->nRows());
  }
  if (got_message) {
    // Make sure that the size has not changed.  If it has, we exit.
    if ( (g_nCols != g_imager_client->nCols()) || (g_nRows != g_imager_client->nRows()) ) {
      fprintf(stderr,"handle_description_message(): Data size changed - exiting!\n");
      exit(-1);
    }
  } else {
    // Get any required information from the imager client.  It will
    // have been filled in by the time this callback is called.
    vrpn_float32 minX = g_imager_client->minX();
    vrpn_float32 maxX = g_imager_client->maxX();
    vrpn_float32 minY = g_imager_client->minY();
    vrpn_float32 maxY = g_imager_client->maxY();
    g_nCols = g_imager_client->nCols();
    g_nRows = g_imager_client->nRows();

    // Allocate a memory buffer that will be used to hold the image
    // data as it comes from the other side before we operate on it and
    // send it back.
    g_image_buffer = new vrpn_float32[g_nCols*g_nRows];
    if (g_image_buffer == NULL) {
      fprintf(stderr,"Out of memory allocating buffer for %dx%d image\n", g_nCols, g_nRows);
      exit(-1);
    }

//-------------------------------------------------------------------
// *** Application code replaces what comes below.  It should prepare
// *** whatever is needed to handle regions once they come.

    // Create the imager server that will be used to send the images.
    g_imager_server = new vrpn_TempImager_Server( return_name, g_connection,
      g_nCols, g_nRows, minX, maxX, minY, maxY);
    if (g_imager_server == NULL) {
      fprintf(stderr,"Out of memory allocating TempImager_Server\n");
      exit(-1);
    }

    // Set the name to append to the incoming channel name when making
    // the outgoing channel.  This should describe the function that is
    // being performed on the data.
    g_name_suffix = "_echo";

    // Note: The application can create more than one return channel, each
    // with a different name.  Code outside the application blocks will need
    // to be changed to make this happen (multiple add_channel calls, for one
    // thing).

// *** Application code replaces what comes above.
//-------------------------------------------------------------------

    got_message = true;
  }
}

//------------------------------------------------------------------------
// The handlers are where the action happens in this program.  This second
// callback handles each region as it comes.  This version sends the same
// data back to the nanoManipulator and it also calculates the volume
// above zero in the image, which it sends back as a text message that is
// located just above the center of the image.  Once the message
// has been sent, it quits.  User code could continue to listen for multiple
// copies of the image, which may come as the microscope scans.

void  handle_region_message(void *, const vrpn_IMAGERREGIONCB info)
{
  if (g_verbose) {
    fprintf(stderr, "Got region message\n");
  }

  // Used to index into the TempImager based on the region info.
  vrpn_int16  channel_index = info.region->d_chanIndex;
  vrpn_float32 offset = g_imager_client->channel(channel_index)->offset;
  vrpn_float32 scale = g_imager_client->channel(channel_index)->scale;

  // If this is the first region, add a channel with the same name to the
  // imager server so we'll have something to send back.  The limits we
  // send for the min and max values are very conservative...
  // Name the channel based on the incoming channel name with the globally-
  // set suffix attached.
  static bool got_message = false;
  if (!got_message) {
    const char  *oldname = g_imager_client->channel(channel_index)->name;
    const char  *units = g_imager_client->channel(channel_index)->units;
    char *newname = new char[strlen(oldname) + strlen(g_name_suffix) + 1];
    if (newname == NULL) {
      fprintf(stderr, "Out of memory in handle_region_message\n");
      exit(-1);
    }
    sprintf(newname, "%s%s", oldname, g_name_suffix);
    g_outgoing_channel = g_imager_server->add_channel(newname, units, -1e10, 1e10, scale, offset);
    if (g_outgoing_channel == -1) {
      fprintf(stderr, "Could not add a channel to send the data back\n");
      exit(-1);
    }
    got_message = true;
    delete [] newname;
  }

  // Get the data from our region into our image buffer.
  info.region->decode_unscaled_region_using_base_pointer(g_image_buffer, 1, g_nCols, g_nRows);

  // Scale the data if needed to get it into the proper units.
  if ( (scale != 1) || (offset != 0) ) {
    vrpn_uint16 r, c;
    for (r = info.region->d_rMin; r <= info.region->d_rMax; r++) {
      for (c = info.region->d_cMin; c <= info.region->d_cMax; c++) {
	g_image_buffer[c + r * g_nCols] = offset + scale * g_image_buffer[c + r * g_nCols];
      }
    }
  }

//-------------------------------------------------------------------
// *** Application code replaces what comes below.  It should handle the
// *** data in the region however it sees fit and pack any messages it
// *** needs to the imager, tracker, and text servers.  Only those portions
// *** of the buffer that have been filled in by earlier calls or that are
// *** filled in by this call will have valid data.  Check that the whole
// *** array is filled in calls, resetting at (0,0) and ending at the last line.
// *** Data from later can come in any order as new scans are made or other
// *** changes are made to the data.

  // Sum up the values of the region to end up with the total signed volume
  // of the region above zero once we divide by the image size.  We are going
  // to quit after the last row has been received, and the first time through,
  // the nM sends all of the rows in order, so it is safe to assume that we
  // will see each row once by the time we get the last row.
  static double volume = 0.0;
  static bool started = false;

  // If we've got the first pixel in the image, then tell that we have
  // started and reset our state.
  if ( (info.region->d_rMin == 0) && (info.region->d_cMin == 0) ) {
    started = true;
    volume = 0.0;
  }

  vrpn_uint16 r,c;
  for (r = info.region->d_rMin; r <= info.region->d_rMax; r++) {
    for (c = info.region->d_cMin; c <= info.region->d_cMax; c++) {
      volume += g_image_buffer[c + r * g_nCols];
    }
  }

  // We also send the region back to the nanoManipulator, in this case not
  // modified.  The app could modify it here before sending it back, of course.
  g_imager_server->send_region_using_base_pointer(g_outgoing_channel,
    info.region->d_cMin, info.region->d_cMax,
    info.region->d_rMin, info.region->d_rMax,
    g_image_buffer, 1, g_nCols, g_nRows, false, &info.msg_time);

  // If this region includes the last pixel, multiply to find the volume, move
  // over the center and send a text message telling the volume.
  // Then reset the "started" flag.
  if ( (info.region->d_rMax == g_nRows - 1) && (info.region->d_cMax == g_nCols - 1) ) {
    if (g_verbose) {
      fprintf(stderr, "Got total region, sending back text\n");
    }

    // Scale the volume by the footprint of one pixel on the image.
    volume *= ((g_imager_client->maxX() - g_imager_client->minX()) / g_imager_client->nCols()) *
	      ((g_imager_client->maxY() - g_imager_client->minY()) / g_imager_client->nRows());

    // Position 10nm above the center pixel, using the identity orientation
    vrpn_float64  pos[3], quat[4] = {0, 0, 0, 1};
    struct timeval  now;
    gettimeofday(&now, NULL);
    pos[0] = (g_imager_client->maxX() + g_imager_client->minX()) / 2;
    pos[1] = (g_imager_client->maxY() + g_imager_client->minY()) / 2;
    pos[2] = g_image_buffer[ g_nCols/2 + g_nRows/2 * g_nCols ] + 10;
    g_tracker_server->report_pose(0, now, pos, quat, vrpn_CONNECTION_RELIABLE);

    // Send a text message with the volume in it
    char msg[1024];
    sprintf(msg, "Signed volume of surface is %g\n", volume);
    g_text_server->send_message(msg);

    started = false;

    // Set this if you want to exit after the first complete image has been
    // read.
    g_quit = true;
  }

// *** Application code replaces what comes above.
//-------------------------------------------------------------------
}

void Usage (const char * s)
{
  fprintf(stderr,"Usage: %s [-v] [device_name]\n",s);
  fprintf(stderr,"     -v: Verbose mode, print out lots of info along the way\n");
  fprintf(stderr,"     device_name: Name of device to connect to (default nM@localhost:5997)\n");
  exit(-1);
}

main (int argc, char * argv[])
{
  //------------------------------------------------------------------------
  // Parse the command line

  char	*device_name = "nM@localhost:5997";
  int	realparams = 0;
  int i;
  i = 1;
  while (i < argc) {
    if (strcmp(argv[i], "-v") == 0) {
	g_verbose = true;
    } else if (argv[i][0] == '-') {	// Unknown flag
	Usage(argv[0]);
    } else switch (realparams) {		// Non-flag parameters
      case 0:
	device_name = argv[i];
	realparams++;
	break;
      default:
	Usage(argv[0]);
    }
    i++;
  }

  //------------------------------------------------------------------------
  // Open a VRPN connection to the Imager device that we are going to listen
  // to.  Then find out what connection we are using for it and create a
  // tracker server and a text server with the same name.  The tracker server
  // will send back entries in its zeroeth sensor report to indicate where
  // any later text should be displayed.  The text sender can be used to send
  // strings to the nanoManipulator to indicate interesting information about
  // the image (computed volumes of objects on the surface, for example).

  if (g_verbose) {
    fprintf(stderr,"Opening connection to imager at %s\n", device_name);
  }
  g_imager_client = new vrpn_TempImager_Remote(device_name);
  g_connection = g_imager_client->connectionPtr();
  if (g_connection == NULL) {
    fprintf(stderr, "Error: NULL connection when opening %s\n", device_name);
    return -1;
  }

  if (g_verbose) {
    fprintf(stderr,"Opening text and tracker servers as %s\n", return_name);
  }
  g_text_server = new vrpn_Text_Sender(return_name, g_connection);
  g_tracker_server = new vrpn_Tracker_Server(return_name, g_connection);

  //------------------------------------------------------------------------
  // Set the callback handlers for the imager client.  The description callback
  // handler is guaranteed to be called before the region callback handler is,
  // so we use it to set up the server that sends values back.

  g_imager_client->register_description_handler(NULL, handle_description_message);
  g_imager_client->register_region_handler(NULL, handle_region_message);

  //------------------------------------------------------------------------
  // Okay, now we keep calling mainloop() on the various objects forever (until
  // it is time to quit).  For this example program, we quit when we've seen and
  // sent the entire image.  The mainloop() will end up calling the handler for
  // the imager_client, which is what does all of the work in this example.

  while (!g_quit) {

    // See if the imager has any data for us.
    g_imager_client->mainloop();

    //------------------------------------------------------------------------
    // We called the imager_client mainloop first so that its callback would
    // be handled and all messages packed by these servers before they are
    // mainlooped below, which happens before we exit because g_quit is set.
    // If we had done this in a different, order, the mainloops() might be
    // missed and the last messages discarded without being sent.

    if (g_imager_server) { g_imager_server->mainloop(); } //< Once it is created, use it.
    g_tracker_server->mainloop();
    g_text_server->mainloop();
    g_connection->mainloop();

    // Sleep a little while to avoid eating the whole CPU
    vrpn_SleepMsecs(1);
  }

  delete g_text_server;
  delete g_tracker_server;
  delete g_imager_client;

  // Run for a fraction of a second to give messages time to clear out before
  // closing the connection.
  for (i = 0; i < 200; i++) {
    g_connection->mainloop();
    vrpn_SleepMsecs(1);
  }
  if (g_imager_server) { delete g_imager_server; }
  // The connection will have been deleted when the last object using it was deleted.
  if (g_image_buffer) { delete [] g_image_buffer; }
  
  return 0;
}
