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
#include <QLabel>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QComboBox>
#include <QStringList>

#include "cxTypeConversions.h"
#include "cxReporter.h"

namespace cx
{

RegistrationWidget::RegistrationWidget(ctkPluginContext *context, QWidget* parent) :
	mPluginContext(context),
	QTabWidget(parent),
	mVerticalLayout(new QVBoxLayout(this))
{
	this->setObjectName("RegistrationWidget");
	this->setWindowTitle("Registration");
	this->setWhatsThis(this->defaultWhatsThis());

//	mVerticalLayout->addWidget(new QLabel("Registration Plugin!"));

	mRegistrationTypes << "ImageToImage" << "ImageToPatient" << "ImageTransform";
	std::cout << "mRegistrationTypes count: " << mRegistrationTypes.count() << std::endl;
	for(int i = 0; i < mRegistrationTypes.count(); ++i)
	{
		QStackedWidget *registrationTypeWidget = new QStackedWidget(this);
		mRegistrationTypeMap[mRegistrationTypes[i]] = registrationTypeWidget;

		QComboBox *methodSelector = new QComboBox(registrationTypeWidget );
		mMethodsSelectorMap[mRegistrationTypes[i]] = methodSelector;

		mVerticalLayout->addWidget(registrationTypeWidget);
		QLayout *registrationTypeWidgetLayout = registrationTypeWidget->layout();

		registrationTypeWidgetLayout->addWidget(methodSelector);
		this->addTab(registrationTypeWidget, mRegistrationTypes[i]);
	}
	this->init();
}

RegistrationWidget::~RegistrationWidget()
{
}

void RegistrationWidget::init()
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
	QWidget* widget = service->getWidget();
	QString registrationType = service->getRegistrationType();
	QString registrationMethod = service->getRegistrationMethod();

//	QStackedWidget *typeWidget = this->findMethodWidget(registrationMethod);
//	typeWidget->addWidget(widget);

	std::cout << "mRegistrationTypes count: " << mRegistrationTypes.count() << std::endl;
//	std::cout << "mRegistrationTypes: " << mRegistrationTypes[0] << " " << mRegistrationTypes[1] << " " << mRegistrationTypes[2] << std::endl;

	if(!mRegistrationTypes.contains(registrationType))
	{
		reportError("Unknown registrationType : " + service->getRegistrationType());
		return;
	}

	mRegistrationTypeMap[registrationType]->addWidget(widget);
	mMethodsSelectorMap[registrationType]->addItem(registrationMethod);
}

//void RegistrationWidget::addWidget()

QStackedWidget *RegistrationWidget::findMethodWidget(QString registrationMethod)
{
	QStackedWidget *methodWidget = NULL;
	if (mMethodsWidgetsMap.find(registrationMethod) != mMethodsWidgetsMap.end())
	{
			std::cout << "found typeWidget: " << registrationMethod << std::endl;
//			retval = mCategorizedWidgets.at(registrationType);
			methodWidget = mMethodsWidgetsMap.at(registrationMethod);
	}
	else
	{
			std::cout << "create typeWidget: " << registrationMethod << std::endl;
			methodWidget = this->createMethodWidget(registrationMethod);
//      retval = this->addAsDockWidget(categoryWidget, registrationType);
			mCategorizedWidgets[registrationMethod] = methodWidget;
	}
	return methodWidget;
}

void RegistrationWidget::onServiceRemoved(RegistrationMethodService *service)
{

}

QStackedWidget *RegistrationWidget::createMethodWidget(QString registrationMethod)
{
		QStackedWidget *categoryWidget = new QStackedWidget(this);
		categoryWidget->setWindowTitle(registrationMethod);
		categoryWidget->setObjectName(registrationMethod);
		mMethodsWidgetsMap[registrationMethod] = categoryWidget;
		QComboBox *methodsSelector = new QComboBox(this);
		mMethodsSelectorMap[registrationMethod] = methodsSelector;
		connect(methodsSelector, SIGNAL(activated(int)),methodsSelector,SLOT(setCurrentIndex(int)));
		mVerticalLayout->addWidget(categoryWidget);
		return categoryWidget;
}

QString RegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Example plugin.</h3>"
      "<p>Used for developers as a starting points for developing a new plugin</p>"
      "</html>";
}



} /* namespace cx */
