#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "vrpn_Connection.h"
#include "vrpn_Ohmmeter.h"

static char * g_defaultConfigFileName = "ohm.cfg";

void	Usage(char *s)
{
  fprintf(stderr,"Usage: %s [-f filename] [-warn] [-v]\n",s);
  fprintf(stderr,"       -f: Full path to config file (default %s)\n",
          g_defaultConfigFileName);
  fprintf(stderr,"       -warn: Only warn on errors (default is to bail)\n");
  fprintf(stderr,"       -v: Verbose\n");
  exit(-1);
}

void main (unsigned argc, char *argv[])
{
	char	*config_file_name = g_defaultConfigFileName;
	FILE	*config_file;
	int	bail_on_error = 1;
	int	verbose = 1;
	int	realparams = 0;
	unsigned int	i;

	// Parse the command line
	i = 1;
	while (i < argc) {
	  if (!strcmp(argv[i], "-f")) {		// Specify config-file name
		if (++i > argc) { Usage(argv[0]); }
		config_file_name = argv[i];
	  } else if (!strcmp(argv[i], "-warn")) {// Don't bail on errors
		bail_on_error = 0;
	  } else if (!strcmp(argv[i], "-v")) {	// Verbose
		verbose = 1;
	  } else if (argv[i][0] == '-') {	// Unknown flag
		Usage(argv[0]);
	  } else switch (realparams) {		// Non-flag parameters
	    case 0:
	    default:
		Usage(argv[0]);
	  }
	  i++;
	}
	
#ifdef _WIN32
	WSADATA wsaData; 
	int status;
    if ((status = WSAStartup(MAKEWORD(1,1), &wsaData)) != 0) {
         fprintf(stderr, "WSAStartup failed with %d\n", status);
    } else if(verbose) {
		 fprintf(stderr, "WSAStartup success\n");
	}
#endif

	vrpn_Synchronized_Connection	connection;
	vrpn_Ohmmeter	*ohmmeter;

	// Open the configuration file
	if (verbose) printf("Reading from config file %s\n", config_file_name);
	if ( (config_file = fopen(config_file_name, "r")) == NULL) {
		perror("Cannot open config file");
		fprintf(stderr,"  (filename %s)\n", config_file_name);
		return; //	return -1;
	}

	// Read the configuration file, creating a device for each entry.
	// Each entry is on one line, which starts with the name of the
	//   class of the object that is to be created.
	// If we fail to open a certain device, print a message and decide
	//  whether we should bail.
      {	char	line[512];	// Line read from the input file
	char	s1[512],s2[512];	// String parameters
	int	i1;				// Integer parameters
	float	f1;				// Float parameters

	// Read lines from the file until we run out
	while ( fgets(line, sizeof(line), config_file) != NULL ) {

	  // Make sure the line wasn't too long
	  if (strlen(line) >= sizeof(line)-1) {
		fprintf(stderr,"Line too long in config file: %s\n",line);
		if (bail_on_error) { return; /* return -1; */}
		else { continue; }	// Skip this line
	  }

	  // Figure out the device from the name and handle appropriately
	  #define isit(s) !strncmp(line,s,strlen(s))
	  
	  if (isit("vrpn_Ohmmeter_ORPX2")) {
		// Get the arguments (class, button_name, portno)
		if (sscanf(line,"%511s%511s%d%f",s1,s2,&i1,&f1) != 4) {
		  fprintf(stderr,"Bad vrpn_Ohmmeter line: %s\n",line);
		  if (bail_on_error) { return; /*return -1;*/ }
		  else { continue; }	// Skip this line
		}

		//open the Ohmmeter
		if(verbose) printf(
			"Opening vrpn_Ohmmeter_ORPX2: %s on port %d\n", s2,i1);
#if defined(_WIN32) && !defined(FAKE_SERVER)
		if ( (ohmmeter =
		     new vrpn_Ohmmeter_ORPX2(s2, &connection, f1)) == NULL){
#else
                printf("Warning: This is the FAKE ORPX server (for testing)\n");
                if ( (ohmmeter =
                     new vrpn_Ohmmeter_Server(s2, &connection, f1)) == NULL){
#endif
		  fprintf(stderr,"Can't create new vrpn_Ohmmeter_ORPX2\n");
		  if (bail_on_error) { return; /*return -1;*/ }
		  else { continue; }	// Skip this line
		}
	  }
	  else {	// Never heard of it
		sscanf(line,"%511s",s1);	// Find out the class name
		fprintf(stderr,"Unknown class: %s\n",s1);
		if (bail_on_error) { return; }
		else { continue; }	// Skip this line
	  }
	}
      }

	// Close the configuration file
	fclose(config_file);

	// Loop forever calling the mainloop()s for all devices
	while (1) {

		ohmmeter->mainloop();

		// Send and receive all messages
		connection.mainloop();
	}

}