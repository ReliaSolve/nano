
/* Gokul Varadhan
 * varadhan@cs.unc.edu
 * Oct 2000
 */

#include <stdio.h>
#include <iostream>
using namespace std;
#include <math.h>               //math.h vs cmath
#include <GL/glut_UNC.h>
#include "Tips.h"
#include "defns.h"

SphereTip :: SphereTip(double _r) {
  set(_r);
}

void SphereTip :: set(double _r) {
  if (_r >= 0) {
    r = _r;
  }
}

void SphereTip :: set_r(double _r) {
  if (_r >= 0) {
    r = _r;
  }
}

void InvConeSphereTip :: set_r(double _r) {
	if (_r >= 0) {
		ConeSphere::set_r(_r);
	}
}

void SphereTip :: print() {
  cout << "r = " << r << endl;
}

// angle is half cone angle. it should be in radians
InvConeSphereTip :: InvConeSphereTip(double _r, double _ch, double _angle,
	int _tesselation) {
  set(_r,_ch,_angle,_tesselation);
  epoch = 0;
}

void InvConeSphereTip :: set(double _r, double _ch, double _angle,
	int _tesselation) {
  ConeSphere::set(_r,_ch,_angle);
  tesselation = _tesselation;
  epoch++;
}

void InvConeSphereTip :: draw() {
 static int firstTime = 1;
  static GLUquadricObj* qobj;
  if( firstTime ) { qobj = gluNewQuadric(); }
  
  glPushMatrix();
  gluQuadricDrawStyle( qobj, GLU_LINE );
  gluQuadricNormals( qobj, GLU_FLAT );

  glRotatef(180, 1.0, 0.0, 0.0 ); 
  double h = this->sphereHeight+this->r;
  glTranslatef(0,0,-h);

  // draw 
  gluCylinder( qobj, cr, topRadius, topHeight, 30, 10);
  // now go to the centre of the embedded sphere
  glTranslatef(0, 0, sphereHeight);
  gluSphere( qobj, r, 30, 30);
  glPopMatrix();
}

Tip :: Tip(SphereTip *_sp, InvConeSphereTip *_ics, int tesselation,
	int default_type) {
  set(_sp,_ics,tesselation,default_type);
}

void Tip :: set(SphereTip *_sp, InvConeSphereTip *_ics, int tesselation,
	int default_type) {
  type = default_type;
  spTip = _sp;
  icsTip = _ics;
}

void Tip :: change_tip_model() {
  if (type == SPHERE_TIP) {
    type = INV_CONE_SPHERE_TIP;
  }
  else {
    type = SPHERE_TIP;
  }
}

void Tip :: inc_r() {
  if (type == SPHERE_TIP) {
    spTip->set_r(spTip->r + TIP_RADIUS_UNIT);
    cout << "r " << spTip->r << endl;
  }
  else {
    icsTip->set_r(icsTip->r + TIP_RADIUS_UNIT);
    cout << "r " << icsTip->r << endl;
  }
}

void Tip :: dec_r() {
  if (type == SPHERE_TIP) {
    spTip->set_r(spTip->r - TIP_RADIUS_UNIT);
    cout << "r " << spTip->r << endl;
  }
  else {
    icsTip->set_r(icsTip->r - TIP_RADIUS_UNIT);
    cout << "r " << icsTip->r << endl;
  }
}


void Tip :: inc_theta() {
  if (type == INV_CONE_SPHERE_TIP) {
    icsTip->set_theta(icsTip->theta+DEG_TO_RAD*TIP_THETA_UNIT);
    cout << "theta " << RAD_TO_DEG*icsTip->theta << endl;
  }
}

void Tip :: dec_theta() {
  if (type == INV_CONE_SPHERE_TIP) {
    if (icsTip->theta > (DEG_TO_RAD*TIP_THETA_UNIT)) {
      icsTip->set_theta(icsTip->theta-DEG_TO_RAD*TIP_THETA_UNIT);
      cout << "theta " << RAD_TO_DEG*icsTip->theta << endl;
    }
  }
}




