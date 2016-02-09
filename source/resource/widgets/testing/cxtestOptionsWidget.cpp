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
#include "cxOptionsWidget.h"
#include "cxBoolProperty.h"


namespace cxtest
{

namespace  {

char* testLabels("[unit][resource][widgets]");

struct testWidget
{
    cx::OptionsWidget optionsWidget;

    testWidget() :
        optionsWidget(cx::ViewServicePtr(), cx::PatientModelServicePtr(), NULL)
    {
    }

    void checkOptions(bool hasOptions, bool hasAdvancedOptions)
    {
        CHECK(optionsWidget.hasOptions() == hasOptions);
        CHECK(optionsWidget.hasAdvancedOptions() == hasAdvancedOptions);
    }

    void addDummyOption(cx::BoolPropertyPtr boolOption)
    {
        std::vector<cx::PropertyPtr> options;
        options.push_back(boolOption);
        optionsWidget.setOptions("uid2", options, false);
    }
};

} //namespace

TEST_CASE("OptionsWidget with no options (and hence no advanced options)", testLabels)
{
    testWidget t;
    t.checkOptions(false, false);
}

TEST_CASE("OptionsWidget with one option which is not advanced", testLabels)
{
    testWidget t;

    cx::BoolPropertyPtr boolOption = cx::BoolProperty::initialize("uid", "name", "help", true);

    t.addDummyOption(boolOption);
    t.checkOptions(true, false);
}

TEST_CASE("OptionsWidget with one advanced option", testLabels)
{
    testWidget t;

    cx::BoolPropertyPtr boolOption = cx::BoolProperty::initialize("uid", "name", "help", true);
    boolOption->setAdvanced(true);

    t.addDummyOption(boolOption);
    t.checkOptions(true, true);
}


} // namespace cxtest
