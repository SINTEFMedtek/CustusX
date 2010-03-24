#ifndef CXVIEWMANAGER_H_
#define CXVIEWMANAGER_H_

#include <map>
#include <vector>
#include <QObject>
#include "sscImage.h"
#include "cxForwardDeclarations.h"
class QGridLayout;
class QWidget;
class QTimer;
class QSettings;
class QTime;

namespace cx
{

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

  Q_OBJECT
public:
  enum LayoutType
  {
    LAYOUT_NONE,
    LAYOUT_3D_1X1,
    LAYOUT_3DACS_2X2,
    LAYOUT_3DACS_1X3,
    LAYOUT_ACSACS_2X3,
    LAYOUT_3DACS_2X2_SNW
  }; ///< the layout types available
  static std::string layoutText(LayoutType type);

  static ViewManager* getInstance(); ///< returns the only instance of this class
  static void destroyInstance();     ///< destroys the only instance of this class

  QWidget* stealCentralWidget(); ///< lets the viewmanager know where to place its layout

  View2DMap* get2DViews(); ///< returns all possible 2D views
  View3DMap* get3DViews(); ///< returns all possible 3D views
  LayoutType getCurrentLayoutType(); ///< returns the current layout type

  ssc::View* getView(const std::string& uid); ///< returns the view with the given uid, use getType to determine if it's a 2D or 3D view
  View2D* get2DView(const std::string& uid); ///< returns a 2D view with a given uid
  View3D* get3DView(const std::string& uid); ///< returns a 3D view with a given uid

  void setRegistrationMode(ssc::REGISTRATION_STATUS mode);

signals:
  void imageDeletedFromViews(ssc::ImagePtr image);///< Emitted when an image is deleted from the views in the cxViewManager
  void fps(int number);///< Emits number of frames per second

public slots:
  void setLayoutFromQActionSlot(); ///< set the layout using data from a QAction
  void deleteImageSlot(ssc::ImagePtr image); ///< Removes deleted image
  void renderingIntervalChangedSlot(int interval); ///< Sets the rendering interval timer
  void shadingChangedSlot(bool shadingOn); ///< Turns shading on/off in the 3D scene

protected slots:
	void currentImageChangedSlot(ssc::ImagePtr currentImage);///< Update views when the current image is changed
  void renderAllViewsSlot(); ///< renders all views

protected:
  ViewManager(); ///< create all needed views
  virtual ~ViewManager();

  void deactivateCurrentLayout();
  void changeLayout(LayoutType toType);
  void activateLayout(LayoutType toType);
  void activateView(ssc::View* view, int row, int col, int rowSpan=1, int colSpan=1);
  void deactivateView(ssc::View* view);

  void activateLayout_3D_1X1(); ///< activate the 3D_1X1 layout
  void deactivatLayout_3D_1X1(); ///< deactivate the 3D_1X1 layout
  void activateLayout_3DACS_2X2(); ///< activate the 3DACS_2X2 layout
  void deactivateLayout_3DACS_2X2(); ///< deactivate the 3DACS_2X2 layout
  void activateLayout_3DACS_1X3(); ///< activate the 3DACS_1X3 layout
  void deactivateLayout_3DACS_1X3(); ///< deactivate the 3DACS_1X3 layout
  void activateLayout_ACSACS_2X3(); ///< activate the ACSACS_2X3 layout
  void deactivateLayout_ACSACS_2X3(); ///< deactivate the ACSACS_2X3 layout

  void activateLayout_3DACS_2X2_SNW(); ///< activate the 3DACS_2X2 layout
  void deactivateLayout_3DACS_2X2_SNW(); ///< deactivate the 3DACS_2X2 layout
  //void removeRepFromViews(ssc::RepPtr rep); ///< Remove the rep from all views

  static ViewManager* mTheInstance; ///< the only instance of this class

  LayoutType      mCurrentLayoutType; ///< what LayoutType is currently active
  QGridLayout*    mLayout;            ///< the layout
  QWidget*        mMainWindowsCentralWidget;     ///< should not be used after stealCentralWidget has been called, because then MainWindow owns it!!!

  const int     MAX_3DVIEWS;      ///< constant defining the max number of 3D views available
  const int     MAX_2DVIEWS;      ///< constant defining the max number of 2D views available
  std::vector<std::string> mView3DNames;  ///< the name of all the 3D views
  std::vector<std::string> mView2DNames;  ///< the name of all the 2D views
  View2DMap     mView2DMap;       ///< a map of all the 3D views
  View3DMap     mView3DMap;       ///< a map of all the 2D views

  QTimer*       mRenderingTimer;  ///< timer that drives rendering
  
  bool mShadingOn; ///< Use shading for rendering?
  QSettings* mSettings; ///< Object for storing all program/user specific settings
  QTime* mRenderingTime; ///< Time object used to calculate number of renderings per second (FPS)
  int mNumberOfRenderings; ///< Variable used to calculate FPS

  typedef std::map<QString, ViewGroupPtr> ViewGroupMap;
  ViewGroupMap mViewGroups;
  ViewGroup3DPtr mViewGroup3D1;
  ViewGroup3DPtr mViewGroup3D2;
  ViewGroupInriaPtr mViewGroupInria1;
  ViewGroupInriaPtr mViewGroupInria2;
  ViewGroup2DPtr mViewGroup2D1;
  ViewGroup2DPtr mViewGroup2D2;

private:
  ViewManager(ViewManager const&);
  ViewManager& operator=(ViewManager const&);
};


}//namespace
#endif /* CXVIEWMANAGER_H_ */
