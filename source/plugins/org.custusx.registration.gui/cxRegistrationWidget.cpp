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
#include <QGroupBox>
#include <QStackedWidget>
#include <QComboBox>
#include <QStringList>

#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxRegistrationDataAdapters.h"
#include "cxLabeledComboBoxWidget.h"

namespace cx
{

RegistrationWidget::RegistrationWidget(ctkPluginContext *pluginContext, QWidget* parent) :
	mPluginContext(pluginContext),
	QTabWidget(parent),
	mVerticalLayout(new QVBoxLayout(this))
{
	this->setObjectName("RegistrationWidget");
	this->setWindowTitle("Registration");
	this->setWhatsThis(this->defaultWhatsThis());

//	mVerticalLayout->addWidget(new QLabel("Registration Plugin!"));

	this->initRegistrationTypesWidgets();
	this->initServiceListener();
}

RegistrationWidget::~RegistrationWidget()
{
}

void RegistrationWidget::initRegistrationTypesWidgets()
{
	mRegistrationTypes << "ImageToImage" << "ImageToPatient" << "ImageTransform";
	std::vector<QVBoxLayout*> layouts (3);
	for(int i = 0; i < mRegistrationTypes.count(); ++i)
	{
		QWidget *widget = new QWidget(this);
		QStackedWidget *registrationTypeWidget = new QStackedWidget(widget);
		mRegistrationTypeMap[mRegistrationTypes[i]] = registrationTypeWidget;

		QComboBox *methodSelector = new QComboBox(registrationTypeWidget );
		mMethodsSelectorMap[mRegistrationTypes[i]] = methodSelector;

		QGroupBox	*groupBox = new QGroupBox(registrationTypeWidget);

		QVBoxLayout *layoutV = new QVBoxLayout(widget);
		QVBoxLayout *layoutGroupBox = new QVBoxLayout(groupBox);
		QHBoxLayout *layoutH = new QHBoxLayout();

		layoutH->addWidget(methodSelector);
		layoutH->addStretch();

		layoutV->addLayout(layoutH);
		layoutV->addWidget(groupBox);

		layoutGroupBox->addWidget(registrationTypeWidget);

		layouts[i] = layoutGroupBox;
		mVerticalLayout->addWidget(widget);
		this->addTab(widget, mRegistrationTypes[i]);
	}

	this->insertImageComboBoxes(layouts);
}

void RegistrationWidget::insertImageComboBoxes(std::vector<QVBoxLayout*> layouts)
{
	StringDataAdapterPtr fixedImage(new RegistrationFixedImageStringDataAdapter(mPluginContext));
	StringDataAdapterPtr movingImage(new RegistrationMovingImageStringDataAdapter(mPluginContext));

	this->insertImageComboInLayout(fixedImage, layouts[0], 0);
	this->insertImageComboInLayout(movingImage, layouts[0], 1);
//	this->insertImageComboInLayout(movingImage, layouts[1], 0);
}

void RegistrationWidget::insertImageComboInLayout(StringDataAdapterPtr adapter, QVBoxLayout *layout, int position)
{
	LabeledComboBoxWidget* imageCombo = new LabeledComboBoxWidget(this, adapter);
	layout->insertWidget(position, imageCombo);
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
	QWidget* widget = service->getWidget();
	QString registrationType = service->getRegistrationType();
	QString registrationMethod = service->getRegistrationMethod();

//	QStackedWidget *typeWidget = this->findMethodWidget(registrationMethod);
//	typeWidget->addWidget(widget);

	if(!mRegistrationTypes.contains(registrationType))
	{
		reportError("Unknown registrationType : " + service->getRegistrationType());
		return;
	}

	mRegistrationTypeMap[registrationType]->addWidget(widget);
	mMethodsSelectorMap[registrationType]->addItem(registrationMethod);
}

void RegistrationWidget::onServiceRemoved(RegistrationMethodService *service)
{

}

/*QStackedWidget *RegistrationWidget::findMethodWidget(QString registrationMethod)
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
}*/

QString RegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Example plugin.</h3>"
      "<p>Used for developers as a starting points for developing a new plugin</p>"
      "</html>";
}



} /* namespace cx */
