#ifndef PATTERNEDITOR_H
#define PATTERNEDITOR_H

#include "imageViewer.h"
#include "nmb_Image.h"
#include "list.h"

class PatternElement {

   double d_startX_nm;
   double d_startY_nm;
   double d_endX_nm;
   double d_endY_nm;
   double d_width_nm;
   double d_exposure_uCoulombs_per_square_cm;
};


class PatternEditor {
  public:
   PatternEditor();
   ~PatternEditor();
   void show();
   void addImage(nmb_Image *im);
   void removeImage(nmb_Image *im);

  protected:
   static int mainWinEventHandler(const ImageViewerWindowEvent &event, 
                                  void *ud);
   static int mainWinDisplayHandler(const ImageViewerDisplayData &data, 
                                    void *ud);
   static int navWinEventHandler(const ImageViewerWindowEvent &event,
                                 void *ud);
   static int navWinDisplayHandler(const ImageViewerDisplayData &data,
                                    void *ud);
   void navWinPositionToWorld(double x, double y,
                                       double &x_nm, double &y_nm);
   void mainWinPositionToWorld(double x, double y,
                                       double &x_nm, double &y_nm);
   void zoomBy(double centerX_nm, double centerY_nm,
                           double magFactor);

   ImageViewer *d_viewer;
   int d_mainWinID;
   int d_navWinID;
   
   double d_worldMinX_nm;
   double d_worldMinY_nm;
   double d_worldMaxX_nm;
   double d_worldMaxY_nm;

   double d_mainWinMinX_nm;
   double d_mainWinMinY_nm;
   double d_mainWinMaxX_nm;
   double d_mainWinMaxY_nm;

   vrpn_bool d_settingRegion;
   vrpn_bool d_settingTranslation;
   double d_dragStartX_nm;
   double d_dragStartY_nm;
   double d_dragEndX_nm;
   double d_dragEndY_nm;

   list<nmb_Image *> d_images;
   list<PatternElement> d_pattern;
};

#endif
