/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLandmarkTranslationRegistration.h"

#include "cxLogger.h"

namespace cx
{

Transform3D LandmarkTranslationRegistration::registerPoints(std::vector<Vector3D> ref,
				std::vector<Vector3D> target, bool* ok)
{
	Transform3D retval = Transform3D::Identity();
	*ok = ref.size() == target.size() && !ref.empty();
	if (*ok)
	{
		Vector3D sum = Vector3D::Zero();
		for (unsigned i = 0; i < ref.size(); ++i)
		{
			sum += ref[i] - target[i];
		}
		retval = createTransformTranslate(sum / ref.size());
	}
	else
	{
		CX_LOG_WARNING() << "Landmark translation registration: different or zero number of ref and target points, aborting.";
	}
	return retval;
}

} // namespace cx
