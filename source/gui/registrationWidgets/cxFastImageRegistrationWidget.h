#ifndef CXFASTIMAGEREGISTRATIONWIDGET_H_
#define CXFASTIMAGEREGISTRATIONWIDGET_H_

#include "cxImageRegistrationWidget.h"

namespace cx
{
/**
 * \class FastImageRegistrationWidget
 *
 * \brief Widget for performing landmark based image registration using only the
 * translation part of the matrix.
 *
 * \date 27. sep. 2010
 * \author: Janne Beate Bakeng
 */
class FastImageRegistrationWidget : public ImageRegistrationWidget
{
public:
  FastImageRegistrationWidget(QWidget* parent, QString objectName, QString windowTitle);
 virtual ~FastImageRegistrationWidget();
 virtual QString defaultWhatsThis() const;

protected:
  virtual void performRegistration();
};

class PlateImageRegistrationWidget : public FastImageRegistrationWidget
{
public:
  PlateImageRegistrationWidget(QWidget* parent);
 virtual ~PlateImageRegistrationWidget();
 virtual QString defaultWhatsThis() const;

protected slots:
 virtual void editLandmarkButtonClickedSlot();

protected:
  virtual void performRegistration();
};

}//namespace cx
#endif /* CXFASTIMAGEREGISTRATIONWIDGET_H_ */
