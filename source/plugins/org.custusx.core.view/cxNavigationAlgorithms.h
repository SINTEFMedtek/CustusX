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
#ifndef CXNAVIGATIONALGORITHMS_H
#define CXNAVIGATIONALGORITHMS_H

#include "org_custusx_core_view_Export.h"

#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxEnumConverter.h"

namespace cx
{

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */

/**
 * A collection of algorithms used in navigation.
 *
 * Stateless.
 */
class org_custusx_core_view_EXPORT NavigationAlgorithms
{
public:

	/**
	 * Given a line (p_line, e_line),
	 * Find a (camera) point on that line at a given distance from focus.
	 */
	static Vector3D findCameraPosOnLineFixedDistanceFromFocus(Vector3D p_line, Vector3D e_line, double distance, Vector3D focus);
	/**
	 * Given a camera looking at a focus with a vup,
	 * Elevate the camera a given angle towards up, keeping distance |camera-focus| constant.
	 * Return the new camera position.
	 */
	static Vector3D elevateCamera(double angle, Vector3D camera, Vector3D focus, Vector3D vup);
	/**
	 * Find a vup (view up vector) orthogonal to vpn (view plane normal),
	 * Use vup_fallback if the two are parallel.
	 */
	static Vector3D orthogonalize_vup(Vector3D vup, Vector3D vpn, Vector3D vup_fallback);
	/**
	 * Given a scene with focus and vpn+vup, and vertical+horizontal view angles,
	 * find a camera pos that keeps the entire input bounding box in view.
	 */
	static Vector3D findCameraPosByZoomingToROI(double viewAngle_vertical, double viewAngle_horizontal,
												Vector3D focus, Vector3D vup, Vector3D vpn,
												const DoubleBoundingBox3D& bb);

	// primitives, used by other algorithms
	/**
	 * Used by findCameraPosByZoomingToROI(), input a plane n+viewAngle and find camera distance
	 * for this plane.
	 */
	static double findMaxCameraDistance(Vector3D n, double viewAngle,
										Vector3D focus, Vector3D vpn,
										const DoubleBoundingBox3D &bb);
	/**
	 * As findCameraDistanceKeepPointInView(), but do all the calculations in a plane defined by n.
	 * The viewAngle is the view angle in plane n.
	 */
	static double findCameraDistanceKeepPointInViewOneAxis(Vector3D n, double viewAngle,
														   Vector3D focus, Vector3D vpn, Vector3D p);
	/**
	 * Given a viewangle, focus and vpn,
	 * find the camera distance required to keep p in view.
	 * The resulting distance is the minimum camera distance required to see p.
	 */
	static double findCameraDistanceKeepPointInView(double viewAngle, Vector3D focus, Vector3D vpn, Vector3D p);
};


/**
 * @}
 */
} //namespace cx

#endif // CXNAVIGATIONALGORITHMS_H
