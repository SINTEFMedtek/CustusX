#ifndef CXFASTORIENTATIONREGISTRATIONWIDGET_H_
#define CXFASTORIENTATIONREGISTRATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"

class QPushButton;
class QCheckBox;

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/**
 * \class FastOrientationRegistrationWidget
 *
 * \brief Widget for registrating the orientation part of a fast registration
 *
 * \date 27. sep. 2010
 * \\author Janne Beate Bakeng
 */

class FastOrientationRegistrationWidget : public RegistrationBaseWidget
{
  Q_OBJECT

public:
  FastOrientationRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent);
  ~FastOrientationRegistrationWidget();
  virtual QString defaultWhatsThis() const;

protected:
  virtual void showEvent(QShowEvent* event);
  virtual void hideEvent(QHideEvent* event);

private slots:
  void setOrientationSlot();
  void dominantToolChangedSlot(const QString& uid);
  void enableToolSampleButtonSlot();
  void toolVisibleSlot(bool visible);
  void setPatientOrientationSlot();

private:
  ssc::Transform3D get_tMtm() const;
  QPushButton* mSetOrientationButton;
  QPushButton* mPatientOrientationButton;
  QCheckBox* mInvertButton;

  ssc::ToolPtr mToolToSample; ///< tool to be sampled from
};

/**
 * @}
 */
}
#endif /* CXFASTORIENTATIONREGISTRATIONWIDGET_H_ */
