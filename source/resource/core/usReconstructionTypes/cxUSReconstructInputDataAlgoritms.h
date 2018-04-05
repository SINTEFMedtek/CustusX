/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXUSRECONSTRUCTINPUTDATAALGORITMS_H
#define CXUSRECONSTRUCTINPUTDATAALGORITMS_H

#include "cxResourceExport.h"

#include "cxUSReconstructInputData.h"

namespace cx
{

/**
 * \addtogroup cx_resource_usreconstructiontypes
 * \{
 */

/** Collection of operations on USReconstructInputData
  *
  * \date Mar 07, 2013
  * \author Christian Askeland, SINTEF
  */
struct cxResource_EXPORT USReconstructInputDataAlgorithm
{
    /**
     * Pre:  mPos is prMt
     * Post: mPos is prMu
     */
    static void transformTrackingPositionsTo_prMu(USReconstructInputData* data);
	/**
     * Pre:  mFrames is prMt
     * Post: mFrames is rMu
     */
    static void transformFramePositionsTo_rMu(USReconstructInputData* data);

    /** Find frame positions based on the existing tool positions and timestamps for both
      * tools and frames.
      * Return a vector containing an error measure for each frame. The error is the max
      * temporal distance from the frame to the two neighbouring tool positions.
      *
      */

    static std::vector<double> interpolateFramePositionsFromTracking(USReconstructInputData *data);
    /**
     * Interpolation between a and b
     * Spherical interpolation of the rotation, and linear interpolation of the position.
     * Uses Quaternion Slerp, so the rotational part of the matrix have to be converted to
     * Quaternion before the interpolation (and back again afterwards).
     */
    static Transform3D slerpInterpolate(const Transform3D& a, const Transform3D& b, double t);

    /**
     * Linear interpolation between a and b. t = 1 means use only b;
     */
    Transform3D interpolate(const Transform3D& a, const Transform3D& b, double t);

};


/**
 * \}
 */

}//namespace cx
#endif // CXUSRECONSTRUCTINPUTDATAALGORITMS_H
