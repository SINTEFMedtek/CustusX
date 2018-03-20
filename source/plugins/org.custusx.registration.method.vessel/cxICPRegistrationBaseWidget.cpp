/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxICPRegistrationBaseWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <vtkCellArray.h>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationProperties.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxMesh.h"
#include "cxView.h"
#include "cxGeometricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxICPWidget.h"
#include "cxMeshInView.h"
#include "cxWidgetObscuredListener.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"
#include "cxProfile.h"

namespace cx
{

ICPRegistrationBaseWidget::ICPRegistrationBaseWidget(RegServicesPtr services, QWidget* parent, QString uid, QString name) :
	RegistrationBaseWidget(services, parent, uid, name)
{
	this->setLayout(new QVBoxLayout); // we need something, otherwise the widget might not be painted at all.
}

ICPRegistrationBaseWidget::~ICPRegistrationBaseWidget()
{
}

void ICPRegistrationBaseWidget::prePaintEvent()
{
	if (!mRegistrator)
	{
		this->initialize();
		this->setup();
	}

	this->initializeRegistrator();

	mICPWidget->enableRegistration(mRegistrator->isValid());
	this->updateDifferenceLines();
	mICPWidget->setRMS(mRegistrator->getResultMetric());
}

void ICPRegistrationBaseWidget::initialize()
{
	delete this->layout(); // dummy layout from construct

	mRegistrator.reset(new SeansVesselReg());
	mOptions = profile()->getXmlSettings().descend("uid");

	this->initializeProperties();

	mICPWidget = new ICPWidget(this);
	mICPWidget->setSettings(this->getAllProperties());
	connect(mICPWidget, &ICPWidget::requestRegister, this, &ICPRegistrationBaseWidget::registerSlot);

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));
}


void ICPRegistrationBaseWidget::initializeProperties()
{
	mAutoLTS = BoolProperty::initialize("autoLTS","Auto LTS",
										"Ignore LTS, instead attempt to find optimal value",
									   this->getDefaultAutoLTS(), mOptions.getElement());
	connect(mAutoLTS.get(), &DoubleProperty::changed, this, &ICPRegistrationBaseWidget::onSettingsChanged);

	mLTSRatio = DoubleProperty::initialize("LTSRatio", "LTS Ratio",
										   "Fraction of points in the lesser point set to use during each iteration.",
										   80, DoubleRange(20,100,1), 0, mOptions.getElement());
	connect(mLTSRatio.get(), &DoubleProperty::changed, this, &ICPRegistrationBaseWidget::onSettingsChanged);

	mNumberOfIterations = DoubleProperty::initialize("Iterations", "Iterations",
										   "Number of iterations",
										   500, DoubleRange(1,1000,1), 0, mOptions.getElement());
	connect(mNumberOfIterations.get(), &DoubleProperty::changed, this, &ICPRegistrationBaseWidget::onSettingsChanged);

	mStopThreshold = DoubleProperty::initialize("StopThreshold", "Stop Threshold",
										   "Differential RMS stop threshold, between iterations",
										   0.001, DoubleRange(0.0001,1,0.0001), 4, mOptions.getElement());
	connect(mStopThreshold.get(), &DoubleProperty::changed, this, &ICPRegistrationBaseWidget::onSettingsChanged);

	mMaxTime = DoubleProperty::initialize("MaxTime", "Max Time",
										   "Maximum time spent iterating, in seconds",
										   60, DoubleRange(1,300,1), 0, mOptions.getElement());
	connect(mMaxTime.get(), &DoubleProperty::changed, this, &ICPRegistrationBaseWidget::onSettingsChanged);

	mMargin = DoubleProperty::initialize("Margin", "Margin",
										   "Data outside a bounding box defined by the intersection\n"
										   "of the fixed/moving by a margin are cropped.",
										   40, DoubleRange(10,100,1), 0, mOptions.getElement());
	connect(mMargin.get(), &DoubleProperty::changed, this, &ICPRegistrationBaseWidget::onSettingsChanged);

	mLinear = BoolProperty::initialize("linear","Linear",
									   "Use only linear iteration",
									   true, mOptions.getElement());
	connect(mLinear.get(), &DoubleProperty::changed, this, &ICPRegistrationBaseWidget::onSettingsChanged);

	mDisplayProgress = BoolProperty::initialize("progress","Display Progress",
												"Display metric and difference lines between point sets",
									   true, mOptions.getElement());
	connect(mDisplayProgress.get(), &DoubleProperty::changed, this, &ICPRegistrationBaseWidget::onDisplayProgressChanged);

	mOneStep = BoolProperty::initialize("onestep","One Step",
										"Registration is done one iteration at a time.",
									   false); // note: do not store the value in this case: always reset to default.
}

std::vector<PropertyPtr> ICPRegistrationBaseWidget::getAllProperties()
{
	std::vector<PropertyPtr> properties;
	properties.push_back(mLTSRatio);
	properties.push_back(mAutoLTS);
	properties.push_back(mMargin);
	properties.push_back(mLinear);
	properties.push_back(mDisplayProgress);
	properties.push_back(mOneStep);
	properties.push_back(mNumberOfIterations);
	properties.push_back(mStopThreshold);
	properties.push_back(mMaxTime);
	return properties;
}

void ICPRegistrationBaseWidget::obscuredSlot(bool obscured)
{
	if (obscured)
	{
		mMeshInView.reset();
	}
	else
	{
		this->onSettingsChanged();
		this->onShown();
	}
}

void ICPRegistrationBaseWidget::onSpacesChanged()
{
	if (mObscuredListener->isObscured())
		return;

	this->setModified();
//	this->initializeRegistrator();

//	mICPWidget->enableRegistration(mRegistrator->isValid());
//	this->updateDifferenceLines();
//	mICPWidget->setRMS(mRegistrator->getResultMetric());
}

void ICPRegistrationBaseWidget::onSettingsChanged()
{
	if (mObscuredListener->isObscured())
		return;
	mRegistrator->mt_auto_lts = mAutoLTS->getValue();
	mRegistrator->mt_ltsRatio = mLTSRatio->getValue();
	mRegistrator->mt_doOnlyLinear = mLinear->getValue();
	mRegistrator->margin = mMargin->getValue();
	mRegistrator->mt_maximumNumberOfIterations = mNumberOfIterations->getValue();
	mRegistrator->mt_distanceDeltaStopThreshold = mStopThreshold->getValue();
	mRegistrator->mt_maximumDurationSeconds = mMaxTime->getValue();
}

void ICPRegistrationBaseWidget::registerSlot()
{
//	int lts_ratio = mLTSRatioSpinBox->value();
//	double stop_delta = 0.001; //TODO, add user interface
//	double lambda = 0; //TODO, add user interface
//	double sigma = 1.0; //TODO, add user interface
//	bool lin_flag = mLinearCheckBox->isChecked(); //TODO, add user interface
//	int sample = 1; //TODO, add user interface
//	int single_point_thre = 1; //TODO, add user interface
//	bool verbose = 1; //TODO, add user interface

	mRegistrator->notifyPreRegistrationWarnings();

	if (mRegistrator->mt_auto_lts)
	{
		reportDebug("Using automatic lts_ratio");
	}
	else
	{
		reportDebug("Using lts_ratio: " + qstring_cast(mRegistrator->mt_ltsRatio));
	}

	bool success = false;
	if (mOneStep->getValue())
		success = mRegistrator->performOneRegistration();
	else
		success = mRegistrator->execute();

	if (!success)
	{
		reportWarning("ICP registration failed.");
		return;
	}

	Transform3D linearTransform = mRegistrator->getLinearResult();

	std::cout << "v2v linear result:\n" << linearTransform << std::endl;
	//std::cout << "v2v inverted linear result:\n" << linearTransform.inverse() << std::endl;

	if ((boost::math::isnan)(linearTransform(0,0)))
	{
		reportWarning("ICP registration failed.");
		return;
	}

	mRegistrator->checkQuality(linearTransform);

	// The registration is performed in space r. Thus, given an old data position rMd, we find the
	// new one as rM'd = Q * rMd, where Q is the inverted registration output.
	// Delta is thus equal to Q:
	Transform3D delta = linearTransform.inv();
	//std::cout << "delta:\n" << delta << std::endl;

	this->applyRegistration(delta);
}


void ICPRegistrationBaseWidget::updateDifferenceLines()
{
	if (!mMeshInView)
		mMeshInView.reset(new MeshInView(mServices->view()));

	bool show = mDisplayProgress->getValue() && mRegistrator->isValid();
	if (show)
		mMeshInView->show(mRegistrator->getDifferenceLines());
	else
		mMeshInView->hide();
}

void ICPRegistrationBaseWidget::onDisplayProgressChanged()
{
	this->updateDifferenceLines();
}


} // namespace cx
