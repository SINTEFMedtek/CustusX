#ifndef CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_
#define CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_

#include "cxBaseWidget.h"
#include "cxConnectedThresholdImageFilter.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_module_algorithm
 * @{
 */

/**
 * \class ConnectedThresholdImageFilterWidget
 *
 * \brief Widget for controlling the connected threshold image filter, a region
 * growing filter.
 *
 * \warning Class used in course, not tested nor fully implemented.
 *
 * \date Apr 26, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ConnectedThresholdImageFilterWidget : public BaseWidget
{
  Q_OBJECT

public:
  ConnectedThresholdImageFilterWidget(QWidget* parent = 0);
  virtual ~ConnectedThresholdImageFilterWidget();

  virtual QString defaultWhatsThis() const;

private slots:
  void handleFinishedSlot();
  void segmentSlot();

private:
  QWidget* createSegmentationOptionsWidget();

  QLabel* mStatusLabel;

  ConnectedThresholdImageFilter mConnectedThresholdImageFilter;
};

/**
 * @}
 */
}

#endif /* CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_ */
