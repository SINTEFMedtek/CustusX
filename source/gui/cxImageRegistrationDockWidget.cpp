#include "cxImageRegistrationDockWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include "cxRepManager.h"
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
  mVerticalLayout(new QVBoxLayout(this)),
  mImagesComboBox(new QComboBox(this)),
  mRepManager(RepManager::getInstance())
{
  this->setLayout(mVerticalLayout);
  //this->setMaximumWidth(250);

  mImagesComboBox->setEditable(false);
  connect(mImagesComboBox, SIGNAL(activated(QString&)),
          this, SLOT(volumetricRepSelectedSlot(QString)));

  mVerticalLayout->addWidget(mImagesComboBox, Qt::AlignHCenter); //TODO: does not work?
}
ImageRegistrationDockWidget::~ImageRegistrationDockWidget()
{}
void ImageRegistrationDockWidget::volumetricRepSelectedSlot(QString& comboBoxText)
{
  //TODO
  //find the image
  //show volumetric rep in View3D and InriaRep in View2D (linked)
    //view3D->getVolumetricRep->setImage()???
    //view3D->getInria3DRep->setImage()???
    //view2D->getInria2DRep->setImage()??? sync with the three others (2d, 2d and 3d)
  //get the images landmarks and populate the landmark table
}
}//namespace cx
