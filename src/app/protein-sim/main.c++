#include <stdlib.h>		//stdlib.h vs cstdlib
#include <stdio.h>		//stdio.h vs cstdio
#include <iostream.h>
#include <vector>
#include <math.h>		//math.h vs cmath
#include <GL/glut.h>
#include "vec3d.h"
#include "3Dobject.h"
#include "ConeSphere.h"
#include "Tips.h"
#include <string.h>

#define DISP_LIST 1

#define PI            3.14159265358979323846
#define DEG_TO_RAD (PI/180.)
#define RAD_TO_DEG	(180. / PI)
#define True  1
#define False 0

GLuint list_sphere;
GLuint list_cylinder;
//#define LIST_CYLINDER 2

#define MAXOB 100000
#define MAX_GRID 128 // changed this to 128 from 64
#define MAX_PATHQ 100
// colors
#define WHITE	0
#define RED		1
#define GREEN	2
#define BLUE	3
#define MAGENTA	4
#define YELLOW	5
#define CYAN	6
#define BLACK	7
// drawing styles
#define OB_SOLID		0
#define OB_WIREFRAME	1
#define OB_POINTS		2
#define OB_SILHOUETTE	3
#define OB_OUTLINE2D	4
#define OB_NONE			5
// grid display style
#define GRID_WIREFRAME 0
#define GRID_SOLID 1
#define GRID_HALF_SOLID 2
#define GRID_NONE 3
// image scan computation method
#define IMAGE_SCAN_APPROX 0
#define IMAGE_SCAN_EXACT  1
// simulation types
#define SIM_NONE 0
#define SIM_TRANSLATE 1
#define SIM_ROLLING 2
#define SIM_SLIDING 3
#define SIM_ROLLING_SLIDING 4
// PPM file interpretation types
#define PPM_INTERPT_RGB_AS_FLAG   1
#define PPM_INTERPT_RGB_AS_HEIGHT 2
#define DEPTHSIZE MAX_GRID


typedef int Bool;

static int dblBuf  = GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH;
OB nullob;
OB ob[MAXOB];	// array of objects used in the simulation
int numObs = 0;	// current count of objects in sim world
int selectedOb = NULLOB;
// display options
int renderStyle = OB_OUTLINE2D;
GLenum drawStyle = GL_FILL;       
GLenum shadingModel = GL_FLAT;   // GL_FLAT or GL_SMOOTH
Bool lightOn[8] = { 1, 1, 0, 0, 0, 0, 0, 0 };
// window stuff
int mainWindowID, viewWindowID, depthWindowID;
double windowWidth  = 600.;
double windowHeight = 600.;
double orthoFrustumCenterX = 64.;	// area of XY plane always visible for all window aspect ratios
double orthoFrustumCenterY = 64.;
double orthoFrustumWidthNominal  = 128. + 10.;
double orthoFrustumHeightNominal = 128. + 10.;
// actual bounds of current ortho view frustum matching window aspect ratio
double orthoFrustumLeftEdge;
double orthoFrustumBottomEdge;
double orthoFrustumWidth;
double orthoFrustumHeight;
// raw values (normalized) from Z-buffer
float zBuffer[ 128*128 ];			
// array of heights: image scan data
double zHeight        [MAX_GRID][MAX_GRID];	
// scan grid resolution
int    scanResolution = 128;	
// scan grid pitch (sample-to-sample spacing)
double scanStep   = 1.;		
// scan grid origin X coord (left side)
double scanXMin =  0.;		
// scan grid origin Y coord (bottom)
double scanYMin =  0.;		
//double scanLength = scanStep * scanResolution;	
//double scanXMax =   scanXMin + (scanStep * scanResolution);
//double scanYMax =   scanYMin + (scanStep * scanResolution);

double minZval=0;

// these might get changed in initObsFromFile
double scanNear =  -100.;	// near end of Z-buffer range
double scanFar  =   0.;	// far  end of Z-buffer range


// mouse and cursor
int xMouseInWindow;	// mouse position in world coords
int yMouseInWindow;	// mouse position in world coords
Vec3d vMouseWorld;	// mouse position in world coords (actually a 2D vector in XY plane)
Vec3d vGrabOffset;	// offset from cursor position to grabbed object (in world coords actually a 2D vector in XY plane)


/* Here are our AFM tips */
SphereTip sTip(5.); // a spherical tip
// a conical tip - sphere radius is 20. cone height is 100 and half cone angle 30 degrees
InvConeSphereTip icsTip(5.,100.,DEG_TO_RAD*20.); // a InvConeSphere Tip

#define SPHERE_TIP 0
#define INV_CONE_SPHERE_TIP 1
#if 0
#define TIP_TYPE SPHERE_TIP
#else
#define TIP_TYPE INV_CONE_SPHERE_TIP
#endif


// FUNCTION PROTOYPES
void	setMaterialColor( GLfloat r, GLfloat g, GLfloat b );
void	setColor( int colorIndex );
// draws unit sphere
void	drawSphere(double radius);
void	drawCylinder( double diameter, double height );
void	drawTube( double diameter, double length, int obIndex );
void	lighting( void );
void	addObject( int obNum, 
		   int type, Vec3d pos, double yaw, double roll, double pitch, double leng, double diam,
		   int nextSeg, int prevSeg);
void	drawObjects( void );
void	drawStuff( void );
void	drawFrame( void );
void	adjustOrthoProjectionToWindow( void );
void	adjustOrthoProjectionToViewWindow( void );
void	reshapeWindow( int newWindowWidth, int newWindowHeight );
void	initObs( void );
void	displayFuncMain( void );
void	displayFuncView( void );
void	commonIdleFunc( void );
void	idleFuncDummy( void );
int		main(int argc, char *argv[]);
void	crossProduct( double  x1, double  y1, double  z1,
		      double  x2, double  y2, double  z2,
		      double* px, double* py, double* pz );
void	normalize( double* px, double* py, double* pz );
void	doImageScanApprox( void );
void	imageScanDepthRender( void );
void	showGrid( void );
void	displayFuncDepth( void );
void	reshapeWindowFuncDummy( int newWindowWidth, int newWindowHeight );
void	commonKeyboardFunc(unsigned char key, int x, int y);
void	mouseFuncMain( int button, int state, int x, int y );
void	mouseMotionFuncMain(int x, int y );
void	calcMouseWorldLoc( int xMouse, int yMouse );
double  norm_xy( Vec3d v );
double  vec_xy_Distance( Vec3d pt1, Vec3d pt2 );
int     findNearestObToMouse( void );
void    moveGrabbedOb( void );
void    grabNearestOb( void );

void
addObject( int obNum, 
	   int type, Vec3d pos, double yaw, double roll, double pitch, double leng, double diam,
	   int nextSeg, int prevSeg )
{
  //  ob[obNum].set(type,pos,DEG_TO_RAD*yaw,DEG_TO_RAD*roll,DEG_TO_RAD*pitch,leng,diam,nextSeg,prevSeg);
  ob[obNum].set(type,pos,yaw,roll,pitch,leng,diam,nextSeg,prevSeg);
  numObs++;
}

// Draw the objects (tip and tubes) in the sim world.
void
drawObjects( void )
{
  int i;

  // draw the objects
  for( i=0; i<numObs; i++ ) {
	  
    // we want different colors for our objects
    if ((i % 3) == 0)
      setColor( YELLOW );
    else if ((i % 3) == 1)
      setColor( GREEN );
    else 
      setColor( BLUE );
    
    // current object is red
    if( i == selectedOb )  setColor( RED );
    

    glPushMatrix();


    // put tube at its (x,y) position
    glTranslatef( ob[i].pos.x, ob[i].pos.y, ob[i].pos.z );


    // set tube yaw angle (in-plane rotation angle)
    glRotatef( ob[i].yaw, 0.0, 0.0, 1.0 ); 

    // set roll angle around tube axis
    glRotatef(ob[i].pitch,  0.0, 1.0, 0.0 );

    // set roll angle around tube axis
    glRotatef( ob[i].roll,  1.0, 0.0, 0.0 );


    // draw the tube or other object
    switch( ob[i].type ) {
    case TUBE:		
      drawTube( ob[i].diam, ob[i].leng, i );
      break;
		  
    case UNUSED:
      // do nothing.
      break;
		  
    default: error( "attempt to draw unknown object type" );
    }
		
    glPopMatrix();
  }
}

// draw a nano tube
void
drawTube( double diameter, double length, int obIndex )
{

  glPushMatrix();

  // draw cylinder with its axis parallel to X-axis
  glPushMatrix();
  /* we now have to align the cylinder with the Z-axis
   * if the tube did not have any translation and rotation
   * we want, the tube to be along the X-axis. GL on the other
   * hand draws a cylinder along the Z-axis by default. And so
   * we need to do the following rotate 
   */
  glRotatef(90, 0.0, 1.0, 0.0 );

  /* The position of the tube is given by its centre on the
   * axis. We want to draw the cylinder starting from its base
   * Get to its base
   */
  glTranslatef( 0., 0., - length /2. );  

#if DISPL_LIST
  glScalef(diameter/2.,diameter/2.,length);
  glCallList(list_cylinder);
#else
  drawCylinder( diameter, length );      // tube axis starts parallel to Z
#endif
  glPopMatrix();

  // draw spherical endcap on tube
  glPushMatrix();
  glTranslatef( length /2., 0., 0. );

#if DISP_LIST
  glScalef(diameter/2.,diameter/2.,diameter/2.);
  glCallList(list_sphere);
#else
  drawSphere(diameter / 2. ); 
#endif
  glPopMatrix();

  // draw spherical endcap on tube's other end
  glPushMatrix();
  glTranslatef( - length /2., 0., 0. );

#if DISP_LIST
  glScalef(diameter/2.,diameter/2.,diameter/2.);
  glCallList(list_sphere);
#else
  drawSphere(diameter / 2. ); 
#endif
  glPopMatrix();

  glPopMatrix();

}

static int firstTime = 1;
static GLUquadricObj* qobj;

// display list for a sphere
void make_cylinder() {

  if( firstTime ) { 
    qobj = gluNewQuadric();
    drawStyle = GLU_FILL;
    gluQuadricDrawStyle( qobj, drawStyle );
    gluQuadricNormals( qobj, GLU_FLAT );
    firstTime=0;
  }

  list_cylinder = glGenLists(1);
  // Create a display list for a sphere
  glNewList(list_cylinder, GL_COMPILE);
  gluCylinder( qobj, 1, 1, 1, 30, 30);
  // End definition of circle
  glEndList();
}

#if 0
void
drawCylinder( double diameter, double height ) {
  glPushMatrix();
  glScalef(diameter/2.,diameter/2.,height);
  //  glScalef(20,20,20);
  // Draw the predefined sphere
  //  cout << "rad = " << radius << endl;
  glCallList(list_cylinder);
  glCallList(11);
  glPopMatrix();

}
#endif

#if ~DISP_LIST
// draw cylinder with given diameter and height.
// The cylinder's axis is on the Z-axis, with one end at the origin
// and the other end on the +Z side.
void
drawCylinder( double diameter, double height )
{
  double radius;
  if( firstTime ) { 
    qobj = gluNewQuadric();
    drawStyle = GLU_FILL;
    gluQuadricDrawStyle( qobj, drawStyle );
    gluQuadricNormals( qobj, GLU_FLAT );  
    firstTime=0;
  }
  
  radius = diameter / 2.;
  gluCylinder( qobj, radius, radius, height, 30, 30);
}
#endif


// display list for a sphere
void make_sphere() {
  static int firstTime = 1;
  static GLUquadricObj* qobj;

  if( firstTime ) { 
    qobj = gluNewQuadric();
    drawStyle = GLU_FILL;
    gluQuadricDrawStyle( qobj, drawStyle);
    gluQuadricNormals( qobj, GLU_FLAT );
    firstTime=0;
  }

  // Create a display list for a sphere
  list_sphere = glGenLists(1);
  //  cout << "mk : sp = " << list_sphere << endl;
  glNewList(list_sphere, GL_COMPILE);
  // draw a sphere of radius 1
  gluSphere( qobj, 1, 30, 30);
  // End definition of circle
  glEndList();
}

#if 0
void drawSphere(double radius)
{
  glPushMatrix();
  glScalef(radius,radius,radius);
  //  glScalef(20,20,20);
  // Draw the predefined sphere
  //  cout << "rad = " << radius << endl;
  glCallList(list_sphere);
  glCallList(11);
  glPopMatrix();
}
#endif

#if ~DISP_LIST
void
drawSphere( double radius )
{
  static int firstTime = 1;
  static GLUquadricObj* qobj;
  if( firstTime ) { 
    qobj = gluNewQuadric();
    drawStyle = GLU_FILL;
    gluQuadricDrawStyle( qobj, drawStyle );
    gluQuadricNormals( qobj, GLU_FLAT );
    //    cout << "v" << endl; 
    firstTime=0;
  }
  gluSphere( qobj, radius, 30, 30);
}
#endif

/* Here are our object. Note we want our objects to be above the surface
 * i.e z >= 0
 */
void
initObs( void )
{
  // We start with no objects.
  numObs = 0;

#if (TIP_TYPE == SPHERE_TIP)	
  // a nano tube
  addObject( numObs, TUBE,  Vec3d( 65., 30., 45.), 0., 0., 0., 1, 10.,   NULLOB, NULLOB );
  //  addObject( numObs, TUBE,  Vec3d( 25., 30., 45.), 0., 40., 0., 20, 10.,   NULLOB, NULLOB );
  //  addObject( numObs, TUBE,  Vec3d( 5., 30., 45.), 0., 80., 0., 20, 10.,   NULLOB, NULLOB );
  //  addObject( numObs, TUBE,  Vec3d( 15., 30., 45.), 0., 140., 0., 20, 10.,   NULLOB, NULLOB );
  //  addObject( numObs, TUBE,  Vec3d( 75., 30., 45.), 0., 240., 0., 20, 10.,   NULLOB, NULLOB );
#endif
  /* a sphere : z coordi of centre is radius, i.e the sphere sits flat 
	 * on the surface */
  addObject( numObs, TUBE,  Vec3d( 25., 60., 30), 0., 0., 0., 0, 10.,   NULLOB, NULLOB );
}

FILE *file;

void addObsFromFile () {
  double x,y,z;
  int stop;

  double rad = 1.5;

  stop=0;
  while (!stop) {
    fscanf(file,"%lf",&x);
    if (!feof(file)) {
      fscanf(file,"%lf",&y);
      fscanf(file,"%lf",&z);
      x = x+40;
      y= y+40; // translation
      //      z = z + 15;
      minZval = ((!minZval) || (z < minZval)) ? z : minZval;
      addObject( numObs, TUBE,  Vec3d( x, y, z), 0., 0., 0., 1., rad,   NULLOB, NULLOB );
      //      cout << x << " " << y << " " << z << endl;
    }
    else
      stop=1;
  }
  scanFar = -minZval; // i am inverting this due to some weirdity
  //  cout << " val = " << scanFar << endl;
}

int
main(int argc, char *argv[])
{



#if 1
  file = fopen("lac.data","r"); 
  addObsFromFile();
#else
  // Other inits.
  initObs();
#endif



  // Deal with command line.
  glutInit(&argc, argv);
  glutInitDisplayMode(dblBuf);

  /* The view on Main window is a view of XY plane from a pt on the +ve 
   * Z-axis. +ve X axis is towards right while +ve Y is to upwards
   */
  
  // MAIN WINDOW
  glutInitWindowSize( (int)windowWidth, (int)windowHeight );
  glutInitWindowPosition( 0, 0 );
  mainWindowID = glutCreateWindow( "3D CNT simulator - Top View" );
  adjustOrthoProjectionToWindow();

  make_sphere();
  make_cylinder();

  
  // pass pointers to callback routines for main window
  glutDisplayFunc(displayFuncMain);
  glutIdleFunc(idleFuncDummy );
  glutReshapeFunc(reshapeWindow);
  glutKeyboardFunc(commonKeyboardFunc);
  glutMouseFunc(mouseFuncMain);
  glutMotionFunc(   mouseMotionFuncMain );

  /* The view on Another View window is a front view from a point on the
   *  -Y axis
   */
  
  // another view WINDOW
  glutInitWindowSize( (int)windowWidth, (int)windowHeight );
  glutInitWindowPosition( 800, 0 );
  viewWindowID = glutCreateWindow( "Front View" );
  adjustOrthoProjectionToViewWindow();

  make_sphere();
  make_cylinder();

  // pass pointers to callback routines for the other view window
  glutDisplayFunc(displayFuncView);
  glutIdleFunc(idleFuncDummy );
  //  glutReshapeFunc(reshapeWindow);
  glutKeyboardFunc(commonKeyboardFunc);


  // Depth WINDOW
  glutInitWindowSize( (int)DEPTHSIZE, (int)DEPTHSIZE );
  glutInitWindowPosition( 0, 650 );
  depthWindowID = glutCreateWindow( "Depth window" );

  make_sphere();
  make_cylinder();

  glutDisplayFunc( displayFuncDepth );
  glutIdleFunc(idleFuncDummy );
  glutReshapeFunc(    reshapeWindowFuncDummy );
  glutKeyboardFunc(commonKeyboardFunc);
  adjustOrthoProjectionToWindow();


  // app's main loop, from which callbacks to above routines occur
  glutMainLoop();

  return 0;               /* ANSI C requires main to return int. */
}


/**************************************************************************************/
// This routine is called only after input events.
void
displayFuncMain( void )
{
  glutSetWindow( mainWindowID );
  // draw graphics for this frame
  drawFrame();
}

void
displayFuncView( void )
{
  glutSetWindow( viewWindowID );
  glPushMatrix();
  glRotatef(-90, 1.0, 0.0, 0.0 ); 
  drawFrame();
  glPopMatrix();
}

// This is the callback for rendering into the depth buffer window,
// as required by "doImageScanApprox".  
void 
displayFuncDepth( void ) 
{

  glutSetWindow( depthWindowID );
  // in Z-buffer of Depth Window using graphics hardware.
  doImageScanApprox();
}

void
drawFrame( void )
{
  // Setup OpenGL state.
  glClearDepth(1.0);
  glClearColor(0.5, 0.5, 0.5, 0.0);   // gray background

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable(GL_DEPTH_TEST);

  shadingModel = GL_SMOOTH;
  lighting();
  glPointSize( 2. );    // se p51 Woo 3rd ed
  glLineWidth( 2. );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  drawStuff(); // Draw objects
  // Draw the image scan grid.
  showGrid();
  // end of display frame, so flip buffers
  glutSwapBuffers();
}

void
lighting( void )
{
  if( shadingModel == GL_FLAT ) {
    glShadeModel(GL_FLAT);
    glDisable( GL_LIGHTING );
  }
  else {
    GLfloat light_ambient[]  = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position0[] = {  1.0, 1.0, 1.0, 0.0 };
    GLfloat light_position1[] = { -1.0, 1.0, 0.0, 0.0 };
		

    glShadeModel(GL_SMOOTH);

    glLightfv( GL_LIGHT0,  GL_AMBIENT,  light_ambient );
    glLightfv( GL_LIGHT0,  GL_DIFFUSE,  light_diffuse );
    glLightfv( GL_LIGHT0,  GL_SPECULAR, light_specular );
    glLightfv( GL_LIGHT0,  GL_POSITION, light_position0 );

    glLightfv( GL_LIGHT1,  GL_AMBIENT,  light_ambient );
    glLightfv( GL_LIGHT1,  GL_DIFFUSE,  light_diffuse );
    glLightfv( GL_LIGHT1,  GL_SPECULAR, light_specular );
    glLightfv( GL_LIGHT1,  GL_POSITION, light_position1 );

    if( lightOn[0] )  glEnable(  GL_LIGHT0 );
    else              glDisable( GL_LIGHT0 );
    if( lightOn[1] )  glEnable(  GL_LIGHT1 );
    else              glDisable( GL_LIGHT1 );

    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    glEnable( GL_LIGHTING );
  }
}

void
setColor( int colorIndex )
{
  switch( colorIndex ) {
  case 0: setMaterialColor(1.0, 1.0, 1.0); break;  /* white */
  case 1: setMaterialColor(1.0, 0.0, 0.0); break;  /* red */
  case 2: setMaterialColor(0.0, 1.0, 0.0); break;  /* green */
  case 3: setMaterialColor(0.0, 0.0, 1.0); break;  /* blue */
  case 4: setMaterialColor(1.0, 0.0, 1.0); break;  /* magenta */
  case 5: setMaterialColor(1.0, 1.0, 0.0); break;  /* yellow */
  case 6: setMaterialColor(0.0, 1.0, 1.0); break;  /* cyan */
  case 7: setMaterialColor(0.0, 0.0, 0.0); break;  /* black */
  }
}

void
setMaterialColor( GLfloat r, GLfloat g, GLfloat b )
{
  if( shadingModel == GL_FLAT ) {
    glColor3f( r, g, b );
  }
  else {
    GLfloat mat_ambient[]    = { 0.2*r, 0.2*g, 0.2*b, 1.0 };
    GLfloat mat_diffuse[]    = { 0.8*r, 0.8*g, 0.8*b, 1.0 };
    GLfloat mat_specular[]   = { 1.0*r, 1.0*g, 1.0*b, 1.0 };
    GLfloat mat_shininess[]  = { 30.0 };

    glMaterialfv( GL_FRONT_AND_BACK,  GL_AMBIENT,  mat_ambient );
    glMaterialfv( GL_FRONT_AND_BACK,  GL_DIFFUSE,  mat_diffuse );
    glMaterialfv( GL_FRONT_AND_BACK,  GL_SPECULAR,  mat_specular );
    glMaterialfv( GL_FRONT_AND_BACK,  GL_SHININESS, mat_shininess );
  }
}

void
drawStuff( void )
{
  // draw all objects at their current locations
  drawObjects();
}

// This idle function marks both windows for redisplay, which will cause
// their display callbacks to be invoked.
void
commonIdleFunc( void )
{
  glutSetWindow( mainWindowID );		glutPostRedisplay();
  glutSetWindow( viewWindowID );		glutPostRedisplay();
  glutSetWindow( depthWindowID );		glutPostRedisplay();
}

void idleFuncDummy( void ) {commonIdleFunc();}
void reshapeWindowFuncDummy( int newWindowWidth, int newWindowHeight ) {}

// callback routine: called when window is resized by user
void
reshapeWindow( int newWindowWidth, int newWindowHeight )
{
  windowWidth  = newWindowWidth;
  windowHeight = newWindowHeight;

  // viewport covers whole window
  glViewport( 0, 0, (int)windowWidth, (int)windowHeight );

  // make graphics projection match window dimensions
  adjustOrthoProjectionToWindow();
}

// adjust the ortho projection to match window aspect ratio and keep circles round.
void
adjustOrthoProjectionToWindow( void )
{
  double orthoFrustumNearEdge =  scanNear;
  /* All far pts get mapped to scanFar. Allow round off of 1 */
  double orthoFrustumFarEdge  =   scanFar+1;

  // set nominal size of window before taking aspect ratio into account
  //	double orthoFrustumLeftEdgeNominal   = orthoFrustumCenterX - orthoFrustumWidthNominal/2.;
  double orthoFrustumBottomEdgeNominal = orthoFrustumCenterY - orthoFrustumHeightNominal/2.;

  // calculate aspect ratio of current window
  double aspectRatio = windowWidth / windowHeight;

  // set vertical extent of window to nominal area of world being viewed.
  orthoFrustumHeight = orthoFrustumHeightNominal;
  orthoFrustumBottomEdge = orthoFrustumBottomEdgeNominal;

  // view horizontal extent of world proportional to window width
  orthoFrustumWidth = orthoFrustumWidthNominal * aspectRatio;
  orthoFrustumLeftEdge   = orthoFrustumCenterX - orthoFrustumWidth / 2.;

  //  cout << "x- " << orthoFrustumLeftEdge << " x+ " << (orthoFrustumLeftEdge   + orthoFrustumWidth) << " y- " << orthoFrustumBottomEdge << " y+ " << (orthoFrustumBottomEdge + orthoFrustumHeight) << endl;
  
  // set projection matrix to orthoscopic projection matching current window
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(  orthoFrustumLeftEdge,   orthoFrustumLeftEdge   + orthoFrustumWidth,
	    orthoFrustumBottomEdge, orthoFrustumBottomEdge + orthoFrustumHeight, orthoFrustumNearEdge,   orthoFrustumFarEdge );
}

// adjust the ortho projection to match window aspect ratio and keep circles round.
void
adjustOrthoProjectionToViewWindow( void )
{
  double orthoFrustumNearEdge =  -scanFar;
  double orthoFrustumFarEdge  =   -scanNear;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(  orthoFrustumLeftEdge,   orthoFrustumLeftEdge   + orthoFrustumWidth,
	    orthoFrustumNearEdge,   orthoFrustumFarEdge, 
	    orthoFrustumBottomEdge, orthoFrustumBottomEdge + orthoFrustumHeight);
}

void
error( char* errMsg )
{
  printf( "\nError: %s\n", errMsg );
  exit( 1 );
}

void 
doImageScanApprox( void ) 
{
  int i;
  int j;

  // Render tube images (enlarged to account for tip radius)
  // into window.  
  // (We don't really care about the image, just the depth.)
  imageScanDepthRender();
  
  // Read (normalized) Z-buffer values from the depth window.
  // Scale them back to correct Z-values and use as 
  // Z-heights in image scan grid.  
  // static double zBuffer[ 128*128 ];
  void* zBufferPtr = &(zBuffer[0]);
  int pixelGridSize = DEPTHSIZE;		// must match window size
  // width and height the same for now
  glReadPixels( 0, 0, pixelGridSize, pixelGridSize, GL_DEPTH_COMPONENT, GL_FLOAT, zBufferPtr );
  for( i=0; i<scanResolution; i++ ) {
    for( j=0; j<scanResolution; j++ ) {
      double zNormalized = zBuffer[ j*pixelGridSize + i ];
      //      double zDepth = scanFar + zNormalized * (scanNear - scanFar);
      // changed this : mapping the far plane to a height zero
      double zDepth = minZval + (1-zNormalized)*abs(scanNear - scanFar);
      zHeight[i][j] = zDepth;
    }
  }
}


/* This is the most critical part of the afmsim. This is the one which 
 * renders the afm scan
 */
// display graphics in the depth window.
void 
imageScanDepthRender( void ) 
{
  double tipRadius, theta, radius;

  // draw into depth window
  glutSetWindow( depthWindowID );

  // Setup OpenGL state.
  glClearDepth(1.0);
  glClearColor(0.5, 0.5, 0.5, 0.0);   // gray background
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable(GL_DEPTH_TEST);

  // set projection matrix to orthoscopic projection matching current window
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(  scanXMin,   scanXMin + (scanStep * scanResolution),
	    scanYMin,   scanYMin + (scanStep * scanResolution),
	    scanNear,   scanFar   );

  // set modeling matrix to identity
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // set drawing parameters
  int saveRenderStyle = renderStyle;
  renderStyle = OB_SOLID;
  shadingModel = GL_SMOOTH;
  lighting();

  setColor( WHITE );
  for( int i=0; i<numObs; i++ ) {
    if( ob[i].type==UNUSED )   continue;
	  
    glPushMatrix();
	  
    switch (TIP_TYPE) {
    case SPHERE_TIP :

      // Lower surface to match real surface height for ridges and plains.
      tipRadius = sTip.r; // the radius for the tip
      glTranslatef( 0., 0., -tipRadius );  

      glTranslatef( ob[i].pos.x, ob[i].pos.y, ob[i].pos.z );
      //##################################################################
      //##################################################################
      //  CHANGEEEEEEEEEEEEEEEE THISSSSSSSSSSSSSSSSSSSSSSSSSSSS
      //##################################################################
      //##################################################################


      // set tube yaw angle (in-plane rotation angle)
      glRotatef( ob[i].yaw, 0.0, 0.0, 1.0 ); 
      // set roll angle around tube axis
      glRotatef( ob[i].pitch,  0.0, 1.0, 0.0 );
      // set roll angle around tube axis
      glRotatef( ob[i].roll,  1.0, 0.0, 0.0 );

      //      ob[i].print();

      // Draw SSL with radius of tube plus radius of tip.
      // This surface is the "solvent-accessible surface" for
      // a "solvent" sphere with the radius of the tip (for this tube).
      // The topmost of all these surfaces for all tubes is
      // the image scan -- the farthest the tip can be lowered
      // until it touches something for each (x,y) grid point.  
      radius = ob[i].diam/2. + tipRadius;
      drawTube( radius * 2., ob[i].leng, i );
      //      drawSphere( radius);
      break;
    case INV_CONE_SPHERE_TIP :
      // Lower surface to match real surface height for ridges and plains.
      tipRadius = icsTip.r; // the radius for the tip
      theta = icsTip.theta; // theta for the tip
      glTranslatef( 0., 0., -tipRadius );  
      radius=ob[i].diam/2.;
      double afm_height = ob[i].pos.z + (radius+tipRadius)/sin(theta);
      //      double afm_base_radius = afm_height*tan(theta);
      //      ConeSphere c = ConeSphere(afm_base_radius,afm_height,radius+tipRadius);
      ConeSphere c = ConeSphere(radius+tipRadius, afm_height, theta);
      glTranslatef(ob[i].pos.x,ob[i].pos.y,0);
      c.draw();
      break;
    }
    glPopMatrix();
  }

  // end of display frame, so flip buffers
  glutSwapBuffers();

  renderStyle = saveRenderStyle;
}

// Display the image scan grid (a depth image).
void
showGrid( void )
{
  int gridColor = GREEN;

  // Display depth image surface.  
  // The variable "gridStyle" controls which of several visualizations
  // of the surface are used.
  shadingModel = GL_SMOOTH;
  lighting();
  setColor( gridColor );

  for( int i=0; i<scanResolution-1; i++ ) {
    for( int j=0; j<scanResolution-1; j++ ) {
      double x = i * scanStep  +  scanXMin;
      double y = j * scanStep  +  scanYMin;
      double dx = scanStep;
      double dy = scanStep;

      // Get the the 4 (x,y,z) coords on the corners of this grid cell.
      // Show objects above the surface only

#if 0
      double x1 = x;     double y1 = y;     double z1 = ((zHeight[i  ][j  ] > 0) ? zHeight[i  ][j  ] : 0.);
      double x2 = x+dx;  double y2 = y;     double z2 = ((zHeight[i+1][j  ] > 0) ? zHeight[i+1][j  ] : 0.);
      double x3 = x+dx;  double y3 = y+dy;  double z3 = ((zHeight[i+1][j+1] > 0) ? zHeight[i+1][j+1] : 0.);
#else
      double x1 = x;     double y1 = y;     double z1 = zHeight[i  ][j  ];
      double x2 = x+dx;  double y2 = y;     double z2 = zHeight[i+1][j  ];
      double x3 = x+dx;  double y3 = y+dy;  double z3 = zHeight[i+1][j+1];
      double x4 = x;     double y4 = y+dy;  double z4 = zHeight[i  ][j+1];
#endif

      // calc normal to plane through P1, P2, P3 using (P1-P3) x (P1-P2)
      double xn = 0., yn = 0., zn = 1.;
      crossProduct( x1-x2, y1-y2, z1-z2, x1-x3, y1-y3, z1-z3, &xn, &yn, &zn );
      normalize( &xn, &yn, &zn );
				
      // draw the triangle 
      glBegin(GL_POLYGON);
      glNormal3f( xn, yn, zn );
      glVertex3f( x1, y1, z1 );
      glVertex3f( x2, y2, z2 );
      glVertex3f( x3, y3, z3 );
      glEnd();
#if 0 // draw solid

    crossProduct( x3-x4, y3-y4, z3-z4, x3-x1, y3-y1, z3-z1, &xn, &yn, &zn );
    normalize( &xn, &yn, &zn );
    
    glBegin(GL_POLYGON);
    glNormal3f( xn, yn, zn );
    glVertex3f( x3, y3, z3 );
    glVertex3f( x4, y4, z4 );
    glVertex3f( x1, y1, z1 );
    glEnd();
#endif

    }

  }

}

// normalize argument vector
void
normalize( double* px, double* py, double* pz )
{
  // calc magnitude of vector
  double leng = sqrt( (*px)*(*px) + (*py)*(*py) + (*pz)*(*pz) );

  if( leng == 0 )
    return;

  // divide each vector component by the magnitude.
  *px /= leng;
  *py /= leng;
  *pz /= leng;
}

void
crossProduct( double  x1, double  y1, double  z1,
	      double  x2, double  y2, double  z2,
	      double* px, double* py, double* pz )
{
  *px = y1*z2 - z1*y2;
  *py = z1*x2 - x1*z2;
  *pz = x1*y2 - y1*x2;
}


// Keyboard callback for main window.
void
commonKeyboardFunc(unsigned char key, int x, int y) {
  int newOb = numObs;

#define DIST_UNIT 3
#define ANGLE_UNIT 3
#define LENGTH_UNIT 3
#define DIAM_UNIT  1
#define DEFAULT_LENGTH 20
#define DEFAULT_DIAM 5
  switch (key) {
  case '+' :       
    /* Right arrow key */
    ob[selectedOb].pos.z += DIST_UNIT;
    break;
  case '-' : 
    /* XXX : This is broken : need to check if any part of the object goes 
     * below the surface
     */
    /* Right arrow key */
    ob[selectedOb].pos.z = (ob[selectedOb].pos.z > DIST_UNIT) ? (ob[selectedOb].pos.z-DIST_UNIT) : 1.;
    break;
    /* Now we consider various rotations */
  case 'x' :
    ob[selectedOb].roll += ANGLE_UNIT;
    break;
  case 'y' :
    ob[selectedOb].pitch += ANGLE_UNIT;
    break;
  case 'z' :
    ob[selectedOb].yaw += ANGLE_UNIT;
    break;
  case 'X' :
    ob[selectedOb].roll -= ANGLE_UNIT;
    break;
  case 'Y' :
    ob[selectedOb].pitch -= ANGLE_UNIT;
    break;
  case 'Z' :
    ob[selectedOb].yaw -= ANGLE_UNIT;
    break;
    /* Object manipulation */
  case 'l' :
    ob[selectedOb].leng += LENGTH_UNIT;
    break;
  case 'L' :
    ob[selectedOb].leng  = (ob[selectedOb].leng >= LENGTH_UNIT) ? (ob[selectedOb].leng-LENGTH_UNIT) : 1.;
    break;
  case 'd' :
    ob[selectedOb].diam += DIAM_UNIT;
    break;
  case 'D' :
    ob[selectedOb].diam = (ob[selectedOb].diam > DIAM_UNIT) ? (ob[selectedOb].diam-DIAM_UNIT) : 1.;
    break;
  case 'n' :

#if (TIP_TYPE == SPHERE_TIP)
    // z of the tube is such that it just sits on the surface.
    addObject( numObs, TUBE,  Vec3d( 0., 0., (DEFAULT_DIAM/2.)), 0., 0., 0., DEFAULT_LENGTH, DEFAULT_DIAM,   NULLOB, NULLOB );
    selectedOb = newOb;
#else
    // only spheres are supported in the inverted tip afm
    addObject( numObs, TUBE,  Vec3d( 0., 0., 5.), 0., 0., 0., 0, 10,   NULLOB, NULLOB );
    selectedOb = newOb;
#endif
    break;
  case 'q' :
    exit(0);
    break;
  }
  
  glutPostRedisplay();	// in case something was changed
}


// Callback routine: called for mouse button events.
void
mouseFuncMain( int button, int state, int x, int y )
{
  calcMouseWorldLoc( x, y );

  switch( button ) {
  case GLUT_LEFT_BUTTON: 
    if(      state == GLUT_DOWN )	{grabNearestOb();}
    else if( state == GLUT_UP )		{}
    break;
  case GLUT_RIGHT_BUTTON: 
    if(      state == GLUT_DOWN )	{}
    else if( state == GLUT_UP )		{}
    break;
  }

  glutPostRedisplay();
}


// Callback routine: called when mouse is moved while a button is down.
// Only called when cursor loc changes.
// x,y:    cursor loc in window coords
// see p658 Woo 3rd ed
void
mouseMotionFuncMain( int x, int y )
{
  // Map mouse cursor window coords to world coords.
  // Since we're using an orthoscopic projection parallel to the Z-axis,
  // we can map (x,y) in window coords to (x,y,0) in world coords.
  calcMouseWorldLoc( x, y );

  // Move the grabbed object, if any, to match mouse movement.
  moveGrabbedOb();

  //	glutPostRedisplay();
}


// Calculate where the cursor maps to in world coordinates, 
// based on the window width and height and the edges of
// the frustum of the orthoscopic projection.
void
calcMouseWorldLoc( int xMouse, int yMouse ) 
{
  double xMouseNormalized;
  double yMouseNormalized;

  // write the cursor loc in window coords to global var
  xMouseInWindow = xMouse;
  yMouseInWindow = yMouse;	

  // calculate normalized cursor position in window: [0,1]
  xMouseNormalized = xMouseInWindow / windowWidth;
  yMouseNormalized = yMouseInWindow / windowHeight;

  // invert normalized Y due to up being - in mouse coords, but + in ortho coords.
  yMouseNormalized = 1. - yMouseNormalized;

  // calculate cursor position in ortho frustum's XY plane
  vMouseWorld.x = (xMouseNormalized * orthoFrustumWidth)  + orthoFrustumLeftEdge;
  vMouseWorld.y = (yMouseNormalized * orthoFrustumHeight) + orthoFrustumBottomEdge;
  vMouseWorld.z = 0; 
}

// search for nearest ob to cursor, and set grab offset vector.
void
grabNearestOb( void )
{
  // search through all objects to find the one nearest to the cursor.
  selectedOb = findNearestObToMouse();  // may be NULLOB

  // calculate grab offset vector
  if( selectedOb != NULLOB ) {
    // note the grab affects X and Y co-ordinates only
    vGrabOffset = Vec3d(ob[selectedOb].pos.x - vMouseWorld.x,
			ob[selectedOb].pos.y - vMouseWorld.y,
			0);
  }
  
  moveGrabbedOb();
}

void
moveGrabbedOb( void )
{
  // move the selected object, if any, to correspond with the mouse movement
  if( selectedOb != NULLOB ) {
    // note the grab affects X and Y co-ordinates only
    ob[selectedOb].pos   = Vec3d(vMouseWorld.x + vGrabOffset.x,
				 vMouseWorld.y + vGrabOffset.y,
				 ob[selectedOb].pos.z);
  }
}

// searches through all objects to find the object nearest the mouse cursor.
// returns the index of the object, or NULLOB if no objects are within threshold.
int
findNearestObToMouse( void ) {
  int i;
  int nearestOb = NULLOB;
  double nearestDist = 1000000.;
  double thresholdDist = 10.;
  double dist;

  for( i=0; i<numObs; i++ ) {
    if( ob[i].type==UNUSED )   continue;

    dist = vec_xy_Distance( vMouseWorld, ob[i].pos );

    if( dist < nearestDist  &&  dist < thresholdDist ) {
      nearestDist = dist;
      nearestOb   = i;
    }
  }

  return nearestOb;
}

double
vec_xy_Distance( Vec3d pt1, Vec3d pt2 )
{
  return norm_xy( pt1 - pt2 );
}

// norm in x and y only
double
norm_xy( Vec3d v )
{
  return sqrt( (v.x * v.x)  +  (v.y * v.y) );
}


