/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxReconstructParams.h"

#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxDoubleRange.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx
{

ReconstructParams::ReconstructParams(PatientModelServicePtr patientModelService, XmlOptionFile settings) :
	mPatientModelService(patientModelService),
	mSettings(settings)
{
	connect(mPatientModelService.get(), &PatientModelService::patientChanged, this, &ReconstructParams::onPatientChanged);
}

ReconstructParams::~ReconstructParams()
{
	mSettings.save();
}

void ReconstructParams::onPatientChanged()
{
	if (mParameters.empty())
		return;

	PresetTransferFunctions3DPtr presets = mPatientModelService->getPresetTransferFunctions3D();
	QStringList presetList;
	if (presets)
	{
		presetList = presets->getPresetList(imUS);
		mPresetTFAdapter->setValueRange(presetList);
	}
}

void ReconstructParams::createParameters()
{
	if (!mParameters.empty())
		return;

	mSettings.getElement("algorithms");

	mOrientationAdapter = StringProperty::initialize("Orientation", "",
		"Algorithm to use for output volume orientation", "MiddleFrame",
		QString("PatientReference MiddleFrame").split(" "),
		mSettings.getElement());
	connect(mOrientationAdapter.get(), &StringProperty::valueWasSet, this, &ReconstructParams::changedInputSettings);
	this->add(mOrientationAdapter);

	// note: value range initialized by onPatientChanged()
	mPresetTFAdapter = StringProperty::initialize("Preset", "",
		"Preset transfer function to apply to the reconstructed volume", "US B-Mode", QStringList(),
		mSettings.getElement());
	connect(mPresetTFAdapter.get(), &StringProperty::valueWasSet, this, &ReconstructParams::transferFunctionChangedSlot);
	this->add(mPresetTFAdapter);

	mMaskReduce = StringProperty::initialize("Reduce mask (% in 1D)", "",
		"Speedup by reducing mask size", "3",
		QString("0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15").split(" "),
		mSettings.getElement());
	connect(mMaskReduce.get(), &StringProperty::valueWasSet, this, &ReconstructParams::changedInputSettings);
	this->add(mMaskReduce);

    mPosFilterStrength = StringProperty::initialize("Position Filter Strength", "",
        "Strength of position data prefiltering - 0 means off", "2",
        QString("0 1 2 3 4 5 6 7").split(" "),
        mSettings.getElement());
    connect(mPosFilterStrength.get(), &StringProperty::valueWasSet, this, &ReconstructParams::changedInputSettings);
    this->add(mPosFilterStrength);

    mAlignTimestamps = BoolProperty::initialize("Align timestamps", "",
		"Align the first of tracker and frame timestamps, ignoring lags.", false,
		mSettings.getElement());
	connect(mAlignTimestamps.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));
	this->add(mAlignTimestamps);

    mPositionThinning = BoolProperty::initialize("Position Thinning", "",
        "If 'on', tracking positions that deviate greatly from neighbours will be replaced with an interpolated value", false,
        mSettings.getElement());
    connect(mPositionThinning.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));
    this->add(mPositionThinning);

	mTimeCalibration = DoubleProperty::initialize("Extra Temporal Calib", "",
		"Set an offset in the frame timestamps, in addition to the one used in acquisition", 0.0,
		DoubleRange(-1000, 1000, 10), 0,
		mSettings.getElement());
	connect(mTimeCalibration.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));
	this->add(mTimeCalibration);

	double maxVolumeSizeFactor = 1024*1024;
	mMaxVolumeSize = DoubleProperty::initialize("Volume Size", "",
		"Output Volume Size (Mb)", 32*maxVolumeSizeFactor,
		DoubleRange(maxVolumeSizeFactor, maxVolumeSizeFactor*500, maxVolumeSizeFactor), 0,
		mSettings.getElement());
	mMaxVolumeSize->setInternal2Display(1.0/maxVolumeSizeFactor);
	connect(mMaxVolumeSize.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));
	this->add(mMaxVolumeSize);

	mAngioAdapter = BoolProperty::initialize("Angio data", "",
		"Ultrasound angio data is used as input", false,
		mSettings.getElement());
	connect(mAngioAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));
	this->add(mAngioAdapter);

	mCreateBModeWhenAngio = BoolProperty::initialize("Dual Angio", "",
		"If angio requested, also create a B-mode reconstruction based on the same data set.", true,
		mSettings.getElement());
	connect(mCreateBModeWhenAngio.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));
	this->add(mCreateBModeWhenAngio);

	mAlgorithmAdapter = StringProperty::initialize("Algorithm", "", "Choose algorithm to use for reconstruction",
			QString(), QStringList(), mSettings.getElement());
	connect(mAlgorithmAdapter.get(), &StringProperty::valueWasSet, this, &ReconstructParams::changedInputSettings);
	this->add(mAlgorithmAdapter);

	this->onPatientChanged();
}

void ReconstructParams::add(PropertyPtr param)
{
	mParameters[param->getUid()] = param;
}

PropertyPtr ReconstructParams::getParameter(QString uid)
{
	if (mParameters.empty())
		this->createParameters();
	if (mParameters.count(uid))
			return mParameters[uid];
	return PropertyPtr();
}

QStringList ReconstructParams::getParameterUids() const
{
	QStringList retval;
	for (std::map<QString, PropertyPtr>::const_iterator iter=mParameters.begin(); iter!=mParameters.end(); ++iter)
		retval << iter->first;
	return retval;
}

void ReconstructParams::transferFunctionChangedSlot()
{
	//Use angio reconstruction also if only transfer function is set to angio
	if(mPresetTFAdapter->getValue() == "US Angio")
	{
		reportDebug("Reconstructing angio (Because of angio transfer function)");
		mAngioAdapter->setValue(true);
	}
	else if(mPresetTFAdapter->getValue() == "US B-Mode" && mAngioAdapter->getValue())
	{
		reportDebug("Not reconstructing angio (Because of B-Mode transfer function)");
		mAngioAdapter->setValue(false);
	}
}



}
