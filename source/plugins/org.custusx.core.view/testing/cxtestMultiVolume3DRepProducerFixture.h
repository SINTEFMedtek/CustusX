/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTMULTIVOLUME3DREPPRODUCERFIXTURE_H
#define CXTESTMULTIVOLUME3DREPPRODUCERFIXTURE_H

#include "cxtest_org_custusx_core_view_export.h"

#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolume.h>

#include "catch.hpp"

#include "cxVolumeHelpers.h"
#include "cxVolumetricRep.h"
#include "cxtestUtilities.h"

#include "cxImage2DRep3D.h"

#include "cxMultiVolume3DRepProducer.h"

namespace cxtest
{

class CXTEST_ORG_CUSTUSX_CORE_VIEW_EXPORT MultiVolume3DRepProducerFixture
{
public:
	MultiVolume3DRepProducerFixture();
	~MultiVolume3DRepProducerFixture();
	void initializeVisualizerAndImages(QString type, int imageCount=1);

	template<class REP>
	boost::shared_ptr<REP> downcastRep(int i)
	{
		return boost::dynamic_pointer_cast<REP>(mBase.getAllReps()[i]);
	}

	cx::MultiVolume3DRepProducer mBase;
	std::vector<cx::ImagePtr> mImages;
};

} // namespace cxtest


#endif // CXTESTMULTIVOLUME3DREPPRODUCERFIXTURE_H
