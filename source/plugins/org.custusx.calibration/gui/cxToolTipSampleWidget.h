/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTOOLTIPSAMPLEWIDGET_H_
#define CXTOOLTIPSAMPLEWIDGET_H_

#include "org_custusx_calibration_Export.h"

#include "cxBaseWidget.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxForwardDeclarations.h"
//#include "cxDataInterface.h"

class QPushButton;
class QGroupBox;
class QLineEdit;

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class StringPropertySelectData> StringPropertySelectDataPtr;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;
typedef boost::shared_ptr<class StringPropertySelectCoordinateSystem> StringPropertySelectCoordinateSystemPtr;
class LabeledComboBoxWidget;

/**
 * \file
 * \addtogroup org_custusx_calibration
 * @{
 */

/**
 * Class for sampling points in a chosable coordinate system and then saving them to file.
 */
class org_custusx_calibration_EXPORT ToolTipSampleWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolTipSampleWidget(VisServicesPtr services, QWidget* parent);
  ~ToolTipSampleWidget();

private slots:
  void saveFileSlot();
  void sampleSlot();
  void coordinateSystemChanged();

private:
  CoordinateSystem getSelectedCoordinateSystem();

  VisServicesPtr mServices;
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
};


/**
 * @}
 */
}

#endif /* CXTOOLTIPSAMPLEWIDGET_H_ */
