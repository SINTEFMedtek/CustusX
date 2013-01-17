// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscReconstructer.h"

#include <algorithm>
#include <QtCore>
#include <vtkImageData.h>
#include "sscBoundingBox3D.h"
#include "sscDataManager.h"
#include "sscXmlOptionItem.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscPNNReconstructAlgorithm.h"
#include "utils/sscReconstructHelper.h"
#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscUtilHelpers.h"
#include "sscVolumeHelpers.h"
#include "sscPresetTransferFunctions3D.h"
#include "sscManualTool.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ssc
{

ReconstructParams::ReconstructParams(XmlOptionFile settings)
{
	mSettings = settings;
	mSettings.getElement("algorithms");

	mOrientationAdapter = StringDataAdapterXml::initialize("Orientation", "",
		"Algorithm to use for output volume orientation", "MiddleFrame",
		QString("PatientReference MiddleFrame").split(" "),
		mSettings.getElement());
	connect(mOrientationAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	ssc::PresetTransferFunctions3DPtr presets = ssc::dataManager()->getPresetTransferFunctions3D();
	mPresetTFAdapter = StringDataAdapterXml::initialize("Preset", "",
		"Preset transfer function to apply to the reconstructed volume", "US B-Mode", presets->getPresetList("US"),
		mSettings.getElement());

	connect(mPresetTFAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(transferFunctionChanged()));
	//connect(mPresetTFAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mMaskReduce = StringDataAdapterXml::initialize("Reduce mask (% in 1D)", "",
		"Speedup by reducing mask size", "3",
		QString("0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15").split(" "),
		mSettings.getElement());
	connect(mMaskReduce.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mAlignTimestamps = BoolDataAdapterXml::initialize("Align timestamps", "",
		"Align the first of tracker and frame timestamps, ignoring lags.", false,
		mSettings.getElement());
	connect(mAlignTimestamps.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mTimeCalibration = DoubleDataAdapterXml::initialize("Extra Temporal Calib", "",
		"Set an offset in the frame timestamps, in addition to the one used in acquisition", 0.0,
		DoubleRange(-1000, 1000, 10), 0,
		mSettings.getElement());
	connect(mTimeCalibration.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	double maxVolumeSizeFactor = 1024*1024;
	mMaxVolumeSize = DoubleDataAdapterXml::initialize("Volume Size", "",
		"Output Volume Size (Mb)", 32*maxVolumeSizeFactor,
		DoubleRange(maxVolumeSizeFactor, maxVolumeSizeFactor*500, maxVolumeSizeFactor), 0,
		mSettings.getElement());
	mMaxVolumeSize->setInternal2Display(1.0/maxVolumeSizeFactor);
	connect(mMaxVolumeSize.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mAngioAdapter = BoolDataAdapterXml::initialize("Angio data", "",
		"Ultrasound angio data is used as input", false,
		mSettings.getElement());
	connect(mAngioAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mCreateBModeWhenAngio = BoolDataAdapterXml::initialize("Dual Angio", "",
		"If angio requested, also create a B-mode reconstruction based on the same data set.", true,
		mSettings.getElement());
	connect(mCreateBModeWhenAngio.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mAlgorithmAdapter = StringDataAdapterXml::initialize("Algorithm", "", "Choose algorithm to use for reconstruction",
		"PNN", QString("ThunderVNN PNN").split(" "),
		mSettings.getElement());
	connect(mAlgorithmAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));
}

ReconstructParams::~ReconstructParams()
{
	mSettings.save();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

Reconstructer::Reconstructer(XmlOptionFile settings, QString shaderPath) :
	mOutputRelativePath(""), mOutputBasePath(""), mShaderPath(shaderPath)
{
	mSettings = settings;
	mSettings.getElement("algorithms");

	mParams.reset(new ReconstructParams(settings));
	connect(mParams.get(), SIGNAL(changedInputSettings()), this, SLOT(setSettings()));
	connect(mParams.get(), SIGNAL(transferFunctionChanged()), this, SLOT(transferFunctionChangedSlot()));

	createAlgorithm();
}

Reconstructer::~Reconstructer()
{
}


void Reconstructer::createAlgorithm()
{
	QString name = mParams->mAlgorithmAdapter->getValue();

	ReconstructCorePtr core; ///< in progress: algorithm part of class moved here.
	core.reset(new ReconstructCore());
	ReconstructAlgorithmPtr algo = core->createAlgorithm(name);

	// generate settings for new algo
	if (algo)
	{
		QDomElement element = mSettings.getElement("algorithms", algo->getName());
		mAlgoOptions = algo->getSettings(element);
		emit algorithmChanged();
	}
}

void Reconstructer::setSettings()
{
	this->createAlgorithm();
	this->updateFromOriginalFileData();
	emit paramsChanged();
}
void Reconstructer::transferFunctionChangedSlot()
{
	//Use angio reconstruction also if only transfer function is set to angio
	if(mParams->mPresetTFAdapter->getValue() == "US Angio")
	{
		ssc::messageManager()->sendDebug("Reconstructing angio (Because of angio transfer function)");
		mParams->mAngioAdapter->setValue(true);
	}
	else if(mParams->mPresetTFAdapter->getValue() == "US B-Mode" && mParams->mAngioAdapter->getValue())
	{
		ssc::messageManager()->sendDebug("Not reconstructing angio (Because of B-Mode transfer function)");
		mParams->mAngioAdapter->setValue(false);
	}
}
void Reconstructer::clearAll()
{
	mFileData = ssc::USReconstructInputData();
	mOriginalFileData = ssc::USReconstructInputData();
	mOutputVolumeParams = OutputVolumeParams();
}

OutputVolumeParams Reconstructer::getOutputVolumeParams() const
{
	return mOutputVolumeParams;
}

void Reconstructer::setOutputVolumeParams(const OutputVolumeParams& par)
{
	mOutputVolumeParams = par;
	this->setSettings();
}

void Reconstructer::setOutputRelativePath(QString path)
{
	mOutputRelativePath = path;
}

void Reconstructer::setOutputBasePath(QString path)
{
	mOutputBasePath = path;
}


bool Reconstructer::validInputData() const
{
	if (mOriginalFileData.mFrames.empty() || !mOriginalFileData.mUsRaw || mOriginalFileData.mPositions.empty())
		return false;
	if(mOriginalFileData.mUsRaw->is4D())
	{
		ssc::messageManager()->sendWarning("US reconstructer do not handle 4D US data");
		return false;
	}
	return true;
}


void Reconstructer::setInputData(ssc::USReconstructInputData fileData)
{
	this->clearAll();
	mOriginalFileData = fileData;
	this->updateFromOriginalFileData();
	emit inputDataSelected(fileData.mFilename);
}


/**Use the mOriginalFileData structure to rebuild all internal data.
 * Useful when settings have changed or data is loaded.
 */
void Reconstructer::updateFromOriginalFileData()
{
	if (!this->validInputData())
		return;

	ReconstructCorePtr core = this->createCore();
	mOutputVolumeParams = core->getOutputVolumeParams();

	emit paramsChanged();
}

ReconstructCore::InputParams Reconstructer::createCoreParameters()
{
	ReconstructCore::InputParams par;
	par.mAlgorithmUid = mParams->mAlgorithmAdapter->getValue();
	par.mAlgoSettings = mSettings.getElement("algorithms", par.mAlgorithmUid).cloneNode(true).toElement();
	par.mOutputBasePath = mOutputBasePath;
	par.mOutputRelativePath = mOutputRelativePath;
	par.mShaderPath = mShaderPath;
	par.mAngio = mParams->mAngioAdapter->getValue();
	par.mTransferFunctionPreset = mParams->mPresetTFAdapter->getValue();
	par.mMaxOutputVolumeSize = mParams->mMaxVolumeSize->getValue();
	par.mExtraTimeCalibration = mParams->mTimeCalibration->getValue();
	par.mAlignTimestamps = mParams->mAlignTimestamps->getValue();
	par.mMaskReduce = mParams->mMaskReduce->getValue().toDouble();
	par.mOrientation = mParams->mOrientationAdapter->getValue();
	return par;
}

ReconstructCorePtr Reconstructer::createCore()
{
	if (!this->validInputData())
		return ReconstructCorePtr();

	ReconstructCorePtr retval(new ReconstructCore());

	ReconstructCore::InputParams par = this->createCoreParameters();

	USReconstructInputData fileData = mOriginalFileData;
	fileData.mUsRaw = mOriginalFileData.mUsRaw->copy();

	retval->initialize(par, fileData);

	return retval;
}

ReconstructCorePtr Reconstructer::createDualCore()
{
	if (!this->validInputData())
		return ReconstructCorePtr();

	ReconstructCorePtr retval(new ReconstructCore());

	ReconstructCore::InputParams par = this->createCoreParameters();
	par.mAngio = false;
	par.mTransferFunctionPreset = "US B-Mode";

	USReconstructInputData fileData = mOriginalFileData;
	fileData.mUsRaw = mOriginalFileData.mUsRaw->copy();
	fileData.mUsRaw->setPurgeInputDataAfterInitialize(false);

	retval->initialize(par, fileData);

	return retval;
}




}
