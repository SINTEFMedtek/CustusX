#ifndef CXSEANSVESSELREGISTRATIONWIDGET_H_
#define CXSEANSVESSELREGISTRATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "sscStringDataAdapter.h"

class QSpinBox;
class QPushButton;
class QLabel;

namespace cx
{
typedef boost::shared_ptr<class SeansVesselRegistrationDebugger> SeansVesselRegistrationDebuggerPtr;

/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/**
 * SeansVesselRegistrationWidget.h
 *
 * \brief Widget for controlling input to
 * Seans Vessel Registration
 *
 * \date Feb 21, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class SeansVesselRegistrationWidget : public RegistrationBaseWidget
{
  Q_OBJECT
public:
  SeansVesselRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent);
  virtual ~SeansVesselRegistrationWidget();
  virtual QString defaultWhatsThis() const;

public slots:
//  void fixedImageSlot(QString uid);
//  void movingImageSlot(QString uid);

private slots:
  void registerSlot();
  void debugInit();
  void debugRunOneStep();
  void debugClear();

private:
  QWidget* createOptionsWidget();
  SeansVesselRegistrationDebuggerPtr mDebugger;

  QSpinBox* mLTSRatioSpinBox;
  QCheckBox* mLinearCheckBox;
  QPushButton* mRegisterButton;
//  QLabel* mFixedImageLabel;
//  QLabel* mMovingImageLabel;

  ssc::StringDataAdapterPtr mFixedImage;
  ssc::StringDataAdapterPtr mMovingImage;
};

/**
 * @}
 */
}//namespace cx

#endif /* CXSEANSVESSELREGISTRATIONWIDGET_H_ */
