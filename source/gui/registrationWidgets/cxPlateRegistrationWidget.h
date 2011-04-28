#ifndef CXPLATEREGISTRATIONWIDGET_H_
#define CXPLATEREGISTRATIONWIDGET_H_

#include "cxWhatsThisWidget.h"

class QPushButton;
class QLabel;

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
  Q_OBJECT
public:
  PlateRegistrationWidget(QWidget* parent);
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
}
#endif /* CXPLATEREGISTRATIONWIDGET_H_ */
