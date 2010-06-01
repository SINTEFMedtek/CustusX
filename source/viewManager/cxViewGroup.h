#ifndef CXVIEWGROUP_H_
#define CXVIEWGROUP_H_

#include <vector>
#include <QObject>
#include <QDomDocument>
#include "sscData.h"
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxViewWrapper.h"
class QMenu;
class QPoint;

namespace cx
{

/**Helper for functions independent of state.
 * Not sure if we need this - think of better place.
 *
 */
class Navigation
{
public:
  void centerToImageCenter();
  void centerToTooltip();
  ssc::Vector3D findGlobalImageCenter();
};

/**
 * \class cxViewGroup.h
 *
 * \brief
 *
 * \date 18. mars 2010
 * \author: jbake
 */
class ViewGroup : public QObject
{
  Q_OBJECT
public:
  ViewGroup();
  virtual ~ViewGroup();

  void addViewWrapper(ViewWrapperPtr wrapper);
  std::vector<ssc::View*> getViews() const;
  ssc::View* initializeView(int index, ssc::PLANE_TYPE plane);
  virtual void addImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);
  virtual void addXml(QDomNode& dataNode); ///< store internal state info in dataNode
  virtual void parseXml(QDomNode dataNode);///< load internal state info from dataNode
  double getZoom2D();

  void setGlobal2DZoom(bool use, SyncedValuePtr val);
  void syncOrientationMode(SyncedValuePtr val);

signals:
  //void viewGroupZoom2DChanged(double newZoom);

public slots:
  //void zoom2DChangeSlot(double newZoom);
  //void orientationChangedSlot(ssc::ORIENTATION_TYPE type);
  void addImage(QString imageUid);
  void removeImage(QString imageUid);

private slots:
  void activateManualToolSlot();
  void activeImageChangeSlot();

protected:
  //zoom2d is the same for all viewwrapper2ds in a viewgroup
  void setZoom2D(double newZoom);

  std::vector<ssc::View*> mViews;

  struct SyncGroup
  {
    void activateGlobal(bool val)
    {
      if (val)
        mActive = mGlobal;
      else
        mActive = mLocal;
    }
    SyncedValuePtr mGlobal;
    SyncedValuePtr mLocal;
    SyncedValuePtr mActive;
  };
  SyncGroup mZoom2D;

  std::vector<ssc::ImagePtr> mImages;
  std::vector<ViewWrapperPtr> mElements;
};
bool isViewWrapper2D(ViewWrapperPtr wrapper);
} // namespace cx

#endif /* CXVIEWGROUP_H_ */
