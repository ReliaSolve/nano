/*===3rdtech===
  Copyright (c) 2000 by 3rdTech, Inc.
  All Rights Reserved.

  This file may not be distributed without the permission of 
  3rdTech, Inc. 
  ===3rdtech===*/
#ifndef NMG_GRAPHICS_H
#define NMG_GRAPHICS_H


#ifndef INCLUDED_V_H
#include <v.h>
#define INCLUDED_V_H
#endif

#include <vrpn_Shared.h>
#include <nmb_Types.h>  // for PointType

class Position_list;  // from Position.h
class nmb_Dataset;  // from nmb_Dataset.h

class vrpn_Connection;  // from vrpn_Connection.h

/** \class nmg_Graphics
 * Abstract base class that defines graphics interface for a nanoManipulator
 * display.
 * Also includes all utility functions needed to drive this display across
 * a VRPN connection.  (Design note:  we ought to break those off onto
 * a separate class that we can reuse by composition instead of inheritance;
 * that will make multiple inheritance within this hierarchy much cleaner.)
 *
 * To extend nmg_Graphics with a function foo (bar), you need to write
 * all the following (and provide implementations;  everything except
 * nmg_Graphics_Implementation::foo() should be obvious from the similar
 * functions, and nmg_Graphics_Implementation::foo() is where you actually
 * add your new function):
 *
 * nmg_Graphics.h
 *  public:
 *    virtual void nmg_Graphics::foo (bar) = 0;
 *  protected:
 *    int d_foo_type;
 *    char * nmg_Graphics::encode_foo (int *, bar)
 *    int nmg_Graphics::decode_foo (const char *, bar *)
 *
 * nmg_GraphicsRemote.h and any other remote subclasses
 *  public:
 *    virtual void nmg_Graphics_Remote::foo (bar);
 *
 * nmg_GraphicsImpl.h and any other implementation subclasses
 *  public:
 *    virtual void nmg_Graphics_Implementation::foo (bar);
 *  protected:
 *    static int handle_foo (void *, vrpn_HANDLERPARAM);
 *
 * You will also need to modify the following:
 *
 *    nmg_Graphics::nmg_Graphics (vrpn_Connection *, const char *)
 *      d_foo_type = c->register_message_type("nmg Graphics foo");
 *
 *    nmg_Graphics_Implementation::nmg_Graphics_Implementation
 *      connection->register_handler(d_foo_type, handle_foo, this,
 *                                   vrpn_ANY_SENDER);
 *
 * (Design note:  I wish there was a way to have this base class abstract
 * and not require that every function be implemented in every child, since
 * that makes adding a function to this interface very heavy-weight, and
 * we want the interface to be easy to extend.  Probably the best thing
 * to do would be to make only the destructor abstract, but leave everything
 * else without a default implementation.)
 *
 * 19 Feb 2000
 *   New member functions are being added non-abstractly to simplify
 * extension (at the expense of losing compile-time detection of when
 * you haven't done the extension for all the subclasses - which most
 * people don't care about).
 *
 * 12 Oct 2001
 * Plan to have most of above functions generated automatically using
 * vrpn gen_rpc perl script. So remove " =0" from base class, so pattern
 * for adding functions is clear. 
 */


#ifdef sgi
#pragma set woff 3201
#endif

class nmg_Graphics {

  public:

    /// indicates which texture is displayed:
    enum TextureMode { NO_TEXTURES, CONTOUR, RULERGRID, ALPHA,
			           COLORMAP, VIDEO, REMOTE_DATA, VISUALIZATION };

    /// how texture coordinates are computed:
    enum TextureTransformMode {RULERGRID_COORD, VIZTEX_COORD, SURFACE_REGISTRATION_COORD, MODEL_REGISTRATION_COORD,
		REMOTE_COORD, PER_QUAD_COORD};

    /// enums for Remote Rendering
    enum RemoteColorMode { NO_COLORS, VERTEX_COLORS, 
                           SUPERSAMPLED_COLORS, CLOUDMODEL_COLORS };

    enum RemoteDepthMode { NO_DEPTH, VERTEX_DEPTH };

    enum RemoteProjectionMode { ORTHO_PROJECTION, PERSPECTIVE_PROJECTION };

    nmg_Graphics (vrpn_Connection *, const char * name);

    virtual ~nmg_Graphics (void) = 0;


    virtual void mainloop (void) = 0;

    // MANIPULATORS

    //  ALL MANIPULATORS MUST NOT BE IMPLEMENTED IN THIS CLASS!

    virtual void changeDataset( nmb_Dataset * data) {};

    virtual void resizeViewport(int width, int height) {};
      ///< changes the size of the display window (actually this only
      ///< handles changing the way the world is drawn into the display
      ///< window and the actual resizing may be done by the window system)
    virtual void getViewportSize(int *width, int * height) {};
    virtual void positionWindow(int x, int y) {};
    ///< position the main window

    virtual void getDisplayPosition (q_vec_type &ll, q_vec_type &ul,
                                                        q_vec_type &ur) {};
      ///< gets position of lower left, upper left and upper right corners
      ///< of the screen in room space

    virtual void loadRulergridImage (const char * name) {};
      ///< Specifies the name of a PPM file to display as the rulergrid.

    virtual void loadVizImage (const char * name) {};
    virtual void setViztexScale (float) {};

    virtual void causeGridReColor (void) {};
      ///< Regenerates vertex colors, but not normals or vertex coords. 
    virtual void causeGridRedraw (void) {};
      ///< Forces surface display lists to be regenerated.
    virtual void causeGridRebuild (void) {};
      ///< Forces surface display lists to be allocated and then regenerated.

    virtual void enableChartjunk (int on) {};
      ///< Controls display of chartjunk (text in screenspace).
      ///< Turn on for real use, off to capture images for publication.
    virtual void enableFilledPolygons (int on, int region = 0) {};
      ///< Controls filling of surface,  Turn off to show wireframe for demos.
    virtual void enableSmoothShading (int on) {};
      ///< Controls smooth shading of surface,  Turn off to show faceting
      ///< for demos.

    //virtual void enableUber (int on) {};
      // Controls execution of UberGraphics addins.  Turn off for
      // multiprocessing (on PCs).
    virtual void enableTrueTip (int on) {};
      ///< Controls latency compensation technique:  displaying second
      ///< tip image.

    virtual void setAlphaColor (float r, float g, float b) {};
      ///< Sets the color of the alpha-blended texture.
    virtual void setAlphaSliderRange (float low, float hi) {};
      ///< Sets the range over which the alpha-blended texture's
      ///< alpha values vary from 0 to 1.

    virtual void setColorMapDirectory (const char *) {};
      ///< Specifies the path in which to search for color maps.
    virtual void setColorMapName (const char *) {};
      ///< Specifies the name of the color map to use.
    virtual void setColorMinMax (float low, float hi) {};
    virtual void setDataColorMinMax (float low, float hi) {};
    virtual void setOpacitySliderRange (float low, float hi) {};
      ///< Specifies the range of values over which to interpolate
      ///< the opacity map.

    virtual void setTextureDirectory (const char *) {};
      ///< Specifies the path in which to search for textures.

    virtual void setContourColor (int r, int g, int b) {};
      ///< Sets the color of the intermediate lines drawn in the
      ///< contour map (The major lines, every 10th,
      ///< are hardwired to be white)
    virtual void setContourWidth (float) {};
      ///< Sets the width of the contour lines.

    virtual void setHandColor (int) {};
      ///< Color for hand icon in measure mode, Red Yellow or Blue.

    virtual void setAlphaPlaneName (const char *) {};
    virtual void setColorPlaneName (const char *) {};
    virtual void setContourPlaneName (const char *) {};
    virtual void setOpacityPlaneName (const char *) {};
    virtual void setHeightPlaneName (const char *) {};
    virtual void setMaskPlaneName (const char *) {};

    virtual void setIconScale (float) {};
      ///< Scale the 3D icons.

    virtual void enableCollabHand (vrpn_bool on) {};
    virtual void setCollabHandPos(double [3], double [4]) {};
      ///<for setting position and rotation of icon for collaborator's hand
      ///<icon

    virtual void setCollabMode(int) {};
      ///<for setting position and rotation of icon for collaborator's hand
      ///<icon

    // arguments in range [0..1]
    virtual void setSurfaceColor (const double [3]) {};
    // arguments in range [0..255]
    virtual void setSurfaceColor (const int [3]) {};

    // Colormap Texture:
    virtual void createColormapTexture( const char * ) {};
    virtual void setColormapTextureSliderRange (float, float, float, float) {};
    virtual void setColormapTextureConversionMap
                    (const char *, const char *) {};
    virtual void setColormapTextureAlpha(float) {};

    virtual void loadRawDataTexture(const int which, const char *image_name,
        const int start_x, const int start_y) {};
    virtual void updateTexture(int which, const char *image_name,
       int start_x, int start_y, 
       int end_x, int end_y) {};

// functionality moved to setTextureMode()
//    virtual void enableRegistration(int on) {};

    virtual void setTextureTransform(double *xform) {};

// functionality moved to setTextureMode()
//    virtual void enableRulergrid (int on) {};
      // Controls display of the rulergrid.

    virtual void setRulergridAngle (float) {};
      ///< Sets the angle at which the rulergrid is rotated.
    virtual void setRulergridColor (int r, int g, int b) {};
      ///< Sets the color used to draw the rulergrid.
    virtual void setRulergridOffset (float x, float y) {};
      ///< Sets the translation of the rulergrid from its default
      ///< position.  (In nm?)
    virtual void setRulergridOpacity (float alpha) {};
      ///< Sets the alpha value to use with the rulergrid.
    virtual void setRulergridScale (float) {};
      ///< Toggles whether or not to set the data with z-values 0.0 (i.e.
      ///< it hasn't been received yet) to be rendered invisible or not
    virtual void setNullDataAlphaToggle( int v ) {};
      ///< Sets the number of nanometers between rulings in the
      ///< rulergrid.
    virtual void setRulergridWidths (float x, float y) {};
      ///< Sets the width of the lines in the rulergrid.

    virtual void setSpecularity (int) {};
      ///< Sets the specularity of the surface.
    virtual void setSpecularColor (float) {};
      ///< Sets color of specular light, may be obsolete, not in interface
    virtual void setLocalViewer (vrpn_bool) {};
      ///< Sets whether the view is local (more realistic) or at infinity
      ///< (faster).
    virtual void setDiffusePercent (float) {};
    virtual void setSurfaceAlpha (float, int region = 0) {};
      ///< Sets transparency of surface
    virtual void setSphereScale (float) {};
      ///< Set the size of the sphere drawn at the stylus tip in modify mode.

    virtual void setTesselationStride (int, int region = 0) {};
      ///< Sets the stride with which the data grid is tesselated.

    virtual void setTextureMode (TextureMode,TextureTransformMode, 
				 int region = 0) {};
    ///< Determines which texture to display currently.
    // q.v. enum TextureMode { NO_TEXTURES, CONTOUR, RULERGRID, ALPHA, OPACITYMAP };

    virtual void setTextureScale (float) {};
      ///< Interval between contour lines.

    virtual void setTrueTipScale (float) {};
      ///< Size at which true tip indicator is drawn
      ///< FOR DEBUGGING ONLY

    virtual void setUserMode (int oldMode, int oldStyle, int newMode, 
			      int style, int tool) {};
      ///< Specifies the mode of interaction currently being used,
      ///< Controls which widgets are displayed.

    virtual void setLightDirection (q_vec_type &) {};
      ///< Sets the direction from which the scene is illuminated.
    virtual void resetLightDirection (void) {};
      ///< Resets the direction from which the scene is illuminated
      ///< to the default.

    // WARNING:  This code assumes there is only one polyline being
    // maintained (by the server).  If you need more, rewrite it.

    virtual int addPolylinePoint( const PointType [2] ) {return 0;};
      ///< Adds a point to the active polyline.
    //    virtual int addPolylinePoint (const float [2][3]) {};
    virtual void emptyPolyline (void) {};
      ///< Throws away all the points in the active polyline.

    virtual void setRubberLineStart (float, float) {};
    virtual void setRubberLineEnd (float, float) {};
    virtual void setRubberLineStart (const float [2]) {};
    virtual void setRubberLineEnd (const float [2]) {};

    virtual void setScanlineEndpoints(const float[3], const float [3]) {};
    virtual void displayScanlinePosition(const int enable) {};

    virtual void positionAimLine (const PointType top,
                                  const PointType bottom) {};
      ///< positionAimLine() is OBSOLETE

    virtual void positionRubberCorner (float x0, float y0,
                                       float x1, float y1, int) {};
    virtual void positionRegionBox (float, float, float, float, float, int) {};
    virtual void positionCrossSection (int, int, float, float, float, 
                                       float, float, int) {};
    virtual void hideCrossSection(int ) {};
    virtual void positionSweepLine (const PointType topL,
                                    const PointType bottomL,
				    const PointType topR,
				    const PointType bottomR) {};
    virtual int addPolySweepPoints (const PointType topL,
				    const PointType bottomL,
				    const PointType topR,
				    const PointType bottomR) { return 0;};
    virtual void setRubberSweepLineStart (const PointType, const PointType) {};
    virtual void setRubberSweepLineEnd (const PointType, const PointType) {};

    virtual void positionSphere (float x, float y, float z) {};

    virtual void setViewTransform (v_xform_type) {};
      /**< Specifies the view transform (in a vlib implementation,
       *  this is v_world.users.xforms[0]).
       */

    /// Screen capture
    virtual void createScreenImage(const char* filename,
                                   const char*  type ) {};

	/*New surface based method.*/
    virtual int createRegion() {return 0;};
    virtual void destroyRegion(int region) {};
    virtual void setRegionMaskHeight(float min_height, float max_height, 
				     int region = 0) {};
    virtual void setRegionControlPlaneName (const char *, int region = 0) {};

    //These functions are related to controlling what changes affect the
    //entire surface and what don't.
    virtual void associateAlpha(vrpn_bool associate, int region) {};
    virtual void associateFilledPolygons(vrpn_bool associate, int region) {};
    virtual void associateTextureDisplayed(vrpn_bool associate, int region) {};
    virtual void associateTextureMode(vrpn_bool associate, int region) {};
    virtual void associateTextureTransformMode(vrpn_bool associate, 
					       int region) {};
    virtual void associateStride(vrpn_bool associate, int region) {};

    // ACCESSORS
    virtual void getLightDirection (q_vec_type *) const {};
      ///< Returns the direction from which the light emanates.
    virtual int getHandColor (void) const {return 0;};
      ///< Returns the last value passed by setHandColor().
    virtual int getSpecularity (void) const { return 0;};
      ///< Returns the specularity of the surface.

    virtual const double * getSurfaceColor (void) const {return NULL;};

    TextureMode getTextureMode (void) const;


  protected:

    TextureMode d_textureMode;


    vrpn_Connection * d_connection;
    vrpn_int32 d_myId;

    vrpn_int32 d_resizeViewport_type;
    vrpn_int32 d_loadRulergridImage_type;
	vrpn_int32 d_loadVizImage_type;
    vrpn_int32 d_causeGridRedraw_type;
    vrpn_int32 d_causeGridRebuild_type;
    vrpn_int32 d_enableChartjunk_type;
    vrpn_int32 d_enableFilledPolygons_type;
    vrpn_int32 d_enableSmoothShading_type;
    //vrpn_int32 d_enableUber_type;
    vrpn_int32 d_enableTrueTip_type;
    vrpn_int32 d_setAlphaColor_type;
    vrpn_int32 d_setAlphaSliderRange_type;
    vrpn_int32 d_setColorMapDirectory_type;
    vrpn_int32 d_setColorMapName_type;
    vrpn_int32 d_setColorMinMax_type;
    vrpn_int32 d_setDataColorMinMax_type;
    vrpn_int32 d_setOpacitySliderRange_type;
    vrpn_int32 d_setTextureDirectory_type;
    vrpn_int32 d_setContourColor_type;
    vrpn_int32 d_setContourWidth_type;
    vrpn_int32 d_setHandColor_type;
    vrpn_int32 d_setAlphaPlaneName_type;
    vrpn_int32 d_setColorPlaneName_type;
    vrpn_int32 d_setContourPlaneName_type;
    vrpn_int32 d_setOpacityPlaneName_type;
	vrpn_int32 d_setHeightPlaneName_type;
	vrpn_int32 d_setMaskPlaneName_type;
    vrpn_int32 d_setIconScale_type;
    vrpn_int32 d_setSurfaceColor_type;
    vrpn_int32 d_enableRulergrid_type;
    vrpn_int32 d_setRulergridAngle_type;
    vrpn_int32 d_setRulergridColor_type;
    vrpn_int32 d_setRulergridOffset_type;
    vrpn_int32 d_setNullDataAlphaToggle_type;
    vrpn_int32 d_setRulergridOpacity_type;
    vrpn_int32 d_setRulergridScale_type;
    vrpn_int32 d_setRulergridWidths_type;
    vrpn_int32 d_setSpecularity_type;
    vrpn_int32 d_setSpecularColor_type;
    vrpn_int32 d_setDiffusePercent_type;
    vrpn_int32 d_setSurfaceAlpha_type;
    vrpn_int32 d_setSphereScale_type;
    vrpn_int32 d_setTesselationStride_type;
    vrpn_int32 d_setTextureMode_type;
    vrpn_int32 d_setTextureScale_type;
    vrpn_int32 d_setTrueTipScale_type;
    vrpn_int32 d_setUserMode_type;
    vrpn_int32 d_setLightDirection_type;
    vrpn_int32 d_resetLightDirection_type;
    vrpn_int32 d_addPolylinePoint_type;
    vrpn_int32 d_addPolySweepPoints_type;
    vrpn_int32 d_emptyPolyline_type;
    vrpn_int32 d_setRubberLineStart_type;
    vrpn_int32 d_setRubberLineEnd_type;
    vrpn_int32 d_setRubberSweepLineStart_type;
    vrpn_int32 d_setRubberSweepLineEnd_type;
    vrpn_int32 d_positionAimLine_type;
    vrpn_int32 d_positionRubberCorner_type;
    vrpn_int32 d_positionSweepLine_type;
    vrpn_int32 d_positionSphere_type;

    ///type for collaborator's hand position/orientation
    vrpn_int32 d_enableCollabHand_type;
    vrpn_int32 d_setCollabHandPos_type;
    vrpn_int32 d_setCollabMode_type;

    /// Colormap Texture Network Types:
    vrpn_int32 d_createColormapTexture_type;
    vrpn_int32 d_setColormapTextureConversionMap_type;
    vrpn_int32 d_setColormapTextureSliderRange_type;
    vrpn_int32 d_setColormapTextureAlpha_type;

    vrpn_int32 d_updateTexture_type;
    vrpn_int32 d_enableRegistration_type;
    vrpn_int32 d_setTextureTransform_type;

    vrpn_int32 d_setScanlineEndpoints_type;
    vrpn_int32 d_displayScanlinePosition_type;
    vrpn_int32 d_setViewTransform_type;

    ///Screen capture
    vrpn_int32 d_createScreenImage_type;

    vrpn_int32 d_setViztexScale_type;
    ///Surface control variables
    vrpn_int32 d_setRegionMaskHeight_type;
    vrpn_int32 d_setRegionControlPlaneName_type;
    vrpn_int32 d_createRegion_type;
    vrpn_int32 d_destroyRegion_type;
    vrpn_int32 d_associateAlpha_type;
    vrpn_int32 d_associateFilledPolygons_type;
    vrpn_int32 d_associateStride_type;
    vrpn_int32 d_associateTextureDisplayed_type;
    vrpn_int32 d_associateTextureMode_type;
    vrpn_int32 d_associateTextureTransformMode_type;

    // Each encode_ routine will allocate a new char [] to hold
    // the appropriate encoding of its arguments, and write the
    // length of that buffer into its first argument.
    // It is the caller's responsibility to delete [] this buffer!
    // (Please forgive the ugliness of that last detail...)
    // It will return NULL (and a length of 0) on error.
    // (Perhaps this should be disambiguated from the successful
    //  case of a 0-length encoding?!)

    // (There is no encode_ routine for messages whose only argument
    // is a single const char *, since they might as well send the array.
    // A NULL array should be sent as a 0-length message.
    // There is also no encode_ routine for messages with no arguments.)

    // Each decode_ routine takes a pointer to a character buffer and
    // a pointer to each of the values to extract.  It will return 0
    // on success, -1 on error.

    char * encode_resizeViewport (int * len, int, int);
    int decode_resizeViewport (const char * buf, int *, int *);

    char * encode_enableChartjunk (int * len, int);
    int decode_enableChartjunk (const char * buf, int *);
    char * encode_enableFilledPolygons (int * len, int, int);
    int decode_enableFilledPolygons (const char * buf, int *, int *);
    char * encode_enableSmoothShading (int * len, int);
    int decode_enableSmoothShading (const char * buf, int *);
    //char * encode_enableUber (int * len, int);
    //int decode_enableUber (const char * buf, int *);
    char * encode_enableTrueTip (int * len, int);
    int decode_enableTrueTip (const char * buf, int *);
    char * encode_setAlphaColor (int * len, float r, float g, float b);
    int decode_setAlphaColor (const char * buf,
                               float * r, float * g, float * b);
    char * encode_setAlphaSliderRange (int * len, float low, float hi);
    int decode_setAlphaSliderRange (const char * buf,
                                     float * low, float * hi);
    char * encode_setColorMinMax (int * len, float low, float hi);
    int decode_setColorMinMax (const char * buf, float * low, float * hi);
    char * encode_setDataColorMinMax (int * len, float low, float hi);
    int decode_setDataColorMinMax (const char * buf, float * low, float * hi);
    char * encode_setOpacitySliderRange (int * len, float low, float hi);
    int decode_setOpacitySliderRange (const char * buf,
				      float * low, float * hi);
    char * encode_setContourColor (int * len, int r, int g, int b);
    int decode_setContourColor (const char * buf, int * r, int * g, int * b);
    char * encode_setContourWidth (int * len, float);
    int decode_setContourWidth (const char * buf, float *);
    char * encode_setHandColor (int * len, int);
    int decode_setHandColor (const char * buf, int *);
    //char * encode_setAlphaPlaneName (int * len, const char *);
    //int decode_setAlphaPlaneName (const char * buf, const char *);
    //char * encode_setColorPlaneName (int * len, const char *);
    //int decode_setColorPlaneName (const char * buf, const char *);
    //char * encode_setContourPlaneName (int * len, const char *);
    //int decode_setContourPlaneName (const char * buf, const char *);
    //char * encode_setHeightPlaneName (int * len, const char *);
    //int decode_setHeightPlaneName (const char * buf, const char *);
    char * encode_setIconScale (int * len, float);
    int decode_setIconScale (const char * buf, float *);
    char * encode_setSurfaceColor (int * len, const double [3]);
    int decode_setSurfaceColor (const char * buf, double [3]);
    char * encode_setSurfaceColor (int * len, const int [3]);
    int decode_setSurfaceColor (const char * buf, int [3]);
    char * encode_enableRulergrid (int * len, int);
    int decode_enableRulergrid (const char * buf, int *);
    char * encode_setRulergridAngle (int * len, float);
    int decode_setRulergridAngle (const char * buf, float *);
    char * encode_setRulergridColor (int * len, int r, int g, int b);
    int decode_setRulergridColor (const char * buf, int * r, int * g, int * b);
    char * encode_setRulergridOffset (int * len, float x, float y);
    int decode_setRulergridOffset (const char * buf, float * x, float * y);
    char * encode_setNullDataAlphaToggle (int * len, int val);
    int decode_setNullDataAlphaToggle (const char *buf, int * val);
    char * encode_setRulergridOpacity (int * len, float alpha);
    int decode_setRulergridOpacity (const char * buf, float * alpha);
    char * encode_setRulergridScale (int * len, float);
    int decode_setRulergridScale (const char * buf, float *);
    char * encode_setRulergridWidths (int * len, float x, float y);
    int decode_setRulergridWidths (const char * buf, float * x, float * y);
    char * encode_setSpecularity (int * len, int);
    int decode_setSpecularity (const char * buf, int *);
    char * encode_setDiffusePercent (int * len, float);
    int decode_setDiffusePercent (const char * buf, float *);
    char * encode_setSurfaceAlpha (int * len, float, int);
    int decode_setSurfaceAlpha (const char * buf, float *, int *);
    char * encode_setSpecularColor (int * len, float);
    int decode_setSpecularColor (const char * buf, float *);
    char * encode_setSphereScale (int * len, float);
    int decode_setSphereScale (const char * buf, float *);
    char * encode_setTesselationStride (int * len, int, int);
    int decode_setTesselationStride (const char * buf, int *, int *);
    char * encode_setTextureMode (int * len, TextureMode, TextureTransformMode, int);
    int decode_setTextureMode (const char * buf, TextureMode *, TextureTransformMode *, int *);
    char * encode_setTextureScale (int * len, float);
    int decode_setTextureScale (const char * buf, float *);
    char * encode_setTrueTipScale (int * len, float);
    int decode_setTrueTipScale (const char * buf, float *);
    char * encode_setUserMode (int * len, int oldMode, int oldStyle, int newMode, 
			       int style, int tool);
    int decode_setUserMode (const char * buf, int * oldMode, int * oldStyle, int * newMode,
                             int * style, int * tool);
    char * encode_setLightDirection (int * len, const q_vec_type &);
    int decode_setLightDirection (const char * buf, q_vec_type &);
    //    char * encode_addPolylinePoint (int * len, const float [2][3]);
    char * encode_addPolylinePoint (int * len, const PointType[2]);
    // int decode_addPolylinePoint (const char * buf, float [2][3]);
    int decode_addPolylinePoint (const char *buf, PointType[2]);

    char * encode_addPolySweepPoints (int * len,
				      const PointType, const PointType,
				      const PointType, const PointType );
    int decode_addPolySweepPoints (const char * buf,
				   PointType, PointType,
				   PointType, PointType );

    char * encode_setRubberLineStart (int * len, const float [2]);
    int decode_setRubberLineStart (const char * buf, float [2]);
    char * encode_setRubberLineEnd (int * len, const float [2]);
    int decode_setRubberLineEnd (const char * buf, float [2]);
    
    char * encode_setRubberSweepLineStart (int * len,
					   const PointType, const PointType );
    int decode_setRubberSweepLineStart (const char * buf,
					PointType, PointType );
    char * encode_setRubberSweepLineEnd (int * len,
					 const PointType, const PointType );
    int decode_setRubberSweepLineEnd (const char * buf,
				      PointType, PointType );

    char * encode_setScanlineEndpoints (int *len, const float [6]);
    int decode_setScanlineEndpoints (const char *buf, float [6]);

    char * encode_displayScanlinePosition (int *len, const int);
    int decode_displayScanlinePosition (const char *buf, int *);

    char * encode_positionAimLine (int * len, const PointType,
                                   const PointType);
    int decode_positionAimLine (const char * buf, PointType, PointType);
    char * encode_positionRubberCorner (int * len, float, float, float, float);
    int decode_positionRubberCorner (const char * buf, float *, float *,
                                      float *, float *);
    char * encode_positionSweepLine (int * len, const PointType,
				     const PointType, const PointType,
				     const PointType);
    int decode_positionSweepLine (const char * buf, PointType, PointType,
				  PointType, PointType);
    char * encode_positionSphere (int * len, float, float, float);
    int decode_positionSphere (const char * buf, float *, float *, float *);

    // for shared hand pointers
    char * encode_enableCollabHand (int * len, vrpn_bool);
    int decode_enableCollabHand (const char *buf, vrpn_bool *);
    char * encode_setCollabHandPos (int * len, double [3], double [4]);
    int decode_setCollabHandPos (const char *buf, double [3], double [4]);
    char * encode_setCollabMode (int * len, int);
    int decode_setCollabMode (const char *buf, int *);

    // Colormap Texture Network Transmission Functions:
    char *encode_setColormapTextureSliderRange ( int *len, float, float, float, float );
    int decode_setColormapTextureSliderRange( const char *buf,float *, float *,float *, float *);

    char *encode_setColormapTextureAlpha ( int *len, float);
    int decode_setColormapTextureAlpha( const char *buf, float *);
    
    char *encode_two_char_arrays ( int *len, const char *, const char * );
    int decode_two_char_arrays ( const char *buf, char **, char **);
    
    char *encode_updateTexture(int *len, int, const char *, int, int,
         int, int);
    int decode_updateTexture(const char *buf,int *,
	char **,int *,int *,int *,int *);

    char *encode_textureTransform(int *len, double *);
    int decode_textureTransform(const char *buf, double *);

    char * encode_setViewTransform (int * len, v_xform_type);
    int decode_setViewTransform (const char * buf, v_xform_type *);

    char *encode_createScreenImage(int *len, const char *filename,
                                   const char*  type );
    int decode_createScreenImage(const char *buf, char **filename,
                                 char** type );

    char * encode_setViztexScale (int * len, float);
    int decode_setViztexScale (const char * buf, float *);

    char * encode_setRegionMaskHeight (int * len, float, float, int);
    int decode_setRegionMaskHeight (const char * buf, float *, float *, int *);
    
    char * encode_setRegionControlPlaneName (int * len, const char *, 
					     int region);
    int decode_setRegionControlPlaneName (const char * buf, char **, 
					  int * region);

    char * encode_createRegion (int * len);
    int decode_createRegion (const char * buf);

    char * encode_destroyRegion (int * len, int region);
    int decode_destroyRegion (const char * buf, int * region);

    char * encode_associate (int * len, vrpn_bool associate, int region);
    int decode_associate (const char * buf, vrpn_bool *associate, int *region);
};

#ifdef sgi
#pragma reset woff 3201
#endif

#endif  // NMG_GRAPHICS_H






