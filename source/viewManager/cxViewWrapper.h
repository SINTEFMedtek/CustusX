#ifndef CXVIEWWRAPPER_H_
#define CXVIEWWRAPPER_H_

#include <QVariant>
#include <QObject>
#include "sscView.h"
#include "sscImage.h"
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"

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
  virtual void addImage(ssc::ImagePtr image) = 0;
  virtual void addMesh(ssc::MeshPtr mesh) = 0;
  virtual std::vector<ssc::ImagePtr> getImages() const = 0;
  virtual ssc::MeshPtr getMesh() const = 0;
  virtual void removeImage(ssc::ImagePtr image) = 0;
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode) {}
  virtual ssc::View* getView() = 0;

  virtual void setZoom2D(SyncedValuePtr value) {}
  virtual void setOrientationMode(SyncedValuePtr value) {}

signals:
  void orientationChanged(ssc::ORIENTATION_TYPE type);
  void imageAdded(QString uid);
  void imageRemoved(QString uid);

protected slots:
  void contextMenuSlot(const QPoint& point);
  void imageActionSlot(); ///< triggered when an imageaction is selected in the contextmenu
  void meshActionSlot();

protected:
  void connectContextMenu(ssc::View* view);
  virtual void appendToContextMenu(QMenu& contextMenu) = 0;
};
typedef boost::shared_ptr<ViewWrapper> ViewWrapperPtr;

}//namespace cx
#endif /* CXVIEWWRAPPER_H_ */
