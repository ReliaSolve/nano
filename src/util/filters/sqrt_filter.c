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

/****************************************************************************
				sqrt.c

	Reads in a bunch of raw floats.  Takes the sqrt() of each.  Writes
 out the new raw floats.  Produces no other output to stdout.  This is
 intended as a filter for datasets from the nanoManipulator.
	Puts in 0 for square root of negative numbers.
	Reads from standard input and writes to standard output.
	Exits with value 0 if it found an integral number of floats in the
 input file.  Exits with value -1 if not.
 ****************************************************************************/

#include	<math.h>
#include	<stdlib.h>
#include	<stdio.h>

const	int	BLOCKSZ = 1000;

main(void)
{
	float	buf[BLOCKSZ];
	int	count;
	int	i;
	int	retcode = 0;

	do {
		// Read in blocks of BLOCKSZ to decrease read/write overhead.
		count = fread(buf, sizeof(float), BLOCKSZ, stdin);

		// Take the square root of the ones we got.
		for (i = 0; i < count; i++) {
		  if (buf[i] < 0) {
			buf[i] = 0.0;
		  } else {
			buf[i] = sqrt(buf[i]);
		  }
		}

		// Write the ones we got back out.
		if (fwrite(buf, sizeof(float), count, stdout) != count) {
			perror("sqrt: Can't write output");
			retcode = -1;
			break;	// Break out of the 'do' loop
		}

	} while (count == BLOCKSZ);

	exit(retcode);
}

