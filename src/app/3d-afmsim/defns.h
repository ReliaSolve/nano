/*$Id$*/

#ifndef _DEFNS_H_
#define _DEFNS_H_

#define PI            3.14159265358979323846
#define DEG_TO_RAD (PI/180.)
#define RAD_TO_DEG	(180. / PI)

#define DIST_UNIT 3
#define ANGLE_UNIT 3
#define LENGTH_UNIT 3
#define DIAM_UNIT  1
#define DEFAULT_LENGTH 20
#define DEFAULT_DIAM 5

#define DEFAULT_TRIANGLE_SIDE 40

#define TIP_RADIUS_UNIT 1
#define TIP_THETA_UNIT 1

#define LEFT_BUTTON 1
#define RIGHT_BUTTON 2

// colors
#define WHITE	0
#define RED		1
#define GREEN	2
#define BLUE	3
#define MAGENTA	4
#define YELLOW	5
#define CYAN	6
#define BLACK	7

#define KEY_DELETE 0x7f

// display list
#define DISP_LIST 1
#define	TIP_DISP_LIST 1
#define SPHERE_LIST 1
#define CYLINDER_LIST 2
#define CONE_SPHERE_LIST 3
#define	TIP_CONE_SPHERE_LIST 4


// rotating protein
#define XLEAST_COUNT 90
#define YLEAST_COUNT 90
#define XMAX 90
#define YMAX 90

#endif