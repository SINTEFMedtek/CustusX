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
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include "cxtestVisServices.h"
#include "cxViewManager.h"
#include "cxDataLocations.h"
#include "cxSettings.h"

namespace cxtest
{
typedef boost::shared_ptr<class ViewManagerFixture> ViewManagerFixturePtr;
class ViewManagerFixture : public cx::ViewManager
{
public:
	ViewManagerFixture(cx::VisServicesPtr services) :
		cx::ViewManager(services)
	{}
	QList<unsigned> getAutoShowViewGroupNumbers()
	{
		return this->getViewGroupsToAutoShowIn();
	}
};

TEST_CASE("ViewManager: Auto show in view groups", "[unit][plugins][org.custusx.core.view]")
{
    cx::DataLocations::setTestMode();

    cx::settings()->setValue("Automation/autoShowNewDataInViewGroup0", true);
    cx::settings()->setValue("Automation/autoShowNewDataInViewGroup4", true);
    cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();
    ViewManagerFixturePtr viewManager = ViewManagerFixturePtr(new ViewManagerFixture(dummyservices));
    QList<unsigned> showInViewGroups = viewManager->getAutoShowViewGroupNumbers();
    REQUIRE(showInViewGroups.size() == 2);
    CHECK(showInViewGroups[0] == 0);
    CHECK(showInViewGroups[1] == 4);
}

} // cxtest