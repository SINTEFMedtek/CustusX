#ifndef CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_
#define CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_

//<1>
//#include "cxWhatsThisWidget.h"
//<7>
//#include "cxConnectedThresholdImageFilter.h"

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
class ConnectedThresholdImageFilterWidget //<1>: public WhatsThisWidget
{
//<1>
//  Q_OBJECT

public:
  ConnectedThresholdImageFilterWidget(QWidget* parent = 0);
  virtual ~ConnectedThresholdImageFilterWidget();

//<1>
//  virtual QString defaultWhatsThis() const;

//<7>
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
