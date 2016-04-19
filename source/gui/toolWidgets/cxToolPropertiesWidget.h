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

#ifndef CXTOOLPROPERTIESWIDGET_H_
#define CXTOOLPROPERTIESWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include <vector>
#include <QTextEdit>
#include "cxForwardDeclarations.h"
#include "cxDoubleWidgets.h"
#include "cxSpaceProperty.h"
#include "cxStringProperty.h"
#include "cxTransform3DWidget.h"
#include "cxPointMetric.h"
#include "cxTrackingService.h"

class QCheckBox;
class QGroupBox;

class UsConfigGui;

namespace cx
{
typedef boost::shared_ptr<class DoublePropertyToolOffset> DoublePropertyToolOffsetPtr;
class LabeledComboBoxWidget;


class cxGui_EXPORT ActiveToolPropertiesWidget : public BaseWidget
{
  Q_OBJECT

public:
  ActiveToolPropertiesWidget(TrackingServicePtr trackingService, SpaceProviderPtr spaceProvider, QWidget* parent);
  virtual ~ActiveToolPropertiesWidget();
};

/**
 * \class ToolPropertiesWidget
 * \ingroup cx_gui
 *
 * \date 2010.04.22
 * \\author Christian Askeland, SINTEF
 */
class cxGui_EXPORT ToolPropertiesWidget : public BaseWidget
{
  Q_OBJECT

public:
	ToolPropertiesWidget(StringPropertyBasePtr tool, TrackingServicePtr trackingService, SpaceProviderPtr spaceProvider, QWidget* parent);
  virtual ~ToolPropertiesWidget();

signals:

protected slots:
  void activeToolChangedSlot();
  void manualToolWidgetChanged();
  void spacesChangedSlot();

protected:
  void setupUI();
  virtual void prePaintEvent();

private:
  ToolPropertiesWidget();
  void toolPositionChanged();
  void updateFrontend();
  void reconnectTools();
  StringPropertyBasePtr mSelector;
  TrackingServicePtr mTrackingService;
  SpaceProviderPtr mSpaceProvider;
  ToolPtr mTool;
  TrackingService::ToolMap mTools;

  QVBoxLayout* mToptopLayout;
  QGroupBox* mManualGroup;
  Transform3DWidget* mManualToolWidget;
  SpacePropertyPtr mSpaceSelector;
  DoublePropertyToolOffsetPtr mToolOffset;

  QLabel* mActiveToolVisibleLabel;
  QLabel* mToolNameLabel;
  QLabel* mReferenceStatusLabel;
  QLabel* mTrackingSystemStatusLabel;
  
  LabeledComboBoxWidget* mUSSectorConfigBox;

  QTextEdit* mMetadataLabel;
  void updateBrowser();
  QString createDescriptionForTool(ToolPtr current);
};

}//end namespace cx


#endif /* CXTOOLPROPERTIESWIDGET_H_ */
