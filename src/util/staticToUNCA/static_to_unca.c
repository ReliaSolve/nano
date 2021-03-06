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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "MicroscopeFlavors.h"
#include "BCGrid.h"
#include "BCPlane.h"

/*******************
 * Global definitions
 *******************/

const int MAXFILES = 1000;

void Usage(char* s)
{
  fprintf(stderr,"Usage: %s filename [filename ...]\n",s);
  fprintf(stderr,"     Converts each filename to one or more files: filename_FIELD.unca\n");
  fprintf(stderr,"     (Where FIELD is different for each data set in the file)\n");
  exit(-1);
}

int translate_file(const char *filename)
{
  // Read the data planes from this file into a grid
  BCGrid    inputGrid;
  TopoFile  topoFile;
  inputGrid.loadFile(filename, topoFile);

  // For each plane, construct an output filename and then
  // write the plane to a UNCA file with that name.
  BCPlane	*p;
  for (p = inputGrid.head(); p != NULL; p = p->next()) {
    char *outfilename = new char[strlen(filename) + strlen(p->name()->c_str()) + 50];
    if (outfilename == NULL) {
      fprintf(stderr,"Out of memory allocating filename\n");
      return -1;
    }
    sprintf(outfilename, "%s_%s.unca", filename, p->name()->c_str());
    FILE *outfile = fopen(outfilename, "wb");
    if (outfile == NULL) {
      fprintf(stderr,"Cannot open %s for output\n", outfile);
      delete [] outfilename;
      return -1;
    }
    inputGrid.writeUNCAFile(outfile, p);

    fclose(outfile);
    delete [] outfilename;
  }

  return 0;
}

int main(int argc, char* argv[])
{
    char	*stm_file_names[MAXFILES];	/* Files to load */
    int		num_stm_files = 0;		/* How many to load */
    int		i;

    int	real_params = 0;		/* Non-flag parameters */

    /* Parse the command line */
    i = 1;
    while (i < argc) {

	if (strcmp(argv[i],"-l") == 0) {
	    Usage(argv[0]);
	} else if (argv[i][0] == '-') {
	    Usage(argv[0]);
	} else {	// Get the next file name
	    if (num_stm_files >= MAXFILES) {
		fprintf(stderr,"Only %d files allowed, ignoring %s\n",
			MAXFILES,argv[i]);
	    }
	    stm_file_names[num_stm_files] = argv[i];
	    num_stm_files++;

	    real_params++;
        }
	i++;
    }
    if (real_params == 0) Usage(argv[0]);

    for (i = 0; i < num_stm_files; i++) {
	if (translate_file(stm_file_names[i])) {
		fprintf(stderr,"Failed on file %s\n",stm_file_names[i]);
		return -1;
	}
    }

    return 0;
}

