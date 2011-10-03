/*
 * sscReconstructedOutputVolumeParams.h
 *
 *  Created on: May 27, 2010
 *      Author: christiana
 */
#ifndef SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_
#define SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_

#include "sscMessageManager.h"

namespace ssc
{

/*
 class ReconstructionParams
 {
 QDomDocument mDocument;
 QString mProcessor; // GPU, CPU,
 QString mOrientation; // PatientReference, MiddleFrame,
 QString mAlgorithm;
 };*/

/** Helper struct for sending and controlling output volume properties.
 */
class OutputVolumeParams
{
public:
	// constants, set only based on input data
	ssc::DoubleBoundingBox3D mExtent;
	double mInputSpacing;
	Eigen::Array3i mInputDim;
	ssc::Transform3D m_rMd; ///< transform from output data space to global ref space r

	OutputVolumeParams() :
		mExtent(0, 0, 0, 0, 0, 0), mInputSpacing(0), mInputDim(0, 0, 0), m_rMd(Transform3D::Identity()),
			mMaxVolumeSize(0), mDim(0, 0, 0), mSpacing(0)
	{
	}
	/** Initialize the volue parameters with sensible defaults.
	 */
	OutputVolumeParams(ssc::DoubleBoundingBox3D extent, double inputSpacing, Eigen::Array3i inputDim) :
		mExtent(extent), mInputSpacing(inputSpacing), mInputDim(inputDim), mMaxVolumeSize(1024 * 1024 * 32)
	{
		// Calculate optimal output image spacing and dimensions based on US frame spacing
		setSpacing(mInputSpacing);
		constrainVolumeSize(mMaxVolumeSize);
	}

	unsigned long getVolumeSize() const
	{
		return mDim[0] * mDim[1] * mDim[2];;
	}

	/** Set a spacing, recalculate dimensions.
	 */
	void setSpacing(double spacing)
	{
		mSpacing = spacing;
		ssc::Vector3D v = mExtent.range() / mSpacing;
		mDim << ::ceil(v[0]), ::ceil(v[1]), ::ceil(v[2]);
		//    mDim = (mExtent.range() / mSpacing).array().ceil();
		//    this->roundDim();
	}
	double getSpacing() const
	{
		return mSpacing;
	}
	/** Set one of the dimensions explicitly, recalculate other dims and spacing.
	 */
	void setDim(int index, int val)
	{
		setSpacing(mExtent.range()[index] / val);
	}
	Eigen::Array3i getDim() const
	{
		return mDim;
	}
	/** Increase spacing in order to keep size below a max size
	 */
	void constrainVolumeSize(double maxSize)
	{
		this->setSpacing(mInputSpacing); // reset to default values

		mMaxVolumeSize = maxSize;
		// Reduce output volume size if optimal volume size is too large
		unsigned long volumeSize = getVolumeSize();
		if (volumeSize > mMaxVolumeSize)
		{
			double scaleFactor = pow(volumeSize / double(mMaxVolumeSize), 1 / 3.0);
			this->setSpacing(mSpacing * scaleFactor);
		}
	}
	unsigned long getMaxVolumeSize() const
	{
		return mMaxVolumeSize;
	}

private:
	// controllable data, set only using the setters
	unsigned long mMaxVolumeSize;
	Eigen::Array3i mDim;
	double mSpacing;
};

} // namespace ssc


#endif /* SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_ */
