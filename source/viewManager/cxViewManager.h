#ifndef CXVIEWMANAGER_H_
#define CXVIEWMANAGER_H_

#include <map>
#include <vector>
#include <QObject>
#include "sscImage.h"
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"
class QGridLayout;
class QWidget;
class QTimer;
class QSettings;
class QTime;
#include "sscEnumConverter.h"


namespace cx
{
enum LayoutType
{
  LAYOUT_NONE=0,
  LAYOUT_3D_1X1,
  LAYOUT_3DACS_2X2,
  LAYOUT_3DACS_1X3,
  LAYOUT_ACSACS_2X3,
  LAYOUT_3DACS_2X2_SNW,
  LAYOUT_3DACS_1X3_SNW,
  LAYOUT_3DAny_1X2_SNW,
  LAYOUT_ACSACS_2X3_SNW,
  LAYOUT_Any_2x3_SNW,
  LAYOUT_COUNT
}; ///< the layout types available
} // namespace cx
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, LayoutType);


namespace cx
{

typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;

/**
 * \class ViewManager
 *
 * \brief Creates a pool of views and offers an interface to them, also handles
 * layouts on a centralwidget.
 *
 * \date Dec 9, 2008
 * \author Janne Beate Bakeng, SINTEF
 */
class ViewManager : public QObject
{
  typedef std::map<std::string, View2D*> View2DMap;
  typedef std::map<std::string, View3D*> View3DMap;
  typedef std::map<std::string, ssc::View*> ViewMap;

  Q_OBJECT
public:
  static std::string layoutText(LayoutType type);
  static LayoutType layoutTypeFromText(std::string text);
  std::vector<LayoutType> availableLayouts() const;

  static ViewManager* getInstance(); ///< returns the only instance of this class
  static void destroyInstance();     ///< destroys the only instance of this class

  QWidget* stealCentralWidget(); ///< lets the viewmanager know where to place its layout

  View2DMap* get2DViews(); ///< returns all possible 2D views
  View3DMap* get3DViews(); ///< returns all possible 3D views

  ssc::View* getView(const std::string& uid); ///< returns the view with the given uid, use getType to determine if it's a 2D or 3D view
  View2D* get2DView(const std::string& uid); ///< returns a 2D view with a given uid
  View3D* get3DView(const std::string& uid); ///< returns a 3D view with a given uid

  void setRegistrationMode(ssc::REGISTRATION_STATUS mode);

  LayoutType getActiveLayout() const; ///< returns the active layout type
  void setActiveLayout(LayoutType layout); ///< change the layout

  ssc::View* getActiveView() const; ///< returns the active view
  void setActiveView(ssc::View* view); ///< change the active view

  void setGlobal2DZoom(bool global); ///< enable/disable global 2d zooming
  bool getGlobal2DZoom(); ///< find out if global 2D zooming is enable

  //Interface for saving/loading
  void addXml(QDomNode& parentNode); ///< adds xml information about the viewmanager and its variables
  void parseXml(QDomNode viewmanagerNode);///< Use a XML node to load data. \param viewmanagerNode A XML data representation of the ViewManager

signals:
  void imageDeletedFromViews(ssc::ImagePtr image);///< Emitted when an image is deleted from the views in the cxViewManager
  void fps(int number);///< Emits number of frames per second
  void activeLayoutChanged(); ///< emitted when the active layout changes
  void activeViewChanged(); ///< emitted when the active view changes

public slots:
  void deleteImageSlot(ssc::ImagePtr image); ///< Removes deleted image
  void renderingIntervalChangedSlot(int interval); ///< Sets the rendering interval timer
  void shadingChangedSlot(bool shadingOn); ///< Turns shading on/off in the 3D scene

protected slots:
  //void currentImageChangedSlot(ssc::ImagePtr currentImage);///< Update views when the current image is changed
  void renderAllViewsSlot(); ///< renders all views
  void global2DZooming(double zoom);

protected:
  ViewManager(); ///< create all needed views
  virtual ~ViewManager();

  void syncOrientationMode(SyncedValuePtr val);

//  void centerToImageCenter();
//  void centerToTooltip();
  void deactivateCurrentLayout();
  void activateLayout(LayoutType toType);
  void activateView(ssc::View* view, int row, int col, int rowSpan=1, int colSpan=1);
  void activate2DView(int group, int index, ssc::PLANE_TYPE plane, int row, int col, int rowSpan=1, int colSpan=1);
  void activate3DView(int group, int index, int row, int col, int rowSpan=1, int colSpan=1);
  void deactivateView(ssc::View* view);

  void activateLayout_3D_1X1(); ///< activate the 3D_1X1 layout
  void activateLayout_3DACS_2X2(); ///< activate the 3DACS_2X2 layout
  void activateLayout_3DACS_1X3(); ///< activate the 3DACS_1X3 layout
  void activateLayout_ACSACS_2X3(); ///< activate the ACSACS_2X3 layout
  void activateLayout_3DAny_1X2_SNW();
  void activateLayout_3DACS_2X2_SNW(); ///< activate the 3DACS_2X2 layout
  void activateLayout_3DACS_1X3_SNW(); ///< activate the 3DACS_1X3 layout
  void activateLayout_ACSACS_2X3_SNW();
  void activateLayout_Any_2X3_SNW();
  //void removeRepFromViews(ssc::RepPtr rep); ///< Remove the rep from all views

  static ViewManager* mTheInstance; ///< the only instance of this class

  LayoutType      mActiveLayout;              ///< the active layout (type)
  QGridLayout*    mLayout;                    ///< the layout
  QWidget*        mMainWindowsCentralWidget;  ///< should not be used after stealCentralWidget has been called, because then MainWindow owns it!!!

  ssc::View*    mActiveView;            ///< the active view
  const int     MAX_3DVIEWS;            ///< constant defining the max number of 3D views available
  const int     MAX_2DVIEWS;            ///< constant defining the max number of 2D views available
  std::vector<std::string> mView3DNames;///< the name of all the 3D views
  std::vector<std::string> mView2DNames;///< the name of all the 2D views
  View2DMap     mView2DMap;             ///< a map of all the 3D views
  View3DMap     mView3DMap;             ///< a map of all the 2D views
  ViewMap       mViewMap;               ///< a map of all the views

  QTimer*       mRenderingTimer;  ///< timer that drives rendering
  
  bool mShadingOn; ///< Use shading for rendering?
  QSettings* mSettings; ///< Object for storing all program/user specific settings
  QTime* mRenderingTime; ///< Time object used to calculate number of renderings per second (FPS)
  int mNumberOfRenderings; ///< Variable used to calculate FPS

  std::vector<ViewGroupPtr> mViewGroups;

  bool mGlobal2DZoom; ///< controlling whether or not 2D zooming is global
  bool mGlobalObliqueOrientation; ///< controlling whether or not all 2d views should be oblique or orthogonal
  SyncedValuePtr mGlobalZoom2DVal;

private:
  ViewManager(ViewManager const&);
  ViewManager& operator=(ViewManager const&);
};
/**Shortcut for accessing the viewmanager instance.
 */
ViewManager* viewManager();
}//namespace
#endif /* CXVIEWMANAGER_H_ */
