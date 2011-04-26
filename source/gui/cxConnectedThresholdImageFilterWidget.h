#ifndef CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_
#define CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_


#include "cxWhatsThisWidget.h"

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
public:
  ConnectedThresholdImageFilterWidget(QWidget* parent);
  virtual ~ConnectedThresholdImageFilterWidget();

  virtual QString defaultWhatsThis() const;
};

}

#endif /* CXCONNECTEDTHRESHOLDIMAGEFILTERWIDGET_H_ */
