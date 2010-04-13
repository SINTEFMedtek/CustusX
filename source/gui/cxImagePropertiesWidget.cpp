#include "cxImagePropertiesWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "sscImage.h"
#include "sscTypeConversions.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "sscImageLUT2D.h"
#include "cxDataInterface.h"

namespace cx
{


ImagePropertiesWidget::ImagePropertiesWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("ImagePropertiesWidget");
  this->setWindowTitle("ImageProperties");

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  mImageNameLabel = new QLabel(this);
  toptopLayout->addWidget(mImageNameLabel);

  QVBoxLayout* winlvlLayout = new QVBoxLayout;
  toptopLayout->addLayout(winlvlLayout);

  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);

  mLevelWidget = new SliderGroup(this, DoubleDataInterfacePtr(new DoubleDataInterfaceLevel), gridLayout, 0);
  mWindowWidget = new SliderGroup(this, DoubleDataInterfacePtr(new DoubleDataInterfaceWindow), gridLayout, 1);


//  toptopLayout->addWidget(mLevelWidget);
//  toptopLayout->addWidget(mWindowWidget);

//  mWindowEdit = new QLineEdit(this);
//  winlvlLayout->addWidget(mWindowEdit);
//  mLevelEdit = new QLineEdit(this);
//  winlvlLayout->addWidget(mLevelEdit);

  connect(dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(updateSlot()));
  updateSlot();

//  QGroupBox* group = new QGroupBox;
//  //group->setFlat(true);
//  toptopLayout->addWidget(group);
//  group->setTitle("Registration Time Control");
//  QHBoxLayout* topLayout = new QHBoxLayout(group);
//
//  mRewindButton = new QPushButton("Rewind");
//  mRewindButton->setToolTip("Use previous registration");
//  connect(mRewindButton, SIGNAL(clicked()), this, SLOT(rewindSlot()));
//  topLayout->addWidget(mRewindButton);
//
//  mRemoveButton = new QPushButton("Remove");
//  mRemoveButton->setToolTip("Remove the latest registration");
//  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeSlot()));
//  topLayout->addWidget(mRemoveButton);
//
//  mForwardButton = new QPushButton("Forward");
//  mForwardButton->setToolTip("Use latest registration");
//  connect(mForwardButton, SIGNAL(clicked()), this, SLOT(forwardSlot()));
//  topLayout->addWidget(mForwardButton);
}

ImagePropertiesWidget::~ImagePropertiesWidget()
{
}

void ImagePropertiesWidget::updateSlot()
{
  ssc::ImagePtr image = dataManager()->getActiveImage();
  if (image)
  {
    mImageNameLabel->setText(qstring_cast(image->getName()));
//    mWindowEdit->setText(qstring_cast(image->getLookupTable2D()->getWindow()));
//    mLevelEdit->setText(qstring_cast(image->getLookupTable2D()->getLevel()));
  }
}

void ImagePropertiesWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);

//  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
//  for (unsigned i=0; i<raw.size(); ++i)
//  {
//    connect(raw[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
//  }
//
//  updateSlot();
}

void ImagePropertiesWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);

//  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
//  for (unsigned i=0; i<raw.size(); ++i)
//  {
//    connect(raw[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
//  }
}


}//end namespace cx
