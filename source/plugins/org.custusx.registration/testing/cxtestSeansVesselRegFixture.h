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

#ifndef CXTESTSEANSVESSELREGFIXTURE_H_
#define CXTESTSEANSVESSELREGFIXTURE_H_

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

class SeansVesselRegFixture {

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
