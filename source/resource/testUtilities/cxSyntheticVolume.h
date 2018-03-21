/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef SYNTHETIC_VOLUME_HPP
#define SYNTHETIC_VOLUME_HPP

#include "cxtestutilities_export.h"

#include "cxUSFrameData.h"
#include <vector>
#include <cstdlib>
#include <time.h>
#include "cxMathBase.h"


namespace cx {

/**
 * This abstract class provides an interface to synthetic volumes.
 * The requirements of this class are as follows:
 *  - The volume must be defined at every point inside the volume defined by [0, bounds]
 *  - The volume must be sliceable with any imaginable slice
 *  - The volume must be the same every time it is used (i.e. no randomness)
 *    - If you want to test noise suppression, add noise to the US slice after it has been sampled
 *      from this class.
 */

class CXTESTUTILITIES_EXPORT cxSyntheticVolume {
public:

	cxSyntheticVolume(Vector3D bounds)
	{
		// Seed the random number generator
		srand(time(NULL));
		mBounds = bounds;
	}
	virtual ~cxSyntheticVolume() { };

	virtual Vector3D
	getBounds()
	{
		return mBounds;
	}

	/**
	 * Sample a set of ultrasound slices.
	 * The planes to use are defined by planes. The Transform3D-s represent the transform
	 * from image space to world space.
	 * The volume is assumed to lie from [0, 0, 0] -> bounds.
	 * If any part of an image plane is outside the definition of the volume,
	 * the pixel value will be set to 0.
	 * @param planes_rMf Transforms from image space to world space (and thus also volume space)
	 *               for the desired slices.
	 * @param pixelSpacing The size of each pixel in the ultrasonic image.
	 * @param sliceDimension The size in pixels of each ultrasound image.
	 * @param output_dMr Position of generated output volume d relative to ref space r.
	 * @param noiseSigma The sigma of the noise distribution on each image
	 * @param noiseMean The sigma of the noise distribution on each image
	 */
	virtual ProcessedUSInputDataPtr
	sampleUsData(const std::vector<Transform3D>& planes_rMf,
	             const Eigen::Array2f& pixelSpacing,
	             const Eigen::Array2i& sliceDimension,
				 const Transform3D& output_dMr,
	             const double noiseSigma,
	             const unsigned char noiseMean) const;
	/** Alternative version.
	  * Input rMt, uses probe to generate required information about probe plane.
	  */
	virtual ProcessedUSInputDataPtr
	sampleUsData(const std::vector<Transform3D>& planes_rMt,
				 const ProbeDefinition& probe,
				 const Transform3D& output_dMr,
				 const double noiseSigma=0,
				 const unsigned char noiseMean=0) const;
	/** Generate one simulated us plane from the input rMt, probe.
	  */
	vtkImageDataPtr
	sampleUsData(const Transform3D& plane_rMt,
				 const ProbeDefinition& probe,
				 const double noiseSigma=0,
				 const unsigned char noiseMean=0) const;
	/** Alternative version. Input primitive components of probe, transform to image plane f.
	  */
	vtkImageDataPtr
	sampleUsData(const Transform3D& plane_rMf,
				 const Eigen::Array2f& pixelSpacing,
				 const Eigen::Array2i& sliceDimension,
				 const double noiseSigma,
				 const unsigned char noiseMean) const;

	/**
	 * Evaluate the volume at position (x, y, z).
	 * The coordinates are given in volume space.
	 */
	virtual unsigned char evaluate(const Vector3D& p) const = 0;
	virtual void printInfo() const {}

	void fillVolume(cx::ImagePtr vol);
	virtual float computeRMSError(cx::ImagePtr vol);

protected:
	unsigned char constrainToUnsignedChar(const int val) const;
	vtkImageDataPtr createEmptyMask(const Eigen::Array2i& sliceDimension) const;

	Vector3D mBounds;

};

typedef boost::shared_ptr<cxSyntheticVolume> cxSyntheticVolumePtr;

CXTESTUTILITIES_EXPORT double calculateRMSError(vtkImageDataPtr a, vtkImageDataPtr b);
CXTESTUTILITIES_EXPORT cx::Vector3D calculateCentroid(cx::ImagePtr image);
CXTESTUTILITIES_EXPORT double calculateMass(cx::ImagePtr image);

}
#endif

