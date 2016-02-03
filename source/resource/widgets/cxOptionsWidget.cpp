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

#include "cxOptionsWidget.h"

#include <QLabel>
#include "cxBaseWidget.h"
#include "cxHelperWidgets.h"

namespace cx {

OptionsWidget::OptionsWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent) :
		mShowAdvanced(false),
		mViewService(viewService),
		mPatientModelService(patientModelService)
{
	this->setSizePolicy(this->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	mStackedLayout = new QStackedLayout(this);
	mStackedLayout->setMargin(0);
}

void OptionsWidget::setOptions(QString uid, std::vector<SelectDataStringPropertyBasePtr> options, bool showAdvanced)
{
	std::vector<PropertyPtr> converted;
	std::copy(options.begin(), options.end(), std::back_inserter(converted));
	this->setOptions(uid, converted, showAdvanced);
}

void OptionsWidget::setOptions(QString uid, std::vector<PropertyPtr> options, bool showAdvanced)
{
	// return if already on uid
	if (mStackedLayout->currentWidget() && (uid == mStackedLayout->currentWidget()->objectName()))
		return;

	mOptions = options;
	mUid = uid;

	this->showAdvanced(showAdvanced);
}

QString OptionsWidget::getCurrentUid()
{
	return mStackedLayout->currentWidget()->objectName();
}

void OptionsWidget::showAdvanced(bool show)
{
	mShowAdvanced = show;
	this->rebuild();
}

void OptionsWidget::rebuild()
{
	this->clear();
	this->populate(mShowAdvanced);
}

bool OptionsWidget::hasOptions() const
{
    if(mOptions.size() == 0)
        return false;

    return true;
}

bool OptionsWidget::hasAdvancedOptions() const
{
    for(std::vector<PropertyPtr>::const_iterator it = mOptions.begin(); it != mOptions.end(); ++it)
    {
        if(it->get()->getAdvanced())
            return true;
    }

    return false;
}

void OptionsWidget::toggleAdvanced()
{
	this->showAdvanced(!mShowAdvanced);
}

void OptionsWidget::clear()
{
	QLayoutItem *child;
	while ((child = mStackedLayout->takeAt(0)) != 0)
	{
		// delete both the layoutitem AND the widget. Not auto done because layoutitem is no QObject.
		QWidget* widget = child->widget();
		delete child;
		delete widget;
	}
}

void OptionsWidget::populate(bool showAdvanced)
{
	// No existing found,
	// create a new stack element for this uid:
	QWidget* widget = new QWidget(this);
	widget->setObjectName(mUid);
	mStackedLayout->addWidget(widget);
	QGridLayout* layout = new QGridLayout(widget);
	layout->setMargin(layout->margin()/2);

	std::map<QString, QWidget*> groupWidgets;
	QWidget* otherWidget = NULL;
	for (unsigned i = 0; i < mOptions.size(); ++i)
	{
		if(showAdvanced || (!showAdvanced && !mOptions[i]->getAdvanced()))
		{
			QWidget* groupWidget = NULL;
			QGridLayout* groupLayout = NULL;

			//make new group if needed
			QString groupName = mOptions[i]->getGroup();
			if(groupName.isEmpty())
				groupName = "other";
			std::map<QString, QWidget*>::iterator it = groupWidgets.find(groupName);
			if(it == groupWidgets.end())
			{
				groupWidget = new QWidget(widget);
				groupWidget->setObjectName(groupName);
				groupLayout = new QGridLayout(groupWidget);
				groupLayout->setMargin(groupLayout->margin()/2);
				QWidget* temp = this->createGroupHeaderWidget(groupName);
				groupLayout->addWidget(temp,0,0,1,2);
				layout->addWidget(groupWidget);
				groupWidgets[groupName] = groupWidget;
				if(groupName == "other")
					otherWidget = temp;
			}
			else
			{
				groupWidget = it->second;
				groupLayout = dynamic_cast<QGridLayout*>(groupWidget->layout());
			}

			//count groupwidgets items to determine row
			int itemsInGroup = groupLayout->count();

			//make dataadaptewidget and add to existing group
			blockSignals(true);
			createDataWidget(mViewService, mPatientModelService, groupWidget, mOptions[i], groupLayout, ++itemsInGroup);
			blockSignals(false);
		}
	}

	//hide group header if only one the "other" group exists
	if((groupWidgets.size() == 1) && (otherWidget != NULL))
		otherWidget->hide();

	mStackedLayout->setCurrentWidget(widget);
}

QWidget* OptionsWidget::createGroupHeaderWidget(QString title)
{
	QWidget* retval = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(retval);
	layout->setMargin(0);
	layout->setSpacing(0);

	QLabel* label = new QLabel(title);
	QFont font = label->font();
	font.setPointSize(8);
	label->setFont(font);
	layout->addWidget(label);
	layout->addWidget(BaseWidget::createHorizontalLine());

	return retval;
}

} /* namespace cx */
