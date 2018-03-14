/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationApplicator.h"
#include "cxMesh.h"
#include "catch.hpp"
#include "cxReporter.h"
#include "cxTypeConversions.h"

class RegistrationApplicatorTestFixture
{
public:
	RegistrationApplicatorTestFixture()
	{
		cx::Reporter::initialize();
	}
	~RegistrationApplicatorTestFixture()
	{
		cx::Reporter::shutdown();
	}

	cx::DataPtr createData(QString frame, QString parentFrame, cx::Transform3D rMd)
	{
		cx::MeshPtr mesh = cx::Mesh::create(frame);
		mesh->get_rMd_History()->setParentSpace(parentFrame);
		mesh->get_rMd_History()->setRegistration(rMd);
		mData[mesh->getUid()] = mesh;
		return mesh;
	}

	cx::Transform3D translation(double x, double y, double z)
	{
		return cx::createTransformTranslate(cx::Vector3D(x,y,z));
	}

	void updateRegistration(cx::DataPtr fixed, cx::DataPtr moving, cx::Transform3D delta_pre_rMd)
	{
		QDateTime oldTime = QDateTime::currentDateTime();

		cx::RegistrationTransform regTrans(delta_pre_rMd, QDateTime::currentDateTime(), "test");
		regTrans.mFixed = fixed->getUid();
		regTrans.mMoving = moving->getUid();

		cx::RegistrationApplicator applicator(mData);
		applicator.updateRegistration(oldTime, regTrans);
	}

	void check(QString frame, QString parentFrame_expected, cx::Transform3D rMd_expected)
	{
		cx::DataPtr data = mData[frame];
		INFO(QString("Checking %1").arg(frame));
		REQUIRE(data.get());
		INFO(QString("parent=%1, expected %2").arg(data->getParentSpace()).arg(parentFrame_expected));
		CHECK(data->getParentSpace() == parentFrame_expected);
		INFO(QString("rMd=\n%1\n, expected\n%2")
			 .arg(qstring_cast(data->get_rMd()))
			 .arg(qstring_cast(rMd_expected)));
		CHECK(cx::similar(data->get_rMd(), rMd_expected));
	}

	std::map<QString, cx::DataPtr> mData;
};


TEST_CASE("org.custusx.registration: I2I reg, simple", "[unit][plugins][org.custusx.registration]")
{
	RegistrationApplicatorTestFixture fixture;

	cx::Transform3D T0 = fixture.translation(0,0,0);
	cx::Transform3D T1 = fixture.translation(10,0,0);
	cx::Transform3D delta_pre_rMd = T1;
	cx::DataPtr fixed = fixture.createData("F1", "F2", T0);
	cx::DataPtr moving = fixture.createData("M1", "M2", T0);

	fixture.check("F1", "F2", T0);
	fixture.check("M1", "M2", T0);

	fixture.updateRegistration(fixed, moving, delta_pre_rMd);

	fixture.check("F1", "F2", T0);
	fixture.check("M1", "F2", T1*T0);
}

TEST_CASE("org.custusx.registration: I2I reg, moving tree", "[unit][plugins][org.custusx.registration]")
{
	RegistrationApplicatorTestFixture fixture;

	cx::Transform3D T0 = fixture.translation(0,0,0);
	cx::Transform3D T1 = fixture.translation(10,0,0);
	cx::Transform3D T2 = fixture.translation(0,5,0);
	cx::Transform3D delta_pre_rMd = T1;
	cx::DataPtr fixed = fixture.createData("F1", "F2", T0);
	cx::DataPtr moving = fixture.createData("M1", "M3", T2);
	cx::DataPtr m2 = fixture.createData("M2", "M1", T2);

	fixture.check("F1", "F2", T0);
	fixture.check("M1", "M3", T2);
	fixture.check("M2", "M1", T2);

	fixture.updateRegistration(fixed, moving, delta_pre_rMd);

	fixture.check("F1", "F2", T0);
	fixture.check("M1", "F2", T1*T2);
	fixture.check("M2", "M1", T1*T2);
}

TEST_CASE("org.custusx.registration: I2I reg, moving tree on root", "[unit][plugins][org.custusx.registration]")
{
	RegistrationApplicatorTestFixture fixture;

	cx::Transform3D T0 = fixture.translation(0,0,0);
	cx::Transform3D delta_pre_rMd = T0;
	cx::DataPtr fixed = fixture.createData("F1", "F2", T0);
	cx::DataPtr moving = fixture.createData("M1", "", T0);
	cx::DataPtr m2 = fixture.createData("M2", "M1", T0);

	fixture.check("F1", "F2", T0);
	fixture.check("M1", "", T0);
	fixture.check("M2", "M1", T0);

	fixture.updateRegistration(fixed, moving, delta_pre_rMd);

	fixture.check("F1", "F2", T0);
	fixture.check("M1", "F2", T0);
	fixture.check("M2", "M1", T0);
}

TEST_CASE("org.custusx.registration: I2I reg, moving and fixed on root", "[unit][plugins][org.custusx.registration]")
{
	RegistrationApplicatorTestFixture fixture;

	cx::Transform3D T0 = fixture.translation(0,0,0);
	cx::Transform3D delta_pre_rMd = T0;
	cx::DataPtr fixed = fixture.createData("F1", "", T0);
	cx::DataPtr moving = fixture.createData("M1", "", T0);
	cx::DataPtr m2 = fixture.createData("M2", "M1", T0);

	fixture.check("F1", "", T0);
	fixture.check("M1", "", T0);
	fixture.check("M2", "M1", T0);

	fixture.updateRegistration(fixed, moving, delta_pre_rMd);

	CHECK(fixed->getParentSpace() != "");
	fixture.check("F1", fixed->getParentSpace(), T0);
	fixture.check("M1", fixed->getParentSpace(), T0);
	fixture.check("M2", "M1", T0);
}





