/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
