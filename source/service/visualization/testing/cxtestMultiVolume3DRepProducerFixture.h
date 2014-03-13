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
#ifndef CXTESTMULTIVOLUME3DREPPRODUCERFIXTURE_H
#define CXTESTMULTIVOLUME3DREPPRODUCERFIXTURE_H

#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolume.h>

#include "catch.hpp"

#include "cxVolumeHelpers.h"
#include "cxVolumetricRep.h"
#include "cxtestUtilities.h"
#include "cxMessageManager.h"
#include "cxImage2DRep3D.h"

#include "cxMultiVolume3DRepProducer.h"

namespace cxtest
{

class MultiVolume3DRepProducerFixture
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
