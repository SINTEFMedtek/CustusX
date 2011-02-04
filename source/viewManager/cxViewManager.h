#ifndef CXVIEWMANAGER_H_
#define CXVIEWMANAGER_H_

#include <map>
#include <vector>
#include <QObject>
#include "sscImage.h"
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"
//#include "sscEnumConverter.h"
#include "cxLayoutData.h"
#include "cxViewCache.h"
#include "cxTreeModelItem.h"
#include "cxInteractiveClipper.h"
//#include "cxInteractiveCropper.h"

class QActionGroup;
class QAction;
class QGridLayout;
class QWidget;
class QTimer;
class QSettings;
class QTime;
typedef boost::shared_ptr<class QSettings> QSettingsPtr;


namespace cx
{

class ViewWrapper;
typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;
typedef boost::shared_ptr<class InteractiveCropper> InteractiveCropperPtr;
typedef boost::shared_ptr<class RenderTimer> RenderTimerPtr;

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
  typedef std::map<QString, View2D*> View2DMap;
  typedef std::map<QString, View3D*> View3DMap;
  typedef std::map<QString, ssc::View*> ViewMap;

  Q_OBJECT
public:

  View3D* get3DView(int group=0, int index=0);
  void fillModelTree(TreeItemPtr root);

  LayoutData getLayoutData(const QString uid) const; ///< get data for given layout
  std::vector<QString> getAvailableLayouts() const; ///< get uids of all defined layouts
  void setLayoutData(const LayoutData& data); ///< add or edit a layout
  QString generateLayoutUid() const; ///< return an uid not used in present layouts.
  void deleteLayoutData(const QString uid);
  QActionGroup* createLayoutActionGroup();
  QActionGroup* createInteractorStyleActionGroup();
  bool isCustomLayout(const QString& uid) const;

  static ViewManager* getInstance(); ///< returns the only instance of this class
  static void destroyInstance();     ///< destroys the only instance of this class

  QWidget* stealCentralWidget(); ///< lets the viewmanager know where to place its layout

  void setRegistrationMode(ssc::REGISTRATION_STATUS mode);

  QString getActiveLayout() const; ///< returns the active layout
  void setActiveLayout(const QString& uid); ///< change the layout

  ViewWrapperPtr getActiveView() const; ///< returns the active view
  //void setActiveView(ViewWrapperPtr view); ///< change the active view
  void setActiveView(QString viewUid); ///< convenient function for setting the active view

  void setGlobal2DZoom(bool global); ///< enable/disable global 2d zooming
  bool getGlobal2DZoom(); ///< find out if global 2D zooming is enable

  //Interface for saving/loading
  void addXml(QDomNode& parentNode); ///< adds xml information about the viewmanager and its variables
  void parseXml(QDomNode viewmanagerNode);///< Use a XML node to load data. \param viewmanagerNode A XML data representation of the ViewManager
  void clear();

  InteractiveClipperPtr getClipper();
  InteractiveCropperPtr getCropper();

  bool getSmartRender() const;
  void setSmartRender(bool on);
  RenderTimerPtr getRenderTimer() { return mRenderTimer; }


signals:
  void imageDeletedFromViews(ssc::ImagePtr image);///< Emitted when an image is deleted from the views in the cxViewManager
  void fps(int number);///< Emits number of frames per second
  void activeLayoutChanged(); ///< emitted when the active layout changes
  void activeViewChanged(); ///< emitted when the active view changes

public slots:
  //void deleteImageSlot(ssc::ImagePtr image); ///< Removes deleted image
  void renderingIntervalChangedSlot(int interval); ///< Sets the rendering interval timer
//  void shadingChangedSlot(bool shadingOn); ///< Turns shading on/off in the 3D scene

protected slots:
  void renderAllViewsSlot(); ///< renders all views
  void setLayoutActionSlot();
  void setInteractionStyleActionSlot();

protected:
  ViewManager(); ///< create all needed views
  virtual ~ViewManager();

  View2DMap* get2DViews(); ///< returns all possible 2D views
  View3DMap* get3DViews(); ///< returns all possible 3D views

  ssc::View* getView(const QString& uid); ///< returns the view with the given uid, use getType to determine if it's a 2D or 3D view
  View2D* get2DView(const QString& uid); ///< returns a 2D view with a given uid
  View3D* get3DView(const QString& uid); ///< returns a 3D view with a given uid

  void syncOrientationMode(SyncedValuePtr val);
  void setStretchFactors(LayoutRegion region, int stretchFactor);

  void deactivateCurrentLayout();
  //void activateLayout(const QString& toType);
  void activateView(ViewWrapperPtr wrapper, int group, LayoutRegion region);
  void activate2DView(int group, ssc::PLANE_TYPE plane, LayoutRegion region);
  void activate3DView(int group, LayoutRegion region);
  void activateRTStreamView(int group, LayoutRegion region);
//  void activate2DView(int group, int index, ssc::PLANE_TYPE plane, LayoutRegion region);
//  void activate3DView(int group, int index, LayoutRegion region);
  //void deactivateView(ssc::View* view);
  void addDefaultLayouts();
  unsigned findLayoutData(const QString uid) const;
  void addDefaultLayout(LayoutData data);
  QAction* addLayoutAction(QString layout, QActionGroup* group);

  void addInteractorStyleAction(QString caption, QActionGroup* group, QString className, QIcon icon, QString helptext);
  void loadGlobalSettings();
  void saveGlobalSettings();

  static ViewManager* mTheInstance; ///< the only instance of this class

  typedef std::vector<LayoutData> LayoutDataVector;
  LayoutDataVector mLayouts;
  std::vector<QString> mDefaultLayouts;

  QString         mActiveLayout;              ///< the active layout (type)
  QGridLayout*    mLayout;                    ///< the layout
  QWidget*        mMainWindowsCentralWidget;  ///< should not be used after stealCentralWidget has been called, because then MainWindow owns it!!!

  QString mActiveView;                    ///< the active view
//  ViewWrapperPtr   mActiveView;            ///< the active view
//  const int     MAX_3DVIEWS;            ///< constant defining the max number of 3D views available
//  const int     MAX_2DVIEWS;            ///< constant defining the max number of 2D views available
//  std::vector<QString> mView3DNames;///< the name of all the 3D views
//  std::vector<QString> mView2DNames;///< the name of all the 2D views
//  View2DMap     mView2DMap;             ///< a map of all the 3D views
//  View3DMap     mView3DMap;             ///< a map of all the 2D views
  ViewMap       mViewMap;               ///< a map of all the views

  QTimer*       mRenderingTimer;  ///< timer that drives rendering
  QDateTime mLastFullRender;

//  bool mShadingOn; ///< Use shading for rendering?
  QSettingsPtr mSettings; ///< Object for storing all program/user specific settings
  RenderTimerPtr mRenderTimer;
//  QTime* mRenderingTime; ///< Time object used to calculate number of renderings per second (FPS)
//  int mNumberOfRenderings; ///< Variable used to calculate FPS

  std::vector<ViewGroupPtr> mViewGroups;

  bool mGlobal2DZoom; ///< controlling whether or not 2D zooming is global
  bool mGlobalObliqueOrientation; ///< controlling whether or not all 2d views should be oblique or orthogonal
  SyncedValuePtr mGlobalZoom2DVal;
  bool mSmartRender; ///< use ssc::View::render()

  ViewCache<View2D> mViewCache2D;
  ViewCache<View3D> mViewCache3D;
  ViewCache<ssc::View> mViewCacheRT;
  InteractiveClipperPtr mInteractiveClipper;
  InteractiveCropperPtr mInteractiveCropper;

private:
  ViewManager(ViewManager const&);
  ViewManager& operator=(ViewManager const&);
};
/**Shortcut for accessing the viewmanager instance.
 */
ViewManager* viewManager();
}//namespace
#endif /* CXVIEWMANAGER_H_ */
