/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWGROUPPROPERTIESWIDGET_H
#define CXVIEWGROUPPROPERTIESWIDGET_H

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include <vector>
#include "cxForwardDeclarations.h"
#include "cxDoubleProperty.h"
#include "cxStringPropertySelectTool.h"

namespace cx
{
typedef boost::shared_ptr<class CameraStyleInteractor> CameraStyleInteractorPtr;

/**
 */
class cxGui_EXPORT ViewGroupPropertiesWidget : public BaseWidget
{
  Q_OBJECT

public:
	ViewGroupPropertiesWidget(int groupIndex, VisServicesPtr services, QWidget* parent);
  virtual ~ViewGroupPropertiesWidget();

signals:

protected slots:
  void onToolChanged();

protected:
  void setupUI();
  virtual void prePaintEvent();

private:
  void updateFrontend();

//  DoublePropertyBasePtr mSelector;
  int mGroupIndex;
  ViewGroupDataPtr getViewGroup();
  VisServicesPtr mServices;
  QVBoxLayout* mLayout;

  DoublePropertyPtr mCameraViewAngle;
  BoolPropertyPtr mCameraFollowTool;
  BoolPropertyPtr mFocusFollowTool;
  BoolPropertyPtr mCameraOnTooltip;

  DoublePropertyPtr mCameraTooltipOffset;
  StringPropertyBasePtr mCameraNotBehindROI;
  BoolPropertyPtr mTableLock;
  BoolPropertyPtr mUniCam;
  DoublePropertyPtr mElevation;
  StringPropertyBasePtr mAutoZoomROI;
  StringPropertyBasePtr mFocusROI;
  std::vector<PropertyPtr> mCameraStyleProperties;
  CameraStyleInteractorPtr mCameraStyleInteractor;
  StringPropertySelectToolPtr mToolSelector;

  void createCameraStyleProperties();
  void onCameraStyleChanged();
  void createCameraStyleWidget();
  void createControllingToolSelector();
};

}//end namespace cx


#endif // CXVIEWGROUPPROPERTIESWIDGET_H
