#include "cxLandmarkImageRegistrationWidget.h"

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
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxPickerRep.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "cxViewManager.h"
#include "cxSettings.h"
#include "cxToolManager.h"
#include "cxLandmarkRep.h"
#include "cxView.h"
#include "cxTypeConversions.h"

namespace cx
{
LandmarkImageRegistrationWidget::LandmarkImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent,
	QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(regManager, parent, objectName, windowTitle)
{
	mCurrentDataAdapter = SelectDataStringDataAdapter::New();
	connect(mCurrentDataAdapter.get(), SIGNAL(changed()), this, SLOT(onCurrentImageChanged()));
	mImageLandmarkSource = ImageLandmarksSource::New();

	mDominantToolProxy = DominantToolProxy::New(trackingService());
	connect(mDominantToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(enableButtons()));
	connect(mDominantToolProxy.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(enableButtons()));

	//pushbuttons
	mAddLandmarkButton = new QPushButton("Add", this);
	mAddLandmarkButton->setToolTip("Add landmark");
	mAddLandmarkButton->setDisabled(true);
	connect(mAddLandmarkButton, SIGNAL(clicked()), this, SLOT(addLandmarkButtonClickedSlot()));

	mEditLandmarkButton = new QPushButton("Sample", this);
	mEditLandmarkButton->setToolTip("Resample landmark");
	mEditLandmarkButton->setDisabled(true);
	connect(mEditLandmarkButton, SIGNAL(clicked()), this, SLOT(editLandmarkButtonClickedSlot()));

	mRemoveLandmarkButton = new QPushButton("Clear", this);
	mRemoveLandmarkButton->setToolTip("Clear selected landmark");
	mRemoveLandmarkButton->setDisabled(true);
	connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

	//layout
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mCurrentDataAdapter));
	mVerticalLayout->addWidget(mLandmarkTableWidget);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);

	QHBoxLayout* landmarkButtonsLayout = new QHBoxLayout;
	landmarkButtonsLayout->addWidget(mAddLandmarkButton);
	landmarkButtonsLayout->addWidget(mEditLandmarkButton);
	landmarkButtonsLayout->addWidget(mRemoveLandmarkButton);
	mVerticalLayout->addLayout(landmarkButtonsLayout);
}

LandmarkImageRegistrationWidget::~LandmarkImageRegistrationWidget()
{
}

QString LandmarkImageRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Landmark based image registration.</h3>"
		"<p>Sample landmarks in the data set. </p>"
		"<p><i>Click the volume and either add or resample landmarks.</i></p>"
		"<p>Landmark image registration will move the active image to the fixed image</p>"
		"</html>";
}

void LandmarkImageRegistrationWidget::onCurrentImageChanged()
{
	DataPtr data = mCurrentDataAdapter->getData();

	mImageLandmarkSource->setData(data);
	this->enableButtons();

	if (data && !mManager->getFixedData())
		mManager->setFixedData(data);
}

PickerRepPtr LandmarkImageRegistrationWidget::getPickerRep()
{
	if (!viewManager()->get3DView(0, 0))
		return PickerRepPtr();

	return RepManager::findFirstRep<PickerRep>(viewManager()->get3DView(0, 0)->getReps());
}

DataPtr LandmarkImageRegistrationWidget::getCurrentData() const
{
	return mImageLandmarkSource->getData();
}

void LandmarkImageRegistrationWidget::addLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		reportError("Could not find a rep to add the landmark to.");
		return;
	}

	DataPtr image = this->getCurrentData();
	if (!image)
		return;

	QString uid = dataManager()->addLandmark();
	Vector3D pos_r = PickerRep->getPosition();
	Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
	image->getLandmarks()->setLandmark(Landmark(uid, pos_d));

    this->activateLandmark(uid);
}


void LandmarkImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		reportError("Could not find a rep to edit the landmark for.");
		return;
	}

	DataPtr image = this->getCurrentData();
	if (!image)
		return;

	QString uid = mActiveLandmark;
	Vector3D pos_r = PickerRep->getPosition();
	Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
	image->getLandmarks()->setLandmark(Landmark(uid, pos_d));

    this->activateLandmark(this->getNextLandmark());
}

void LandmarkImageRegistrationWidget::removeLandmarkButtonClickedSlot()
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;

    QString next = this->getNextLandmark();
	image->getLandmarks()->removeLandmark(mActiveLandmark);
    this->activateLandmark(next);
}

void LandmarkImageRegistrationWidget::cellClickedSlot(int row, int column)
{
	LandmarkRegistrationWidget::cellClickedSlot(row, column);
	this->enableButtons();
}

void LandmarkImageRegistrationWidget::enableButtons()
{
	bool selected = !mLandmarkTableWidget->selectedItems().isEmpty();
	bool loaded = this->getCurrentData() != 0;

	mEditLandmarkButton->setEnabled(selected);
	mRemoveLandmarkButton->setEnabled(selected);
	mAddLandmarkButton->setEnabled(loaded);

	DataPtr image = this->getCurrentData();
	if (image)
	{
		mAddLandmarkButton->setToolTip(QString("Add landmark to image %1").arg(image->getName()));
		mEditLandmarkButton->setToolTip(QString("Resample landmark in image %1").arg(image->getName()));
	}
//	this->setModified();
}

void LandmarkImageRegistrationWidget::showEvent(QShowEvent* event)
{
	LandmarkRegistrationWidget::showEvent(event);

	ImagePtr image = dataManager()->getActiveImage();
	if (image)
		mCurrentDataAdapter->setValue(image->getUid());
//	if (image && !mManager->getFixedData())
//		mManager->setFixedData(image);
//	if (image && !mImageLandmarkSource->getData())
//		mImageLandmarkSource->setData(image);

	viewManager()->setRegistrationMode(rsIMAGE_REGISTRATED);
	LandmarkRepPtr rep = RepManager::findFirstRep<LandmarkRep>(viewManager()->get3DView(0, 0)->getReps());
	if (rep)
	{
		rep->setPrimarySource(mImageLandmarkSource);
		rep->setSecondarySource(LandmarksSourcePtr());
	}
}

void LandmarkImageRegistrationWidget::hideEvent(QHideEvent* event)
{
	LandmarkRegistrationWidget::hideEvent(event);

	if(viewManager()->get3DView(0, 0))
	{
		LandmarkRepPtr rep = RepManager::findFirstRep<LandmarkRep>(viewManager()->get3DView(0, 0)->getReps());
		if (rep)
		{
			rep->setPrimarySource(LandmarksSourcePtr());
			rep->setSecondarySource(LandmarksSourcePtr());
		}
	}
	viewManager()->setRegistrationMode(rsNOT_REGISTRATED);
}

void LandmarkImageRegistrationWidget::prePaintEvent()
{
    LandmarkRegistrationWidget::prePaintEvent();

	std::vector<Landmark> landmarks = this->getAllLandmarks();

	//update buttons
	mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
	mEditLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
}

LandmarkMap LandmarkImageRegistrationWidget::getTargetLandmarks() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return LandmarkMap();

	return image->getLandmarks()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D LandmarkImageRegistrationWidget::getTargetTransform() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return Transform3D::Identity();
	return image->get_rMd();
}

void LandmarkImageRegistrationWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return;
	image->getLandmarks()->setLandmark(Landmark(uid, p_target));
}

QString LandmarkImageRegistrationWidget::getTargetName() const
{
	DataPtr image = this->getCurrentData();
	if (!image)
		return "None";
	return image->getName();
}


}//namespace cx
