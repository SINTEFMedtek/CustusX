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

#include "sscReconstructPreprocessor.h"


#include "sscReconstructCore.h"
#include <vtkImageData.h>
#include <QFileInfo>
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscPNNReconstructAlgorithm.h"
#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscVolumeHelpers.h"
#include "sscPresetTransferFunctions3D.h"
#include "sscTimeKeeper.h"

namespace ssc
{

ReconstructPreprocessor::ReconstructPreprocessor()
{
    mMaxTimeDiff = 100; // TODO: Change default value for max allowed time difference between tracking and image time tags
}

ReconstructPreprocessor::~ReconstructPreprocessor()
{
}

void ReconstructPreprocessor::initializeCores(std::vector<ReconstructCorePtr> cores)
{
	TimeKeeper timer;

	std::vector<bool> angio;
    for (unsigned i=0; i<cores.size(); ++i)
        angio.push_back(cores[i]->getInputParams().mAngio);

    std::vector<std::vector<vtkImageDataPtr> > frames = mFileData.mUsRaw->initializeFrames(angio);

    for (unsigned i=0; i<cores.size(); ++i)
    {
        ProcessedUSInputDataPtr input(new ProcessedUSInputData(frames[i],
                                                               mFileData.mFrames,
                                                               mFileData.mMask,
                                                               mFileData.mFilename,
                                                               QFileInfo(mFileData.mFilename).completeBaseName() ));
        cores[i]->initialize(input, this->getOutputVolumeParams());
    }


	timer.printElapsedSeconds("Reconstruct preprocess time");
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
    ssc::messageManager()->sendInfo("Generate time calibration based on input time stamps.");
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
    ProbeData sector = mFileData.mProbeData.mData;
    ProbeData::ProbeImageData imageSector = sector.getImage();
    IntBoundingBox3D cropbox(imageSector.mClipRect_p.begin());
    Eigen::Vector3i shift = cropbox.corner(0,0,0).cast<int>();
    Eigen::Vector3i size = cropbox.range().cast<int>() + Eigen::Vector3i(1,1,0); // convert from extent format to size format by adding 1
    mFileData.mUsRaw->setCropBox(cropbox);


    for (unsigned i=0; i<3; ++i)
    {
        imageSector.mClipRect_p[2*i] -= shift[i];
        imageSector.mClipRect_p[2*i+1] -= shift[i];
        imageSector.mOrigin_p[i] -= shift[i];
    }
    imageSector.mSize.setWidth(size[0]);
    imageSector.mSize.setHeight(size[1]);
    sector.setImage(imageSector);
    mFileData.mProbeData.setData(sector);

    vtkImageDataPtr mask = mFileData.mProbeData.getMask();
    mFileData.mMask = ssc::ImagePtr(new ssc::Image("mask", mask, "mask")) ;
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
    if (!ssc::similar(0.0, timeshift))
        ssc::messageManager()->sendInfo("Applying reconstruction-time calibration to tracking data: " + qstring_cast(
            timeshift) + "ms");
    this->calibrateTimeStamps(timeshift, 1.0);

    // ignore calibrations
    if (mInput.mAlignTimestamps)
    {
        this->alignTimeSeries();
    }
}

/**
 * Linear interpolation between a and b. t = 1 means use only b;
 */
Transform3D ReconstructPreprocessor::interpolate(const Transform3D& a, const Transform3D& b, double t)
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
Transform3D ReconstructPreprocessor::slerpInterpolate(const Transform3D& a, const Transform3D& b, double t)
{
    //Convert input transforms to Quaternions
    Eigen::Quaterniond aq = Eigen::Quaterniond(a.matrix().block<3, 3>(0,0));
    Eigen::Quaterniond bq = Eigen::Quaterniond(b.matrix().block<3, 3>(0,0));

    Eigen::Quaterniond cq = aq.slerp(t, bq);

    Transform3D c;
    c.matrix().block<3, 3>(0, 0) = Eigen::Matrix3d(cq);


    for (int i = 0; i < 4; i++)
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
void ReconstructPreprocessor::interpolatePositions()
{
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
void ReconstructPreprocessor::transformPositionsTo_prMu()
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
std::vector<ssc::Vector3D> ReconstructPreprocessor::generateInputRectangle()
{
    std::vector<ssc::Vector3D> retval(4);
    if (!mFileData.mMask)
    {
        ssc::messageManager()->sendError("Reconstructer::generateInputRectangle() + requires mask");
        return retval;
    }
    Eigen::Array3i dims = mFileData.mUsRaw->getDimensions();
    ssc::Vector3D spacing = mFileData.mUsRaw->getSpacing();

    Eigen::Array3i maskDims(mFileData.mMask->getBaseVtkImageData()->GetDimensions());

    if (( maskDims[0]<dims[0] )||( maskDims[1]<dims[1] ))
        ssc::messageManager()->sendError(QString("input data (%1) and mask (%2) dim mimatch")
                                         .arg(qstring_cast(dims))
                                         .arg(qstring_cast(maskDims)));

    int xmin = maskDims[0];
    int xmax = 0;
    int ymin = maskDims[1];
    int ymax = 0;

    unsigned char* ptr = static_cast<unsigned char*> (mFileData.mMask->getBaseVtkImageData()->GetScalarPointer());
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
ssc::Transform3D ReconstructPreprocessor::applyOutputOrientation()
{
    ssc::Transform3D prMdd = Transform3D::Identity();

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
void ReconstructPreprocessor::findExtentAndOutputTransform()
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
    mOutputVolumeParams = OutputVolumeParams(extent, inputSpacing);
    mOutputVolumeParams.setMaxVolumeSize(mInput.mMaxOutputVolumeSize);

    if (ssc::ToolManager::getInstance())
        mOutputVolumeParams.m_rMd = (*ssc::ToolManager::getInstance()->get_rMpr()) * prMd;
    else
        mOutputVolumeParams.m_rMd = prMd;

    mOutputVolumeParams.constrainVolumeSize();
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

    this->transformPositionsTo_prMu();
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


} /* namespace ssc */
