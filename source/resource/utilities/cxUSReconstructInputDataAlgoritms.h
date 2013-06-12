// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXUSRECONSTRUCTINPUTDATAALGORITMS_H
#define CXUSRECONSTRUCTINPUTDATAALGORITMS_H

#include "cxUSReconstructInputData.h"
#include "sscToolManager.h"

namespace cx
{

/**
 * \addtogroup cxResourceUtilities
 * \{
 */

/** Collection of operations on ssc::USReconstructInputData
  *
  * \date Mar 07, 2013
  * \author Christian Askeland, SINTEF
  */
struct USReconstructInputDataAlgorithm
{
    /**
     * Pre:  mPos is prMt
     * Post: mPos is prMu
     */
    static void transformTrackingPositionsTo_prMu(ssc::USReconstructInputData* data);
	/**
     * Pre:  mFrames is prMt
     * Post: mFrames is rMu
     */
    static void transformFramePositionsTo_rMu(ssc::USReconstructInputData* data);

    /** Find frame positions based on the existing tool positions and timestamps for both
      * tools and frames.
      * Return a vector containing an error measure for each frame. The error is the max
      * temporal distance from the frame to the two neighbouring tool positions.
      *
      */
    static std::vector<double> interpolateFramePositionsFromTracking(ssc::USReconstructInputData *data);
    /**
     * Interpolation between a and b
     * Spherical interpolation of the rotation, and linear interpolation of the position.
     * Uses Quaternion Slerp, so the rotational part of the matrix have to be converted to
     * Quaternion before the interpolation (and back again afterwards).
     */
    static ssc::Transform3D slerpInterpolate(const ssc::Transform3D& a, const ssc::Transform3D& b, double t);

    /**
     * Linear interpolation between a and b. t = 1 means use only b;
     */
    ssc::Transform3D interpolate(const ssc::Transform3D& a, const ssc::Transform3D& b, double t);

};


/**
 * \}
 */

}//namespace cx
#endif // CXUSRECONSTRUCTINPUTDATAALGORITMS_H
