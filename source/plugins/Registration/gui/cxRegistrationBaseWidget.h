/*
 * cxRegistrationBaseWidget.h
 *
 *  Created on: Jun 16, 2011
 *      Author: christiana
 */

#ifndef CXREGISTRATIONBASEWIDGET_H_
#define CXREGISTRATIONBASEWIDGET_H_

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"

namespace cx
{
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;

class RegistrationBaseWidget : public BaseWidget
{
//  Q_OBJECT
public:
	RegistrationBaseWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle);
  virtual ~RegistrationBaseWidget() {}

protected:
  // manager
  RegistrationManagerPtr mManager;
};

}

#endif /* CXREGISTRATIONBASEWIDGET_H_ */
