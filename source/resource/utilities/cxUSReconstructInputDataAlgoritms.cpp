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

#include "cxUSReconstructInputDataAlgoritms.h"

namespace cx
{

void USReconstructInputDataAlgorithm::transformTrackingPositionsTo_prMu(ssc::USReconstructInputData* data)
{
    // Transform from image coordinate syst with origin in upper left corner
    // to t (tool) space. TODO check is u is ul corner or ll corner.
    ssc::Transform3D tMu = data->mProbeData.get_tMu() * data->mProbeData.get_uMv();

    //mPos is prMt
    for (unsigned i = 0; i < data->mPositions.size(); i++)
    {
        ssc::Transform3D prMt = data->mPositions[i].mPos;
        data->mPositions[i].mPos = prMt * tMu;
    }
    //mPos is prMu
}

void USReconstructInputDataAlgorithm::transformFramePositionsTo_rMu(ssc::USReconstructInputData* data)
{
    ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
    // Transform from image coordinate syst with origin in upper left corner to t (tool) space.
    ssc::Transform3D tMv = data->mProbeData.get_tMu() * data->mProbeData.get_uMv();

	//mFrames is prMt
    for (unsigned i = 0; i < data->mFrames.size(); i++)
    {
        ssc::Transform3D prMt = data->mFrames[i].mPos;
        data->mFrames[i].mPos = rMpr * prMt * tMv;
    }
    //mFrames is rMu
}

std::vector<double> USReconstructInputDataAlgorithm::interpolateFramePositionsFromTracking(ssc::USReconstructInputData *data)
{
    std::vector<double> error(data->mFrames.size(), 1000);

	if (data->mPositions.empty())
		return error;

	for (unsigned i_frame = 0; i_frame < data->mFrames.size(); i_frame++)
    {
        std::vector<ssc::TimedPosition>::iterator posIter;
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
        if (!ssc::similar(t_delta_tracking, 0))
            t = (data->mFrames[i_frame].mTime - data->mPositions[i_pos].mTime) / t_delta_tracking;
        data->mFrames[i_frame].mPos = slerpInterpolate(data->mPositions[i_pos].mPos, data->mPositions[i_pos + 1].mPos, t);
        error[i_frame] = diff;
    }

    return error;
}

ssc::Transform3D USReconstructInputDataAlgorithm::slerpInterpolate(const ssc::Transform3D& a, const ssc::Transform3D& b, double t)
{
    //Convert input transforms to Quaternions
    Eigen::Quaterniond aq = Eigen::Quaterniond(a.matrix().block<3, 3>(0,0));
    Eigen::Quaterniond bq = Eigen::Quaterniond(b.matrix().block<3, 3>(0,0));

    Eigen::Quaterniond cq = aq.slerp(t, bq);

    ssc::Transform3D c;
    c.matrix().block<3, 3>(0, 0) = Eigen::Matrix3d(cq);

    for (int i = 0; i < 4; i++)
        c(i, 3) = (1 - t) * a(i, 3) + t * b(i, 3);
    return c;
}

ssc::Transform3D USReconstructInputDataAlgorithm::interpolate(const ssc::Transform3D& a, const ssc::Transform3D& b, double t)
{
    ssc::Transform3D c;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            c(i, j) = (1 - t) * a(i, j) + t * b(i, j);
    return c;
}


} // namespace cx
