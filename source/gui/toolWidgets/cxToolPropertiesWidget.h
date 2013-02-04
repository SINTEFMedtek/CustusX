#ifndef CXTOOLPROPERTIESWIDGET_H_
#define CXTOOLPROPERTIESWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include "sscForwardDeclarations.h"
#include "sscDoubleWidgets.h"
#include "sscStringDataAdapterXml.h"
#include "cxTransform3DWidget.h"
#include "sscPointMetric.h"


class QCheckBox;
class QGroupBox;

class UsConfigGui;

namespace cx
{

/**
 * \class ToolPropertiesWidget
 * \ingroup cxGUI
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

  ssc::ToolPtr mReferenceTool;
  ssc::ToolPtr mActiveTool;

  QVBoxLayout* mToptopLayout;
  QGroupBox* mManualGroup;
  Transform3DWidget* mManualToolWidget;
  ssc::StringDataAdapterXmlPtr mSpaceSelector;
  ssc::CoordinateSystemListenerPtr mListener;

//  ssc::SliderGroupWidget* mToolOffsetWidget;
  QLabel* mActiveToolVisibleLabel;
  QLabel* mToolNameLabel;
  QLabel* mReferenceStatusLabel;
  QLabel* mTrackingSystemStatusLabel;
  
  ssc::LabeledComboBoxWidget* mUSSectorConfigBox;
//  QLabel* mUSSectorConfigLabel;   ///< Label for the mUSSectorConfigBox
//  QComboBox* mUSSectorConfigBox;  ///< List of US sector config parameters: depth (and width)
};

}//end namespace cx


#endif /* CXTOOLPROPERTIESWIDGET_H_ */
