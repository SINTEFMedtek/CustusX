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
#ifndef CXTESTMULTIVOLUME3DREPPRODUCERFIXTURE_H
#define CXTESTMULTIVOLUME3DREPPRODUCERFIXTURE_H

#include "cxtest_org_custusx_core_visualization_export.h"

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

class CXTEST_ORG_CUSTUSX_CORE_VISUALIZATION_EXPORT MultiVolume3DRepProducerFixture
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
