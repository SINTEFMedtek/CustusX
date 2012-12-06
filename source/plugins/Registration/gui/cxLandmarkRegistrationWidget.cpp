#include "cxLandmarkRegistrationWidget.h"

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
#include "cxRegistrationManager.h"
#include "sscDataManager.h"
#include "cxView3D.h"
#include "cxView2D.h"
#include "cxRegistrationHistoryWidget.h"
#include "sscTypeConversions.h"
#include "sscManualTool.h"
#include "cxToolManager.h"

namespace cx
{
LandmarkRegistrationWidget::LandmarkRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent,
	QString objectName, QString windowTitle) :
	RegistrationBaseWidget(regManager, parent, objectName, windowTitle), mVerticalLayout(new QVBoxLayout(this)),
		mLandmarkTableWidget(new QTableWidget(this)), mAvarageAccuracyLabel(new QLabel(QString(" "), this))
{
	//table widget
	connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(cellClickedSlot(int, int)));
	connect(mLandmarkTableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(cellChangedSlot(int,int)));

	mActiveImageProxy = ActiveImageProxy::New();
	connect(mActiveImageProxy.get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
	connect(mActiveImageProxy.get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));
	connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));

	this->setLayout(mVerticalLayout);
}

LandmarkRegistrationWidget::~LandmarkRegistrationWidget()
{
}

QString LandmarkRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Registration.</h3>"
		"<p>Interface for registrating.</p>"
		"<p><i></i></p>"
		"</html>";
}

void LandmarkRegistrationWidget::activeImageChangedSlot()
{
	if (!this->isVisible())
		return;
	ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();
	if (mCurrentImage == activeImage)
		return;

	mCurrentImage = activeImage;

	//get the images landmarks and populate the landmark table
    this->setModified();
}

void LandmarkRegistrationWidget::cellClickedSlot(int row, int column)
{
	if (row < 0 || column < 0)
		return;

	if (!mLandmarkTableWidget)
		ssc::messageManager()->sendDebug("mLandmarkTableWidget is null");

	mActiveLandmark = mLandmarkTableWidget->item(row, column)->data(Qt::UserRole).toString();


	ssc::LandmarkMap targetData = this->getTargetLandmarks();
	if (targetData.count(mActiveLandmark))
	{
		ssc::Vector3D p_d = targetData[mActiveLandmark].getCoord();
		ssc::Vector3D p_r = this->getTargetTransform().coord(p_d);
		ssc::Vector3D p_pr = ssc::toolManager()->get_rMpr()->coord(p_r);;
		this->setManualToolPosition(p_r);
	}

}

void LandmarkRegistrationWidget::setManualToolPosition(ssc::Vector3D p_r)
{
	ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
	ssc::Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ssc::ManualToolPtr tool = ToolManager::getInstance()->getManualTool();
	ssc::Vector3D offset = tool->get_prMt().vector(ssc::Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	ssc::dataManager()->setCenter(p_r);
	ssc::Vector3D p0_pr = tool->get_prMt().coord(ssc::Vector3D(0, 0, 0));
	tool->set_prMt(ssc::createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
}

void LandmarkRegistrationWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	connect(ssc::dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(landmarkUpdatedSlot()));
	this->activeImageChangedSlot();

	mManager->restart();
    this->setModified();
}

void LandmarkRegistrationWidget::hideEvent(QHideEvent* event)
{
	QWidget::hideEvent(event);
	disconnect(ssc::dataManager(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(landmarkUpdatedSlot()));

	mCurrentImage.reset();
}

void LandmarkRegistrationWidget::prePaintEvent()
{
	mLandmarkTableWidget->blockSignals(true);
	mLandmarkTableWidget->clear();

	QString fixedName;
	ssc::DataPtr fixedData = boost::shared_dynamic_cast<ssc::Data>(mManager->getFixedData());
	if (fixedData)
		fixedName = fixedData->getName();

	// active image is irrelevant here: remove
//	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
//
//	if (!image) //Image is deleted
//		return;

	std::vector<ssc::Landmark> landmarks = this->getAllLandmarks();
	ssc::LandmarkMap targetData = this->getTargetLandmarks();
	ssc::Transform3D rMtarget = this->getTargetTransform();

	//ready the table widget
	mLandmarkTableWidget->setRowCount(landmarks.size());
	mLandmarkTableWidget->setColumnCount(4);
	QStringList headerItems(QStringList() << "Name" << "Status" << "Coordinates" << "Accuracy (mm)");
	mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
	mLandmarkTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	for (unsigned i = 0; i < landmarks.size(); ++i)
	{
		std::vector<QTableWidgetItem*> items(4); // name, status, coordinates, accuracy

		ssc::LandmarkProperty prop = ssc::dataManager()->getLandmarkProperties()[landmarks[i].getUid()];
		ssc::Vector3D coord = landmarks[i].getCoord();
		coord = rMtarget.coord(coord); // display coordinates in space r (in principle, this means all coords should be equal)

		items[0] = new QTableWidgetItem(qstring_cast(prop.getName()));
		items[0]->setToolTip(QString("Landmark name. Double-click to rename."));

		items[1] = new QTableWidgetItem;

		if (prop.getActive())
			items[1]->setCheckState(Qt::Checked);
		else
			items[1]->setCheckState(Qt::Unchecked);
		items[1]->setToolTip(QString("Check to use landmark in current registration."));

		QString coordText = "Not sampled";
		if (targetData.count(prop.getUid()))
		{
			int width = 5;
			int prec = 1;
			coordText = tr("(%1, %2, %3)").arg(coord[0], width, 'f', prec).arg(coord[1], width, 'f', prec).arg(
				coord[2], width, 'f', prec);
		}

		items[2] = new QTableWidgetItem(coordText);
		items[2]->setToolTip(QString("Landmark coordinates of target [%1] in reference space.").arg(this->getTargetName()));

		items[3] = new QTableWidgetItem(tr("%1").arg(this->getAccuracy(landmarks[i].getUid())));
		items[3]->setToolTip(QString("Distance from target [%1] to fixed [%2].").arg(this->getTargetName()).arg(fixedName));

		for (unsigned j = 0; j < items.size(); ++j)
		{
			items[j]->setData(Qt::UserRole, qstring_cast(prop.getUid()));
			mLandmarkTableWidget->setItem(i, j, items[j]);
		}

		//highlight selected row
		if (prop.getUid() == mActiveLandmark)
		{
			mLandmarkTableWidget->setCurrentItem(items[2]);
		}
	}

	this->updateAvarageAccuracyLabel();
	mLandmarkTableWidget->blockSignals(false);
}

//void LandmarkRegistrationWidget::nextRow()
//{
//    if (mLandmarkTableWidget->rowCount()==0)
//    {
//        std::cout << "LandmarkRegistrationWidget::nextRow() no rows!" << std::endl;
//        return;
//    }
//	int selectedRow = mLandmarkTableWidget->currentRow();

//	if (selectedRow == -1 && mLandmarkTableWidget->rowCount() != 0) //no row is selected yet
//	{
//		mLandmarkTableWidget->selectRow(0);
//		selectedRow = mLandmarkTableWidget->currentRow();
//	}

//	int nextRow = selectedRow + 1;
//	int lastRow = mLandmarkTableWidget->rowCount() - 1;
//	if (nextRow > lastRow)
//	{
//		nextRow = lastRow;
//	}

//	selectedRow = nextRow;
//	mLandmarkTableWidget->selectRow(selectedRow);
//	mLandmarkTableWidget->setCurrentCell(selectedRow, 0);

//	mActiveLandmark = mLandmarkTableWidget->currentItem()->data(Qt::UserRole).toString();
//}

void LandmarkRegistrationWidget::activateLandmark(QString uid)
{
    mActiveLandmark = uid;
    this->setModified();
}

/** Return the next landmark in the series of available landmarks,
  * beginning with the active landmark.
  */
QString LandmarkRegistrationWidget::getNextLandmark()
{
    std::vector<ssc::Landmark> lm = this->getAllLandmarks();

    for (int i=0; i<lm.size()-1; ++i)
    {
        if (lm[i].getUid()==mActiveLandmark)
        {
            return lm[i+1].getUid();
        }
    }

    return "";
}

std::vector<ssc::Landmark> LandmarkRegistrationWidget::getAllLandmarks() const
{
	std::vector<ssc::Landmark> retval;
	ssc::LandmarkMap targetData = this->getTargetLandmarks();
	std::map<QString, ssc::LandmarkProperty> dataData = ssc::dataManager()->getLandmarkProperties();
	std::map<QString, ssc::LandmarkProperty>::iterator iter;

	for (iter = dataData.begin(); iter != dataData.end(); ++iter)
	{
		if (targetData.count(iter->first))
			retval.push_back(targetData[iter->first]);
		else
			retval.push_back(ssc::Landmark(iter->first));
	}

	std::sort(retval.begin(), retval.end());

	return retval;
}

void LandmarkRegistrationWidget::cellChangedSlot(int row, int column)
{
	QTableWidgetItem* item = mLandmarkTableWidget->item(row, column);
	QString uid = item->data(Qt::UserRole).toString();

	if (column == 0)
	{
		QString name = item->text();
		ssc::dataManager()->setLandmarkName(uid, name);
	}
	if (column == 1)
	{
		Qt::CheckState state = item->checkState();
		ssc::dataManager()->setLandmarkActive(uid, state == Qt::Checked);
	}
	if (column == 2)
	{
		QString val = item->text();
		// remove formatting stuff:
		val = val.replace('(', " ");
		val = val.replace(')', " ");
		val = val.replace(',', " ");

		ssc::Transform3D rMtarget = this->getTargetTransform();

		ssc::Vector3D p_r = ssc::Vector3D::fromString(val);
		ssc::Vector3D p_target = rMtarget.inv().coord(p_r);
		this->setTargetLandmark(uid, p_target);
	}
}

void LandmarkRegistrationWidget::landmarkUpdatedSlot()
{
	this->performRegistration();
    this->setModified();
}

void LandmarkRegistrationWidget::updateAvarageAccuracyLabel()
{
	QString fixedName;
	ssc::DataPtr fixedData = boost::shared_dynamic_cast<ssc::Data>(mManager->getFixedData());
	if (fixedData)
		fixedName = fixedData->getName();

	mAvarageAccuracyLabel->setText(tr("Mean accuracy %1 mm").arg(this->getAvarageAccuracy()));
	mAvarageAccuracyLabel->setToolTip(QString("Average landmark accuracy from target [%1] to fixed [%2].").arg(this->getTargetName()).arg(fixedName));
}

double LandmarkRegistrationWidget::getAvarageAccuracy()
{
	std::map<QString, ssc::LandmarkProperty> props = ssc::dataManager()->getLandmarkProperties();

	double sum = 0;
	int count = 0;
	std::map<QString, ssc::LandmarkProperty>::iterator it = props.begin();
	for (; it != props.end(); ++it)
	{
		if (!it->second.getActive()) //we don't want to take into account not active landmarks
			continue;
		QString uid = it->first;
		double val = this->getAccuracy(uid);
		if (!ssc::similar(val, 1000.0))
		{
			sum = sum + val;
			count++;
		}
	}
	if (count == 0)
		return 1000;
	return sum / count;
}

double LandmarkRegistrationWidget::getAccuracy(QString uid)
{
	ssc::ImagePtr fixedData = boost::shared_dynamic_cast<ssc::Image>(mManager->getFixedData());
	if (!fixedData)
		return 1000.0;

	ssc::Landmark masterLandmark = fixedData->getLandmarks()[uid]; //TODO : sjekk ut masterimage etc etc
	ssc::Landmark targetLandmark = this->getTargetLandmarks()[uid];
	if (masterLandmark.getUid().isEmpty() || targetLandmark.getUid().isEmpty())
		return 1000.0;

	ssc::Vector3D p_master_master = masterLandmark.getCoord();
	ssc::Vector3D p_target_target = targetLandmark.getCoord();
	ssc::Transform3D rMmaster = fixedData->get_rMd();
	ssc::Transform3D rMtarget = this->getTargetTransform();

	ssc::Vector3D p_target_r = rMtarget.coord(p_target_target);
	ssc::Vector3D p_master_r = rMmaster.coord(p_master_master);

	return (p_target_r - p_master_r).length();
}

}//namespace cx
