#include "cxImageRegistrationDockWidget.h"

#include <QVBoxLayout>

/**
 * cxImageRegistrationDockWidget.cpp
 *
 * \brief
 *
 * \date Jan 27, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
ImageRegistrationDockWidget::ImageRegistrationDockWidget() :
  mVerticalLayout(new QVBoxLayout(this))
{
  this->setLayout(mVerticalLayout);
  this->setMaximumWidth(250);
}
ImageRegistrationDockWidget::~ImageRegistrationDockWidget()
{}
void ImageRegistrationDockWidget::setVolumetricRep(VolumetricRepPtr volumetricRep)
{
  mVolumetricRep = volumetricRep;
}
}//namespace cx
