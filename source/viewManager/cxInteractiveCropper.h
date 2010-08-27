/*
 * cxInteractiveCropper.h
 *
 *  Created on: Aug 24, 2010
 *      Author: christiana
 */

#ifndef CXINTERACTIVECROPPER_H_
#define CXINTERACTIVECROPPER_H_

#include <QObject>
#include <QPointer>
#include <vtkSmartPointer.h>
#include "sscBoundingBox3D.h"
#include "sscForwardDeclarations.h"

typedef vtkSmartPointer<class vtkBoxWidget> vtkBoxWidgetPtr;
typedef vtkSmartPointer<class vtkBoxWidget2> vtkBoxWidget2Ptr;
typedef vtkSmartPointer<class vtkBoxRepresentation> vtkBoxRepresentationPtr;
typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;
typedef vtkSmartPointer<class vtkVolumeMapper> vtkVolumeMapperPtr;

namespace cx
{
typedef vtkSmartPointer<class CropBoxCallback> CropBoxCallbackPtr;
typedef vtkSmartPointer<class CropBoxEnableCallback> CropBoxEnableCallbackPtr;

/**Helper class for cropping the active volume using a
 * bounding box.
 * The bounding box can be set either explicitly or using
 * a vtkBoxWidget.
 */
class InteractiveCropper : public QObject
{
  Q_OBJECT
public:
  InteractiveCropper();
  void setView(ssc::View* view); ///< adds an interactive box widget to the view. Press 'I' to show
  ssc::DoubleBoundingBox3D getBoundingBox(); ///< get BB in reference space
  void setBoundingBox(const ssc::DoubleBoundingBox3D& bb_r); ///< set BB in reference space
  void resetBoundingBox(); ///< set bounding box back to initial size (entire volume)
  ssc::DoubleBoundingBox3D getMaxBoundingBox();
  bool getUseCropping();
  bool getShowBoxWidget() const;
signals:
  void changed();
public slots:
  void showBoxWidget(bool on);
  void useCropping(bool on);
private slots:
  void imageChangedSlot();

private:
  void boxWasShown(bool val);
  friend class CropBoxCallback;
  friend class CropBoxEnableCallback;
  void setBoxWidgetSize(const ssc::DoubleBoundingBox3D& bb_r);
  ssc::DoubleBoundingBox3D getBoxWidgetSize();
  void setCroppingRegion(ssc::DoubleBoundingBox3D bb_r);
  void updateBoxWidgetInteractor();

  vtkVolumeMapperPtr getMapper();
  QPointer<ssc::View> mView;
  //vtkBoxRepresentationPtr mBoxRep;
  //vtkBoxWidget2Ptr mBoxWidget2;
  vtkBoxWidgetPtr mBoxWidget;
  CropBoxCallbackPtr mCropBoxCallback;
  CropBoxEnableCallbackPtr mCropBoxEnableCallback;
  CropBoxEnableCallbackPtr mCropBoxDisableCallback;
  bool mShowBoxWidget;
};

typedef boost::shared_ptr<InteractiveCropper> InteractiveCropperPtr;


} // namespace cx

#endif /* CXINTERACTIVECROPPER_H_ */
