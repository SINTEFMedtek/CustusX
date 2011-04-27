#ifndef CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_
#define CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "cxConnectedThresholdImageFilter.h"

namespace cx
{

/**
 * ConnectedThresholdImageFilterWidget
 *
 * \brief
 *
 * \date Apr 26, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ConnectedThresholdImageFilterWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  ConnectedThresholdImageFilterWidget(QWidget* parent);
  virtual ~ConnectedThresholdImageFilterWidget();

  virtual QString defaultWhatsThis() const;
//
//private slots:
//  void handleFinishedSlot();
//  void segmentSlot();
//
//private:
//  ConnectedThresholdImageFilterWidget();
//  QWidget* createSegmentationOptionsWidget();
//
//  QLabel* mStatusLabel;
//
//  ConnectedThresholdImageFilter mConnectedThresholdImageFilter;
};

}

#endif /* CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_ */
