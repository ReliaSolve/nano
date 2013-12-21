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

#ifndef MSIFILE_GENERATOR_H
#define MSIFILE_GENERATOR_H

#include "FileGenerator.h"

/*For MSISphere*/
typedef GLfloat VertexType[3];
#define xx .525731112119133606
#define zz .850650808352039932

class MSIFileGenerator; //forward declaration

//MSISphere is basically mysphere of graphics/globjects.c, but modified
//and in class form.  It is used to draw atoms.
class MSISphere{
  //private:
    GLuint dl;
    int sphere_depth;
  public:
    MSISphere();
    void SetSphereDepth(int);
    void Subdivide(float*,float*,float*,int);
    GLuint DisplayList();
    ~MSISphere();
    friend class MSIFileGenerator;
};

//This class handles all loading of a geometry contained in a .msi file into a display list, which is stored in an uberGraphics tree
class MSIFileGenerator : public FileGenerator {
  private:
    GLuint dl; //index of display list
    MSISphere* atom_ptr; //pointer to the atom we want to use in our display list

    float *atomx; //an array of atom x-coordinates
    float *atomy; //an array of atom y-coordinates
    float *atomz; //an array of atom z-coordinates
    int *bond1;   //an array of the atoms from which bonds are drawn
    int *bond2;   //an array of the atoms to which bonds are drawn
    int bond_count,model_count,atom_count;
    float bond_width,sphere_radius,sphere_depth;
    int import_mode; //import_mode is 0 for bond mode, 1 for sphere mode
    int visibility_mode; //visibility_mode is 0 for hide mode, 1 for show mode
  public:
    MSIFileGenerator(const char* fname = NULL);
    
    virtual int Load(URender *node, GLuint *&Dlist_array);
    virtual int ReLoad(URender *node, GLuint *&Dlist_array);

    void BuildListMSI(GLuint);
    void SetImportMode(int);
    void SetVisibilityMode(int);
    void SetBondWidth(float);
    void SetSphereRadius(float);
    void SetSphereDepth(int);
    ~MSIFileGenerator();
};

#endif

