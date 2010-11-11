#ifndef CXPLATEREGISTRATIONWIDGET_H_
#define CXPLATEREGISTRATIONWIDGET_H_

#include "cxWhatsThisWidget.h"

class QPushButton;

namespace cx
{
/**
 * \class PlateRegistrationWidget
 *
 * \brief Widget for performing registration between a (dyna-CT) volume and a
 * predefined aurora tool with ct markers.
 *
 * \date 10. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class PlateRegistrationWidget : public WhatsThisWidget
{
public:
  PlateRegistrationWidget(QWidget* parent);
  virtual ~PlateRegistrationWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  void plateRegistrationSlot();

private:
   QPushButton* mPlateRegistrationButton;
};
}
#endif /* CXPLATEREGISTRATIONWIDGET_H_ */
