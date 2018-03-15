/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTSEANSVESSELREGFIXTURE_H_
#define CXTESTSEANSVESSELREGFIXTURE_H_

#include "cxtest_org_custusx_registration_export.h"

#include <vector>
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace cxtest {
/**
 * \brief 
 *
 * \date Sep 26, 2013
 * \author Janne Beate Bakeng, SINTEF
 */

class CXTEST_ORG_CUSTUSX_REGISTRATION_EXPORT SeansVesselRegFixture {

public:
	SeansVesselRegFixture();
	~SeansVesselRegFixture();
	void setUp();
	void tearDown();

protected:
	void doTestVessel2VesselRegistration(cx::Transform3D perturbation, QString filenameSource, QString filenameTarget, double tol_dist, double tol_angle);
	vtkPolyDataPtr generatePolyData(std::vector<cx::Vector3D> pts);
	QStringList generateTestData();
	void saveVTKFile(vtkPolyDataPtr data, QString filename);
	QString saveVTKFile(std::vector<cx::Vector3D>, QString filename);
	cx::Vector3D append_line(std::vector<cx::Vector3D>* pts, cx::Vector3D a, cx::Vector3D b, double spacing);
	cx::Vector3D append_pt(std::vector<cx::Vector3D>* pts, cx::Vector3D a);
	std::vector<cx::Transform3D> generateTransforms();
};

} /* namespace cxtest */

#endif /* CXTESTSEANSVESSELREGFIXTURE_H_ */
