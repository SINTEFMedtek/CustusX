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

#include "sscFrame3D.h"
#include "catch.hpp"

using namespace cx;

namespace
{

void singleTestFrame(const Transform3D& transform)
{
	Frame3D frame = Frame3D::create(transform);
	Transform3D restored = frame.transform();

	boost::array<double, 6> rep = frame.getCompactAxisAngleRep();
	Transform3D restored_rep = Frame3D::fromCompactAxisAngleRep(rep).transform();

	if (!similar(transform, restored))
	{
		std::cout << "--------------------------------------" << std::endl;
		std::cout << "mismatch:" << std::endl;
		std::cout << "original:" << std::endl;
		std::cout << transform << std::endl;
		std::cout << "frame: " << frame << std::endl;
		std::cout << "axis: " << frame.rotationAxis() << std::endl;
		std::cout << std::endl;
		std::cout << "restored:" << std::endl;
		std::cout << restored << std::endl;
		std::cout << "--------------------------------------" << std::endl;
	}
	else if (similar(transform, restored))
	{
//		std::cout << "success" << std::endl;
//		std::cout << transform << std::endl;
//		std::cout << "frame: " << frame << std::endl;
//		std::cout << "axis: " << frame.rotationAxis() << std::endl;
	}

	CHECK(similar(transform, restored));
  CHECK(similar(transform, restored_rep));
}

void singleTestFrameRotationAxis(const Vector3D& k)
{
	Frame3D frame;
	frame.setRotationAxis(k);
	CHECK(similar(k, frame.rotationAxis()));
	CHECK(similar(frame.rotationAxis().normal(), frame.rotationAxis()));
}

} // namespace

//#define SINGLE_TEST_FRAME(expr) std::cout << "testing: " << # expr << std::endl; singleTestFrame(expr);
#define SINGLE_TEST_FRAME(expr) singleTestFrame(expr);

TEST_CASE("Frame3D works", "[unit][resource][core]")
{
	singleTestFrameRotationAxis((createTransformRotateZ(M_PI/4)).vector(Vector3D(1,0,0)));
	singleTestFrameRotationAxis((createTransformRotateY(M_PI/4)).vector(Vector3D(1,0,0)));
	singleTestFrameRotationAxis((createTransformRotateZ(M_PI/4)*createTransformRotateY(M_PI/4)).vector(Vector3D(1,0,0)));

	SINGLE_TEST_FRAME( Transform3D::Identity() );
	SINGLE_TEST_FRAME( createTransformTranslate(Vector3D(1,0,0)) );
	SINGLE_TEST_FRAME( createTransformTranslate(Vector3D(4,3,2)) );
	SINGLE_TEST_FRAME( createTransformRotateX(M_PI_2) );
	SINGLE_TEST_FRAME( createTransformRotateX(M_PI_4) );
	SINGLE_TEST_FRAME( createTransformRotateY(0/4)*createTransformRotateX(M_PI/4)*createTransformTranslate(Vector3D(0,0,0)) );
	SINGLE_TEST_FRAME( createTransformRotateX(M_PI/4) );
	SINGLE_TEST_FRAME( createTransformRotateY(4*M_PI/4) * createTransformRotateX(M_PI/4) );
	SINGLE_TEST_FRAME( createTransformRotateY(M_PI/4)*createTransformRotateX(M_PI/3)*createTransformTranslate(Vector3D(3,4,5)) );
	SINGLE_TEST_FRAME( createTransformRotateY(M_PI/4.1)*createTransformRotateX(M_PI/3.3)*createTransformTranslate(Vector3D(3,4,5))*createTransformRotateZ(M_PI/4) );
	SINGLE_TEST_FRAME( createTransformRotateY(M_PI/2)*createTransformRotateX(M_PI/2)*createTransformTranslate(Vector3D(3,4,5)) );
	SINGLE_TEST_FRAME( createTransformRotateX(M_PI)*createTransformTranslate(Vector3D(3,4,5)) );
	SINGLE_TEST_FRAME( createTransformRotateY(M_PI)*createTransformTranslate(Vector3D(3,4,5)) );
	SINGLE_TEST_FRAME( createTransformRotateZ(M_PI)*createTransformTranslate(Vector3D(3,4,5)) );
}



