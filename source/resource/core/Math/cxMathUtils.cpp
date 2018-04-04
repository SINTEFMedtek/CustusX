/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMathUtils.h"
namespace cx {
double roundAwayFromZero(double val)
{
	if(val >= 0)
		return int(val+0.5);
	else
		return int(val-0.5);
}

unsigned int roundUnsigned(double val)
{
    return int(val+0.5);
}


}
