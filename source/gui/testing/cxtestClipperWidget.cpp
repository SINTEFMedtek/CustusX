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
#include "catch.hpp"
#include "cxClipperWidget.h"
#include "cxVisServices.h"
#include "cxClippers.h"
#include "cxInteractiveClipper.h"

namespace cxtest
{

class ClipperWidgetFixture : public cx::ClipperWidget
{
public:
	ClipperWidgetFixture() :
		ClipperWidget(cx::VisServices::getNullObjects(), NULL)
	{
		cx::ClippersPtr clippers = cx::ClippersPtr(new cx::Clippers(mServices));
		QString clipperName = clippers->getClipperNames().first();
		testClipper = clippers->getClipper(clipperName);

		clipperName = clippers->getClipperNames().last();
		testClipper2 = clippers->getClipper(clipperName);
	}

	cx::InteractiveClipperPtr testClipper;
	cx::InteractiveClipperPtr testClipper2;
};

}//cxtest

TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Set clipper", "[unit][gui][widget]")
{
	CHECK_FALSE(mClipper);
	this->setClipper(testClipper);
	CHECK(mClipper);
}

TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Enable clipper", "[unit][gui][widget]")
{
	this->setClipper(testClipper);
	this->mUseClipperCheckBox->setChecked(true);
	CHECK(mUseClipperCheckBox->isChecked());
	CHECK(testClipper->getUseClipper());

	this->mUseClipperCheckBox->setChecked(false);
	CHECK_FALSE(testClipper->getUseClipper());

	this->setClipper(testClipper2);
	this->mUseClipperCheckBox->setChecked(true);
	CHECK(testClipper2->getUseClipper());

	this->setClipper(testClipper);
	CHECK_FALSE(testClipper->getUseClipper());
	CHECK_FALSE(mUseClipperCheckBox->isChecked());
}
