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

#include "cxRegistrationWidget.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QStackedWidget>
#include <QComboBox>
#include <QStringList>

#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxRegistrationProperties.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"
#include "cxStringProperty.h"
#include "cxProfile.h"

namespace cx
{

RegistrationTypeWidget::RegistrationTypeWidget(QString type, QString defVal, XmlOptionFile options, QWidget* parent) :
	BaseWidget(parent, type, type),
	mOptions(options)
{
	mStackedWidget = new QStackedWidget(this);
	mStackedWidget->setFocusPolicy(Qt::StrongFocus);

	QVBoxLayout *layoutV = new QVBoxLayout(this);
//		layoutV->setMargin(0);

	mMethodSelector = StringProperty::initialize(type,
												 "Method",
												 "Select registration method",
												 defVal,
												 QStringList(),
												 mOptions.getElement());
	connect(mMethodSelector.get(), &StringProperty::valueWasSet, this, &RegistrationTypeWidget::onIndexChanged);

	layoutV->addWidget(new LabeledComboBoxWidget(this, mMethodSelector));
	layoutV->addWidget(mStackedWidget);
}

void RegistrationTypeWidget::onIndexChanged()
{
	QString method = mMethodSelector->getValue();
	int pos = mMethodSelector->getValueRange().indexOf(method);
	mStackedWidget->setCurrentIndex(pos);
	this->setObjectName(mStackedWidget->currentWidget()->objectName());
}

void RegistrationTypeWidget::addMethod(RegistrationMethodService* service)
{
	mStackedWidget->addWidget(service->createWidget());
	QStringList values = mMethodSelector->getValueRange();
	values << service->getRegistrationMethod();
	mMethodSelector->setValueRange(values);

	// initialize if not set
	if (mMethodSelector->getValue().isEmpty())
	{
		mMethodSelector->setValue(service->getRegistrationMethod());
	}

	if (mMethodSelector->getValue() == service->getRegistrationMethod())
		this->onIndexChanged();
}

void RegistrationTypeWidget::removeMethod(RegistrationMethodService *service)
{
	this->removeWidgetFromStackedWidget(service->getWidgetName());

	QStringList values = mMethodSelector->getValueRange();
	if (!values.removeOne(service->getRegistrationMethod()))
		reportWarning("RegistrationWidget::onServiceRemoved: Cannot find and remove service from combobox: "+ service->getRegistrationMethod());
	mMethodSelector->setValueRange(values);
}

void RegistrationTypeWidget::removeWidgetFromStackedWidget(QString widgetName)
{
	for(int i = 0; i < mStackedWidget->count(); ++i)
	{
		QWidget* widget = mStackedWidget->widget(i);
		if(widget->objectName() == widgetName)
		{
			mStackedWidget->removeWidget(widget);
			delete widget;
		}
	}
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


RegistrationWidget::RegistrationWidget(ctkPluginContext *pluginContext, QWidget* parent) :
	TabbedWidget(parent, "org_custusx_registration_gui_widget", "Registration"),
	mPluginContext(pluginContext),
	mOptions(profile()->getXmlSettings().descend("RegistrationWidget"))
{
	connect(mTabWidget, &QTabWidget::currentChanged, this, &RegistrationWidget::onCurrentChanged);
	this->initRegistrationTypesWidgets();
	this->initServiceListener();
}

void RegistrationWidget::initRegistrationTypesWidgets()
{
	mRegistrationTypes << "ImageToPatient" <<	"ImageToImage" << "ImageTransform";
	QStringList typeDefaults;
	typeDefaults << "Landmark" << "Landmark" << "";

	for(int i = 0; i < mRegistrationTypes.count(); ++i)
	{
		QString type = mRegistrationTypes[i];
		RegistrationTypeWidget* widget =
				new RegistrationTypeWidget(type,
									   typeDefaults[i],
									   mOptions, this);

		mRegistrationTypeMap[type] = widget;
		this->addTab(widget, type);

	}

	// Create typeselector after the widgets, as the addwidget trigger
	// a signal that causes type to be overwritten.
	mTypeSelector = StringProperty::initialize("RegistrationTypes",
													 "Registration Types",
													 "Select registration type",
													 "",
													 mRegistrationTypes,
													 mOptions.getElement());
	if (mRegistrationTypeMap.count(mTypeSelector->getValue()))
		mTabWidget->setCurrentWidget(mRegistrationTypeMap[mTypeSelector->getValue()]);
}

void RegistrationWidget::onCurrentChanged(int index)
{
	if (index<0)
		return;
	if (mTypeSelector)
		mTypeSelector->setValue(mRegistrationTypes[index]);
}

void RegistrationWidget::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<RegistrationMethodService>(
							   mPluginContext,
							   boost::bind(&RegistrationWidget::onServiceAdded, this, _1),
							   boost::function<void (RegistrationMethodService*)>(),
							   boost::bind(&RegistrationWidget::onServiceRemoved, this, _1)
							   ));
	mServiceListener->open();
}

void RegistrationWidget::onServiceAdded(RegistrationMethodService* service)
{
	QString type = service->getRegistrationType();
	if(!mRegistrationTypeMap.count(type))
	{
		reportError("Unknown registrationType : " + type);
		return;
	}
	mRegistrationTypeMap[type]->addMethod(service);
}

void RegistrationWidget::onServiceRemoved(RegistrationMethodService *service)
{
	QString type = service->getRegistrationType();
	mRegistrationTypeMap[type]->removeMethod(service);
}

} /* namespace cx */
