/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

void RegistrationTypeWidget::selectRegistrationMethod(QString selectRegistrationMethod)
{
	mMethodSelector->setValue(selectRegistrationMethod);
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

void RegistrationWidget::selectRegistrationMethod(QString registrationType, QString registrationMethodName)
{
	RegistrationTypeWidget* widget = mRegistrationTypeMap[registrationType];
	if(widget)
		widget->selectRegistrationMethod(registrationMethodName);
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
