/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNavigationAlgorithms.h"
#include "cxLogger.h"
#include "cxBoundingBox3D.h"

namespace cx
{

Vector3D NavigationAlgorithms::findCameraPosOnLineFixedDistanceFromFocus(Vector3D p_line, Vector3D e_line, double distance, Vector3D focus)
{
	e_line = e_line.normal();

	// project focus onto line, then use pythoagoras:
	//      dist^2 = |focus-pff|^2 + q^2
	// where we want to find q, dist from p_ff to camera along line.
	Vector3D p_ff = p_line + e_line * dot(focus-p_line, e_line);
	double q_sq = distance*distance - dot(focus-p_ff, focus-p_ff);
	if (q_sq<0)
	{
		// too close: revert to use point on p_ff - focus line.
		Vector3D p_c = focus - (p_ff-focus).normal() * distance;
		return p_c;
	}
	double q = sqrt(q_sq);

	Vector3D p_c = p_ff - q * e_line;
	Vector3D p2_c = p_ff + q * e_line;

	if (similar((p_c-focus).length(), distance))
		return p_c;
	else if (similar((p2_c-focus).length(), distance))
	{
		return p2_c;
	}
	else
	{
		CX_LOG_CHANNEL_DEBUG("CA") << "find point failed - error in distance!!!!!!!!!!!!!!!!!";
		return p_c;
	}
}

Vector3D NavigationAlgorithms::orthogonalize_vup(Vector3D vup, Vector3D vpn, Vector3D vup_fallback)
{
	if (cross(vup, vpn).length() < 0.01)
	{
//		CX_LOG_CHANNEL_DEBUG("CA") << "warning ,  cross(vup_r, vpn_r)=" << cross(vup, vpn).length();
		vup = vup_fallback;
	}
	else
	{
		Vector3D left = cross(vup, vpn).normal();
		vup = cross(vpn, left).normal();
	}

	return vup;
}

Vector3D NavigationAlgorithms::elevateCamera(double angle, Vector3D camera, Vector3D focus, Vector3D vup)
{
	if (similar(angle, 0.0))
		return camera;

	// clean up input in order to have two orthogonal unit vectors
	Vector3D i = (camera-focus).normal();
	Vector3D j = vup;
	Vector3D k = cross(i, j).normal();
	j = cross(k, i).normal();

	// define a new space spanned by (x=vpn,y=vup), centered in focus
	// rotate the angle in this space
	// input space is A, focus-centered space is B
	Transform3D aMb = createTransformIJC(i, j, focus);
	Transform3D bMa = aMb.inv();
	Transform3D R = createTransformRotateZ(angle);
	camera = (aMb*R*bMa).coord(camera);

	return camera;
}

Vector3D NavigationAlgorithms::findCameraPosByZoomingToROI(double viewAngle_vertical, double viewAngle_horizontal, Vector3D focus, Vector3D vup, Vector3D vpn, const DoubleBoundingBox3D& bb)
{
	Vector3D n_vertical = cross(vpn, vup).normal();
	double dist_v = NavigationAlgorithms::findMaxCameraDistance(n_vertical, viewAngle_vertical, focus, vpn, bb);
	Vector3D n_horizontal = vup;
	double dist_h = NavigationAlgorithms::findMaxCameraDistance(n_horizontal, viewAngle_horizontal, focus, vpn, bb);
	double dist = std::max(dist_v, dist_h);

	Vector3D camera_r_t = focus + vpn*dist;
	return camera_r_t;
}

double NavigationAlgorithms::findMaxCameraDistance(Vector3D n, double viewAngle, Vector3D focus, Vector3D vpn, const DoubleBoundingBox3D& bb)
{
	std::vector<double> dists;
	for (unsigned x=0; x<2; ++x)
		for (unsigned y=0; y<2; ++y)
			for (unsigned z=0; z<2; ++z)
			{
				Vector3D p = bb.corner(x,y,z);
				double d = NavigationAlgorithms::findCameraDistanceKeepPointInViewOneAxis(n, viewAngle, focus, vpn, p);
				dists.push_back(d);
			}

	double maxDist = *std::max_element(dists.begin(), dists.end());
	return maxDist;
}

double NavigationAlgorithms::findCameraDistanceKeepPointInViewOneAxis(Vector3D n, double viewAngle, Vector3D focus, Vector3D vpn, Vector3D p)
{
	// project all items into plane	n
	Vector3D focus_p = focus - dot(focus, n)*n;
	Vector3D p_p = p - dot(p, n)*n;
	Vector3D vpn_p = (vpn - dot(vpn, n)*n).normal();

	// find distance in projection plane n
	double d_p = NavigationAlgorithms::findCameraDistanceKeepPointInView(viewAngle, focus_p, vpn_p, p_p);

	// recalculate non-projected distance.
	double cos_plane_angle = dot(vpn, vpn_p); // cosine(angle) between plane n and original vpn direction
	d_p = d_p / cos_plane_angle;
	return d_p;
}

double NavigationAlgorithms::findCameraDistanceKeepPointInView(double viewAngle, Vector3D focus, Vector3D vpn, Vector3D p)
{
	Vector3D pp = focus + vpn*dot(p-focus, vpn); // p projected onto the camera line defined by focus and vpn.
	double beta = (p-pp).length() / tan(viewAngle/2); // distance from pp to camera
	beta = fabs(beta);
	double dist = beta + dot(pp-focus, vpn); // total distance from focus to camera
	return dist;
}


}//namespace cx


