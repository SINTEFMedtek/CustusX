/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef AIRWAYSFIXTURE_H
#define AIRWAYSFIXTURE_H

#include <QString>

#include "cxtest_org_custusx_filter_airways_export.h"

namespace cxtest {

class CXTEST_ORG_CUSTUSX_FILTER_AIRWAYS_EXPORT AirwaysFixture
{
public:
    AirwaysFixture();
    ~AirwaysFixture();

    void testLungAirwaysCT();

private:
	void runFilter(const QString& preset);
};

}
#endif // AIRWAYSFIXTURE_H
