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
  FastImageRegistrationWidget(QWidget* parent);
 virtual ~FastImageRegistrationWidget();

protected:
  virtual void performRegistration();
};

}//namespace cx
#endif /* CXFASTIMAGEREGISTRATIONWIDGET_H_ */
