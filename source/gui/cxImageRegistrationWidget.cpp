#include "cxImageRegistrationWidget.h"

#include <sstream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QSlider>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxDataManager.h"
#include "cxView3D.h"
#include "cxView2D.h"
//#include "cxInriaRep2D.h"
#include "cxRegistrationHistoryWidget.h"
#include "sscTypeConversions.h"

namespace cx
{
ImageRegistrationWidget::ImageRegistrationWidget(QWidget* parent) :
  QWidget(parent),
  mVerticalLayout(new QVBoxLayout(this)),
  mLandmarkTableWidget(new QTableWidget(this)),
  mAddLandmarkButton(new QPushButton("Add landmark", this)),
  mEditLandmarkButton(new QPushButton("Resample landmark", this)),
  mRemoveLandmarkButton(new QPushButton("Remove landmark", this)),
  mThresholdLabel(new QLabel("Probing treshold:", this)),
  mThresholdSlider(new QSlider(Qt::Horizontal, this))
  //mCurrentRow(-1),
  //mCurrentColumn(-1)
{
  //widget
  this->setWindowTitle("Image Registration");

  //pushbuttons
  mAddLandmarkButton->setDisabled(true);
  connect(mAddLandmarkButton, SIGNAL(clicked()),
          this, SLOT(addLandmarkButtonClickedSlot()));
  mEditLandmarkButton->setDisabled(true);
  connect(mEditLandmarkButton, SIGNAL(clicked()),
          this, SLOT(editLandmarkButtonClickedSlot()));
  mRemoveLandmarkButton->setDisabled(true);
  connect(mRemoveLandmarkButton, SIGNAL(clicked()),
          this, SLOT(removeLandmarkButtonClickedSlot()));

  //slider
  connect(mThresholdSlider, SIGNAL(valueChanged(int)),
          this, SLOT(thresholdChangedSlot(int)));

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)),
          this, SLOT(landmarkSelectedSlot(int, int)));
  connect(mLandmarkTableWidget, SIGNAL(cellChanged(int,int)),
          this, SLOT(cellChangedSlot(int,int)));

  //layout
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mAddLandmarkButton);
  mVerticalLayout->addWidget(mEditLandmarkButton);
  mVerticalLayout->addWidget(mRemoveLandmarkButton);
  mVerticalLayout->addWidget(mThresholdLabel);
  mVerticalLayout->addWidget(mThresholdSlider);
  mVerticalLayout->addWidget(new RegistrationHistoryWidget(this));

  this->setLayout(mVerticalLayout);

}
ImageRegistrationWidget::~ImageRegistrationWidget()
{}
void ImageRegistrationWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = dataManager()->getActiveImage();
  if(mCurrentImage == activeImage)
    return;

  //disconnect from the old image
  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(std::string)),
               this, SLOT(landmarkAddedSlot(std::string)));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(std::string)),
               this, SLOT(landmarkRemovedSlot(std::string)));
  }

  //Set new current image
  mCurrentImage = activeImage;

  if(mCurrentImage)
  {
    messageManager()->sendInfo("ImageRegistrationWidget got a new current image to work on: "+mCurrentImage->getUid()+"");

    connect(mCurrentImage.get(), SIGNAL(landmarkAdded(std::string)),
               this, SLOT(landmarkAddedSlot(std::string)));
    connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(std::string)),
               this, SLOT(landmarkRemovedSlot(std::string)));

    //set a default treshold
    mThresholdSlider->setRange(mCurrentImage->getPosMin(), mCurrentImage->getPosMax());
    ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
    mThresholdSlider->setValue(probeRep->getThreshold());
    }
  else
  {
    messageManager()->sendWarning("Empty mCurrentImage in ImageRegistrationWidget::activeImageChangedSlot(), return");
    return;
  }

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);

  //enable the add point button
  mAddLandmarkButton->setEnabled(mCurrentImage!=0);

}
void ImageRegistrationWidget::addLandmarkButtonClickedSlot()
{
  ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  if(!probeRep)
  {
    messageManager()->sendError("Could not find a rep to add the landmark to.");
    return;
  }
  //int index = mLandmarkTableWidget->rowCount()+1;

  std::string uid = dataManager()->addLandmark();
  ssc::Vector3D pos_r = probeRep->getPosition();
  ssc::Vector3D pos_d = mCurrentImage->get_rMd().inv().coord(pos_r);
  std::cout << "ImageRegistrationWidget::addLandmarkButtonClickedSlot()" << uid << ", " << pos_r << "ci=" << mCurrentImage.get() << std::endl;
  mCurrentImage->setLandmark(ssc::Landmark(uid, pos_d));
  //probeRep->makeLandmarkPermanent(uid); //TODO
}

void ImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
  ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  if(!probeRep)
  {
    messageManager()->sendError("Could not find a rep to edit the landmark for.");
    return;
  }
//  int index = mCurrentRow+1;
  std::string uid = mActiveLandmark;
  ssc::Vector3D pos_r = probeRep->getPosition();
  mCurrentImage->setLandmark(ssc::Landmark(uid, pos_r));

//  probeRep->makeLandmarkPermanent(index); //TODO
}

void ImageRegistrationWidget::removeLandmarkButtonClickedSlot()
{
//  if(mCurrentRow < 0 || mCurrentColumn < 0)
//    return;

//  int index = mCurrentRow+1;
  std::string uid = mActiveLandmark;
  mCurrentImage->removeLandmark(uid);

//  LandmarkRepPtr landmarkRep = repManager()->getLandmarkRep("LandmarkRep_1");
//  landmarkRep->removePermanentPoint(index);
}

void ImageRegistrationWidget::landmarkSelectedSlot(int row, int column)
{
  mActiveLandmark = string_cast(mLandmarkTableWidget->item(row, column)->data(Qt::UserRole).toString());
//  mCurrentRow = row;
//  mCurrentColumn = column;

  mEditLandmarkButton->setEnabled(true);
  mRemoveLandmarkButton->setEnabled(true);
}

void ImageRegistrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void ImageRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  mLandmarkTableWidget->clear();

  if(!image) //Image is deleted
    return;

  std::vector<ssc::Landmark> landmarks =  this->getAllLandmarks();

  //ready the table widget
  mLandmarkTableWidget->setRowCount(landmarks.size());
  mLandmarkTableWidget->setColumnCount(3);
  QStringList headerItems(QStringList() << "Name" << "Status" << "Landmark");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

  std::cout << "landmarks.size()" << landmarks.size() << std::endl;

  for(unsigned i=0; i<landmarks.size();++i)
  {
    std::vector<QTableWidgetItem*> items(3); // name, status, coordinates

    ssc::LandmarkProperty prop = dataManager()->getLandmarkProperties()[landmarks[i].getUid()];
    ssc::Vector3D coord = landmarks[i].getCoord();

    items[0] = new QTableWidgetItem(qstring_cast(prop.getName()));
    items[1] = new QTableWidgetItem;

    if (prop.getActive())
      items[1]->setCheckState(Qt::Checked);
    else
      items[1]->setCheckState(Qt::Unchecked);

    //std::cout << "coord[" << i << "]=" << coord << std::endl;
    items[2] = new QTableWidgetItem(tr("(%1, %2, %3)").arg(coord[0]).arg(coord[1]).arg(coord[2]));

    for (unsigned j=0; j<items.size(); ++j)
    {
      items[j]->setData(Qt::UserRole, qstring_cast(prop.getUid()));
      mLandmarkTableWidget->setItem(i, j, items[j]);
    }

    //highlight selected row
    if (prop.getUid()==mActiveLandmark)
    {
      mLandmarkTableWidget->setCurrentItem(items[2]);
    }
  }

  //update buttons
  mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.empty());
  mEditLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.empty());

//  if(landmarks.empty())
//  {
//    mRemoveLandmarkButton->setDisabled(true);
//    mEditLandmarkButton->setDisabled(true);
//  }
//  else
//  {
//    mRemoveLandmarkButton->setEnabled(true);
//    if(!mActiveLandmark.empty())
//      mEditLandmarkButton->setEnabled(true);
//  }
}

std::vector<ssc::Landmark> ImageRegistrationWidget::getAllLandmarks() const
{
  std::vector<ssc::Landmark> retval;
  ssc::LandmarkMap imageData = mCurrentImage->getLandmarks();
  std::map<std::string, ssc::LandmarkProperty> dataData = dataManager()->getLandmarkProperties();
  std::cout << "imageData.size()" << imageData.size() << std::endl;
  std::cout << "dataData.size()" << dataData.size() << std::endl;

  std::map<std::string, ssc::LandmarkProperty>::iterator iter;

  for (iter=dataData.begin(); iter!=dataData.end(); ++iter)
  {
    if (imageData.count(iter->first))
      retval.push_back(imageData[iter->first]);
    else
      retval.push_back(ssc::Landmark(iter->first));
  }

  return retval;
}

void ImageRegistrationWidget::cellChangedSlot(int row,int column)
{
  QTableWidgetItem* item = mLandmarkTableWidget->item(row, column);
  std::string uid = string_cast(item->data(Qt::UserRole).toString());

  if(column==1)
  {
    Qt::CheckState state = item->checkState();
    dataManager()->setLandmarkActive(uid, state==Qt::Checked);
    //registrationManager()->setGlobalPointsActiveSlot(row, state);
  }

  if(column==0)
  {
    std::string name = item->text().toStdString();
    dataManager()->setLandmarkName(uid, name);
//    int index = row+1;
//    registrationManager()->setGlobalPointsNameSlot(index, name);
  }
}
void ImageRegistrationWidget::thresholdChangedSlot(const int value)
{
  emit thresholdChanged(value);
  std::stringstream message;
  message << "Threshold set to " << value;
  messageManager()->sendInfo(message.str());

  QString text = "Probing threshold: ";
  QString valueText;
  valueText.setNum(value);
  text.append(valueText);
  mThresholdLabel->setText(text);
}

void ImageRegistrationWidget::landmarkAddedSlot(std::string uid)
{
  //make sure the masterImage is set
  ssc::ImagePtr masterImage = registrationManager()->getMasterImage();
  if(!masterImage)
    registrationManager()->setMasterImage(mCurrentImage);

  registrationManager()->doImageRegistration(mCurrentImage);
// //check if its time to do image registration
//  if(mCurrentImage->getLandmarks().size() > 2)
//  {
//    messageManager()->sendInfo(mCurrentImage->getUid());
//    //registrationManager()->setGlobalPointSet(mCurrentImage->getLandmarks());
//    registrationManager()->doImageRegistration(mCurrentImage);
//  }

  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void ImageRegistrationWidget::landmarkRemovedSlot(std::string uid)
{
  this->populateTheLandmarkTableWidget(mCurrentImage);
}


}//namespace cx
