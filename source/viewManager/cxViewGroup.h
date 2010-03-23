#ifndef CXVIEWGROUP_H_
#define CXVIEWGROUP_H_

#include <vector>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "sscData.h"
#include "sscDefinitions.h"
class QMenu;
class QPoint;
namespace cx
{
std::string planeToString(ssc::PLANE_TYPE val);

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

  std::vector<ssc::View*> getViews() const;
  virtual void setImage(ssc::ImagePtr image) = 0;
  virtual void removeImage(ssc::ImagePtr image) = 0;

  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode) = 0;

private slots:
  void contexMenuSlot(const QPoint& point);

protected:
  std::string toString(int i) const;
  void connectContextMenu();

  std::vector<ssc::View*> mViews;
};

/**
 *
 */
class ViewGroupInria : public ViewGroup
{
  Q_OBJECT
public:
  ViewGroupInria(int startIndex, ssc::View* view1,
      ssc::View* view2, ssc::View* view3);
  virtual ~ViewGroupInria();

  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);

protected:
  int mStartIndex;
  ssc::ImagePtr mImage;
};

/**
 *
 */
class ViewGroup3D : public ViewGroup
{
  Q_OBJECT
public:
  ViewGroup3D(int startIndex, ssc::View* view);
  virtual ~ViewGroup3D();

  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);

protected:
  int mStartIndex;
  ssc::ImagePtr mImage;
  ssc::VolumetricRepPtr mVolumetricRep;
  LandmarkRepPtr mLandmarkRep;
  ssc::ProbeRepPtr mProbeRep;
};

/**
 */
class ViewGroup2D : public ViewGroup
{
public:
  ViewGroup2D(int startIndex, ssc::View* view1,
      ssc::View* view2, ssc::View* view3);
  virtual ~ViewGroup2D();

  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode) {}

protected:
  int mStartIndex;
  ssc::ImagePtr mImage;
  class ViewGroupElement
  {
  public:
    ssc::SliceProxyPtr mSliceProxy;
    ssc::SliceRepSWPtr mSliceRep;
    ssc::ToolRep2DPtr mToolRep2D;
    ssc::OrientationAnnotationRepPtr mOrientationAnnotationRep;
    ssc::DisplayTextRepPtr mPlaneTypeText;
  };
  std::vector<ViewGroupElement> mElements;


};
}
#endif /* CXVIEWGROUP_H_ */
