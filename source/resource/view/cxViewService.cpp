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

#include "cxViewService.h"
#include "cxViewServiceNull.h"
#include "cxNullDeleter.h"

#include "cxRepContainer.h"
#include "cxView.h"

namespace cx
{
ViewServicePtr ViewService::getNullObject()
{
	static ViewServicePtr mNull;
	if (!mNull)
		mNull.reset(new ViewServiceNull, null_deleter());
	return mNull;
}


unsigned ViewService::groupCount() const
{
	int count = 0;
	while(this->getGroup(count))
		++count;
	return count;
}

void ViewService::deactivateLayout()
{
	this->setActiveLayout("", 0);
	this->setActiveLayout("", 1);
}

RepContainerPtr ViewService::get3DReps(int group, int index)
{
	ViewPtr view = this->get3DView(group, index);

	if(view)
		return RepContainerPtr(new RepContainer(view->getReps()));
	else
		return RepContainerPtr(new RepContainer(std::vector<RepPtr>()));
}

ViewGroupDataPtr ViewService::getActiveViewGroup()
{
	int groupId = this->getActiveGroupId();
	return this->getGroup(groupId);
}

} //cx


