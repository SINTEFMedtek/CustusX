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

#include "cxViewGroupPropertiesWidget.h"
#include <QLabel>
#include "cxStringListSelectWidget.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"

namespace cx
{

ViewGroupPropertiesWidget::ViewGroupPropertiesWidget(DoublePropertyBasePtr selector,
													 ViewServicePtr viewService,
													 QWidget* parent) :
	BaseWidget(parent, "ViewGroupPropertiesWidget", "View Properties"),
	mSelector(selector),
	mViewService(viewService),
	mWidget(NULL)
{
	mLayout = new QVBoxLayout(this);
	this->setModified();
}

ViewGroupPropertiesWidget::~ViewGroupPropertiesWidget()
{
}

ViewGroupDataPtr ViewGroupPropertiesWidget::getViewGroup()
{
	return mViewService->getGroup(mSelector->getValue());
}

void ViewGroupPropertiesWidget::setupUI()
{
	if (mWidget) // already created
		return;

	ViewGroupDataPtr viewGroup = this->getViewGroup();
	StringListSelectWidget* slices3D = new StringListSelectWidget(this, viewGroup->getSliceDefinitionProperty());
//	slices3D->showLabel(false);
//	slices3D->setIcon(QIcon(":/icons/open_icon_library/eye.png.png"));
	mLayout->addWidget(slices3D);

	mLayout->addWidget(new QLabel("view group props"));
	mLayout->addStretch();

	connect(mSelector.get(), &DoublePropertyBase::changed, this, &ViewGroupPropertiesWidget::onSelectorChanged);

	this->onSelectorChanged();
}

void ViewGroupPropertiesWidget::prePaintEvent()
{
	this->setupUI();
	this->updateFrontend();
	//	this->toolPositionChanged();
}

void ViewGroupPropertiesWidget::onSelectorChanged()
{
}

void ViewGroupPropertiesWidget::updateFrontend()
{

}

} // cx
