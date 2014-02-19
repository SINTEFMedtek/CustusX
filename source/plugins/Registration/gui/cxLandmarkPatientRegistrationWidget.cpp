#include "cxLandmarkPatientRegistrationWidget.h"

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
#include "cxViewManager.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "cxRegistrationManager.h"
#include "sscToolManager.h"
#include "cxDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxRepManager.h"
#include "cxLandmarkRep.h"
#include "sscView.h"

namespace cx
{
LandmarkPatientRegistrationWidget::LandmarkPatientRegistrationWidget(RegistrationManagerPtr regManager,
	QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(regManager, parent, objectName, windowTitle), mToolSampleButton(new QPushButton(
		"Sample Tool", this))
{
	mImageLandmarkSource = ImageLandmarksSource::New();
	mFixedDataAdapter.reset(new RegistrationFixedImageStringDataAdapter(regManager));
	connect(mManager.get(), SIGNAL(fixedDataChanged(QString)), this, SLOT(fixedDataChanged()));
	connect(toolManager(), SIGNAL(rMprChanged()), this, SLOT(setModified()));

	//buttons
	mToolSampleButton->setDisabled(true);
	connect(mToolSampleButton, SIGNAL(clicked()), this, SLOT(toolSampleButtonClickedSlot()));

	mRemoveLandmarkButton = new QPushButton("Clear", this);
	mRemoveLandmarkButton->setToolTip("Clear selected landmark");
	//  mRemoveLandmarkButton->setDisabled(true);
	connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

	mRegisterButton = new QPushButton("Register", this);
	mRegisterButton->setToolTip("Perform registration");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

	//toolmanager
	mDominantToolProxy = DominantToolProxy::New();
	connect(mDominantToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolSampleButton()));
	connect(mDominantToolProxy.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(updateToolSampleButton()));
	connect(cxDataManager::getInstance(), SIGNAL(debugModeChanged(bool)), this, SLOT(updateToolSampleButton()));

	//layout
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mFixedDataAdapter));
	mVerticalLayout->addWidget(mLandmarkTableWidget);
	mVerticalLayout->addWidget(mToolSampleButton);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(mRegisterButton);
	buttonsLayout->addWidget(mRemoveLandmarkButton);
	mVerticalLayout->addLayout(buttonsLayout);

	this->updateToolSampleButton();
}

LandmarkPatientRegistrationWidget::~LandmarkPatientRegistrationWidget()
{
}

QString LandmarkPatientRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Landmark based patient registration.</h3>"
		"<p>Sample points on the patient that corresponds to 3 or more landmarks already sampled in the data set. </p>"
		"<p><i>Point on the patient using a tool and click the Sample button.</i></p>"
		"<p>Landmark patient registration will move the patient into the global coordinate system (r).</p>"
		"</html>";
}

void LandmarkPatientRegistrationWidget::registerSlot()
{
	this->performRegistration();
}

void LandmarkPatientRegistrationWidget::fixedDataChanged()
{
	LandmarkRegistrationWidget::activeImageChangedSlot();
//	ImagePtr image = dataManager()->getActiveImage();
	ImagePtr image = boost::dynamic_pointer_cast<Image>(mManager->getFixedData());
	mImageLandmarkSource->setImage(image);
}

void LandmarkPatientRegistrationWidget::updateToolSampleButton()
{
	ToolPtr tool = toolManager()->getDominantTool();

	bool enabled = false;
	enabled = tool && tool->getVisible() && (!tool->hasType(Tool::TOOL_MANUAL) || cxDataManager::getInstance()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.
	mToolSampleButton->setEnabled(enabled);

	if (toolManager()->getDominantTool())
		mToolSampleButton->setText("Sample " + qstring_cast(tool->getName()));
	else
		mToolSampleButton->setText("No tool");
}

void LandmarkPatientRegistrationWidget::toolSampleButtonClickedSlot()
{
	ToolPtr tool = toolManager()->getDominantTool();

	if (!tool)
	{
		messageManager()->sendError("mToolToSample is NULL!");
		return;
	}
	//TODO What if the reference frame isnt visible?
	Transform3D lastTransform_prMt = tool->get_prMt();
	Vector3D p_pr = lastTransform_prMt.coord(Vector3D(0, 0, tool->getTooltipOffset()));

	// TODO: do we want to allow sampling points not defined in image??
	if (mActiveLandmark.isEmpty() && !dataManager()->getLandmarkProperties().empty())
		mActiveLandmark = dataManager()->getLandmarkProperties().begin()->first;

	toolManager()->setLandmark(Landmark(mActiveLandmark, p_pr));
	messageManager()->playSampleSound();

    this->activateLandmark(this->getNextLandmark());

	this->performRegistration(); // automatic when sampling in physical patient space (Mantis #0000674)s
}

void LandmarkPatientRegistrationWidget::showEvent(QShowEvent* event)
{
//	std::cout << "LandmarkPatientRegistrationWidget::showEvent" << std::endl;
	LandmarkRegistrationWidget::showEvent(event);
	connect(toolManager(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
	connect(toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

	viewManager()->setRegistrationMode(rsPATIENT_REGISTRATED);

	LandmarkRepPtr rep = RepManager::findFirstRep<LandmarkRep>(viewManager()->get3DView(0, 0)->getReps());
	if (rep)
	{
		rep->setPrimarySource(mImageLandmarkSource);
		rep->setSecondarySource(PatientLandmarksSource::New());
		rep->setSecondaryColor(QColor::fromRgbF(0, 0.6, 0.8));
	}
}

void LandmarkPatientRegistrationWidget::hideEvent(QHideEvent* event)
{
//	std::cout << "LandmarkPatientRegistrationWidget::hideEvent" << std::endl;
	LandmarkRegistrationWidget::hideEvent(event);
	disconnect(toolManager(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
	disconnect(toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

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

void LandmarkPatientRegistrationWidget::removeLandmarkButtonClickedSlot()
{
    QString next = this->getNextLandmark();
	toolManager()->removeLandmark(mActiveLandmark);
    this->activateLandmark(next);
}

void LandmarkPatientRegistrationWidget::cellClickedSlot(int row, int column)
{
	LandmarkRegistrationWidget::cellClickedSlot(row, column);

	mRemoveLandmarkButton->setEnabled(true);
}

void LandmarkPatientRegistrationWidget::prePaintEvent()
{
    LandmarkRegistrationWidget::prePaintEvent();

	std::vector<Landmark> landmarks = this->getAllLandmarks();
	mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
}

/** Return the landmarks associated with the current widget.
 */
LandmarkMap LandmarkPatientRegistrationWidget::getTargetLandmarks() const
{
	return toolManager()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D LandmarkPatientRegistrationWidget::getTargetTransform() const
{
	Transform3D rMpr = *(toolManager()->get_rMpr());
	return rMpr;
}

void LandmarkPatientRegistrationWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	toolManager()->setLandmark(Landmark(uid, p_target));
	messageManager()->playSampleSound();
}

void LandmarkPatientRegistrationWidget::performRegistration()
{
	if (!mManager->getFixedData())
		mManager->setFixedData(dataManager()->getActiveImage());

	if (toolManager()->getLandmarks().size() < 3)
		return;

	mManager->doPatientRegistration();

	this->updateAvarageAccuracyLabel();
}

QString LandmarkPatientRegistrationWidget::getTargetName() const
{
	return "Patient";
}

}//namespace cx
