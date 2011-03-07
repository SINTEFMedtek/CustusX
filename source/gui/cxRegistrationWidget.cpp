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
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "sscDataManager.h"
#include "cxView3D.h"
#include "cxView2D.h"
#include "cxRegistrationHistoryWidget.h"
#include "sscTypeConversions.h"


namespace cx
{
RegistrationWidget::RegistrationWidget(QWidget* parent) :
  WhatsThisWidget(parent),
  mVerticalLayout(new QVBoxLayout(this)),
  mLandmarkTableWidget(new QTableWidget(this)),
  mAvarageAccuracyLabel(new QLabel(QString(" "), this))
{
  //widget
  this->setObjectName("RegistrationWidget");
  this->setWindowTitle("Registration");

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(cellClickedSlot(int, int)));
  connect(mLandmarkTableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(cellChangedSlot(int,int)));

  this->setLayout(mVerticalLayout);
}

RegistrationWidget::~RegistrationWidget()
{
}

void RegistrationWidget::activeImageChangedSlot()
{
  if (!this->isVisible())
    return;
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
  if(mCurrentImage == activeImage)
    return;

  //disconnect from the old image
  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));
  }

  mCurrentImage = activeImage;

  if(mCurrentImage)
  {
    connect(mCurrentImage.get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
    connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));
  }

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void RegistrationWidget::cellClickedSlot(int row, int column)
{
  if(row < 0 || column < 0)
     return;

  if(!mLandmarkTableWidget)
    ssc::messageManager()->sendDebug("mLandmarkTableWidget is null");

  mActiveLandmark = mLandmarkTableWidget->item(row, column)->data(Qt::UserRole).toString();
}

void RegistrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  connect(ssc::dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(landmarkUpdatedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  this->activeImageChangedSlot();

  registrationManager()->initialize();
  this->populateTheLandmarkTableWidget(mCurrentImage);
}


void RegistrationWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
  disconnect(ssc::dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(landmarkUpdatedSlot()));
  disconnect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));

  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));
  }
}

void RegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  mLandmarkTableWidget->blockSignals(true);
  mLandmarkTableWidget->clear();

  if(!image) //Image is deleted
    return;

  std::vector<ssc::Landmark> landmarks =  this->getAllLandmarks();
  ssc::LandmarkMap targetData = this->getTargetLandmarks();
  ssc::Transform3D rMtarget = this->getTargetTransform();

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

    ssc::LandmarkProperty prop = ssc::dataManager()->getLandmarkProperties()[landmarks[i].getUid()];
    ssc::Vector3D coord = landmarks[i].getCoord();
    coord = rMtarget.coord(coord); // display coordinates in space r (in principle, this means all coords should be equal)

    items[0] = new QTableWidgetItem(qstring_cast(prop.getName()));
    items[1] = new QTableWidgetItem;

    if (prop.getActive())
      items[1]->setCheckState(Qt::Checked);
    else
      items[1]->setCheckState(Qt::Unchecked);

    QString coordText = "Not sampled";
    if (targetData.count(prop.getUid()))
    {
      int width = 5;
      int prec = 1;
      coordText = tr("(%1, %2, %3)").arg(coord[0],width,'f',prec).arg(coord[1],width,'f',prec).arg(coord[2],width,'f',prec);
    }

    items[2] = new QTableWidgetItem(coordText);

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

  this->updateAvarageAccuracyLabel();
  mLandmarkTableWidget->blockSignals(false);
}

void RegistrationWidget::nextRow()
{
  int selectedRow = mLandmarkTableWidget->currentRow();

  if(selectedRow == -1 && mLandmarkTableWidget->rowCount() != 0) //no row is selected yet
  {
    mLandmarkTableWidget->selectRow(0);
    selectedRow = mLandmarkTableWidget->currentRow();
  }

  int nextRow = selectedRow+1;
  int lastRow = mLandmarkTableWidget->rowCount()-1;
  if(nextRow > lastRow)
  {
    nextRow = lastRow;
  }

  selectedRow = nextRow;
  mLandmarkTableWidget->selectRow(selectedRow);
  mLandmarkTableWidget->setCurrentCell(selectedRow, 0);

  mActiveLandmark = mLandmarkTableWidget->currentItem()->data(Qt::UserRole).toString();
}

std::vector<ssc::Landmark> RegistrationWidget::getAllLandmarks() const
{
  std::vector<ssc::Landmark> retval;
  ssc::LandmarkMap targetData = this->getTargetLandmarks();
  std::map<QString, ssc::LandmarkProperty> dataData = ssc::dataManager()->getLandmarkProperties();
  std::map<QString, ssc::LandmarkProperty>::iterator iter;

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
  QString uid = item->data(Qt::UserRole).toString();

  if(column==0)
  {
    QString name = item->text();
    ssc::dataManager()->setLandmarkName(uid, name);
  }
  if(column==1)
  {
    Qt::CheckState state = item->checkState();
    ssc::dataManager()->setLandmarkActive(uid, state==Qt::Checked);
  }
}

void RegistrationWidget::landmarkUpdatedSlot()
{
  this->performRegistration();
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void RegistrationWidget::updateAvarageAccuracyLabel()
{
  mAvarageAccuracyLabel->setText(tr("Average accuracy %1 mm").arg(this->getAvarageAccuracy()));
}

double RegistrationWidget::getAvarageAccuracy()
{
  std::map<QString, ssc::LandmarkProperty> props = ssc::dataManager()->getLandmarkProperties();

  double sum = 0;
  int count = 0;
  std::map<QString, ssc::LandmarkProperty>::iterator it = props.begin();
  for(; it != props.end(); ++it)
  {
    if(!it->second.getActive()) //we don't want to take into account not active landmarks
      continue;
    QString uid = it->first;
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

double RegistrationWidget::getAccuracy(QString uid)
{
  if (!mCurrentImage)
    return 0;

  ssc::ImagePtr fixedData = boost::shared_dynamic_cast<ssc::Image>(registrationManager()->getFixedData());
  if(!fixedData)
    return 1000.0;

  ssc::Landmark masterLandmark = fixedData->getLandmarks()[uid]; //TODO : sjekk ut masterimage etc etc
  ssc::Landmark targetLandmark = this->getTargetLandmarks()[uid];
  if(masterLandmark.getUid().isEmpty() || targetLandmark.getUid().isEmpty())
    return 1000.0;

  ssc::Vector3D p_master_master = masterLandmark.getCoord();
  ssc::Vector3D p_target_target = targetLandmark.getCoord();
  ssc::Transform3D rMmaster = fixedData->get_rMd();
  ssc::Transform3D rMtarget = this->getTargetTransform();

  ssc::Vector3D p_target_r = rMtarget.coord(p_target_target);
  ssc::Vector3D p_master_r = rMmaster.coord(p_master_master);

  return (p_target_r - p_master_r).length();

//  ssc::Vector3D diff = p_target_r - p_master_r;
//
//  double retval =
//      sqrt(pow(diff[0],2) +
//           pow(diff[1],2) +
//           pow(diff[2],2));
//
//  return retval;
}


}//namespace cx
