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
#include "cxReporter.h"
#include "cxRegistrationDataAdapters.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"
#include "cxStringDataAdapterXml.h"
#include "cxDataLocations.h"

namespace cx
{

RegistrationWidget::RegistrationWidget(ctkPluginContext *pluginContext, QWidget* parent) :
	mPluginContext(pluginContext),
	QTabWidget(parent),
	mVerticalLayout(new QVBoxLayout(this)),
	mOptions(XmlOptionFile(DataLocations::getXmlSettingsFile()).descend("RegistrationWidget"))
{
	this->setObjectName("RegistrationWidget");
	this->setWindowTitle("Registration");
	this->setWhatsThis(this->defaultWhatsThis());

	this->initRegistrationTypesWidgets();
	this->initServiceListener();
}

RegistrationWidget::~RegistrationWidget()
{
}

void RegistrationWidget::initRegistrationTypesWidgets()
{
	mRegistrationTypes << "ImageToPatient" << "ImageToImage" << "ImageTransform";
	for(int i = 0; i < mRegistrationTypes.count(); ++i)
	{
		QWidget *widget = new QWidget(this);
		QStackedWidget *registrationTypeWidget = new QStackedWidget(widget);
		mRegistrationTypeMap[mRegistrationTypes[i]] = registrationTypeWidget;

		QVBoxLayout *layoutV = new QVBoxLayout(widget);

		StringDataAdapterXmlPtr methodSelector = StringDataAdapterXml::initialize(mRegistrationTypes[i],
																				  "Method",
																				  "Select registration method",
																				  "",
																				  QStringList(),
																				  mOptions.getElement());
		mMethodsSelectorMap[mRegistrationTypes[i]] = methodSelector;
		boost::function<void()> f = boost::bind(&RegistrationWidget::indexChanged, this, mRegistrationTypes[i]);
		connect(methodSelector.get(), &StringDataAdapterXml::valueWasSet, f);

		layoutV->addWidget(new LabeledComboBoxWidget(this, methodSelector));
		layoutV->addWidget(registrationTypeWidget);

		mVerticalLayout->addWidget(widget);
		this->addTab(widget, mRegistrationTypes[i]);
	}
}

void RegistrationWidget::indexChanged(QString registrationType)
{
	StringDataAdapterXmlPtr comboBox = mMethodsSelectorMap[registrationType];
	QStackedWidget *stackedWidget = mRegistrationTypeMap[registrationType];
	this->selectStackWidget(comboBox, stackedWidget);
}

void RegistrationWidget::selectStackWidget(StringDataAdapterXmlPtr comboBox, QStackedWidget *stackedWidget)
{
	QString method = comboBox->getValue();
	int pos = comboBox->getValueRange().indexOf(method);
	stackedWidget->setCurrentIndex(pos);
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
	if(!this->knownType(service->getRegistrationType()))
		return;

	StringDataAdapterXmlPtr comboBox = mMethodsSelectorMap[service->getRegistrationType()];
	QStackedWidget *stackedWidget = mRegistrationTypeMap[service->getRegistrationType()];

	stackedWidget->addWidget(service->createWidget());
	QStringList values = comboBox->getValueRange();
	values << service->getRegistrationMethod();
	comboBox->setValueRange(values);

	if (comboBox->getValue() == service->getRegistrationMethod())
		this->selectStackWidget(comboBox, stackedWidget);
}

bool RegistrationWidget::knownType(QString registrationType)
{
	if(!mRegistrationTypes.contains(registrationType))
	{
		reportError("Unknown registrationType : " + registrationType);
		return false;
	}
	return true;
}

void RegistrationWidget::onServiceRemoved(RegistrationMethodService *service)
{
	QStackedWidget *stackedWidget = mRegistrationTypeMap[service->getRegistrationType()];
	this->removeWidgetFromStackedWidget(service->getWidgetName(), stackedWidget);

	StringDataAdapterXmlPtr comboBox = mMethodsSelectorMap[service->getRegistrationType()];
	QStringList values = comboBox->getValueRange();
	if (!values.removeOne(service->getRegistrationMethod()))
		reportWarning("RegistrationWidget::onServiceRemoved: Cannot find and remove service from combobox: "+ service->getRegistrationMethod());
	comboBox->setValueRange(values);
}

void RegistrationWidget::removeWidgetFromStackedWidget(QString widgetName, QStackedWidget *stackedWidget)
{
	for(int i = 0; i < stackedWidget->count(); ++i)
	{
		QWidget* widget = stackedWidget->widget(i);
		if(widget->objectName() == widgetName)
		{
			stackedWidget->removeWidget(widget);
			delete widget;
		}
	}
}

QString RegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Example plugin.</h3>"
	  "<p>Collection of all registration methods</p>"
      "</html>";
}

} /* namespace cx */
