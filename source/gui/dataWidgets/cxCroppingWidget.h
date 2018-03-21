/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCROPPINGWIDGET_H_
#define CXCROPPINGWIDGET_H_

#include "cxGuiExport.h"

#include "cxForwardDeclarations.h"
#include "cxStringPropertyBase.h"
#include "cxBaseWidget.h"

#include "cxBoundingBoxWidget.h"

class QCheckBox;
class QLabel;

namespace cx
{

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/**
 * \class CroppingWidget
 *
 * \date  Aug 20, 2010
 * \author Christian Askeland, SINTEF
 */
class cxGui_EXPORT CroppingWidget : public BaseWidget
{
  Q_OBJECT

public:
  CroppingWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent);

private:
  void hideOldAndShowNewVolume(ImagePtr oldImage, ImagePtr newImage);///< This only works with view group 0

  InteractiveCropperPtr mInteractiveCropper;
  BoundingBoxWidget* mBBWidget;
  QCheckBox* mUseCropperCheckBox;
  QCheckBox* mShowBoxCheckBox;
  QLabel* mBoundingBoxDimensions;
  PatientModelServicePtr mPatientModelService;
  ViewServicePtr mViewService;

private slots:
  void setupUI();
  void boxValuesChanged();
  void cropperChangedSlot();
  ImagePtr cropClipButtonClickedSlot();///< Crete a new image based on the images crop and clip values
};

/**
 * @}
 */
}//namespace cx


#endif /* CXCROPPINGWIDGET_H_ */
