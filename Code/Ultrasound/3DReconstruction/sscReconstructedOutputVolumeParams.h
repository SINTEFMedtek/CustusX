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

#ifndef SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_
#define SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_

#include "sscMessageManager.h"

namespace ssc
{

/**
 * \addtogroup sscUSReconstruction
 * \{
 */

/** \brief Helper struct for sending and controlling output volume properties.
 *
 *  \date May 27, 2010
 *  \author christiana
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
//		std::cout << "OutputVolumeParams::setSpacing("<< spacing <<") " << v << " -- " << mDim << std::endl;
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
//		std::cout << "OutputVolumeParams::constrainVolumeSize "<< mMaxVolumeSize << " vol="<< volumeSize << std::endl;
		if (volumeSize > mMaxVolumeSize)
		{
			ssc::Vector3D ext = mExtent.range();
			double newSpacing = pow(ext[0]*ext[1]*ext[2] / double(mMaxVolumeSize), 1 / 3.0);

//			double scaleFactor = pow(volumeSize / double(mMaxVolumeSize), 1 / 3.0);
//			std::cout << "old s: " << mSpacing * scaleFactor << ", new s: " << newSpacing << std::endl;
//			this->setSpacing(mSpacing * scaleFactor);
			this->setSpacing(newSpacing);
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

/**
 * \}
 */

} // namespace ssc


#endif /* SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_ */
