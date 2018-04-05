/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxReconstructPreprocessor.h"

#include "cxLogger.h"
#include "cxReconstructCore.h"
#include <vtkImageData.h>
#include <QFileInfo>
#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxRegistrationTransform.h"
#include "cxVolumeHelpers.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxTimeKeeper.h"
#include "cxUSFrameData.h"

#include "cxUSReconstructInputDataAlgoritms.h"
#include "cxPatientModelService.h"

namespace cx
{

ReconstructPreprocessor::ReconstructPreprocessor(PatientModelServicePtr patientModelService) :
	mInput(ReconstructCore::InputParams()),
	mPatientModelService(patientModelService)
{
    mMaxTimeDiff = 250; // TODO: Change default value for max allowed time difference between tracking and image time tags
    // TODO Legg inn andre rekonstruksjonsparametre?
    //
    //
}

ReconstructPreprocessor::~ReconstructPreprocessor()
{
}

std::vector<ProcessedUSInputDataPtr> ReconstructPreprocessor::createProcessedInput(std::vector<bool> angio)
{

	std::vector<std::vector<vtkImageDataPtr> > frames = mFileData.mUsRaw->initializeFrames(angio);

	std::vector<ProcessedUSInputDataPtr> retval;

	for (unsigned i=0; i<angio.size(); ++i)
	{
		ProcessedUSInputDataPtr input;
		input.reset(new ProcessedUSInputData(frames[i],
											 mFileData.mFrames,
											 mFileData.getMask(),
											 mFileData.mFilename,
											 QFileInfo(mFileData.mFilename).completeBaseName() ));
		CX_ASSERT(Eigen::Array3i(frames[i][0]->GetDimensions()).isApprox(Eigen::Array3i(mFileData.getMask()->GetDimensions())));
		retval.push_back(input);
	}
	return retval;
}

namespace
{
bool within(int x, int min, int max)
{
	return (x >= min) && (x <= max);
}
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
void ReconstructPreprocessor::calibrateTimeStamps(double offset, double scale)
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
void ReconstructPreprocessor::alignTimeSeries()
{
	report("Generate time calibration based on input time stamps.");
	double framesSpan = mFileData.mFrames.back().mTime - mFileData.mFrames.front().mTime;
	double positionsSpan = mFileData.mPositions.back().mTime - mFileData.mPositions.front().mTime;
	double scale = framesSpan / positionsSpan;

	double offset = mFileData.mFrames.front().mTime - scale * mFileData.mPositions.front().mTime;

	this->calibrateTimeStamps(offset, scale);
}

/**Crop the input data with the image cliprect.
 * Update probe sector info accordingly
 */
void ReconstructPreprocessor::cropInputData()
{
	//IntBoundingBox3D
	ProbeDefinition sector = mFileData.mProbeDefinition.mData;
	IntBoundingBox3D cropbox(sector.getClipRect_p().begin());
	cropbox = this->reduceCropboxToImageSize(cropbox, sector.getSize());
	Eigen::Vector3i shift = cropbox.corner(0,0,0).cast<int>();
	Eigen::Vector3i size = cropbox.range().cast<int>() + Eigen::Vector3i(1,1,0); // convert from extent format to size format by adding 1
	mFileData.mUsRaw->setCropBox(cropbox);

	DoubleBoundingBox3D clipRect_p = sector.getClipRect_p();
	Vector3D origin_p = sector.getOrigin_p();

	for (unsigned i=0; i<3; ++i)
	{
		clipRect_p[2*i] -= shift[i];
		clipRect_p[2*i+1] -= shift[i];
		origin_p[i] -= shift[i];
	}

	sector.setClipRect_p(clipRect_p);
	sector.setOrigin_p(origin_p);
	sector.setSize(QSize(size[0], size[1]));
	mFileData.mProbeDefinition.setData(sector);
}

IntBoundingBox3D ReconstructPreprocessor::reduceCropboxToImageSize(IntBoundingBox3D cropbox, QSize size)
{
	cropbox[0] = std::max(cropbox[0], 0);
	cropbox[2] = std::max(cropbox[2], 0);
	cropbox[4] = std::max(cropbox[4], 0);

	cropbox[1] = std::min(cropbox[1], size.width() - 1);
	cropbox[3] = std::min(cropbox[3], size.height() - 1);

	return cropbox;
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
void ReconstructPreprocessor::applyTimeCalibration()
{
	double timeshift = mInput.mExtraTimeCalibration;
	// The shift is on frames. The calibrate function applies to tracker positions,
	// hence the positive sign. (real use: subtract from frame data)
	//  std::cout << "TIMESHIFT " << timeshift << std::endl;
	//  timeshift = -timeshift;
	if (!similar(0.0, timeshift))
		report("Applying reconstruction-time calibration to tracking data: " + qstring_cast(
																 timeshift) + "ms");
	this->calibrateTimeStamps(timeshift, 1.0);

	// ignore calibrations
	if (mInput.mAlignTimestamps)
	{
		this->alignTimeSeries();
	}
}


struct RemoveDataType
{
	RemoveDataType() : count(0), err(-1) {}
	void add(double _err) { ++count; err=((err<0)?_err:std::min(_err, err)); }
	int count;
	double err;
};

void ReconstructPreprocessor::filterPositions()
{
    int filterStrength = mInput.mPosFilterStrength;

    if (filterStrength > 0) //Position filter enabled
    {
        int filterLength(1+2*filterStrength);
        int nPositions(mFileData.mPositions.size());
        if (nPositions > filterLength) //Position sequence sufficient long
        {
            // Define quaternion array
            Eigen::ArrayXXd qPosArray(7,(nPositions+(2*filterStrength))); // Add room for FIR-filtering
            Transform3D localTx;
            Eigen::Quaterniond qA;

            unsigned int sourceIdx(0);
            for (unsigned int i = 0; i < (nPositions+(2*filterStrength)); i++)
            {

                // Convert to quaternions
                sourceIdx =  (i > filterStrength) ? (i-filterStrength) : 0; // Pad array with edge elements
                sourceIdx =  (sourceIdx < nPositions) ? sourceIdx : (nPositions-1);

                localTx = mFileData.mPositions[sourceIdx].mPos;

                qPosArray.block<3,1>(4,i) = localTx.matrix().block<3, 1>(0,3); // Translation part
                qA = Eigen::Quaterniond(localTx.matrix().block<3, 3>(0,0)); //Convert rot to quaternions
                qPosArray.block<4,1>(0,i) = qA.coeffs(); //Rotation parameters

            }

            // Filter quaternion arrays (simple averaging filter)
            Eigen::ArrayXXd qPosFiltered = Eigen::ArrayXXd::Zero(7,nPositions); // Fill with zeros
            for (unsigned int i = 0; i < (1+2*filterStrength); i++)
            {
                qPosFiltered = qPosFiltered + qPosArray.block(0,i,7,nPositions);
            }
            qPosFiltered = qPosFiltered / (1+2*filterStrength);

            for (unsigned int i = 0; i < mFileData.mPositions.size(); i++)
            {
                // Convert back to position data
                qA.coeffs() = qPosFiltered.block<4,1>(0,i);
                localTx.matrix().block<3, 3>(0,0) = qA.toRotationMatrix();
                localTx.matrix().block<3, 1>(0,3) = qPosFiltered.block<3,1>(4,i);
                mFileData.mPositions[i].mPos = localTx;
            }
        }
    }

}

void ReconstructPreprocessor::positionThinning()
{
    // If enabled, try to remove "suspect" data (large jumps etc.)
    // Replace tracking positions that deviate greatly from neighbours with an interpolated value

}


/**
 * Find interpolated position values for each frame based on the input position
 * data.
 * Current implementation:
 * Linear interpolation
 */
void ReconstructPreprocessor::interpolatePositions()
{
	mFileData.mUsRaw->resetRemovedFrames();
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
		double timeToPos1 = timeToPosition(i_frame, i_pos);
		double timeToPos2 = timeToPosition(i_frame, i_pos+1);
		if ((timeToPos1 > mMaxTimeDiff) || (timeToPos2 > mMaxTimeDiff))
		{
			removedData[i_frame].add(std::max(timeToPos1, timeToPos2));

			mFileData.mFrames.erase(mFileData.mFrames.begin() + i_frame);
			mFileData.mUsRaw->removeFrame(i_frame);
		}
		else
		{
			double t_delta_tracking = mFileData.mPositions[i_pos + 1].mTime - mFileData.mPositions[i_pos].mTime;
			double t = 0;
			if (!similar(t_delta_tracking, 0))
				t = (mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos].mTime) / t_delta_tracking;
			//			mFileData.mFrames[i_frame].mPos = interpolate(mFileData.mPositions[i_pos].mPos, mFileData.mPositions[i_pos + 1].mPos, t);
			mFileData.mFrames[i_frame].mPos = cx::USReconstructInputDataAlgorithm::slerpInterpolate(mFileData.mPositions[i_pos].mPos, mFileData.mPositions[i_pos + 1].mPos, t);
			i_frame++;// Only increment if we didn't delete the frame
		}
	}

	int removeCount=0;
	for (std::map<int,RemoveDataType>::iterator iter=removedData.begin(); iter!=removedData.end(); ++iter)
	{
		int first = iter->first+removeCount;
		int last = first + iter->second.count-1;
		report(QString("Removed input frame [%1-%2]. Time diff=%3").arg(first).arg(last).arg(iter->second.err, 0, 'f', 1));
		removeCount += iter->second.count;
	}

	double removed = double(startFrames - mFileData.mFrames.size()) / double(startFrames);
	if (removed > 0.02)
	{
		double percent = removed * 100;
		if (percent > 1)
		{
			reportWarning("Removed " + QString::number(percent, 'f', 1) + "% of the "+ qstring_cast(startFrames) + " frames.");
		}
		else
		{
			report("Removed " + QString::number(percent, 'f', 1) + "% of the " + qstring_cast(startFrames) + " frames.");
		}
	}
}


double ReconstructPreprocessor::timeToPosition(unsigned i_frame, unsigned i_pos)
{
		return fabs(mFileData.mFrames[i_frame].mTime - mFileData.mPositions[i_pos].mTime);
}

/**
 * Generate a rectangle (2D) defining ROI in input image space
 */
std::vector<Vector3D> ReconstructPreprocessor::generateInputRectangle()
{
	std::vector<Vector3D> retval(4);
	vtkImageDataPtr mask = mFileData.getMask();
	if (!mask)
	{
		reportError("Reconstructer::generateInputRectangle() + requires mask");
		return retval;
	}
	Eigen::Array3i dims = mFileData.mUsRaw->getDimensions();
	Vector3D spacing = mFileData.mUsRaw->getSpacing();

	Eigen::Array3i maskDims(mask->GetDimensions());

	if (( maskDims[0]<dims[0] )||( maskDims[1]<dims[1] ))
		reportError(QString("input data (%1) and mask (%2) dim mimatch")
																.arg(qstring_cast(dims))
																.arg(qstring_cast(maskDims)));

	int xmin = maskDims[0];
	int xmax = 0;
	int ymin = maskDims[1];
	int ymax = 0;

	unsigned char* ptr = static_cast<unsigned char*> (mask->GetScalarPointer());
	for (int x = 0; x < maskDims[0]; x++)
		for (int y = 0; y < maskDims[1]; y++)
		{
			unsigned char val = ptr[x + y * maskDims[0]];
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
	double red = mInput.mMaskReduce;
	int reduceX = (xmax - xmin) * (red / 100);
	int reduceY = (ymax - ymin) * (red / 100);

	xmin += reduceX;
	xmax -= reduceX;
	ymin += reduceY;
	ymax -= reduceY;

	retval[0] = Vector3D(xmin * spacing[0], ymin * spacing[1], 0);
	retval[1] = Vector3D(xmax * spacing[0], ymin * spacing[1], 0);
	retval[2] = Vector3D(xmin * spacing[0], ymax * spacing[1], 0);
	retval[3] = Vector3D(xmax * spacing[0], ymax * spacing[1], 0);

	return retval;
}

/**Compute the orientation part of the transform prMd, denoted as prMdd.
 * /return the prMdd.
 *
 * Pre:  mFrames[i].mPos = prMu
 * Post: mFrames[i].mPos = d'Mu, where d' is an oriented but not translated data space.
 */
Transform3D ReconstructPreprocessor::applyOutputOrientation()
{
	Transform3D prMdd = Transform3D::Identity();

	if (mInput.mOrientation == "PatientReference")
	{
		// identity
	}
	else if (mInput.mOrientation == "MiddleFrame")
	{
		prMdd = mFileData.mFrames[mFileData.mFrames.size() / 2].mPos;
	}
	else
	{
		reportError("no orientation algorithm selected in reconstruction");
	}

	// apply the selected orientation to the frames.
	Transform3D ddMpr = prMdd.inv();
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
void ReconstructPreprocessor::findExtentAndOutputTransform()
{
	if (mFileData.mFrames.empty())
		return;
	// A first guess for d'Mu with correct orientation
	Transform3D prMdd = this->applyOutputOrientation();
	//mFrames[i].mPos = d'Mu, d' = only rotation

	// Find extent of all frames as a point cloud
	std::vector<Vector3D> inputRect = this->generateInputRectangle();
	std::vector<Vector3D> outputRect;
	for (unsigned slice = 0; slice < mFileData.mFrames.size(); slice++)
	{
		Transform3D dMu = mFileData.mFrames[slice].mPos;
		for (unsigned i = 0; i < inputRect.size(); i++)
		{
			outputRect.push_back(dMu.coord(inputRect[i]));
		}
	}

	DoubleBoundingBox3D extent = DoubleBoundingBox3D::fromCloud(outputRect);

	// Translate dMu to output volume origo
	Transform3D T_origo = createTransformTranslate(extent.corner(0, 0, 0));
	Transform3D prMd = prMdd * T_origo; // transform from output space to patref, use when storing volume.
	for (unsigned i = 0; i < mFileData.mFrames.size(); i++)
	{
		mFileData.mFrames[i].mPos = T_origo.inv() * mFileData.mFrames[i].mPos;
	}

	// Calculate optimal output image spacing and dimensions based on US frame spacing
	double inputSpacing = std::min(mFileData.mUsRaw->getSpacing()[0], mFileData.mUsRaw->getSpacing()[1]);
	mOutputVolumeParams = OutputVolumeParams(extent, inputSpacing, mInput.mMaxOutputVolumeSize);

	mOutputVolumeParams.set_rMd((mPatientModelService->get_rMpr()) * prMd);
}

/**Use the mOriginalFileData structure to rebuild all internal data.
 * Useful when settings have changed or data is loaded.
 */
void ReconstructPreprocessor::updateFromOriginalFileData()
{
	// uncomment to test cropping of data before reconstructing
	this->cropInputData();

	// Only use this if the time stamps have different formats
	// The function assumes that both lists of time stamps start at the same time,
	// and that is not completely correct.
	//this->calibrateTimeStamps();
	// Use the time calibration from the acquisition module
	//this->calibrateTimeStamps(0.0, 1.0);
	this->applyTimeCalibration();

    // Smooth tracking data before further processing
    // User preferences apply
    //this->positionThinning(); //Do thinning before filtering if enabled
    this->filterPositions();

	cx::USReconstructInputDataAlgorithm::transformTrackingPositionsTo_prMu(&mFileData);
	//mPos (in mPositions) is now prMu

	this->interpolatePositions();
	// mFrames: now mPos as prMu

	if (mFileData.mFrames.empty()) // if all positions are filtered out
		return;

	this->findExtentAndOutputTransform();
	//mPos in mFrames is now dMu
}

void ReconstructPreprocessor::initialize(ReconstructCore::InputParams input, USReconstructInputData fileData)
{
	mInput = input;
	mFileData = fileData;
	this->updateFromOriginalFileData();
}


} /* namespace cx */
