#ifndef CXVIEWMANAGER_H_
#define CXVIEWMANAGER_H_

#include <map>
#include <QObject>
#include "sscImage.h"

class QGridLayout;
class QWidget;
class QTimer;
namespace ssc
{
class View;
}
namespace cx
{
class MessageManager;
class RepManager;
class View2D;
class View3D;

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
    LAYOUT_ACSACS_2X3
  }; ///< the layout types available

  static ViewManager* getInstance(); ///< returns the only instance of this class
  static void destroyInstance();     ///< destroys the only instance of this class

  QWidget* stealCentralWidget(); ///< lets the viewmanager know where to place its layout

  View2DMap* get2DViews(); ///< returns all possible 2D views
  View3DMap* get3DViews(); ///< returns all possible 3D views
  LayoutType getCurrentLayoutType(); ///< returns the current layout type

  ssc::View* getView(const std::string& uid); ///< returns the view with the given uid, use getType to determine if it's a 2D or 3D view
  View2D* get2DView(const std::string& uid); ///< returns a 2D view with a given uid
  View3D* get3DView(const std::string& uid); ///< returns a 3D view with a given uid

signals:
  void imageDeletedFromViews(ssc::ImagePtr image);///< Emitted when an image is deleted from the views in the cxViewManager

public slots:
  void setLayoutTo_3D_1X1();    ///< sets the layout to 3D_1X1
  void setLayoutTo_3DACS_2X2(); ///< sets the layout to 3DACS_2X2
  void setLayoutTo_3DACS_1X3(); ///< sets the layout to 3DACS_1X3
  void setLayoutTo_ACSACS_2X3(); ///< sets the layout to 2X3
  void deleteImageSlot(ssc::ImagePtr image); ///< Removes deleted image

protected slots:
	void currentImageChangedSlot(ssc::ImagePtr currentImage);///< Update views when the current image is changed
  void renderAllViewsSlot(); ///< renders all views

protected:
  ViewManager(); ///< create all needed views
  virtual ~ViewManager();

  void activateLayout_3D_1X1(); ///< activate the 3D_1X1 layout
  void deactivatLayout_3D_1X1(); ///< deactivate the 3D_1X1 layout
  void activateLayout_3DACS_2X2(); ///< activate the 3DACS_2X2 layout
  void deactivateLayout_3DACS_2X2(); ///< deactivate the 3DACS_2X2 layout
  void activateLayout_3DACS_1X3(); ///< activate the 3DACS_1X3 layout
  void deactivateLayout_3DACS_1X3(); ///< deactivate the 3DACS_1X3 layout
  void activateLayout_ACSACS_2X3(); ///< activate the ACSACS_2X3 layout
  void deactivateLayout_ACSACS_2X3(); ///< deactivate the ACSACS_2X3 layout
  void removeRepFromViews(ssc::RepPtr rep); ///< Remove the rep from all views

  static ViewManager* mTheInstance; ///< the only instance of this class
  MessageManager*     mMessageManager; ///< device for sending messages to the statusbar
  RepManager* mRepManager; ///< has a pool of reps

  LayoutType      mCurrentLayoutType; ///< what LayoutType is currently active
  QGridLayout*    mLayout;            ///< the layout
  QWidget*        mMainWindowsCentralWidget;     ///< should not be used after stealCentralWidget has been called, because then MainWindow owns it!!!

  const int     MAX_3DVIEWS;      ///< constant defining the max number of 3D views available
  const int     MAX_2DVIEWS;      ///< constant defining the max number of 2D views available
  std::string   mView3DNames[2];  ///< the name of all the 3D views
  std::string   mView2DNames[9];  ///< the name of all the 2D views
  View2DMap     mView2DMap;       ///< a map of all the 3D views
  View3DMap     mView3DMap;       ///< a map of all the 2D views

  QTimer*       mRenderingTimer;  ///< timer that drives rendering

private:
  ViewManager(ViewManager const&);
  ViewManager& operator=(ViewManager const&);
};
}//namespace
#endif /* CXVIEWMANAGER_H_ */
