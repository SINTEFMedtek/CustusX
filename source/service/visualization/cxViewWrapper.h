#ifndef CXVIEWWRAPPER_H_
#define CXVIEWWRAPPER_H_

#include <QVariant>
#include <QObject>
#include "sscView.h"
#include "sscImage.h"
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

class QMenu;
class QActionGroup;

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;


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
  ViewGroupData();
  void requestInitialize();
  std::vector<ssc::DataPtr> getData() const;
  void addData(ssc::DataPtr data);
  void removeData(ssc::DataPtr data);
  void clearData();
  std::vector<ssc::ImagePtr> getImages() const;
  std::vector<ssc::MeshPtr> getMeshes() const;

  //void setCamera3D(CameraDataPtr transform) { mCamera3D = transform; }
  CameraDataPtr getCamera3D() { return mCamera3D; }

  // view options for this group.
  struct Options
  {
    Options() : mShowLandmarks(false), mShowPointPickerProbe(false) {}

    bool mShowLandmarks;
    bool mShowPointPickerProbe;
  };

  Options getOptions() const;
  void setOptions(Options options);

private slots:
  void removeDataSlot(QString uid);
signals:
  void dataAdded(QString uid);
  void dataRemoved(QString uid);
  void initialized();
  void optionsChanged();

private:
  std::vector<ssc::DataPtr> mData;
  CameraDataPtr mCamera3D;
  Options mOptions;
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
//  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode) {}
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
//  virtual void imageAdded(ssc::ImagePtr image) = 0;
//  virtual void meshAdded(ssc::MeshPtr mesh) = 0;
//  virtual void imageRemoved(const QString& uid) = 0;
//  virtual void meshRemoved(const QString& uid) = 0;

  virtual void dataAdded(ssc::DataPtr data) = 0;
  virtual void dataRemoved(const QString& uid) = 0;

  void connectContextMenu(ssc::View* view);
  virtual void appendToContextMenu(QMenu& contextMenu) = 0;
  void addDataAction(QString uid, QMenu* contextMenu);
  QStringList getAllDataNames() const;

  ViewGroupDataPtr mViewGroup;

private:
  QString mLastDataActionUid;

};

}//namespace cx
#endif /* CXVIEWWRAPPER_H_ */
