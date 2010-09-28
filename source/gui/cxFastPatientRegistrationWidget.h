#ifndef CXFASTPATIENTREGISTRATIONWIDGET_H_
#define CXFASTPATIENTREGISTRATIONWIDGET_H_

#include "cxPatientRegistrationWidget.h"

namespace cx
{
/**
 * \class FastPatientRegistrationWidget
 *
 * \brief Widget for performing a fast and aprox landmark based patient registration,
 * using only the matrix translation part.
 *
 * \date 27. sep. 2010
 * \author: Janne Beate Bakeng
 */
class FastPatientRegistrationWidget : public PatientRegistrationWidget
{
public:
  FastPatientRegistrationWidget(QWidget* parent);
  virtual ~FastPatientRegistrationWidget();
  virtual QString defaultWhatsThis() const;

private:
  virtual void performRegistration();
};
}
#endif /* CXFASTPATIENTREGISTRATIONWIDGET_H_ */
