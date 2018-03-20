/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTPROCESSWRAPPERFIXTURE_H_
#define CXTESTPROCESSWRAPPERFIXTURE_H_

#include "cxProcessWrapper.h"

namespace cxtest {

/**
 * ProcessWrapperFixture
 *
 *  \date Oct 22, 2013
 *  \author: Janne Beate Bakeng, SINTEF
 */

class ProcessWrapperFixture
{

public:
	ProcessWrapperFixture();
	virtual ~ProcessWrapperFixture();

	static bool canLaunchGit_Version();
	static bool canLaunchVLC_Version();
	static bool canLaunchNotExistingExecutable();

private:
	static bool getResultFromFinishedExecution(cx::ProcessWrapperPtr exe);
};

} /* namespace cxtest */
#endif /* CXTESTPROCESSWRAPPERFIXTURE_H_ */
