#ifndef CXIMAGEREGISTRATIONDOCKWIDGET_H_
#define CXIMAGEREGISTRATIONDOCKWIDGET_H_

#include <QDockWidget>
#include "cxVolumetricRep.h"

/**
 * cxImageRegistrationDockWidget.h
 *
 * \brief This class represents an interface to a dockwidget used for image to
 * image registration.
 *
 * \date Jan 27, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

class QVBoxLayout;

namespace cx
{
class ImageRegistrationDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  ImageRegistrationDockWidget();
  ~ImageRegistrationDockWidget();

  void setVolumetricRep(VolumetricRepPtr volumetricRep);

protected:
  VolumetricRepPtr mVolumetricRep;

  QVBoxLayout* mVerticalLayout;
};
}//namespace cx

#endif /* CXIMAGEREGISTRATIONDOCKWIDGET_H_ */
