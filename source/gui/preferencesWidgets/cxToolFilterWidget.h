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
