/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLUSCONNECTWIDGET_H
#define CXPLUSCONNECTWIDGET_H

//#include <QWidget>
#include "cxTabbedWidget.h"

class QPushButton;

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class OpenIGTLinkStreamerService> OpenIGTLinkStreamerServicePtr;

class PlusConnectWidget : public BaseWidget
{
//  Q_OBJECT
public:
  PlusConnectWidget(VisServicesPtr services, QWidget *parent);
private slots:
  void connectButtonClickedSlot();

private:
  VisServicesPtr mServices;
  QPushButton* mConnectButton;
  bool mPlusRunning;
  OpenIGTLinkStreamerServicePtr getOpenIGTLinkStreamerService();
  bool startPlus();
  bool stopPlus();
};

}//namespace cx
#endif // CXPLUSCONNECTWIDGET_H
