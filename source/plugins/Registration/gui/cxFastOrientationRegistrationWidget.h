#ifndef CXFASTORIENTATIONREGISTRATIONWIDGET_H_
#define CXFASTORIENTATIONREGISTRATIONWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "sscForwardDeclarations.h"

class QPushButton;
class QCheckBox;

namespace cx
{
/**
 * \class FastOrientationRegistrationWidget
 *
 * \brief Widget for registrating the orientation part of a fast registration
 *
 * \date 27. sep. 2010
 * \author: Janne Beate Bakeng
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

private:
  QPushButton* mSetOrientationButton;
  QCheckBox* mInvertButton;

  ssc::ToolPtr mToolToSample; ///< tool to be sampled from
};
}
#endif /* CXFASTORIENTATIONREGISTRATIONWIDGET_H_ */
