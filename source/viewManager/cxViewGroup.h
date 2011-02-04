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
  void centerToImage(ssc::ImagePtr image);
  void centerToView(const std::vector<ssc::ImagePtr>& images);
  void centerToGlobalImageCenter();
  void centerToTooltip();

private:
  ssc::Vector3D findImageCenter(ssc::ImagePtr image);
  ssc::Vector3D findViewCenter(const std::vector<ssc::ImagePtr>& images);
  ssc::Vector3D findGlobalImageCenter();

  void centerManualTool(ssc::Vector3D& p_r);

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

  void addView(ViewWrapperPtr wrapper);
  void removeViews();
  ViewWrapperPtr getViewWrapperFromViewUid(QString viewUid);
  std::vector<ssc::View*> getViews() const;
  ViewGroupDataPtr getData() { return mViewGroupData; }
  //ssc::View* initializeView(int index, ssc::PLANE_TYPE plane);
//  virtual void addImage(ssc::ImagePtr image);
//  virtual void removeImage(ssc::ImagePtr image);
//  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);
  virtual void addXml(QDomNode& dataNode); ///< store internal state info in dataNode
  virtual void parseXml(QDomNode dataNode);///< load internal state info from dataNode
  void clearPatientData();
  double getZoom2D();
  std::vector<ssc::ImagePtr> getImages();
  ssc::SlicePlanesProxyPtr getSlicePlanesProxy() { return mSlicePlanesProxy; }
//  void addMesh(ssc::MeshPtr data);
//  void removeMesh(ssc::MeshPtr data);

  void setGlobal2DZoom(bool use, SyncedValuePtr val);
  void syncOrientationMode(SyncedValuePtr val);

public slots:
//  void addImage(QString imageUid);
//  void removeImage(QString imageUid);
//  void addMesh(QString uid);
//  void removeMesh(QString uid);

private slots:
  void activateManualToolSlot();
  void mouseClickInViewGroupSlot();

protected:
  //zoom2d is the same for all viewwrapper2ds in a viewgroup
  void setZoom2D(double newZoom);
  void setSlicePlanesProxy();

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

  ViewGroupDataPtr mViewGroupData;
//  std::vector<ssc::MeshPtr> mMeshes;
//  std::vector<ssc::ImagePtr> mImages;
  std::vector<ViewWrapperPtr> mViewWrappers;
//  ssc::REGISTRATION_STATUS mRegistrationMode;
  ssc::SlicePlanesProxyPtr mSlicePlanesProxy;
};
bool isViewWrapper2D(ViewWrapperPtr wrapper);
} // namespace cx

#endif /* CXVIEWGROUP_H_ */
