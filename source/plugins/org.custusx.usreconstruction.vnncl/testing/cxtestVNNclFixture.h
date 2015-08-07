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
#ifndef CXTESTVNNCLFIXTURE_H
#define CXTESTVNNCLFIXTURE_H

#include "cxtest_org_custusx_usreconstruction_vnncl_export.h"

#include "cxtestReconstructionManagerFixture.h"
#include "cxtestReconstructRealData.h"

#include "cxtestReconstructionAlgorithmFixture.h"
#include "cxtestJenkinsMeasurement.h"

namespace cx
{
class VNNclReconstructionMethodService;
}

namespace cxtest
{

/** Test fixture for the VNNcl US reconstruction plugin
 *
 *
 * \ingroup cxtest
 * \date august 06, 2015
 * \author Ole Vegard Solberg, SINTEF
 */
class CXTEST_ORG_CUSTUSX_USRECONSTRUCTION_VNNCL_EXPORT VNNclFixture
{
public:
    VNNclFixture();
    ~VNNclFixture();

    void initVNN();
    void initVNN2();
    void initDW();
    void initAnisotropic();
    void initVNNMultistart();
    void initVNNClosest();

    void reconstruct();
    void verify();
private:
	cx::VNNclReconstructionMethodService* mAlgorithm;
	ReconstructionManagerTestFixture mFixture;
	ReconstructRealTestData mRealData;
	QDomElement mAlgo;
};

class CXTEST_ORG_CUSTUSX_USRECONSTRUCTION_VNNCL_EXPORT VNNclSyntheticFixture
{
public:
	VNNclSyntheticFixture();
	~VNNclSyntheticFixture();

	void initVNN();
	void initVNN2();
	void initDW();
	void initAnisotropic();
	void initVNNMultistart();

	void reconstruct();
	void verify();
private:
	ReconstructionAlgorithmFixture mFixture;
	cx::VNNclReconstructionMethodService* mAlgorithm;
	QDomElement mSettings;
	QString mMethodName;
	QDomDocument mDomdoc;
};

}//namespace

#endif // CXTESTVNNCLFIXTURE_H
