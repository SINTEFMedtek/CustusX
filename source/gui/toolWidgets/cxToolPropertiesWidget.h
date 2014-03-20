#ifndef CXTOOLPROPERTIESWIDGET_H_
#define CXTOOLPROPERTIESWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include "cxForwardDeclarations.h"
#include "cxDoubleWidgets.h"
#include "cxStringDataAdapterXml.h"
#include "cxTransform3DWidget.h"
#include "cxPointMetric.h"


class QCheckBox;
class QGroupBox;

class UsConfigGui;

namespace cx
{
class LabeledComboBoxWidget;

/**
 * \class ToolPropertiesWidget
 * \ingroup cx_gui
 *
 * \date 2010.04.22
 * \\author Christian Askeland, SINTEF
 */
class ToolPropertiesWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolPropertiesWidget(QWidget* parent);
  virtual ~ToolPropertiesWidget();

  virtual QString defaultWhatsThis() const;

signals:

protected slots:
  void updateSlot();
  void dominantToolChangedSlot();
  void referenceToolChangedSlot();
//  void configurationChangedSlot(int index);
//  void toolsSectorConfigurationChangedSlot();///< Update the combo box when the tools configuration is changed outside the widget. Also used initially to read the tools value.
  void manualToolChanged();
  void manualToolWidgetChanged();
  void spacesChangedSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  ToolPropertiesWidget();
//  void populateUSSectorConfigBox();

  ToolPtr mReferenceTool;
  ToolPtr mActiveTool;

  QVBoxLayout* mToptopLayout;
  QGroupBox* mManualGroup;
  Transform3DWidget* mManualToolWidget;
  StringDataAdapterXmlPtr mSpaceSelector;

//  SliderGroupWidget* mToolOffsetWidget;
  QLabel* mActiveToolVisibleLabel;
  QLabel* mToolNameLabel;
  QLabel* mReferenceStatusLabel;
  QLabel* mTrackingSystemStatusLabel;
  
  LabeledComboBoxWidget* mUSSectorConfigBox;
//  QLabel* mUSSectorConfigLabel;   ///< Label for the mUSSectorConfigBox
//  QComboBox* mUSSectorConfigBox;  ///< List of US sector config parameters: depth (and width)
};

}//end namespace cx


#endif /* CXTOOLPROPERTIESWIDGET_H_ */
