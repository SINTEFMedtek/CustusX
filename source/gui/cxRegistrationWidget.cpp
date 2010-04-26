/*
 * cxRegistrationWidget.cpp
 *
 *  Created on: Apr 21, 2010
 *      Author: dev
 */

#include "cxRegistrationWidget.h"

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
#include "cxRegistrationHistoryWidget.h"
#include "sscTypeConversions.h"

namespace cx
{
RegistrationWidget::RegistrationWidget(QWidget* parent) :
  QWidget(parent),
  mVerticalLayout(new QVBoxLayout(this)),
  mLandmarkTableWidget(new QTableWidget(this)),
  mAvarageAccuracyLabel(new QLabel(QString(" "), this))
{
  //widget
  this->setWindowTitle("Registration");

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)),
          this, SLOT(cellClickedSlot(int, int)));
  connect(mLandmarkTableWidget, SIGNAL(cellChanged(int,int)),
          this, SLOT(cellChangedSlot(int,int)));

  this->setLayout(mVerticalLayout);
}

RegistrationWidget::~RegistrationWidget()
{
}

void RegistrationWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = dataManager()->getActiveImage();
  if(mCurrentImage == activeImage)
    return;

  //disconnect from the old image
  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(std::string)), this, SLOT(landmarkUpdatedSlot(std::string)));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(std::string)), this, SLOT(landmarkUpdatedSlot(std::string)));
  }

  mCurrentImage = activeImage;

  if(mCurrentImage)
  {
    connect(mCurrentImage.get(), SIGNAL(landmarkAdded(std::string)), this, SLOT(landmarkUpdatedSlot(std::string)));
    connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(std::string)), this, SLOT(landmarkUpdatedSlot(std::string)));
  }

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void RegistrationWidget::cellClickedSlot(int row, int column)
{
  mActiveLandmark = string_cast(mLandmarkTableWidget->item(row, column)->data(Qt::UserRole).toString());
}

void RegistrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  registrationManager()->initialize();
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void RegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  mLandmarkTableWidget->clear();

  if(!image) //Image is deleted
    return;

  std::vector<ssc::Landmark> landmarks =  this->getAllLandmarks();

  //ready the table widget
  mLandmarkTableWidget->setRowCount(landmarks.size());
  mLandmarkTableWidget->setColumnCount(4);
  QStringList headerItems(QStringList() << "Name" << "Status" << "Coordinates" << "Accuracy (mm)");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

  for(unsigned i=0; i<landmarks.size();++i)
  {
    std::vector<QTableWidgetItem*> items(4); // name, status, coordinates, accuracy

    ssc::LandmarkProperty prop = dataManager()->getLandmarkProperties()[landmarks[i].getUid()];
    ssc::Vector3D coord = landmarks[i].getCoord();

    items[0] = new QTableWidgetItem(qstring_cast(prop.getName()));
    items[1] = new QTableWidgetItem;

    if (prop.getActive())
      items[1]->setCheckState(Qt::Checked);
    else
      items[1]->setCheckState(Qt::Unchecked);

    int width = 5;
    int prec = 1;
    items[2] = new QTableWidgetItem(tr("(%1, %2, %3)").arg(coord[0],width,'f',prec).arg(coord[1],width,'f',prec).arg(coord[2],width,'f',prec));

    items[3] = new QTableWidgetItem(tr("%1").arg(this->getAccuracy(landmarks[i].getUid())));


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

  mAvarageAccuracyLabel->setText(tr("Avrage accuracy %1 mm").arg(this->getAvarageAccuracy()));
}

void RegistrationWidget::nextRow()
{
  //std::cout << "RegistrationWidget::nextRow()" << std::endl;
  int selectedRow = mLandmarkTableWidget->currentIndex().row();
  int nextRow = selectedRow;
  int lastRow = mLandmarkTableWidget->rowCount()-1;

  if(selectedRow == -1) //no row is selected yet
  {
    selectedRow = lastRow;
    mLandmarkTableWidget->setCurrentItem(mLandmarkTableWidget->itemAt(selectedRow, 0));
  }
  if(selectedRow < lastRow)
  {
    nextRow=nextRow+1;
  }
  else if(selectedRow > lastRow)
  {
    nextRow = lastRow;
  }

  QModelIndex nextIndex = mLandmarkTableWidget->currentIndex().sibling(nextRow,0);
  if(nextIndex.isValid())
  {
    mLandmarkTableWidget->setCurrentIndex(nextIndex);
    mActiveLandmark = mLandmarkTableWidget->currentItem()->data(Qt::UserRole).toString().toStdString();
    //std::cout << "mActiveLandmark: " << mActiveLandmark.c_str() << std::endl;
  }
}

std::vector<ssc::Landmark> RegistrationWidget::getAllLandmarks() const
{
  std::vector<ssc::Landmark> retval;
  ssc::LandmarkMap targetData = this->getTargetLandmarks();
  std::map<std::string, ssc::LandmarkProperty> dataData = dataManager()->getLandmarkProperties();
  std::map<std::string, ssc::LandmarkProperty>::iterator iter;

  for (iter=dataData.begin(); iter!=dataData.end(); ++iter)
  {
    if (targetData.count(iter->first))
      retval.push_back(targetData[iter->first]);
    else
      retval.push_back(ssc::Landmark(iter->first));
  }

  return retval;
}

void RegistrationWidget::cellChangedSlot(int row,int column)
{
  QTableWidgetItem* item = mLandmarkTableWidget->item(row, column);
  std::string uid = string_cast(item->data(Qt::UserRole).toString());

  if(column==0)
  {
    std::string name = item->text().toStdString();
    dataManager()->setLandmarkName(uid, name);
  }
  if(column==1)
  {
    Qt::CheckState state = item->checkState();
    dataManager()->setLandmarkActive(uid, state==Qt::Checked);
  }
}

void RegistrationWidget::landmarkUpdatedSlot(std::string uid)
{
  this->performRegistration();
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

double RegistrationWidget::getAvarageAccuracy()
{
  std::map<std::string, ssc::LandmarkProperty> props = dataManager()->getLandmarkProperties();

  double sum = 0;
  int count = 0;
  std::map<std::string, ssc::LandmarkProperty>::iterator it = props.begin();
  for(; it != props.end(); ++it)
  {
    std::string uid = it->first;
    double val = this->getAccuracy(uid);
    if(!ssc::similar(val, 1000.0))
    {
      sum=sum+val;
      count++;
    }
  }
  if(count==0)
    return 1000;
  return sum/count;
}

double RegistrationWidget::getAccuracy(std::string uid)
{
  ssc::Landmark masterLandmark = mCurrentImage->getLandmarks()[uid]; //TODO : sjekk ut masterimage etc etc
  ssc::Landmark targetLandmark = this->getTargetLandmarks()[uid];
  if(masterLandmark.getUid().empty() || targetLandmark.getUid().empty())
    return 1000.0;

  ssc::Vector3D p_master_master = masterLandmark.getCoord();
  ssc::Vector3D p_target_target = targetLandmark.getCoord();
  ssc::Transform3D rMmaster = mCurrentImage->get_rMd();
  ssc::Transform3D rMtarget = this->getTargetTransform();

  ssc::Vector3D p_target_r = rMtarget.coord(p_target_target);
  ssc::Vector3D p_master_r = rMmaster.coord(p_master_master);

  ssc::Vector3D diff = p_target_r - p_master_r;

  double retval =
      sqrt(pow(diff[0],2) +
           pow(diff[1],2) +
           pow(diff[2],2));

  return retval;
}


}//namespace cx
