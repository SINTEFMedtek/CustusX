/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "catch.hpp"
#include "cxOptionsWidget.h"
#include "cxBoolProperty.h"

#include "cxtestresourcewidgets_export.h"


namespace cxtest
{

namespace  {

struct CXTESTRESOURCEWIDGETS_EXPORT testWidget
{
    cx::OptionsWidget mOptionsWidget;
    bool mExpectedHasOptionsValue;
    bool mExpectedHasAdvancedOptionsValue;

    testWidget(bool expectedHasOptionsValue, bool expectedHasAdvancedOptionsValue) :
        mExpectedHasOptionsValue(expectedHasOptionsValue),
        mExpectedHasAdvancedOptionsValue(expectedHasAdvancedOptionsValue),
        mOptionsWidget(cx::ViewServicePtr(), cx::PatientModelServicePtr(), NULL)
    {
    }

    void testThatHasOptionsMethodsReturnTheExpected()
    {
        CHECK(mOptionsWidget.hasOptions() == mExpectedHasOptionsValue);
        CHECK(mOptionsWidget.hasAdvancedOptions() == mExpectedHasAdvancedOptionsValue);
    }

    void addDummyOption(cx::BoolPropertyPtr boolOption)
    {
        std::vector<cx::PropertyPtr> options;
        options.push_back(boolOption);
        mOptionsWidget.setOptions("uid2", options, false);
    }
};

} //namespace

TEST_CASE("OptionsWidget with no options (and hence no advanced options)", "[unit][resource][widgets]")
{
    testWidget tw(false, false);
    tw.testThatHasOptionsMethodsReturnTheExpected();
}

TEST_CASE("OptionsWidget with one option which is not advanced", "[unit][resource][widgets]")
{
    testWidget tw(true, false);

    cx::BoolPropertyPtr boolOption = cx::BoolProperty::initialize("uid", "name", "help", true);

    tw.addDummyOption(boolOption);
    tw.testThatHasOptionsMethodsReturnTheExpected();
}

TEST_CASE("OptionsWidget with one advanced option", "[unit][resource][widgets]")
{
    testWidget tw(true, true);

    cx::BoolPropertyPtr boolOption = cx::BoolProperty::initialize("uid", "name", "help", true);
    boolOption->setAdvanced(true);

    tw.addDummyOption(boolOption);
    tw.testThatHasOptionsMethodsReturnTheExpected();
}


} // namespace cxtest
