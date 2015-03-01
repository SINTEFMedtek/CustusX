#ifndef CXIMAGE2PLATEREGISTRATIONWIDGET_H
#define CXIMAGE2PLATEREGISTRATIONWIDGET_H

#include "cxTabbedWidget.h"

namespace cx
{

/**
 * \ingroup org_custusx_registration_method_plate
 */
class Image2PlateRegistrationWidget : public TabbedWidget
{
public:
  Image2PlateRegistrationWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~Image2PlateRegistrationWidget(){}
  virtual QString defaultWhatsThis() const;
};

} //cx

#endif // CXIMAGE2PLATEREGISTRATIONWIDGET_H
