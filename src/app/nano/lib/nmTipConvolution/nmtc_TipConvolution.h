#ifndef NMTC_TIPCONVOLUTION_H
#define NMTC_TIPCONVOLUTION_H
#include "nmg_Graphics.h"
#include "nmb_Image.h"
#include "nmb_String.h"
#include "nmr_Registration_Proxy.h"
#include "Tcl_Linkvar.h"
#include "Tcl_Netvar.h"
#include <nmui_Component.h>

class nmtc_TipConvolution {
 public:
  nmtc_TipConvolution(nmg_Graphics *g, nmb_ImageList *im);
  ~nmtc_TipConvolution();
  void changeDataset(nmb_ImageList *im);
  static void handle_resultImageName_change(const char *name, void *ud);
  static void handle_convolutionImageData_change(const char *name, void *ud);
  static void handle_convolutionTipName_change(const char *name, void *ud); 

  void CreateConvolutionImage(const char *imageName);
  
  // used to setup synchronization on protected
  // TclNet member variables
  void nmtc_SetupSync(nmui_Component * container);

  // used to teardown synchronization on protected
  // TclNet member variables
  void nmtc_TeardownSync(nmui_Component * container);

 protected:

  TclNet_string d_convolutionImageData;
  TclNet_string d_convolutionTipName;
  // Tclvar_string d_convolutionEnabled;
  TclNet_string d_resultImageName;

  nmg_Graphics *d_graphicsDisplay;
  nmb_ImageList *d_imageList;

};
#endif








