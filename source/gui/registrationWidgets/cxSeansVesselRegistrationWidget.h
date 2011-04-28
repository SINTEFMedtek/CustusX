#ifndef CXSEANSVESSELREGISTRATIONWIDGET_H_
#define CXSEANSVESSELREGISTRATIONWIDGET_H_

#include "cxWhatsThisWidget.h"

class QSpinBox;
class QPushButton;
class QLabel;

namespace cx
{
/**
 * SeansVesselRegistrationWidget.h
 *
 * \brief Widget for controlling input to
 * Seans Vessel Registration
 *
 * \date Feb 21, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class SeansVesselRegistrationWidget : public WhatsThisWidget
{
  Q_OBJECT
public:
  SeansVesselRegistrationWidget(QWidget* parent);
  virtual ~SeansVesselRegistrationWidget();
  virtual QString defaultWhatsThis() const;

public slots:
  void fixedImageSlot(QString uid);
  void movingImageSlot(QString uid);

private slots:
  void registerSlot();

private:
  QWidget* createOptionsWidget();

  QSpinBox* mLTSRatioSpinBox;
  QCheckBox* mLinearCheckBox;
  QPushButton* mRegisterButton;
  QLabel* mFixedImageLabel;
  QLabel* mMovingImageLabel;
};

}//namespace cx

#endif /* CXSEANSVESSELREGISTRATIONWIDGET_H_ */
