#ifndef CXVIEWMANAGER_H_
#define CXVIEWMANAGER_H_

#include <map>
#include <QObject>

/**
 * cxViewManager.h
 *
 * \brief
 *
 * \date Dec 9, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class QGridLayout;
class QWidget;
namespace ssc
{
class View;
}
namespace cx
{
class MessageManager;
class View2D;
class View3D;

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

  void setCentralWidget(QWidget& centralWidget); ///< lets the viewmanager know where to place its layout

  View2DMap* get2DViews(); ///< returns all possible 2D views
  View3DMap* get3DViews(); ///< returns all possible 3D views
  LayoutType getCurrentLayoutType(); ///< returns the current layout type

  ssc::View* getView(const std::string& uid); ///< returns the view with the given uid, use getType to determine if it's a 2D or 3D view
  View2D* get2DView(const std::string& uid); ///< returns a 2D view with a given uid
  View3D* get3DView(const std::string& uid); ///< returns a 3D view with a given uid

public slots:
  void setLayoutTo_3D_1X1();    ///< sets the layout to 3D_1X1
  void setLayoutTo_3DACS_2X2(); ///< sets the layout to 3DACS_2X2
  void setLayoutTo_3DACS_1X3(); ///< sets the layout to 3DACS_1X3
  void setLayoutTo_ACSACS_2X3(); ///< sets the layout to 2X3

protected:
  ViewManager(); ///< create all needed views
  virtual ~ViewManager();

  void activateLayout_3D_1X1();
  void deactivatLayout_3D_1X1();
  void activateLayout_3DACS_2X2();
  void deactivateLayout_3DACS_2X2();
  void activateLayout_3DACS_1X3();
  void deactivateLayout_3DACS_1X3();
  void activateLayout_ACSACS_2X3();
  void deactivateLayout_ACSACS_2X3();

  static ViewManager* mTheInstance; ///< the only instance of this class
  MessageManager*     mMessageManager; ///< device for sending messages to the statusbar

  LayoutType      mCurrentLayoutType; ///< what LayoutType is currently active
  QGridLayout*    mLayout;            ///< the layout
  QWidget*        mCentralWidget;     ///< the widget the layout should be used on

  const int     MAX_3DVIEWS;      ///< constant defining the max number of 3D views available
  const int     MAX_2DVIEWS;      ///< constant defining the max number of 2D views available
  std::string   mView3DNames[2];  ///< the name of all the 3D views
  std::string   mView2DNames[9];  ///< the name of all the 2D views
  View2DMap     mView2DMap;       ///< a map of all the 3D views
  View3DMap     mView3DMap;       ///< a map of all the 2D views

private:
  ViewManager(ViewManager const&);
  ViewManager& operator=(ViewManager const&);
};
}//namespace
#endif /* CXVIEWMANAGER_H_ */
