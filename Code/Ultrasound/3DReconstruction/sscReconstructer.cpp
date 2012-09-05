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

/*
 *  sscReconstructer.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */
#include "sscReconstructer.h"

#include <algorithm>
#include <QtCore>
#include <vtkImageData.h>
//#include "matrixInterpolation.h"
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
//#include "cxCreateProbeDataFromConfiguration.h"
#include "sscVolumeHelpers.h"
//#include "cxUsReconstructionFileReader.h"
#include "sscPresetTransferFunctions3D.h"
//#include "cxToolManager.h"
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

	mAngioAdapter = BoolDataAdapterXml::initialize("Angio data", "",
		"Ultrasound angio data is used as input", false,
		mSettings.getElement());
	connect(mAngioAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

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
	mOutputVolumeParams(settings),
	mOutputRelativePath(""), mOutputBasePath(""), mShaderPath(shaderPath), mMaxTimeDiff(100),// TODO: Change default value for max allowed time difference between tracking and image time tags
	mIsReconstructing(false)
{
//	mFileReader.reset(new cx::UsReconstructionFileReader());
	mSuccess = false;

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

//std::map<QString, ReconstructionAlgorithmPtr> mLoadedAlgorithms;

void Reconstructer::createAlgorithm()
{
	QString name = mParams->mAlgorithmAdapter->getValue();

	if (mAlgorithm && mAlgorithm->getName() == name)
		return;

	// create new algo
	if (name == "ThunderVNN")
	{
		mAlgorithm = ReconstructAlgorithmPtr(new ThunderVNNReconstructAlgorithm(mShaderPath));
	}
	else if (name == "PNN")
		mAlgorithm = ReconstructAlgorithmPtr(new PNNReconstructAlgorithm());
	else
		mAlgorithm.reset();

	// generate settings for new algo
	if (mAlgorithm)
	{
		QDomElement algo = mSettings.getElement("algorithms", mAlgorithm->getName());
		mAlgoOptions = mAlgorithm->getSettings(algo);
		ssc::messageManager()->sendInfo("Using reconstruction algorithm " + mAlgorithm->getName());

		emit algorithmChanged();
	}
}

/**if called during reconstruction, emit a warning and
 * return true
 *
 */
bool Reconstructer::checkAndWarnForReconstruction()
{
	if (!mIsReconstructing)
		return false;

	ssc::messageManager()->sendWarning("Tried to change reconstruction parameters while reconstructing.");
	return true;
}

void Reconstructer::setSettings()
{
	if (this->checkAndWarnForReconstruction())
		return;

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
	mOutputVolumeParams = OutputVolumeParams(mSettings);
	this->clearOutput();
}

void Reconstructer::clearOutput()
{
	mOutput.reset();
}

OutputVolumeParams Reconstructer::getOutputVolumeParams() const
{
	return mOutputVolumeParams;
}

void Reconstructer::setOutputVolumeParams(const OutputVolumeParams& par)
{
	mOutputVolumeParams = par;
	  this->setSettings();
//	emit paramsChanged();
}

void Reconstructer::setOutputRelativePath(QString path)
{
	mOutputRelativePath = path;
}

void Reconstructer::setOutputBasePath(QString path)
{
	mOutputBasePath = path;
}

bool within(int x, int min, int max)
{
	return (x >= min) && (x <= max);
}

/**
 * Apply time calibration function y = ax + b, where
 *  y = calibrated(new) position timestamp
 *  x = old position timestamp
 *  a = input scale
 *  b = input offset
 * \param offset Offset between images and positions. 
 * \param scale Scale between images and positions.
 */
void Reconstructer::calibrateTimeStamps(double offset, double scale)
{
	for (unsigned int i = 0; i < mFileData.mPositions.size(); i++)
	{
		mFileData.mPositions[i].mTime = scale * mFileData.mPositions[i].mTime + offset;
	}
}

/**
 * Calculate timestamp calibration in an adhoc way, by assuming that 
 * images and positions start and stop at the exact same time.
 *
 * This is an option only - use with care!
 */
void Reconstructer::alignTimeSeries()
{
	ssc::messageManager()->sendInfo("Generate time calibration based on input time stamps.");
	double framesSpan = mFileData.mFrames.back().mTime - mFileData.mFrames.front().mTime;
	double positionsSpan = mFileData.mPositions.back().mTime - mFileData.mPositions.front().mTime;
	double scale = framesSpan / positionsSpan;

	double offset = mFileData.mFrames.front().mTime - scale * mFileData.mPositions.front().mTime;

	this->calibrateTimeStamps(offset, scale);
}

/** Calibrate the input tracker and frame timestamps.
 *
 *  Add a  constant time shift if set. (this comes in addition to
 *  a time calibration set in the probe calibration file).
 *
 *  If set, ignore the relative positioning between time series
 *  and rather set the first tracker and frame time equal.
 *
 */
void Reconstructer::applyTimeCalibration()
{
	double timeshift = mParams->mTimeCalibration->getValue();
	// The shift is on frames. The calibrate function applies to tracker positions,
	// hence the positive sign. (real use: subtract from frame data)
	//  std::cout << "TIMESHIFT " << timeshift << std::endl;
	//  timeshift = -timeshift;
	if (!ssc::similar(0.0, timeshift))
		ssc::messageManager()->sendInfo("Applying reconstruction-time calibration to tracking data: " + qstring_cast(
			timeshift) + "ms");
	this->calibrateTimeStamps(timeshift, 1.0);

	// ignore calibrations
	if (mParams->mAlignTimestamps->getValue())
	{
		this->alignTimeSeries();
	}
}

/**
 * Linear interpolation between a and b. t = 1 means use only b;
 */
Transform3D Reconstructer::interpolate(const Transform3D& a, const Transform3D& b, double t)
{
	Transform3D c;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			c(i, j) = (1 - t) * a(i, j) + t * b(i, j);
	return c;
}

/**
 * Interpolation between a and b
 * Spherical interpolation of the rotation, and linear interpolation of the position.
 * Uses Quaternion Slerp, so the rotational part of the matrix have to be converted to
 * Quaternion before the interpolation (and back again afterwards).
 */
Transform3D Reconstructer::slerpInterpolate(const Transform3D& a, const Transform3D& b, double t)
{
	//Convert input transforms to Quaternions
	Eigen::Quaterniond aq = Eigen::Quaterniond(a.matrix().block<3, 3>(0,0));
	Eigen::Quaterniond bq = Eigen::Quaterniond(b.matrix().block<3, 3>(0,0));

	Eigen::Quaterniond cq = aq.slerp(t, bq);

	Transform3D c;
	c.matrix().block<3, 3>(0, 0) = Eigen::Matrix3d(cq);


	for (int i = 0; i < 4; i++)
//		for (int j = 0; j < 4; j++)
		c(i, 3) = (1 - t) * a(i, 3) + t * b(i, 3);
	return c;
}

struct RemoveDataType
{
	RemoveDataType() : count(0), err(-1) {}
	void add(double _err) { ++count; err=((err<0)?_err:std::min(_err, err)); }
	int count;
	double err;
};

/**
 * Find interpolated position values for each frame based on the input position
 * data.
 * Current implementation: 
 * Linear interpolation
 */
void Reconstructer::interpolatePositions()
{
	//TODO: Check if the affine transforms still are affine after the linear interpolation
	int startFrames = mFileData.mFrames.size();

	std::map<int,RemoveDataType> removedData;

	for (unsigned i_frame = 0; i_frame < mFileData.mFrames.size();)
	{
		std::vector<TimedPosition>::iterator posIter;
		posIter = lower_bound(mFileData.mPositions.begin(), mFileData.mPositions.end(), mFileData.mFrames[i_frame]);

		unsigned i_pos = distance(mFileData.mPositions.begin(), posIter);
		if (i_pos != 0)
			i_pos--;

		if (i_pos >= mFileData.mPositions.size() - 1)
			i_pos = mFileData.mPositions.size() - 2;

		// Remove frames too far from the positions
		// Don't increment frame index since the index now points to the next element
		if ((fabs(mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos].mTime) > mMaxTimeDiff) || (fabs(
			mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos + 1].mTime) > mMaxTimeDiff))
		{
			double diff1 = fabs(mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos].mTime);
			double diff2 = fabs(mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos + 1].mTime);
//			//      ssc::messageManager()->sendInfo("Time difference is too large. Removed input frame: " + qstring_cast(i_frame) + ", difference is: "+ qstring_cast(diff1) + " or "+ qstring_cast(diff2));
//			ssc::messageManager()->sendInfo("Removed input frame: " + qstring_cast(i_frame) + ", difference is: "
//				+ QString::number(diff1, 'f', 1) + " or " + QString::number(diff2, 'f', 1)
//				+ " Time difference is too large.");
			removedData[i_frame].add(std::max(diff1, diff2));

			mFileData.mFrames.erase(mFileData.mFrames.begin() + i_frame);
			mFileData.mUsRaw->removeFrame(i_frame);
		}
		else
		{
			double t_delta_tracking = mFileData.mPositions[i_pos + 1].mTime - mFileData.mPositions[i_pos].mTime;
			double t = 0;
			if (!similar(t_delta_tracking, 0))
				t = (mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos].mTime) / t_delta_tracking;
			//    mFrames[i_frame].mPos = mPositions[i_pos].mPos;
//			mFileData.mFrames[i_frame].mPos = interpolate(mFileData.mPositions[i_pos].mPos, mFileData.mPositions[i_pos + 1].mPos, t);
			mFileData.mFrames[i_frame].mPos = slerpInterpolate(mFileData.mPositions[i_pos].mPos, mFileData.mPositions[i_pos + 1].mPos, t);
			i_frame++;// Only increment if we didn't delete the frame
		}
	}

	int removeCount=0;
	for (std::map<int,RemoveDataType>::iterator iter=removedData.begin(); iter!=removedData.end(); ++iter)
	{
		int first = iter->first+removeCount;
		int last = first + iter->second.count-1;
		ssc::messageManager()->sendInfo(QString("Removed input frame [%1-%2]. Time diff=%3").arg(first).arg(last).arg(iter->second.err, 0, 'f', 1));
		removeCount += iter->second.count;
//		ssc::messageManager()->sendInfo("Removed input frame: " + qstring_cast(i_frame) + ", difference is: "
//			+ QString::number(diff1, 'f', 1) + " or " + QString::number(diff2, 'f', 1)
//			+ " Time difference is too large.");
//		removedData[i_frame].add(std::min(diff1, diff2));
	}

	double removed = double(startFrames - mFileData.mFrames.size()) / double(startFrames);
	if (removed > 0.02)
	{
		double percent = removed * 100;
		if (percent > 1)
			ssc::messageManager()->sendWarning("Removed " + QString::number(percent, 'f', 1) + "% of the "
				+ qstring_cast(startFrames) + " frames.");
		else
			ssc::messageManager()->sendInfo("Removed " + QString::number(percent, 'f', 1) + "% of the " + qstring_cast(
				startFrames) + " frames.");
	}
}


/**
 * Pre:  mPos is prMt
 * Post: mPos is prMu
 */
void Reconstructer::transformPositionsTo_prMu()
{
	// Transform from image coordinate syst with origin in upper left corner
	// to t (tool) space. TODO check is u is ul corner or ll corner.
	ssc::Transform3D tMu = mFileData.mProbeData.get_tMu() * mFileData.mProbeData.get_uMv();

	//mPos is prMt
	for (unsigned i = 0; i < mFileData.mPositions.size(); i++)
	{
		ssc::Transform3D prMt = mFileData.mPositions[i].mPos;
		mFileData.mPositions[i].mPos = prMt * tMu;
	}
	//mPos is prMu
}


/**
 * Generate a rectangle (2D) defining ROI in input image space
 */
std::vector<ssc::Vector3D> Reconstructer::generateInputRectangle()
{
	std::vector<ssc::Vector3D> retval(4);
	if (!mFileData.mMask)
	{
		ssc::messageManager()->sendError("Reconstructer::generateInputRectangle() + requires mask");
		return retval;
	}
	int* dims = mFileData.mUsRaw->getDimensions();
	ssc::Vector3D spacing = mFileData.mUsRaw->getSpacing();

	int xmin = dims[0];
	int xmax = 0;
	int ymin = dims[1];
	int ymax = 0;
	unsigned char* ptr = static_cast<unsigned char*> (mFileData.mMask->getBaseVtkImageData()->GetScalarPointer());
	for (int x = 0; x < dims[0]; x++)
		for (int y = 0; y < dims[1]; y++)
		{
			unsigned char val = ptr[x + y * dims[0]];
			if (val != 0)
			{
				xmin = std::min(xmin, x);
				ymin = std::min(ymin, y);
				xmax = std::max(xmax, x);
				ymax = std::max(ymax, y);
			}
		}

	//Test: reduce the output volume by reducing the mask when determining
	//      output volume size
	double red = mParams->mMaskReduce->getValue().toDouble();
	int reduceX = (xmax - xmin) * (red / 100);
	int reduceY = (ymax - ymin) * (red / 100);

	xmin += reduceX;
	xmax -= reduceX;
	ymin += reduceY;
	ymax -= reduceY;

	retval[0] = ssc::Vector3D(xmin * spacing[0], ymin * spacing[1], 0);
	retval[1] = ssc::Vector3D(xmax * spacing[0], ymin * spacing[1], 0);
	retval[2] = ssc::Vector3D(xmin * spacing[0], ymax * spacing[1], 0);
	retval[3] = ssc::Vector3D(xmax * spacing[0], ymax * spacing[1], 0);

	return retval;
}

/**Compute the orientation part of the transform prMd, denoted as prMdd.
 * /return the prMdd.
 *
 * Pre:  mFrames[i].mPos = prMu
 * Post: mFrames[i].mPos = d'Mu, where d' is an oriented but not translated data space.
 */
ssc::Transform3D Reconstructer::applyOutputOrientation()
{
	//  QString newOrient = mSettings.getStringOption("Orientation").getValue();
	QString newOrient = mParams->mOrientationAdapter->getValue();
	ssc::Transform3D prMdd = Transform3D::Identity();

	if (newOrient == "PatientReference")
	{
		// identity
	}
	else if (newOrient == "MiddleFrame")
	{
		prMdd = mFileData.mFrames[mFileData.mFrames.size() / 2].mPos;
	}
	else
	{
		ssc::messageManager()->sendError("no orientation algorithm selected in reconstruction");
	}

	// apply the selected orientation to the frames.
	ssc::Transform3D ddMpr = prMdd.inv();
	for (unsigned i = 0; i < mFileData.mFrames.size(); i++)
	{
		// mPos = prMu
		mFileData.mFrames[i].mPos = ddMpr * mFileData.mFrames[i].mPos;
		// mPos = ddMu
	}

	return prMdd;
}

/**
 * Compute the transform from input to output space using the
 * orientation of the mid-frame and the point cloud from the mask.
 * mExtent is computed as a by-product
 *
 * Pre:  mFrames[i].mPos = prMu
 * Post: mFrames[i].mPos = dMu
 *       mExtent is defined
 */
void Reconstructer::findExtentAndOutputTransform()
{
	if (mFileData.mFrames.empty())
		return;
	// A first guess for d'Mu with correct orientation
	ssc::Transform3D prMdd = this->applyOutputOrientation();
	//mFrames[i].mPos = d'Mu, d' = only rotation

	// Find extent of all frames as a point cloud
	std::vector<ssc::Vector3D> inputRect = this->generateInputRectangle();
	std::vector<ssc::Vector3D> outputRect;
	for (unsigned slice = 0; slice < mFileData.mFrames.size(); slice++)
	{
		Transform3D dMu = mFileData.mFrames[slice].mPos;
		for (unsigned i = 0; i < inputRect.size(); i++)
		{
			outputRect.push_back(dMu.coord(inputRect[i]));
		}
	}

	ssc::DoubleBoundingBox3D extent = ssc::DoubleBoundingBox3D::fromCloud(outputRect);

	// Translate dMu to output volume origo
	ssc::Transform3D T_origo = ssc::createTransformTranslate(extent.corner(0, 0, 0));
	ssc::Transform3D prMd = prMdd * T_origo; // transform from output space to patref, use when storing volume.
	for (unsigned i = 0; i < mFileData.mFrames.size(); i++)
	{
		mFileData.mFrames[i].mPos = T_origo.inv() * mFileData.mFrames[i].mPos;
	}

	// Calculate optimal output image spacing and dimensions based on US frame spacing
	double inputSpacing = std::min(mFileData.mUsRaw->getSpacing()[0], mFileData.mUsRaw->getSpacing()[1]);
	mOutputVolumeParams = OutputVolumeParams(mSettings, extent, inputSpacing, Eigen::Array3i(mFileData.mUsRaw->getDimensions()));

	if (ssc::ToolManager::getInstance())
		mOutputVolumeParams.m_rMd = (*ssc::ToolManager::getInstance()->get_rMpr()) * prMd;
	else
		mOutputVolumeParams.m_rMd = prMd;

	mOutputVolumeParams.constrainVolumeSize();
}

/** Generate an output uid based on the assumption that input uid
 * is on the format "US-Acq_01_20001224T170000".
 * Change to "US_01_20001224T170000",
 * or add a "rec" postfix if a different name format is detected.
 */
QString Reconstructer::generateOutputUid()
{
	QString base = mFileData.mUsRaw->getUid();
	QString name = mOriginalFileData.mFilename.split("/").back();
	name = name.split(".").front();

	QStringList split = name.split("_");
	QStringList prefix = split.front().split("-");
	if (prefix.size() == 2)
	{
		split[0] = prefix[0];
	}
	else
	{
		split[0] += "rec";
	}

	return split.join("_");
}

/**Generate a pretty name for for volume based on the filename.
 * Assume filename has format US-Acq_01_20001224T170000 or similar.
 * Format: US <counter> <hh:mm>, for example US 3 15:34
 */
QString Reconstructer::generateImageName(QString uid) const
{
	QString name = uid.split("/").back();
	name = name.split(".").front();
	QString prefix = name.split("_").front(); // retrieve US-Acq part
	prefix = prefix.split("-").front(); // retrieve US part.
	if (prefix.isEmpty())
		prefix = "US";

	// retrieve  index counter from _99_
	QString counter = "";
	QRegExp countReg("_[0-9]{1,2}_");
	if (countReg.indexIn(name) > 0)
	{
		counter = countReg.cap(0).remove("_");
	}

	// retrieve timestamp as HH:MM
	QRegExp tsReg("[0-9]{8}T[0-9]{6}");
	if (tsReg.indexIn(name) > 0)
	{
		QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
		QString timestamp = datetime.toString("hh:mm");
		return prefix + " " + counter + " " + timestamp;
	}

	return name;
}

bool Reconstructer::validInputData() const
{
	if (mOriginalFileData.mFrames.empty() || !mOriginalFileData.mUsRaw || mOriginalFileData.mPositions.empty())
		return false;
	return true;
}

/**
 * Pre:  All data read, mExtent is calculated
 * Post: Output volume is initialized
 */
ImagePtr Reconstructer::generateOutputVolume()
{
	Eigen::Array3i dim = mOutputVolumeParams.getDim();
	ssc::Vector3D spacing = ssc::Vector3D(1, 1, 1) * mOutputVolumeParams.getSpacing();

	vtkImageDataPtr data = ssc::generateVtkImageData(dim, spacing, 0);

	//If no output path is selecetd, use the same path as the input
	QString filePath;
	if (mOutputBasePath.isEmpty() && mOutputRelativePath.isEmpty())
		filePath = qstring_cast(mFileData.mUsRaw->getFilePath());
	else
		filePath = mOutputRelativePath;

	QString uid = this->generateOutputUid();
	QString name = this->generateImageName(uid);
	std::cout << "creating vol " << uid << " with name " << name << std::endl;

	ImagePtr image = dataManager()->createImage(data, uid + "_%1", name + " %1", filePath);
	image->get_rMd_History()->setRegistration(mOutputVolumeParams.m_rMd);
	image->setModality("US");
	if (mParams->mAngioAdapter->getValue())
		image->setImageType("Angio");
	else
		image->setImageType("B-Mode");

	ssc::PresetTransferFunctions3DPtr presets = ssc::dataManager()->getPresetTransferFunctions3D();
	presets->load(mParams->mPresetTFAdapter->getValue(), image);

	return image;
}

void Reconstructer::setInputData(ssc::USReconstructInputData fileData)
{
	if (this->checkAndWarnForReconstruction())
		return;

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
	this->clearOutput();

	if (!this->validInputData())
		return;

	mFileData = mOriginalFileData;
	//  mFileData.mUsRaw.reset(new ssc::USFrameData(mOriginalFileData.mUsRaw->getBase()));
	mFileData.mUsRaw->setAngio(mParams->mAngioAdapter->getValue());
	mFileData.mUsRaw->reinitialize();

	// Only use this if the time stamps have different formats
	// The function assumes that both lists of time stamps start at the same time,
	// and that is not completely correct.
	//this->calibrateTimeStamps();
	// Use the time calibration from the acquisition module
	//this->calibrateTimeStamps(0.0, 1.0);
	this->applyTimeCalibration();

	this->transformPositionsTo_prMu();

	//mPos (in mPositions) is now prMu

	this->interpolatePositions();
	// mFrames: now mPos as prMu
	if (!this->validInputData())
	{
		ssc::messageManager()->sendError("Invalid reconstruct input.");
		return;
	}

	if (mFileData.mFrames.empty()) // if all positions are filtered out
		return;

	this->findExtentAndOutputTransform();
	//  mOutput = this->generateOutputVolume();
	//mPos in mFrames is now dMu

	emit paramsChanged();
}

void Reconstructer::reconstruct()
{
	if (!this->validInputData())
	{
		ssc::messageManager()->sendError("Reconstruct failed: no data loaded");
		return;
	}
	ssc::messageManager()->sendInfo("Perform reconstruction on: " + mOriginalFileData.mFilename);

	this->threadedPreReconstruct();
	this->threadedReconstruct();
	this->threadedPostReconstruct();
}

/**The reconstruct part that must be fun pre-rec in the main thread.
 *
 */
void Reconstructer::threadedPreReconstruct()
{
	if (!this->validInputData())
		return;
	mOutput = this->generateOutputVolume();
}

/**The reconstruct part that can be run in a separate thread.
 *
 */
void Reconstructer::threadedReconstruct()
{
	if (!this->validInputData())
		return;

	QDateTime startTime = QDateTime::currentDateTime();

	mIsReconstructing = true;
	QDomElement algoSettings = mSettings.getElement("algorithms", mAlgorithm->getName());
	mSuccess = mAlgorithm->reconstruct(mFileData.mFrames, mFileData.mUsRaw, mOutput, mFileData.mMask, algoSettings);
	mIsReconstructing = false;

	QTime tempTime = QTime(0, 0);
	tempTime = tempTime.addMSecs(startTime.time().msecsTo(QDateTime::currentDateTime().time()));
	ssc::messageManager()->sendInfo("Reconstruct time: " + tempTime.toString("hh:mm:ss:zzz"));
}

/**The reconstruct part that must be done post-rec in the main thread.
 *
 */
void Reconstructer::threadedPostReconstruct()
{
	if (!this->validInputData())
		return;

	if (mSuccess)
	{
		ssc::messageManager()->sendSuccess("Reconstruction done, " + mOutput->getName());

		DataManager::getInstance()->loadData(mOutput);
		DataManager::getInstance()->saveImage(mOutput, mOutputBasePath);
	}
	else
	{
		ssc::messageManager()->sendError("Reconstruction failed");
	}
}

ImagePtr Reconstructer::getOutput()
{
	return mOutput;
}



}
