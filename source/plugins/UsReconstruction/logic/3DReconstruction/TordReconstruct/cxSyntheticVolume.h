#ifndef SYNTHETIC_VOLUME_HPP
#define SYNTHETIC_VOLUME_HPP

#include "sscUSFrameData.h"
#include <vector>
#include "sscMathBase.h"


namespace cx {

/**
 * This abstract class provides an interface to synthetic volumes.
 * The requirements of this class are as follows:
 *  - The volume must be defined at every point inside the volume defined by [0, dimensions]
 *  - The volume must be sliceable with any imaginable slice
 *  - The volume must be the same every time it is used (i.e. no randomness)
 *    - If you want to test noise suppression, add noise to the US slice after it has been sampled
 *      from this class.
 */

class cxSyntheticVolume {
public:
	
	cxSyntheticVolume(const Eigen::Array3i& dimensions)
	{
		mDims = dimensions;
	}
	virtual ~cxSyntheticVolume() { };

	virtual Eigen::Array3i
	getDimensions()
	{
		return mDims;
	}
	
	/**
	 * Sample a set of ultrasound slices.
	 * The planes to use are defined by planes. The Transform3D-s represent the transform
	 * from image space to world space.
	 * The volume is assumed to lie from [0, 0, 0] -> dimensions.
	 * If any part of an image plane is outside the definition of the volume,
	 * the pixel value will be set to 0.
	 * @param planes Transforms from image space to world space (and thus also volume space)
	 *               for the desired slices.
	 * @param pixelSpacing The size of each pixel in the ultrasonic image.
	 * @param sliceDimension The size in pixels of each ultrasound image.
	 */
	virtual ProcessedUSInputDataPtr
	sampleUsData(const std::vector<Transform3D>& planes,
	             const Eigen::Array2f& pixelSpacing,
	             const Eigen::Array2i& sliceDimension) const;

	/**
	 * Evaluate the volume at position (x, y, z).
	 * The coordinates are given in volume space.
	 */	 
	virtual unsigned char
	evaluate(const float x,
	         const float y,
	         const float z) const = 0;

	virtual float computeRMSError(vtkImageDataPtr vol);
	
protected:
	Eigen::Array3i mDims;
	
};

}
#endif

