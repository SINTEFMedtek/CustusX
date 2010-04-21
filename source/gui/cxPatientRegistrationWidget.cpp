#include "cxPatientRegistrationWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QSpinBox>
#include <vtkDoubleArray.h>
#include <sscVector3D.h>
#include <sscVolumetricRep.h>
#include "sscTypeConversions.h"
#include "cxLandmarkRep.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxToolManager.h"
#include "cxDataManager.h"
#include "cxRegistrationHistoryWidget.h"

namespace cx
{
PatientRegistrationWidget::PatientRegistrationWidget(QWidget* parent) :
  QWidget(parent),
  mVerticalLayout(new QVBoxLayout(this)),
  mLandmarkTableWidget(new QTableWidget(this)),
  mToolSampleButton(new QPushButton("Sample Tool", this)),
  mAvarageAccuracyLabel(new QLabel(QString(" "), this)),
  mOffsetLabel(new QLabel(QString("Manual offset:"), this)),
  mOffsetWidget(new QWidget(this)),
  mOffsetsGridLayout(new QGridLayout(mOffsetWidget)),
  mXLabel(new QLabel(QString("X "), this)),
  mYLabel(new QLabel(QString("Y "), this)),
  mZLabel(new QLabel(QString("Z "), this)),
  mXOffsetSlider(new QSlider(Qt::Horizontal, this)),
  mYOffsetSlider(new QSlider(Qt::Horizontal, this)),
  mZOffsetSlider(new QSlider(Qt::Horizontal, this)),
  mXSpinBox(new QSpinBox(mOffsetWidget)),
  mYSpinBox(new QSpinBox(mOffsetWidget)),
  mZSpinBox(new QSpinBox(mOffsetWidget)),
  mResetOffsetButton(new QPushButton("Clear offset", this)),
  //mCurrentRow(-1),
  //mCurrentColumn(-1),
  mMinValue(-200),
  mMaxValue(200),
  mDefaultValue(0)
{
  //Dock widget
  this->setWindowTitle("Patient Registration");

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellChanged(int, int)),
          this, SLOT(cellChangedSlot(int, int)));
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)),
          this, SLOT(rowSelectedSlot(int, int)));

  //buttons
  mToolSampleButton->setDisabled(true);
  connect(mToolSampleButton, SIGNAL(clicked()),
          this, SLOT(toolSampleButtonClickedSlot()));
  mResetOffsetButton->setDisabled(true);
  connect(mResetOffsetButton, SIGNAL(clicked()),
          this, SLOT(resetOffsetSlot()));

  //toolmanager
  connect(toolManager(), SIGNAL(dominantToolChanged(const std::string&)),
          this, SLOT(dominantToolChangedSlot(const std::string&)));
//  connect(toolManager(), SIGNAL(toolSampleAdded(double,double,double,unsigned int)),
//          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));
//  connect(toolManager(), SIGNAL(toolSampleRemoved(double,double,double,unsigned int)),
//          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));

  connect(toolManager(), SIGNAL(landmarkAdded(std::string)),   this, SLOT(patientLandmarkChangedSlot(std::string)));
  connect(toolManager(), SIGNAL(landmarkRemoved(std::string)), this, SLOT(patientLandmarkChangedSlot(std::string)));

  //registrationmanager
  connect(RegistrationManager::getInstance(), SIGNAL(patientRegistrationPerformed()),
          this, SLOT(activateManualRegistrationFieldSlot()));
  connect(ToolManager::getInstance(), SIGNAL(rMprChanged()),
          this, SLOT(activateManualRegistrationFieldSlot()));

  //sliders
  mXOffsetSlider->setRange(mMinValue,mMaxValue);
  mXOffsetSlider->setValue(mDefaultValue);
  mYOffsetSlider->setRange(mMinValue,mMaxValue);
  mYOffsetSlider->setValue(mDefaultValue);
  mZOffsetSlider->setRange(mMinValue,mMaxValue);
  mZOffsetSlider->setValue(mDefaultValue);

  //spinboxes
  mXSpinBox->setRange(mMinValue,mMaxValue);
  mXSpinBox->setValue(mDefaultValue);
  mYSpinBox->setRange(mMinValue,mMaxValue);
  mYSpinBox->setValue(mDefaultValue);
  mZSpinBox->setRange(mMinValue,mMaxValue);
  mZSpinBox->setValue(mDefaultValue);

  //connect sliders to the spinbox
  connect(mXOffsetSlider, SIGNAL(valueChanged(int)),
          mXSpinBox, SLOT(setValue(int)));
  connect(mYOffsetSlider, SIGNAL(valueChanged(int)),
          mYSpinBox, SLOT(setValue(int)));
  connect(mZOffsetSlider, SIGNAL(valueChanged(int)),
          mZSpinBox, SLOT(setValue(int)));
  connect(mXSpinBox, SIGNAL(valueChanged(int)),
          mXOffsetSlider, SLOT(setValue(int)));
  connect(mYSpinBox, SIGNAL(valueChanged(int)),
          mYOffsetSlider, SLOT(setValue(int)));
  connect(mZSpinBox, SIGNAL(valueChanged(int)),
          mZOffsetSlider, SLOT(setValue(int)));

  //connect sliders registrationmanager,
  //spinboxes are connected to the sliders so no need to connect them to the
  //registration manager as well
  connect(mXOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));
  connect(mYOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));
  connect(mZOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));

  //layout
  mOffsetsGridLayout->addWidget(mXLabel, 0, 0);
  mOffsetsGridLayout->addWidget(mYLabel, 1, 0);
  mOffsetsGridLayout->addWidget(mZLabel, 2, 0);
  mOffsetsGridLayout->addWidget(mXOffsetSlider, 0, 1);
  mOffsetsGridLayout->addWidget(mYOffsetSlider, 1, 1);
  mOffsetsGridLayout->addWidget(mZOffsetSlider, 2, 1);
  mOffsetsGridLayout->addWidget(mXSpinBox, 0, 2);
  mOffsetsGridLayout->addWidget(mYSpinBox, 1, 2);
  mOffsetsGridLayout->addWidget(mZSpinBox, 2, 2);

  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mToolSampleButton);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);
  mVerticalLayout->addWidget(mOffsetLabel);
  mVerticalLayout->addWidget(mOffsetWidget);
  mVerticalLayout->addWidget(mResetOffsetButton);
  mVerticalLayout->addWidget(new RegistrationHistoryWidget(this));

  this->setLayout(mVerticalLayout);

  mOffsetWidget->setDisabled(true);

  ssc::ToolPtr dominantTool = toolManager()->getDominantTool();
  if(dominantTool)
    this->dominantToolChangedSlot(dominantTool->getUid());
}

PatientRegistrationWidget::~PatientRegistrationWidget()
{
}

void PatientRegistrationWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = dataManager()->getActiveImage();
  if(mCurrentImage == activeImage)
    return;

  //disconnect from the old image
  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(std::string)), this, SLOT(landmarkAddedSlot(std::string)));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(std::string)), this, SLOT(landmarkRemovedSlot(std::string)));
  }

  //Set new current image
  mCurrentImage = activeImage;

  if(mCurrentImage)
  {
    messageManager()->sendInfo("PatientRegistrationWidget got a new current image to work on: "+mCurrentImage->getUid()+"");

    connect(mCurrentImage.get(), SIGNAL(landmarkAdded(std::string)), this, SLOT(landmarkAddedSlot(std::string)));
    connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(std::string)), this, SLOT(landmarkRemovedSlot(std::string)));

  }
  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

//void PatientRegistrationWidget::imageLandmarksUpdateSlot(double notUsedX, double notUsedY, double notUsedZ, unsigned int notUsedIndex)
//{
//  //repopulate the tablewidget
//  this->populateTheLandmarkTableWidget(mCurrentImage);
//}

//void PatientRegistrationWidget::toolSampledUpdateSlot(double notUsedX, double notUsedY, double notUsedZ,unsigned int notUsedIndex)
//{
//  int numberOfToolSamples = toolManager()->getToolSamples()->GetNumberOfTuples();
//  int numberOfActiveToolSamples = 0;
//  RegistrationManager::NameListType landmarkActiveMap = registrationManager()->getGlobalPointSetNameList();
//  RegistrationManager::NameListType::iterator it = landmarkActiveMap.begin();
//  while(it != landmarkActiveMap.end())
//  {
//    if(it->second.second)
//      numberOfActiveToolSamples++;
//    it++;
//  }
//  //TODO REMOVE just for debugging
//  /*std::stringstream stream;
//  stream<<"ActiveToolSamples: "<<numberOfActiveToolSamples<<", ToolSamples: "<< numberOfToolSamples;
//  messageMan()->sendWarning(stream.str());*/
//  //END
//  if(numberOfActiveToolSamples >= 3 && numberOfToolSamples >= 3)
//  {
//    this->doPatientRegistration();
//    this->updateAccuracy();
//  }
//}

void PatientRegistrationWidget::toolVisibleSlot(bool visible)
{
  mToolSampleButton->setEnabled(visible);
}

void PatientRegistrationWidget::toolSampleButtonClickedSlot()
{  
  if(!mToolToSample)
  {
    messageManager()->sendError("mToolToSample is NULL!");
    return;
  }
  //TODO What if the reference frame isnt visible?
  //ssc::Transform3DPtr lastTransform_prMt = mToolToSample->getLastTransform();
  ssc::Transform3D lastTransform_prMt = mToolToSample->get_prMt();

//  vtkMatrix4x4Ptr lastTransformMatrix = lastTransform_prMt.matrix();
//  double x = lastTransformMatrix->GetElement(0,3);
//  double y = lastTransformMatrix->GetElement(1,3);
//  double z = lastTransformMatrix->GetElement(2,3);
  ssc::Vector3D p_pr = lastTransform_prMt.coord(ssc::Vector3D(0,0,0));

  // TODO: do we want to allow sampling points not defined in image??
  if (mActiveLandmark.empty() && !dataManager()->getLandmarkProperties().empty())
    mActiveLandmark = dataManager()->getLandmarkProperties().begin()->first;

  toolManager()->setLandmark(ssc::Landmark(mActiveLandmark, p_pr));

//  if(mCurrentRow == -1)
//    mCurrentRow = 0;
//  unsigned int index = mCurrentRow+1;
//
//  //TODO REMOVE just for debugging
//  /*std::stringstream message;
//  message<<"Sampling row "<<mCurrentRow<<" for LANDMARK: "<<index;
//  messageMan()->sendWarning(message.str());*/
//  //END
//
//  toolManager()->addToolSampleSlot(x, y, z, index);
}
void PatientRegistrationWidget::rowSelectedSlot(int row, int column)
{
  mActiveLandmark = string_cast(mLandmarkTableWidget->item(row, column)->data(Qt::UserRole).toString());
//
//  mCurrentRow = row;
//  mCurrentColumn = column;
//
//  //TODO REMOVE just for debugging
//  std::stringstream stream;
//  stream<<"You clicked cell: ("<<mCurrentRow<<","<<mCurrentColumn<<").";
//  messageManager()->sendInfo(stream.str());
//  //END
}
void PatientRegistrationWidget::cellChangedSlot(int row, int column)
{
  QTableWidgetItem* item = mLandmarkTableWidget->item(row, column);
  std::string uid = string_cast(item->data(Qt::UserRole).toString());

  if(column==1)
  {
    Qt::CheckState state = item->checkState();
    dataManager()->setLandmarkActive(uid, state==Qt::Checked);
  }

  if(column==0)
  {
    std::string name = item->text().toStdString();
    dataManager()->setLandmarkName(uid, name);
  }
}

void PatientRegistrationWidget::dominantToolChangedSlot(const std::string& uid)
{
  if(mToolToSample && mToolToSample->getUid() == uid)
    return;

  ssc::ToolPtr dominantTool = toolManager()->getDominantTool();

  if(mToolToSample)
  {
    disconnect(mToolToSample.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
  }

  mToolToSample = dominantTool;

  if(mToolToSample)
  {
    connect(mToolToSample.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
  }

  //update button
  mToolSampleButton->setEnabled(mToolToSample && mToolToSample->getVisible());
}

void PatientRegistrationWidget::resetOffsetSlot()
{
  if(mXOffsetSlider->value() != mDefaultValue ||
     mYOffsetSlider->value() != mDefaultValue ||
     mZOffsetSlider->value() != mDefaultValue)
  {
    mXOffsetSlider->setValue(mDefaultValue);
    mYOffsetSlider->setValue(mDefaultValue);
    mZOffsetSlider->setValue(mDefaultValue);
    mResetOffsetButton->setDisabled(true);
    mLandmarkTableWidget->setEnabled(true);
  }
}

void PatientRegistrationWidget::setOffsetSlot(int value)
{
  mResetOffsetButton->setEnabled(true);
  mLandmarkTableWidget->setDisabled(true);
  
  vtkMatrix4x4* offsetMatrix = vtkMatrix4x4::New(); //identity
  offsetMatrix->SetElement(0, 3, mXOffsetSlider->value());
  offsetMatrix->SetElement(1, 3, mYOffsetSlider->value());
  offsetMatrix->SetElement(2, 3, mZOffsetSlider->value());
  
  ssc::Transform3DPtr offsetPtr(new ssc::Transform3D(offsetMatrix));
  registrationManager()->setManualPatientRegistrationOffsetSlot(offsetPtr);
}

void PatientRegistrationWidget::activateManualRegistrationFieldSlot()
{
  mOffsetWidget->setEnabled(true);
}

void PatientRegistrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void PatientRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  mLandmarkTableWidget->clear();

  if(!image) //Image is deleted
    return;

  std::vector<ssc::Landmark> landmarks =  this->getAllLandmarks();

  //ready the table widget
  //mLandmarkTableWidget->setRowCount(0);
  mLandmarkTableWidget->setColumnCount(4);
  QStringList headerItems(QStringList() << "Name" << "Status" << "Patient coordinates" << "Accuracy");
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

    items[2] = new QTableWidgetItem(tr("(%1, %2, %3)").arg(coord[0]).arg(coord[1]).arg(coord[2]));

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
  mAvarageAccuracyLabel->setText(tr("%1").arg(this->getAvarageAccuracy()));
}

std::vector<ssc::Landmark> PatientRegistrationWidget::getAllLandmarks() const
{
  std::vector<ssc::Landmark> retval;
  ssc::LandmarkMap toolData = toolManager()->getLandmarks();
  std::map<std::string, ssc::LandmarkProperty> dataData = dataManager()->getLandmarkProperties();

  std::map<std::string, ssc::LandmarkProperty>::iterator iter;

  for (iter=dataData.begin(); iter!=dataData.end(); ++iter)
  {
    if (toolData.count(iter->first))
      retval.push_back(toolData[iter->first]);
    else
      retval.push_back(ssc::Landmark(iter->first));
  }

  return retval;
}

double PatientRegistrationWidget::getAvarageAccuracy()
{
  std::map<std::string, ssc::LandmarkProperty> props = dataManager()->getLandmarkProperties();
  ssc::LandmarkMap imageLandmarkMap = mCurrentImage->getLandmarks();
  ssc::LandmarkMap toolLandmark = toolManager()->getLandmarks();

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

double PatientRegistrationWidget::getAccuracy(std::string uid)
{
  ssc::Landmark imageLandmark = mCurrentImage->getLandmarks()[uid];
  ssc::Landmark toolLandmark = toolManager()->getLandmarks()[uid];
  if(imageLandmark.getUid().empty() || toolLandmark.getUid().empty())
    return 1000.0;

  ssc::Vector3D p_image_r = imageLandmark.getCoord();
  ssc::Vector3D p_tool_pr = toolLandmark.getCoord();
  ssc::Transform3D rMpr = *(toolManager()->get_rMpr());

  ssc::Vector3D p_tool_r = rMpr.coord(p_tool_pr);

  ssc::Vector3D diff = p_tool_r - p_image_r;

  double retval =
      sqrt(pow(diff[0],2) +
           pow(diff[1],2) +
           pow(diff[2],2));

  return retval;
}

//void PatientRegistrationWidget::updateAccuracy()
//{


//  ssc:Image masterImage = registrationManager()->getMasterImage();
//  if(masterImage->is)
//
//  vtkDoubleArrayPtr globalPointset = registrationManager()->getGlobalPointSet();
//  vtkDoubleArrayPtr toolSamplePointset = toolManager()->getToolSamples();
//
//  ssc::Transform3DPtr rMpr = toolManager()->get_rMpr();
//
//  int numberOfGlobalImagePoints = globalPointset->GetNumberOfTuples();
//  int numberOfToolSamplePoints = toolSamplePointset->GetNumberOfTuples();
//
//  // First reset the accuracy table
//  for (int i=0; i < numberOfGlobalImagePoints; i++)
//  {
//    double* imagePoint = globalPointset->GetTuple(i);
//    mLandmarkRegistrationAccuracyMap[imagePoint[3]] = 1000;
//  }

  // Calculate and fill the accuracy table
//  for (int i=0; i < numberOfGlobalImagePoints; i++)
//  {
//    for(int j=0; j < numberOfToolSamplePoints; j++)
//    {
//      double* targetPoint = globalPointset->GetTuple(i);
//      double* sourcePoint = toolSamplePointset->GetTuple(j);
//
//      if(sourcePoint[3] == targetPoint[3])
//      {
//        //check the mLandmarkActiveVector...
//        RegistrationManager::NameListType landmarkActiveMap = registrationManager()->getGlobalPointSetNameList();
//        RegistrationManager::NameListType::iterator it = landmarkActiveMap.find(sourcePoint[3]);
//        if(it != landmarkActiveMap.end())
//        {
//          if(it->second.second)
//          {
//            // Calculate accuracy - Set mLandmarkAccuracy
//            ssc::Vector3D sourcePointVector(sourcePoint[0],
//                                            sourcePoint[1],
//                                            sourcePoint[2]);
//            ssc::Vector3D transformedPointVector = rMpr->coord(sourcePointVector);
//
//            double xAccuracy = targetPoint[0] - transformedPointVector[0];
//            double yAccuracy = targetPoint[1] - transformedPointVector[1];
//            double zAccuracy = targetPoint[2] - transformedPointVector[2];
//
//            //TODO REMOVE
//            /*std::stringstream stream;
//            stream<<"Landmark: "<<targetPoint[3]<<" ("<<targetPoint[0]<<","<<targetPoint[1]<<","<<targetPoint[2]<<")"<<std::endl;
//            stream<<"Toolpoint: "<<sourcePoint[3]<<" ("<<sourcePoint[0]<<","<<sourcePoint[1]<<","<<sourcePoint[2]<<")"<<std::endl;
//            messageMan()->sendInfo(stream.str());*/
//            //END
//
//            mLandmarkRegistrationAccuracyMap[sourcePoint[3]] =
//                sqrt(pow(xAccuracy,2) +
//                     pow(yAccuracy,2) +
//                     pow(zAccuracy,2));
//          }
//        }
//      }
//    }
//  }
//
//  // Calculate total registration accuracy
//  mAverageRegistrationAccuracy = 0;
//  RegistrationManager::NameListType landmarkActiveMap = registrationManager()->getGlobalPointSetNameList();
//  RegistrationManager::NameListType::iterator it = landmarkActiveMap.begin();
//  for (int i=0; i < numberOfGlobalImagePoints; i++)
//  {
//    if(it->second.second)
//    {
//      mAverageRegistrationAccuracy = mAverageRegistrationAccuracy +
//                                    mLandmarkRegistrationAccuracyMap[i];
//      it++;
//    }
//  }

  //make sure the accuracy is filled in the table widget
//  this->populateTheLandmarkTableWidget(mCurrentImage);
//}

void PatientRegistrationWidget::doPatientRegistration()
{
  registrationManager()->doPatientRegistration();
}

void PatientRegistrationWidget::patientLandmarkChangedSlot(std::string uid)
{
  this->doPatientRegistration();
  //this->updateAccuracy();
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void PatientRegistrationWidget::landmarkAddedSlot(std::string uid)
{
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void PatientRegistrationWidget::landmarkRemovedSlot(std::string uid)
{
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

}//namespace cx
