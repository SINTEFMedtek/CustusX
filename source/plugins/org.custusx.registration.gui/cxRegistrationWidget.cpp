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
#include <QLabel>
#include <QVBoxLayout>
#include <QTabWidget>

namespace cx
{

RegistrationWidget::RegistrationWidget(QWidget* parent) :
    QWidget(parent),
    mVerticalLayout(new QVBoxLayout(this))
{
		this->setObjectName("RegistrationWidget");
		this->setWindowTitle("Registration");
    this->setWhatsThis(this->defaultWhatsThis());

		mVerticalLayout->addWidget(new QLabel("Registration Plugin!"));

		mServiceListener.reset(new ServiceTrackerListener<RegistrationMethodService>(
									 mPluginContext,
									 boost::bind(&RegistrationWidget::onServiceAdded, this, _1),
									 boost::function<void (RegistrationMethodService*)>(),
									 boost::bind(&RegistrationWidget::onServiceRemoved, this, _1)
									 ));
		mServiceListener->open();

}

RegistrationWidget::~RegistrationWidget()
{
}

void RegistrationWidget::onServiceAdded(RegistrationMethodService* service)
{
	QWidget* widget = service->getWidget();
	QString registrationType = service->getRegistrationType();

	QTabWidget *typeWidget = this->findTypeWidget(registrationType);
	typeWidget->addTab(widget, widget->windowTitle());
}

QTabWidget *RegistrationWidget::findTypeWidget(QString registrationType)
{
	QTabWidget *typeWidget = NULL;
	if (mCategoryWidgetsMap.find(registrationType) != mCategoryWidgetsMap.end())
	{
			std::cout << "found typeWidget: " << registrationType << std::endl;
			retval = mCategorizedWidgets.at(registrationType);
			typeWidget = mCategoryWidgetsMap.at(registrationType);
	}
	else
	{
			std::cout << "create typeWidget: " << registrationType << std::endl;
			typeWidget = this->createTypeWidget(registrationType);
//      retval = this->addAsDockWidget(categoryWidget, registrationType);
			mCategorizedWidgets[registrationType] = retval;
	}
	return typeWidget;
}

void RegistrationWidget::onServiceRemoved(RegistrationMethodService *service)
{

}

QTabWidget *RegistrationWidget::createTypeWidget(QString registrationType)
{
		QTabWidget *categoryWidget = new QTabWidget(this);
		categoryWidget->setWindowTitle(registrationType);
		categoryWidget->setObjectName(registrationType);
		mCategoryWidgetsMap[category] = categoryWidget;
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
