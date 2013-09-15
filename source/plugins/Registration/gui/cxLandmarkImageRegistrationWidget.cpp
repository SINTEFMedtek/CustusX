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
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscPickerRep.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "cxViewManager.h"
#include "cxSettings.h"
#include "sscToolManager.h"
#include "Rep/cxLandmarkRep.h"
#include "sscView.h"

namespace cx
{
LandmarkImageRegistrationWidget::LandmarkImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent,
	QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(regManager, parent, objectName, windowTitle)
{
	mActiveImageAdapter = ActiveImageStringDataAdapter::New();
	mImageLandmarkSource = ImageLandmarksSource::New();

	mDominantToolProxy = DominantToolProxy::New();
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
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mActiveImageAdapter));
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

void LandmarkImageRegistrationWidget::activeImageChangedSlot()
{
	LandmarkRegistrationWidget::activeImageChangedSlot();

	ImagePtr image = dataManager()->getActiveImage();

	if (image)
	{
		if (!mManager->getFixedData())
			mManager->setFixedData(image);
	}

	mImageLandmarkSource->setImage(image);

	//enable the add point button
	//  mAddLandmarkButton->setEnabled(image!=0);
	this->enableButtons();
}

PickerRepPtr LandmarkImageRegistrationWidget::getPickerRep()
{
	if (!viewManager()->get3DView(0, 0))
		return PickerRepPtr();

	return RepManager::findFirstRep<PickerRep>(viewManager()->get3DView(0, 0)->getReps());
}

void LandmarkImageRegistrationWidget::addLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		messageManager()->sendError("Could not find a rep to add the landmark to.");
		return;
	}

	ImagePtr image = dataManager()->getActiveImage();
	if (!image)
		return;

	QString uid = dataManager()->addLandmark();
	Vector3D pos_r = PickerRep->getPosition();
	Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
	image->setLandmark(Landmark(uid, pos_d));

    this->activateLandmark(uid);
}


void LandmarkImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
	PickerRepPtr PickerRep = this->getPickerRep();
	if (!PickerRep)
	{
		messageManager()->sendError("Could not find a rep to edit the landmark for.");
		return;
	}

	ImagePtr image = dataManager()->getActiveImage();
	if (!image)
		return;

	QString uid = mActiveLandmark;
	Vector3D pos_r = PickerRep->getPosition();
	Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
	image->setLandmark(Landmark(uid, pos_d));

    this->activateLandmark(this->getNextLandmark());
}

void LandmarkImageRegistrationWidget::removeLandmarkButtonClickedSlot()
{
	ImagePtr image = dataManager()->getActiveImage();
	if (!image)
		return;

    QString next = this->getNextLandmark();
    image->removeLandmark(mActiveLandmark);
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
//	bool tracking = toolManager()->getDominantTool() && !toolManager()->getDominantTool()->hasType(Tool::TOOL_MANUAL)
//		&& toolManager()->getDominantTool()->getVisible();
	bool loaded = dataManager()->getActiveImage() != 0;

	// you might want to add landmarks using the tracking pointer in rare cases.
	// Thus is must be allowed to do that.
//	mEditLandmarkButton->setEnabled(selected && !tracking);
//	mRemoveLandmarkButton->setEnabled(selected && !tracking);
//	mAddLandmarkButton->setEnabled(loaded && !tracking);

	mEditLandmarkButton->setEnabled(selected);
	mRemoveLandmarkButton->setEnabled(selected);
	mAddLandmarkButton->setEnabled(loaded);

	ImagePtr image = dataManager()->getActiveImage();
	if (image)
	{
		mAddLandmarkButton->setToolTip(QString("Add landmark to image %1").arg(image->getName()));
		mEditLandmarkButton->setToolTip(QString("Resample landmark in image %1").arg(image->getName()));
	}


}

void LandmarkImageRegistrationWidget::showEvent(QShowEvent* event)
{
	LandmarkRegistrationWidget::showEvent(event);

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
	ImagePtr image = dataManager()->getActiveImage();
	if (!image)
		return LandmarkMap();

	return image->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D LandmarkImageRegistrationWidget::getTargetTransform() const
{
	ImagePtr image = dataManager()->getActiveImage();
	if (!image)
		return Transform3D::Identity();
	return image->get_rMd();
}

void LandmarkImageRegistrationWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	ImagePtr image = dataManager()->getActiveImage();
	if (!image)
		return;
	image->setLandmark(Landmark(uid, p_target));
}

QString LandmarkImageRegistrationWidget::getTargetName() const
{
	ImagePtr image = dataManager()->getActiveImage();
	if (!image)
		return "None";
	return image->getName();
}


}//namespace cx
