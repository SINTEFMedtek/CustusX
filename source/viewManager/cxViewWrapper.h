#ifndef CXVIEWWRAPPER_H_
#define CXVIEWWRAPPER_H_

#include <QVariant>
#include <QObject>
#include "sscView.h"
#include "sscImage.h"
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"

class QMenu;
class QActionGroup;

namespace cx
{

typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;

class SyncedValue : public QObject
{
  Q_OBJECT
public:
  SyncedValue(QVariant val=QVariant());
  static SyncedValuePtr create(QVariant val=QVariant());
  void set(QVariant val);
  QVariant get() const;
private:
  QVariant mValue;
signals:
  void changed();
};

typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;

/** Container for data shared between all members of a view group
 */
class ViewGroupData : public QObject
{
  Q_OBJECT
public:
  std::vector<ssc::DataPtr> getData() const;
  void addData(ssc::DataPtr data);
  void removeData(ssc::DataPtr data);
  void clearData();
  std::vector<ssc::ImagePtr> getImages() const;
  std::vector<ssc::MeshPtr> getMeshes() const;

signals:
  void dataAdded(QString uid);
  void dataRemoved(QString uid);
private:
  std::vector<ssc::DataPtr> mData;
};

/**
 * \class cxViewWrapper.h
 *
 * \brief
 *
 * \date 6. apr. 2010
 * \author: jbake
 */
class ViewWrapper : public QObject
{
  Q_OBJECT
public:
  virtual ~ViewWrapper() {}
  virtual void initializePlane(ssc::PLANE_TYPE plane) {}
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode) {}
  virtual ssc::View* getView() = 0;
  virtual void setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy) = 0;
  virtual void setViewGroup(ViewGroupDataPtr group);

  virtual void setZoom2D(SyncedValuePtr value) {}
  virtual void setOrientationMode(SyncedValuePtr value) {}

signals:
  void orientationChanged(ssc::ORIENTATION_TYPE type);

protected slots:
  void contextMenuSlot(const QPoint& point);
  void dataActionSlot();

  void dataAddedSlot(QString uid);
  void dataRemovedSlot(QString uid);

protected:
  virtual void imageAdded(ssc::ImagePtr image) = 0;
  virtual void meshAdded(ssc::MeshPtr mesh) = 0;
  virtual void imageRemoved(ssc::ImagePtr image) = 0;
  virtual void meshRemoved(ssc::MeshPtr mesh) = 0;

  void connectContextMenu(ssc::View* view);
  virtual void appendToContextMenu(QMenu& contextMenu) = 0;
  void addDataAction(std::string uid, QMenu* contextMenu);

  ViewGroupDataPtr mViewGroup;
};

}//namespace cx
#endif /* CXVIEWWRAPPER_H_ */
