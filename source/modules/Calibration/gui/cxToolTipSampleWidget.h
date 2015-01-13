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
#ifndef CXTOOLTIPSAMPLEWIDGET_H_
#define CXTOOLTIPSAMPLEWIDGET_H_

#include "cxPluginCalibrationExport.h"

#include "cxBaseWidget.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxForwardDeclarations.h"
#include "cxDataInterface.h"

class QPushButton;
class QGroupBox;
class QLineEdit;

namespace cx
{
typedef boost::shared_ptr<class StringPropertySelectData> StringPropertySelectDataPtr;
class LabeledComboBoxWidget;

/**
 * \file
 * \addtogroup cx_module_calibration
 * @{
 */

/**
 * Class for sampling points in a chosable coordinate system and then saving them to file.
 */
class cxPluginCalibration_EXPORT ToolTipSampleWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolTipSampleWidget(PatientModelServicePtr patientModelService, QWidget* parent);
  ~ToolTipSampleWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  void saveFileSlot();
  void sampleSlot();
  void coordinateSystemChanged();

private:
  CoordinateSystem getSelectedCoordinateSystem();

  QPushButton* mSampleButton;
  QLabel*      mSaveToFileNameLabel;
  QPushButton* mSaveFileButton;
  StringPropertySelectCoordinateSystemPtr mCoordinateSystems;
  StringPropertySelectToolPtr mTools;
  StringPropertySelectDataPtr mData;
  LabeledComboBoxWidget* mCoordinateSystemComboBox;
  LabeledComboBoxWidget* mToolComboBox;
  LabeledComboBoxWidget* mDataComboBox;
  bool mTruncateFile;
  PatientModelServicePtr mPatientModelService;
};


/**
 * @}
 */
}

#endif /* CXTOOLTIPSAMPLEWIDGET_H_ */
