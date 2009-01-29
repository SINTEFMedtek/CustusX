#include "cxImageRegistrationDockWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
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
  mGuiContainer(new QWidget(this)),
  mVerticalLayout(new QVBoxLayout(mGuiContainer)),
  mImagesComboBox(new QComboBox(mGuiContainer)),
  mLandmarkTableWidget(new QTableWidget(mGuiContainer)),
  mAddPointButton(new QPushButton("Add point", mGuiContainer)),
  mRemovePointButton(new QPushButton("Remove point", mGuiContainer)),
  mRepManager(RepManager::getInstance())
{
  this->setWindowTitle("Image Registration");
  this->setWidget(mGuiContainer);

  //combobox
  mImagesComboBox->setEditable(false);
  connect(mImagesComboBox, SIGNAL(activated(const QString&)),
          this, SLOT(volumetricRepSelectedSlot(const QString&)));

  //tablewidget
  mLandmarkTableWidget->setColumnCount(2);
  QStringList headerItems(QStringList() << "Point nr."
                          << "Image space");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->
    setResizeMode(QHeaderView::ResizeToContents);

  //pushbuttons
  mAddPointButton->setDisabled(true);
  mRemovePointButton->setDisabled(true);

  //layout
  mVerticalLayout->addWidget(mImagesComboBox);
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mAddPointButton);
  mVerticalLayout->addWidget(mRemovePointButton);

  mGuiContainer->setLayout(mVerticalLayout);

  this->populateTheImageComboBox();
}
ImageRegistrationDockWidget::~ImageRegistrationDockWidget()
{}
void ImageRegistrationDockWidget::volumetricRepSelectedSlot(const QString& comboBoxText)
{
  //TODO
  //find the image
  //show volumetric rep in View3D and InriaRep in View2D (linked)
    //view3D->getVolumetricRep->setImage()???
    //view3D->getInria3DRep->setImage()???
    //view2D->getInria2DRep->setImage()??? sync with the three others (2d, 2d and 3d)
  //get the images landmarks and populate the landmark table
}
void ImageRegistrationDockWidget::populateTheImageComboBox()
{
  //TODO
  //get a list of images from the datamanager
  //add these to the combobox
  //enable the add point button if any images was found
  //enable the remove point button if the selected image has any landmarks
}
}//namespace cx
