#ifndef CXTOOLFILTERWIDGET_H_
#define CXTOOLFILTERWIDGET_H_

#include <QGroupBox>
#include "sscDefinitions.h"

class QComboBox;
class QButtonGroup;

namespace cx
{
class SelectionGroupBox;
class FilteringToolListWidget;

/**
 * ToolFilterWidget
 *
 * \brief Widget for easily filtering tools based on clinical application and  tracking system.
 * \ingroup cxGUI
 *
 * Used in conjunction with ToolConfigureGroupBox, it is possible to drag tools
 * from  ToolFilterGroupBox and drop them into configurations in
 * ToolConfigureGroupBox.
 *
 * \date Mar 30, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ToolFilterGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  ToolFilterGroupBox(QWidget* parent = NULL);
  virtual ~ToolFilterGroupBox();

signals:
  void toolSelected(QString absoluteFilePath);

public slots:
  void setClinicalApplicationSlot(CLINICAL_APPLICATION clinicalApplication);
  void setTrackingSystemSlot(TRACKING_SYSTEM trackingSystem);

private slots:
  void filterSlot();

private:
  SelectionGroupBox* mApplicationGroupBox;
  SelectionGroupBox* mTrackingSystemGroupBox;

  FilteringToolListWidget*      mToolListWidget;
};
}//namespace cx

#endif /* CXTOOLFILTERWIDGET_H_ */
