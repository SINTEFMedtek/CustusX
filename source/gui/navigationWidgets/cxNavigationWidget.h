#ifndef CXNAVIGATIONWIDGET_H_
#define CXNAVIGATIONWIDGET_H_

#include "cxBaseWidget.h"

class QGroupBox;
class QRadioButton;
class QVBoxLayout;
class QLabel;
class QSlider;

#include "cxCameraStyle.h"

namespace cx
{

/**
 * \class NavigationWidget
 *
 * \brief Widget for controlling camera follow style.
 * \ingroup cx_gui
 *
 * \date 22. feb. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class NavigationWidget : public BaseWidget
{
  Q_OBJECT

public:
  NavigationWidget(QWidget* parent);
  ~NavigationWidget();

  virtual QString defaultWhatsThis() const;

protected:
  QVBoxLayout*  mVerticalLayout; ///< vertical layout is used

  //camera settings
  QGroupBox*    mCameraGroupBox; ///< widget to put all camera specific settings
  QVBoxLayout*  mCameraGroupLayout; ///< layout for the cameragroupbox
};
}
#endif /* CXNAVIGATIONWIDGET_H_ */
