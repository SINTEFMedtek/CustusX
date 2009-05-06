#include "cxPatientRegistrationWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
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
  mRegistrationManager(RegistrationManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mMessageManager(MessageManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mCurrentRow(-1),
  mCurrentColumn(-1)
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

  //toolmanager
  connect(mToolManager, SIGNAL(dominantToolChanged(const std::string&)),
          this, SLOT(dominantToolChangedSlot(const std::string&)));
  connect(mToolManager, SIGNAL(toolSampleAdded(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));
  connect(mToolManager, SIGNAL(toolSampleRemoved(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));

  //layout
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mToolSampleButton);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);
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
  }

  //save active points before changing image
  mRegistrationManager->setActivePointsMap(mLandmarkActiveMap);
  mLandmarkActiveMap = mRegistrationManager->getActivePointsMap(); //TODO is this correct?

  mCurrentImage = currentImage;

  //connect to new image
  connect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
          this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
  connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
          this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void PatientRegistrationWidget::imageLandmarksUpdateSlot(double notUsedX, double notUsedY, double notUsedZ, unsigned int notUsedIndex)
{
  //update the active vector in registration manager
  mRegistrationManager->setActivePointsMap(mLandmarkActiveMap);

  //repopulate the tablewidget
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void PatientRegistrationWidget::toolSampledUpdateSlot(double notUsedX, double notUsedY, double notUsedZ,unsigned int notUsedIndex)
{
  int numberOfToolSamples = mToolManager->getToolSamples()->GetNumberOfTuples();
  int numberOfActiveToolSamples = 0;
  std::map<int, bool>::iterator it = mLandmarkActiveMap.begin();
  while(it != mLandmarkActiveMap.end())
  {
    if(it->second)
      numberOfActiveToolSamples++;
    it++;
  }
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
  ssc::Transform3DPtr lastTransform = mToolToSample->getLastTransform();
  if(lastTransform.get() == NULL)
    return;

  vtkMatrix4x4Ptr lastTransformMatrix = lastTransform->matrix();
  double x = lastTransformMatrix->GetElement(0,3);
  double y = lastTransformMatrix->GetElement(1,3);
  double z = lastTransformMatrix->GetElement(2,3);

  if(mCurrentRow == -1)
    mCurrentRow = 0;
  unsigned int index = mCurrentRow+1;
  mToolManager->addToolSampleSlot(x, y, z, index);
}
void PatientRegistrationWidget::rowSelectedSlot(int row, int column)
{
  mCurrentRow = row;
  mCurrentColumn = column;
}
void PatientRegistrationWidget::cellChangedSlot(int row, int column)
{
  if (column!=0)
    return;

  Qt::CheckState state = mLandmarkTableWidget->item(row,column)->checkState();
  mLandmarkActiveMap[row] = state;

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

  //TODO: REMOVE
  //only for testing...
  ssc::ToolRep3DPtr toolRep3D_1 = mRepManager->getToolRep3DRep("ToolRep3D_1");
  toolRep3D_1->setTool(mToolToSample);
  View3D* view = mViewManager->get3DView("View3D_1");
  view->addRep(toolRep3D_1);

  //update button
  mToolSampleButton->setEnabled(mToolToSample->getVisible());
}
void PatientRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  //get globalPointsNameList from the RegistrationManager
  RegistrationManager::NameListType nameList = mRegistrationManager->getGlobalPointSetNameList();

  //get the landmarks from the image
  vtkDoubleArrayPtr landmarks =  image->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  //ready the table widget
  mLandmarkTableWidget->clear();
  mLandmarkTableWidget->setRowCount(0);
  mLandmarkTableWidget->setColumnCount(3);
  QStringList headerItems(QStringList() << "Active" << "Name" << "Accuracy");
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
    QTableWidgetItem* columnOne;
    QTableWidgetItem* columnTwo;
    QTableWidgetItem* columnThree;

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
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem(tr("(%1, %2, %3)").arg(landmark[0]).arg(landmark[1]).arg(landmark[2]));
        columnThree = new QTableWidgetItem(tr("%1").arg(landmarkRegistrationAccuracy));
      }
      else
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem();
      }
      //check the mLandmarkActiveVector...
      std::map<int, bool>::iterator it = mLandmarkActiveMap.find(row);
      if(it != mLandmarkActiveMap.end())
      {
        if(!it->second)
          columnOne->setCheckState(Qt::Unchecked);
        else
          columnOne->setCheckState(Qt::Checked);
      }
      else
      {
        mLandmarkActiveMap[row] = true;
        columnOne->setCheckState(Qt::Checked);
      }
      columnTwo->setFlags(Qt::ItemIsSelectable);
      columnThree->setFlags(Qt::ItemIsSelectable);
      mLandmarkTableWidget->setItem(row, 0, columnOne);
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
      mLandmarkTableWidget->setItem(row, 2, columnThree);
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
    QTableWidgetItem* columnTwo;

    if(index > mLandmarkTableWidget->rowCount())
    {
      mLandmarkTableWidget->setRowCount(index);
      columnTwo = new QTableWidgetItem();
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
    }
    else
    {
      columnTwo = mLandmarkTableWidget->item(row, 1);
/*      if(columnTwo == NULL) //TODO: remove
        std::cout << "columnTwo == NULL!!!" << std::endl;*/
    }
    columnTwo->setText(QString(name.c_str()));
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
        std::map<int, bool>::iterator it = mLandmarkActiveMap.find(sourcePoint[3]);
        if(it != mLandmarkActiveMap.end())
        {
          if(!it->second)
          {
            // Calculate accuracy - Set mLandmarkAccuracy
            ssc::Vector3D sourcePointVector(sourcePoint[0],
                                            sourcePoint[1],
                                            sourcePoint[2]);
            ssc::Vector3D transformedPointVector = rMpr->coord(sourcePointVector);

            double xAccuracy = targetPoint[0] - transformedPointVector[0];
            double yAccuracy = targetPoint[1] - transformedPointVector[1];
            double zAccuracy = targetPoint[2] - transformedPointVector[2];

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
  std::map<int, bool>::iterator it = mLandmarkActiveMap.begin();
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    if(it->second)
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
