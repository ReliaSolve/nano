#ifndef PATTERNEDITOR_H
#define PATTERNEDITOR_H

#include "imageViewer.h"
#include "nmb_Image.h"
#include "list.h"
#include "Tcl_Linkvar.h"

class PatternPoint {
  public:
   PatternPoint(double x=0, double y=0 ): d_x(x), d_y(y) {};
   operator== (const PatternPoint &ppt) {
        return (d_x == ppt.d_x && d_y == ppt.d_y);}
   void translate(double x, double y)
   { d_x += x; d_y += y;}

   double d_x, d_y;
};

typedef enum {PS_POLYLINE, PS_POLYGON} ShapeType;

class PatternShape {
  public:
    PatternShape(double lw = 0, double exp = 0, ShapeType type=PS_POLYLINE);
    PatternShape(const PatternShape &sh);
    operator== (const PatternShape &sh) {return (d_ID == sh.d_ID);}
    void addPoint(double x, double y);
    void translate(double x, double y)
    { d_trans_x += x; d_trans_y += y; }
    void draw();

    int d_ID;
    static int s_nextID;

    double d_lineWidth_nm;
    double d_exposure_uCoulombs_per_square_cm;
    list<PatternPoint> d_points;
    ShapeType d_type;
    double d_trans_x, d_trans_y;
};

class ImageElement {
  public:
   ImageElement():d_red(1.0), d_green(1.0), d_blue(1.0),
                  d_opacity(1.0), d_enabled(vrpn_FALSE),d_image(NULL) {}
   ImageElement(nmb_Image *im, double r=1.0, double g=1.0, double b=1.0, 
                double o=1.0, vrpn_bool e=vrpn_FALSE):
            d_red(r), d_green(g), d_blue(b), d_opacity(o), 
            d_enabled(e), d_image(im) {}
   operator== (const ImageElement& ie) {return (d_image == ie.d_image);}
   
   double d_red;
   double d_green;
   double d_blue;
   double d_opacity;
   vrpn_bool d_enabled;
   nmb_Image *d_image;
};

typedef enum {PE_IDLE, PE_SET_REGION, PE_SET_TRANSLATE,
                 PE_DRAWMODE, PE_GRABMODE} PE_UserMode;
typedef enum {PE_POLYLINE, PE_POLYGON} PE_DrawTool;

class PatternEditor {
  public:
   PatternEditor();
   ~PatternEditor();
   void show();
   void newPosition(nmb_Image *im);
   void addImage(nmb_Image *im, double opacity = 1.0, 
                 double r=1.0, double g=1.0, double b=1.0);
   void removeImage(nmb_Image *im);
   void setImageEnable(nmb_Image *im, vrpn_bool displayEnable);
   void setImageOpacity(nmb_Image *im, double opacity);
   void setImageColor(nmb_Image *im, double r, double g, double b);
   ImageElement *getImageParameters(nmb_Image *im);
   void showSingleImage(nmb_Image *im);
   int mainWinID() {return d_mainWinID;}
   void setDrawingParameters(double lineWidth_nm, 
                             double exposure);
   void setDrawingTool(PE_DrawTool tool);
   void clearShape();

  protected:
   static int mainWinEventHandler(const ImageViewerWindowEvent &event, 
                                  void *ud);
   int handleMainWinEvent(const ImageViewerWindowEvent &event);
   static int mainWinDisplayHandler(const ImageViewerDisplayData &data, 
                                    void *ud);
   // helper for mainWinDisplayHandler:
   void drawImage(const ImageElement &ie);
   void drawPattern();

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

   // stuff for creating a pattern
   int startShape(ShapeType type);
   int addPoint(const double x_nm, const double y_nm);
   int endShape();
   void clearDrawingState();

   // stuff for manipulating a pattern
   vrpn_bool grab(const double x_nm, const double y_nm);
   void updateGrab(const double x_nm, const double y_nm);

   void setUserMode(PE_UserMode mode);
   PE_UserMode getUserMode();

   int d_mainWinWidth, d_mainWinHeight;
   int d_navWinWidth, d_navWinHeight;

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

   double d_mainWinMinXadjust_nm;
   double d_mainWinMinYadjust_nm;
   double d_mainWinMaxXadjust_nm;
   double d_mainWinMaxYadjust_nm;

   // these determine grab behavior in main window
   double d_nearDistX_pix;
   double d_nearDistY_pix;
   double d_nearDistX_nm;
   double d_nearDistY_nm;

   PE_DrawTool d_drawingTool;
   PE_UserMode d_userMode;
   double d_lineWidth_nm;
   double d_exposure_uCoulombs_per_square_cm;

   vrpn_bool d_displaySingleImage;
   nmb_Image *d_currentSingleDisplayImage;

   double d_navDragStartX_nm;
   double d_navDragStartY_nm;
   double d_navDragEndX_nm;
   double d_navDragEndY_nm;

   double d_mainDragStartX_nm;
   double d_mainDragStartY_nm;
   double d_mainDragEndX_nm;
   double d_mainDragEndY_nm;

   double d_grabX_nm;
   double d_grabY_nm;

   vrpn_bool d_shapeInProgress;
   PatternShape *d_currShape;

   PatternShape *d_grabShape;
   PatternPoint *d_grabPoint;

   list<ImageElement> d_images;
   list<PatternShape> d_pattern;

};

#endif