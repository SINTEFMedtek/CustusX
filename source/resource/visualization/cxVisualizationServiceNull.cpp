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

#include "cxVisualizationServiceNull.h"
#include "cxReporter.h"

namespace cx
{
VisualizationServiceNull::VisualizationServiceNull()
{
}

ViewPtr VisualizationServiceNull::get3DView(int group, int index)
{
	printWarning();
	return ViewPtr();
}

int VisualizationServiceNull::getActiveViewGroup() const
{
	printWarning();
	return -1;
}

ViewGroupDataPtr VisualizationServiceNull::getViewGroupData(int groupIdx)
{
	printWarning();
	return ViewGroupDataPtr();
}
bool VisualizationServiceNull::isNull()
{
	return true;
}

void VisualizationServiceNull::printWarning() const
{
	reportWarning("Trying to use VideoServiceNull. Is VideoService (org.custusx.core.visualization) disabled?");
}

void VisualizationServiceNull::setRegistrationMode(cx::REGISTRATION_STATUS mode)
{
	printWarning();
}

void VisualizationServiceNull::autoShowData(cx::DataPtr data)
{
	printWarning();
}

} //cx
