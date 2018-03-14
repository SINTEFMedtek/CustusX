/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLMANAGERWIDGET_H_
#define CXTOOLMANAGERWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

class QPushButton;

namespace cx
{

/**
 * \class ToolManagerWidget
 *
 * \brief Designed as a debugging widget for the cxToolManager
 * \ingroup cx_gui
 *
 * \date May 25, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT ToolManagerWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolManagerWidget(TrackingServicePtr trackingService, QWidget* parent = NULL);
  virtual ~ToolManagerWidget();

private slots:
  void configureClickedSlot(bool);
  void deconfigureClickedSlot(bool);
  void initializeClickedSlot(bool);
  void uninitializeClickedSlot(bool);
  void startTrackingClickedSlot(bool);
  void stopTrackingClickedSlot(bool);

  void updateButtonStatusSlot(); ///< makes sure that the buttons represent the status of the toolmanager

private:
  QPushButton* mConfigureButton;
  QPushButton* mDeConfigureButton;
  QPushButton* mInitializeButton;
  QPushButton* mUnInitializeButton;
  QPushButton* mStartTrackingButton;
  QPushButton* mStopTrackingButton;
  TrackingServicePtr mTrackingService;
};

}

#endif /* CXTOOLMANAGERWIDGET_H_ */
