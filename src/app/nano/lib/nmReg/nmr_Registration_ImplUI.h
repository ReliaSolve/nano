#ifndef NMR_REGISTRATION_IMPLUI_H
#define NMR_REGISTRATION_IMPLUI_H

#include "nmb_Image.h"
#include "nmr_Registration_Interface.h"
#include "nmr_Registration_Impl.h"
#include "correspondenceEditor.h"

class nmr_Registration_Impl;

/// This class provides a graphical user-interface to the
/// nmr_Registration_Impl in order let the user edit correspondence 
/// information and to allow the user to initiate registration based on
/// the current correspondence
class nmr_Registration_ImplUI {
  public:
    nmr_Registration_ImplUI(nmr_Registration_Impl *impl);
    ~nmr_Registration_ImplUI();
    void enable(vrpn_bool enable);
    void registerImages();
    void mainloop();

    void newScanline(nmr_ImageType whichImage,
                                vrpn_int32 row, nmb_Image *im);

    void setFiducial(nmr_ImageType whichImage,
          vrpn_float32 x_n, vrpn_float32 y_n, vrpn_float32 z);
    void getCorrespondence(Correspondence &c, int &srcIndex, int &tgtIndex);

  protected:
    static int s_numImages;
    static char *s_imageWinNames[];
    static int s_sourceImageIndex, s_targetImageIndex;

    static void handle_registration_start_change(vrpn_int32 val, 
                                                 void *userdata);
    static void handle_ServerMessage( void *ud,
                         const nmr_ServerChangeHandlerData &info);

    CorrespondenceEditor *d_ce; // ui to display landmarks and
                              // let user set them

    nmr_Registration_Impl *d_impl;
};

#endif
