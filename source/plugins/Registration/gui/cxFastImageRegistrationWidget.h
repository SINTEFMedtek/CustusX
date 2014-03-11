#ifndef CXFASTIMAGEREGISTRATIONWIDGET_H_
#define CXFASTIMAGEREGISTRATIONWIDGET_H_

#include "cxLandmarkImageRegistrationWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_plugin_registration
 * @{
 */

/**
 * \class FastImageRegistrationWidget
 *
 * \brief Widget for performing landmark based image registration using only the
 * translation part of the matrix.
 *
 * \date 27. sep. 2010
 * \\author Janne Beate Bakeng
 */
class FastImageRegistrationWidget : public LandmarkImageRegistrationWidget
{
	Q_OBJECT
public:
  FastImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle);
 virtual ~FastImageRegistrationWidget();
 virtual QString defaultWhatsThis() const;

protected:
  virtual void performRegistration();
};

class PlateImageRegistrationWidget : public FastImageRegistrationWidget
{
	Q_OBJECT
public:
  PlateImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent);
 virtual ~PlateImageRegistrationWidget();
 virtual QString defaultWhatsThis() const;

protected slots:
 virtual void editLandmarkButtonClickedSlot();

protected:
  virtual void performRegistration();
};

/**
 * @}
 */
}//namespace cx

#endif /* CXFASTIMAGEREGISTRATIONWIDGET_H_ */
