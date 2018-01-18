/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
