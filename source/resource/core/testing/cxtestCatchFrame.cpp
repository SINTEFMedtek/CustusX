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

#include "cxFrame3D.h"
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



