/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUSReconstructInputDataAlgoritms.h"

namespace cx
{

void USReconstructInputDataAlgorithm::transformTrackingPositionsTo_prMu(USReconstructInputData* data)
{
    // Transform from image coordinate syst with origin in upper left corner
    // to t (tool) space. TODO check is u is ul corner or ll corner.
    Transform3D tMu = data->mProbeDefinition.get_tMu() * data->mProbeDefinition.get_uMv();

    //mPos is prMt
    for (unsigned i = 0; i < data->mPositions.size(); i++)
    {
        Transform3D prMt = data->mPositions[i].mPos;
        data->mPositions[i].mPos = prMt * tMu;
    }
    //mPos is prMu
}

void USReconstructInputDataAlgorithm::transformFramePositionsTo_rMu(USReconstructInputData* data)
{
	Transform3D rMpr = data->rMpr;
    // Transform from image coordinate syst with origin in upper left corner to t (tool) space.
    Transform3D tMv = data->mProbeDefinition.get_tMu() * data->mProbeDefinition.get_uMv();

	//mFrames is prMt
    for (unsigned i = 0; i < data->mFrames.size(); i++)
    {
        Transform3D prMt = data->mFrames[i].mPos;
        data->mFrames[i].mPos = rMpr * prMt * tMv;
    }
    //mFrames is rMu
}

std::vector<double> USReconstructInputDataAlgorithm::interpolateFramePositionsFromTracking(USReconstructInputData *data)
{
    std::vector<double> error(data->mFrames.size(), 1000);

	if (data->mPositions.empty())
		return error;

	for (unsigned i_frame = 0; i_frame < data->mFrames.size(); i_frame++)
    {
        std::vector<TimedPosition>::iterator posIter;
        posIter = lower_bound(data->mPositions.begin(), data->mPositions.end(), data->mFrames[i_frame]);

        unsigned i_pos = distance(data->mPositions.begin(), posIter);
        if (i_pos != 0)
            i_pos--;

        if (i_pos >= data->mPositions.size() - 1)
            i_pos = data->mPositions.size() - 2;

        double diff1 = fabs(data->mFrames[i_frame].mTime - data->mPositions[i_pos].mTime);
        double diff2 = fabs(data->mFrames[i_frame].mTime - data->mPositions[i_pos + 1].mTime);
        double diff = std::max(diff1, diff2);

        double t_delta_tracking = data->mPositions[i_pos + 1].mTime - data->mPositions[i_pos].mTime;
        double t = 0;
        if (!similar(t_delta_tracking, 0))
            t = (data->mFrames[i_frame].mTime - data->mPositions[i_pos].mTime) / t_delta_tracking;
        data->mFrames[i_frame].mPos = slerpInterpolate(data->mPositions[i_pos].mPos, data->mPositions[i_pos + 1].mPos, t);
        error[i_frame] = diff;
    }

    return error;
}

Transform3D USReconstructInputDataAlgorithm::slerpInterpolate(const Transform3D& a, const Transform3D& b, double t)
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

Transform3D USReconstructInputDataAlgorithm::interpolate(const Transform3D& a, const Transform3D& b, double t)
{
    Transform3D c;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            c(i, j) = (1 - t) * a(i, j) + t * b(i, j);
    return c;
}


} // namespace cx
