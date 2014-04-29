/*
 * cxRegistrationBaseWidget.h
 *
 *  \date Jun 16, 2011
 *      \author christiana
 */

#ifndef CXREGISTRATIONBASEWIDGET_H_
#define CXREGISTRATIONBASEWIDGET_H_

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"

namespace cx
{
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;

/**
 * \file
 * \addtogroup cx_module_registration
 * @{
 */

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

/**
 * @}
 */
}

#endif /* CXREGISTRATIONBASEWIDGET_H_ */
