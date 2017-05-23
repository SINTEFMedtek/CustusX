/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
