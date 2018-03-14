/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
