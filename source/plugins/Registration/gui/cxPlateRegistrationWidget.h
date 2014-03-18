#ifndef CXPLATEREGISTRATIONWIDGET_H_
#define CXPLATEREGISTRATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"

class QPushButton;
class QLabel;

namespace cx
{
/**
 * \file
 * \addtogroup cx_plugin_registration
 * @{
 */

/**
 * \class PlateRegistrationWidget
 *
 * \brief Widget for performing registration between a (dyna-CT) volume and a
 * predefined aurora tool with ct markers.
 *
 * \date 10. nov. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class PlateRegistrationWidget : public RegistrationBaseWidget
{
  Q_OBJECT
public:
  PlateRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent);
  virtual ~PlateRegistrationWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  virtual void showEvent(QShowEvent* event);
  virtual void hideEvent(QHideEvent* event);
  void landmarkUpdatedSlot();
  void plateRegistrationSlot();
  void internalUpdate();

private:
   QPushButton* mPlateRegistrationButton;
   QLabel*      mReferenceToolInfoLabel;
};

/**
 * @}
 */
}
#endif /* CXPLATEREGISTRATIONWIDGET_H_ */
