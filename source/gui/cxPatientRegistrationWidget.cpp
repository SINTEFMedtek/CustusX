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
#include "cxLandmarkRep.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"

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
  mRegistrationManager(RegistrationManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mMessageManager(MessageManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mCurrentRow(-1),
  mCurrentColumn(-1),
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
          this, SLOT(resetOffset()));

  //toolmanager
  connect(mToolManager, SIGNAL(dominantToolChanged(const std::string&)),
          this, SLOT(dominantToolChangedSlot(const std::string&)));
  connect(mToolManager, SIGNAL(toolSampleAdded(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));
  connect(mToolManager, SIGNAL(toolSampleRemoved(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));

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
          this, SLOT(setOffset(int)));
  connect(mYOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffset(int)));
  connect(mZOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffset(int)));

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
  this->setLayout(mVerticalLayout);

  ssc::ToolPtr dominantTool = mToolManager->getDominantTool();
  if(dominantTool.get() != NULL)
    this->dominantToolChangedSlot(dominantTool->getUid());
}
PatientRegistrationWidget::~PatientRegistrationWidget()
{}
void PatientRegistrationWidget::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  if(mCurrentImage == currentImage)
    return;

  //disconnect from the old image
  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    mMessageManager->sendInfo("Disconnected from old image "+mCurrentImage->getUid());
  }

  mCurrentImage = currentImage;

  //connect to new image
  if (mCurrentImage) //Don't use image if deleted
  {
    connect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
            this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
            this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
  }
  
  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void PatientRegistrationWidget::imageLandmarksUpdateSlot(double notUsedX, double notUsedY, double notUsedZ, unsigned int notUsedIndex)
{
  //repopulate the tablewidget
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void PatientRegistrationWidget::toolSampledUpdateSlot(double notUsedX, double notUsedY, double notUsedZ,unsigned int notUsedIndex)
{
  int numberOfToolSamples = mToolManager->getToolSamples()->GetNumberOfTuples();
  int numberOfActiveToolSamples = 0;
  RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
  RegistrationManager::NameListType::iterator it = landmarkActiveMap.begin();
  while(it != landmarkActiveMap.end())
  {
    if(it->second.second)
      numberOfActiveToolSamples++;
    it++;
  }
  //TODO REMOVE just for debugging
  /*std::stringstream stream;
  stream<<"ActiveToolSamples: "<<numberOfActiveToolSamples<<", ToolSamples: "<< numberOfToolSamples;
  mMessageManager->sendWarning(stream.str());*/
  //END
  if(numberOfActiveToolSamples >= 3 && numberOfToolSamples >= 3)
  {
    this->doPatientRegistration();
    this->updateAccuracy();
  }
}
void PatientRegistrationWidget::toolVisibleSlot(bool visible)
{
  if(visible)
    mToolSampleButton->setEnabled(true);
  else
    mToolSampleButton->setEnabled(false);
}
void PatientRegistrationWidget::toolSampleButtonClickedSlot()
{  
  //TODO What if the reference frame isnt visible?
  ssc::Transform3DPtr lastTransform = mToolToSample->getLastTransform();
  if(lastTransform.get() == NULL)
  {
    mMessageManager->sendError("The last transform was NULL!");
    return;
  }

  vtkMatrix4x4Ptr lastTransformMatrix = lastTransform->matrix();
  double x = lastTransformMatrix->GetElement(0,3);
  double y = lastTransformMatrix->GetElement(1,3);
  double z = lastTransformMatrix->GetElement(2,3);

  if(mCurrentRow == -1)
    mCurrentRow = 0;
  unsigned int index = mCurrentRow+1;
  
  //TODO REMOVE just for debugging
  /*std::stringstream message;
  message<<"Sampling row "<<mCurrentRow<<" for LANDMARK: "<<index;
  mMessageManager->sendWarning(message.str());*/
  //END
  
  mToolManager->addToolSampleSlot(x, y, z, index);
}
void PatientRegistrationWidget::rowSelectedSlot(int row, int column)
{
  mCurrentRow = row;
  mCurrentColumn = column;
  
  //TODO REMOVE just for debugging
  std::stringstream stream;
  stream<<"You clicked cell: ("<<mCurrentRow<<","<<mCurrentColumn<<").";
  mMessageManager->sendInfo(stream.str());
  //END
}
void PatientRegistrationWidget::cellChangedSlot(int row, int column)
{
  if(column!=1) //can only make changes to the status (landmark active or not)
    return;

  Qt::CheckState state = mLandmarkTableWidget->item(row,column)->checkState();
  mRegistrationManager->setGlobalPointsActiveSlot(row, state);

}
void PatientRegistrationWidget::dominantToolChangedSlot(const std::string& uid)
{
  if(mToolToSample.get() != NULL && mToolToSample->getUid() == uid)
    return;

  ToolPtr newTool = ToolPtr(dynamic_cast<Tool*>(mToolManager->getDominantTool().get()));
  if(mToolToSample.get() != NULL)
  {
    if(newTool.get() == NULL)
      return;

    disconnect(mToolToSample.get(), SIGNAL(toolVisible(bool)),
                this, SLOT(toolVisibleSlot(bool)));
  }

  mToolToSample = newTool;
  connect(mToolToSample.get(), SIGNAL(toolVisible(bool)),
              this, SLOT(toolVisibleSlot(bool)));

  //update button
  mToolSampleButton->setEnabled(mToolToSample->getVisible());
}
void PatientRegistrationWidget::resetOffset()
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
void PatientRegistrationWidget::setOffset(int value)
{
  mResetOffsetButton->setEnabled(true);
  mLandmarkTableWidget->setDisabled(true);

  ssc::Transform3DPtr offsetPtr(new ssc::Transform3D());
  (*offsetPtr.get())[0][3] = mXOffsetSlider->value();
  (*offsetPtr.get())[1][3] = mYOffsetSlider->value();
  (*offsetPtr.get())[2][3] = mZOffsetSlider->value();
  mRegistrationManager->setManualPatientRegistrationOffsetSlot(offsetPtr);
}
void PatientRegistrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void PatientRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  if (!image)
  {
    mLandmarkTableWidget->clear();
    return;
  }
  //get globalPointsNameList from the RegistrationManager
  RegistrationManager::NameListType nameList = mRegistrationManager->getGlobalPointSetNameList();

  //get the landmarks from the image
  vtkDoubleArrayPtr landmarks =  image->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  //ready the table widget
  mLandmarkTableWidget->clear();
  mLandmarkTableWidget->setRowCount(0);
  mLandmarkTableWidget->setColumnCount(4);
  QStringList headerItems(QStringList() << "Name" << "Status" << "Patient coordinates" << "Accuracy");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

  //fill the table widget with rows for the landmarks
  int row = 0;
  for(int i=0; i<numberOfLandmarks; i++)
  {
    double* landmark = landmarks->GetTuple(i);
    std::map<int, double>::iterator it = mLandmarkRegistrationAccuracyMap.find(landmark[3]);
    double landmarkRegistrationAccuracy = it->second;

    if(landmark[3] > mLandmarkTableWidget->rowCount())
      mLandmarkTableWidget->setRowCount(landmark[3]);
    QTableWidgetItem* columnOne; //name
    QTableWidgetItem* columnTwo; //status
    QTableWidgetItem* columnThree; //patient coordinates
    QTableWidgetItem* columnFour; //accuracy

    int rowToInsert = landmark[3]-1;
    int tempRow = -1;
    if(rowToInsert < row)
    {
      tempRow = row;
      row = rowToInsert;
    }
    for(; row <= rowToInsert; row++)
    {
      if(row == rowToInsert)
      {
        columnOne = new QTableWidgetItem(tr("(%1, %2, %3)").arg(landmark[0]).arg(landmark[1]).arg(landmark[2]));
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem();
        columnFour = new QTableWidgetItem(tr("%1").arg(landmarkRegistrationAccuracy));
      }
      else
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem();
        columnFour = new QTableWidgetItem();
      }
      //check the mLandmarkActiveVector...
      RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
      RegistrationManager::NameListType::iterator it = landmarkActiveMap.find(row+1);
      if(it != landmarkActiveMap.end())
      {
        if(!it->second.second)
          columnTwo->setCheckState(Qt::Unchecked);
        else
          columnTwo->setCheckState(Qt::Checked);
      }
      else
      {
        columnTwo->setCheckState(Qt::Checked);
        mRegistrationManager->setGlobalPointsActiveSlot(row, true);
      }
      //set flags and add the item to the table
      //TODO dosnt work
      /*columnOne->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate); //name should be editable and selectable
      columnTwo->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate);//status should be ?
      columnThree->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate);//tool coords. should be ?
      columnFour->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate);//accuracy should be ?*/
      mLandmarkTableWidget->setItem(row, 0, columnOne);
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
      mLandmarkTableWidget->setItem(row, 2, columnThree);
      mLandmarkTableWidget->setItem(row, 3, columnFour);
    }
    if(tempRow != -1)
      row = tempRow;
  }
  //fill in names
  typedef RegistrationManager::NameListType::iterator Iterator;
  for(Iterator it = nameList.begin(); it != nameList.end(); ++it)
  {
    std::string name = it->second.first;
    int index = it->first;
    int row = index-1;
    QTableWidgetItem* columnOne;

    if(mLandmarkTableWidget->rowCount() == 0 ||
       index > mLandmarkTableWidget->rowCount()) //we need more rows
    {
      mLandmarkTableWidget->setRowCount(index);
      columnOne = new QTableWidgetItem();
      //set flags and add the item to the table
      //TODO dosnt work
      /*columnOne->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate);//name should be selectable and editable*/ 
      mLandmarkTableWidget->setItem(row, 0, columnOne);
    }
    else //we have all the rows we need atm
    {
      columnOne = mLandmarkTableWidget->item(row, 0);
    }
    if(columnOne != NULL && !name.empty())
      columnOne->setText(QString(name.c_str()));
  }
  //get the patient coordinates from the toolmanager
  vtkDoubleArrayPtr toolsamples = mToolManager->getToolSamples();
  int numberOfToolSamples = toolsamples->GetNumberOfTuples();
 
  //TODO REMOVE for debugging
  /*std::stringstream stream;
  stream<<"Number of tools sampled: "<<numberOfToolSamples;
  mMessageManager->sendWarning(stream.str());*/
  //END

  //fill in toolsamples
  for(int i=0; i<numberOfToolSamples; i++)
  {
    double* toolSample = toolsamples->GetTuple(i);
    int row = toolSample[3]-1;
    QTableWidgetItem* columnThree = mLandmarkTableWidget->item(row, 2);
    if(columnThree == NULL)
    {
      mMessageManager->sendError("Couldn't find a cell in the table to put the toolsample in.");
    }
    else
    {
      columnThree->setText(tr("(%1, %2, %3)").arg(toolSample[0]).arg(toolSample[1]).arg(toolSample[2]));
    }
  }
}
void PatientRegistrationWidget::updateAccuracy()
{
  //ssc:Image masterImage = mRegistrationManager->getMasterImage();
  vtkDoubleArrayPtr globalPointset = mRegistrationManager->getGlobalPointSet();
  vtkDoubleArrayPtr toolSamplePointset = mToolManager->getToolSamples();

  ssc::Transform3DPtr rMpr = mToolManager->get_rMpr();

  int numberOfGlobalImagePoints = globalPointset->GetNumberOfTuples();
  int numberOfToolSamplePoints = toolSamplePointset->GetNumberOfTuples();

  // First reset the accuracy table
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    double* imagePoint = globalPointset->GetTuple(i);
    mLandmarkRegistrationAccuracyMap[imagePoint[3]] = 1000;
  }

  // Calculate and fill the accuracy table
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    for(int j=0; j < numberOfToolSamplePoints; j++)
    {
      double* targetPoint = globalPointset->GetTuple(i);
      double* sourcePoint = toolSamplePointset->GetTuple(j);
      
      if(sourcePoint[3] == targetPoint[3])
      {
        //check the mLandmarkActiveVector...
        RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
        RegistrationManager::NameListType::iterator it = landmarkActiveMap.find(sourcePoint[3]);
        if(it != landmarkActiveMap.end())
        {
          if(it->second.second)
          {
            // Calculate accuracy - Set mLandmarkAccuracy
            ssc::Vector3D sourcePointVector(sourcePoint[0],
                                            sourcePoint[1],
                                            sourcePoint[2]);
            ssc::Vector3D transformedPointVector = rMpr->coord(sourcePointVector);

            double xAccuracy = targetPoint[0] - transformedPointVector[0];
            double yAccuracy = targetPoint[1] - transformedPointVector[1];
            double zAccuracy = targetPoint[2] - transformedPointVector[2];

            //TODO REMOVE
            /*std::stringstream stream;
            stream<<"Landmark: "<<targetPoint[3]<<" ("<<targetPoint[0]<<","<<targetPoint[1]<<","<<targetPoint[2]<<")"<<std::endl;
            stream<<"Toolpoint: "<<sourcePoint[3]<<" ("<<sourcePoint[0]<<","<<sourcePoint[1]<<","<<sourcePoint[2]<<")"<<std::endl;
            mMessageManager->sendInfo(stream.str());*/
            //END
            
            mLandmarkRegistrationAccuracyMap[sourcePoint[3]] =
                sqrt(pow(xAccuracy,2) +
                     pow(yAccuracy,2) +
                     pow(zAccuracy,2));
          }
        }
      }
    }
  }

  // Calculate total registration accuracy
  mAverageRegistrationAccuracy = 0;
  RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
  RegistrationManager::NameListType::iterator it = landmarkActiveMap.begin();
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    if(it->second.second)
    {
      mAverageRegistrationAccuracy = mAverageRegistrationAccuracy +
                                    mLandmarkRegistrationAccuracyMap[i];
      it++;
    }
  }

  //make sure the accuracy is filled in the table widget
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void PatientRegistrationWidget::doPatientRegistration()
{
  mRegistrationManager->doPatientRegistration();
}
}//namespace cx
