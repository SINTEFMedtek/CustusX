/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewService.h"
#include "cxViewServiceNull.h"
#include "cxNullDeleter.h"

#include "cxRepContainer.h"
#include "cxView.h"
#include "cxEnumConverter.h"

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, CAMERA_STYLE_TYPE, cstCOUNT)
{
	"DEFAULT_STYLE",
	"TOOL_STYLE",
	"ANGLED_TOOL_STYLE",
	"UNICAM_STYLE"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, CAMERA_STYLE_TYPE, cstCOUNT)

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


