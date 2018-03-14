/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLFILTERWIDGET_H_
#define CXTOOLFILTERWIDGET_H_

#include "cxGuiExport.h"

#include <QGroupBox>
#include "cxDefinitions.h"
#include "cxStringProperty.h"

class QComboBox;
class QButtonGroup;

namespace cx
{
class SelectionGroupBox;
class FilteringToolListWidget;
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;

/**
 * ToolFilterWidget
 *
 * \brief Widget for easily filtering tools based on clinical application and  tracking system.
 * \ingroup cx_gui
 *
 * Used in conjunction with ToolConfigureGroupBox, it is possible to drag tools
 * from  ToolFilterGroupBox and drop them into configurations in
 * ToolConfigureGroupBox.
 *
 * \date Mar 30, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT ToolFilterGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  ToolFilterGroupBox(TrackingServicePtr trackingService, QWidget* parent = NULL);
  virtual ~ToolFilterGroupBox();

  void setTrackingSystemSelector(StringPropertyBasePtr selector);

signals:
  void toolSelected(QString absoluteFilePath);

public slots:
  void setClinicalApplicationSlot(QString val);

private slots:
  void filterSlot();

private:
  void createAppSelector();

  StringPropertyPtr mAppSelector;
  StringPropertyBasePtr mTrackingSystemSelector;
  FilteringToolListWidget*      mToolListWidget;
  TrackingServicePtr mTrackingService;
};
}//namespace cx

#endif /* CXTOOLFILTERWIDGET_H_ */
