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
#include "cxViewManager.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxView3D.h"
#include "cxView2D.h"
#include "cxInriaRep2D.h"

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
  mThresholdSlider(new QSlider(Qt::Horizontal, this)),
  mRepManager(RepManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mRegistrationManager(RegistrationManager::getInstance()),
  mCurrentRow(-1),
  mCurrentColumn(-1)
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
  this->setLayout(mVerticalLayout);

}
ImageRegistrationWidget::~ImageRegistrationWidget()
{}
void ImageRegistrationWidget::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  //disconnect from the old image
  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
  }

  //if there isn't a new image available
  if(!currentImage)
    return;

  //Set new current image
  mCurrentImage = currentImage;
  if(mCurrentImage)// Don't use image if deleted
  {
    connect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
            this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
            this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
  }

  //set a default treshold
  mThresholdSlider->setRange(mCurrentImage->getPosMin(), mCurrentImage->getPosMax());
  ProbeRepPtr probeRep = mRepManager->getProbeRep("ProbeRep_1");
  mThresholdSlider->setValue(probeRep->getThreshold());

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);

  //enable the add point button
  if (mCurrentImage)
    mAddLandmarkButton->setEnabled(true);
  else
    mAddLandmarkButton->setEnabled(false);
}
void ImageRegistrationWidget::addLandmarkButtonClickedSlot()
{
  ProbeRepPtr probeRep = mRepManager->getProbeRep("ProbeRep_1");
  if(!probeRep)
  {
    messageManager()->sendError("Could not find a rep to add the landmark to.");
    return;
  }
  int index = mLandmarkTableWidget->rowCount()+1;
  probeRep->makeLandmarkPermanent(index);
}
void ImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
  ProbeRepPtr probeRep = mRepManager->getProbeRep("ProbeRep_1");
  if(!probeRep)
  {
    messageManager()->sendError("Could not find a rep to edit the landmark for.");
    return;
  }
  int index = mCurrentRow+1;
  probeRep->makeLandmarkPermanent(index);
}
void ImageRegistrationWidget::removeLandmarkButtonClickedSlot()
{
  if(mCurrentRow < 0 || mCurrentColumn < 0)
    return;

  int index = mCurrentRow+1;

  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  landmarkRep->removePermanentPoint(index);
}
void ImageRegistrationWidget::imageLandmarksUpdateSlot(double notUsedX, double notUsedY, double notUsedZ, unsigned int notUsedIndex)
{
  //make sure the masterImage is set
  ssc::ImagePtr masterImage = mRegistrationManager->getMasterImage();
  if(!masterImage)
    mRegistrationManager->setMasterImage(mCurrentImage);
  
  //check if its time to do image registration
  if(mCurrentImage->getLandmarks()->GetNumberOfTuples() > 2)
  {
    messageManager()->sendInfo(mCurrentImage->getUid());
    mRegistrationManager->setGlobalPointSet(mCurrentImage->getLandmarks());
    mRegistrationManager->doImageRegistration(mCurrentImage);
  }
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void ImageRegistrationWidget::landmarkSelectedSlot(int row, int column)
{
  mCurrentRow = row;
  mCurrentColumn = column;

  mEditLandmarkButton->setEnabled(true);
}
void ImageRegistrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void ImageRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  if(!image) //Image is deleted
  {
    mLandmarkTableWidget->clear();
    return;
  }

  //get the landmarks from the image
  vtkDoubleArrayPtr landmarks =  image->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  //ready the table widget
  mLandmarkTableWidget->clear();
  mLandmarkTableWidget->setRowCount(0);
  mLandmarkTableWidget->setColumnCount(3);
  QStringList headerItems(QStringList() << "Name" << "Status" << "Landmark");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

  //fill the table widget with rows for the landmarks
  int row = 0;
  for(int i=0; i<numberOfLandmarks; i++)
  {
    double* landmark = landmarks->GetTuple(i);
    if(landmark[3] > mLandmarkTableWidget->rowCount())
      mLandmarkTableWidget->setRowCount(landmark[3]);
    QTableWidgetItem* columnOne; //Name
    QTableWidgetItem* columnTwo; //Status
    QTableWidgetItem* columnThree; //Image coordinates

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
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem(tr("(%1, %2, %3)").arg(landmark[0]).arg(landmark[1]).arg(landmark[2]));
      }
      else
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem();
      }
      //check the LandmarkActiveMap...
      //mapindex for landmarkActiveMap starts at 0 
      //and tablerow and coloumns start at 0,0
      RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
      int index = row+1;
      RegistrationManager::NameListType::iterator it = landmarkActiveMap.find(index);
      if(it != landmarkActiveMap.end())
      {
        if(!it->second.second)
          columnTwo->setCheckState(Qt::Unchecked);
        else
          columnTwo->setCheckState(Qt::Checked);
      }
      else //if the landmark wasn't in the map it should be added
      {
        //send update to registrationmanage
        mRegistrationManager->setGlobalPointsActiveSlot(index, true);
        columnTwo->setCheckState(Qt::Checked);
      }
      //Set item properties and add the items to the table
      //TODO dosnt work, makes every cell inactive...
      /*columnOne->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate); //name should be selectable and editable
      columnTwo->setFlags(!Qt::ItemIsEditable); //status shouldnt allow writing in the cell
      columnThree->setFlags(Qt::ItemIsSelectable &&
                            Qt::ItemIsEditable &&
                            !Qt::ItemIsDragEnabled &&
                            !Qt::ItemIsDropEnabled &&
                            !Qt::ItemIsUserCheckable &&
                            !Qt::ItemIsEnabled &&
                            !Qt::ItemIsTristate);//image coords should be selectable*/
      mLandmarkTableWidget->setItem(row, 0, columnOne);
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
      mLandmarkTableWidget->setItem(row, 2, columnThree);
    }
    if(tempRow != -1)
      row = tempRow;
  }
  //get globalPointsNameList from the RegistrationManager
  RegistrationManager::NameListType nameList = mRegistrationManager->getGlobalPointSetNameList();
  //fill in names
  typedef RegistrationManager::NameListType::iterator Iterator;
  for(Iterator it = nameList.begin(); it != nameList.end(); ++it)
  {
    std::string name = it->second.first;
    int index = it->first;
    int row = index-1;
    QTableWidgetItem* columnOne;

    if(index > mLandmarkTableWidget->rowCount())
    {
      mLandmarkTableWidget->setRowCount(index);
      columnOne = new QTableWidgetItem();
      QTableWidgetItem* columnThree = new QTableWidgetItem();
      
      //Set item properties and add the items to the table
      //TODO dosnt work, makes every cell inactive...
      /*columnOne->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate); //name should be selectable and editable
      columnThree->setFlags(Qt::ItemIsSelectable &&
                            !Qt::ItemIsEditable &&
                            !Qt::ItemIsDragEnabled &&
                            !Qt::ItemIsDropEnabled &&
                            !Qt::ItemIsUserCheckable &&
                            !Qt::ItemIsEnabled &&
                            !Qt::ItemIsTristate);//image coords should be selectable*/
      mLandmarkTableWidget->setItem(row, 1, columnOne);
      mLandmarkTableWidget->setItem(row, 2, columnThree);
    }
    else
    {
      columnOne = mLandmarkTableWidget->item(row, 1);
    }
    if(columnOne != NULL && !name.empty())
      columnOne->setText(QString(name.c_str()));
  }

  //highlight selected row
  if(mCurrentRow != -1 && mCurrentColumn != -1)
    mLandmarkTableWidget->setCurrentCell(mCurrentRow, mCurrentColumn);

  //update buttons
  if(numberOfLandmarks == 0)
  {
    mRemoveLandmarkButton->setDisabled(true);
    mEditLandmarkButton->setDisabled(true);
  }
  else
  {
    mRemoveLandmarkButton->setEnabled(true);
    if(mCurrentRow != -1 && mCurrentColumn != -1)
      mEditLandmarkButton->setEnabled(true);
  }
}
void ImageRegistrationWidget::cellChangedSlot(int row,int column)
{
  if(column==1)
  {
    Qt::CheckState state = mLandmarkTableWidget->item(row,column)->checkState();
    mRegistrationManager->setGlobalPointsActiveSlot(row, state);
  }

  if(column==0)
  {
    std::string name = mLandmarkTableWidget->item(row, column)->text().toStdString();
    int index = row+1;
    mRegistrationManager->setGlobalPointsNameSlot(index, name);
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
}//namespace cx
