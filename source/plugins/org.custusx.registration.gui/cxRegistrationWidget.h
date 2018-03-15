/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONWIDGET_H_
#define CXREGISTRATIONWIDGET_H_

#include "org_custusx_registration_gui_Export.h"
#include "cxTabbedWidget.h"
#include "cxServiceTrackerListener.h"
#include "cxRegistrationMethodService.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
class QVBoxLayout;
class QComboBox;
class QStackedWidget;

namespace cx
{
typedef boost::shared_ptr<class StringPropertyBase> StringPropertyBasePtr;


/**
 * Widget for one registration type (image2patient, image2image, ...).
 * Option to select one specific algorithm.
 *
 * \ingroup org_custusx_registration_gui
 *
 * \date Sep 08 2014
 * \author Ole Vegard Solberg, SINTEF
 * \author Geir Arne Tangen, SINTEF
 */
class RegistrationTypeWidget : public BaseWidget
{
	Q_OBJECT
public:
	RegistrationTypeWidget(QString type, QString defVal, XmlOptionFile options, QWidget* parent = 0);
	virtual ~RegistrationTypeWidget() {}

	void selectRegistrationMethod(QString registrationMethodName);

	void addMethod(RegistrationMethodService* service);
	void removeMethod(RegistrationMethodService *service);

private slots:
	void onIndexChanged();
private:
	void removeWidgetFromStackedWidget(QString widgetName);

	QStackedWidget *mStackedWidget;
	XmlOptionFile mOptions;
	StringPropertyPtr mMethodSelector;
};

/**
 * Widget for use in the Registration GUI plugin
 *
 * \ingroup org_custusx_registration_gui
 *
 * \date Sep 08 2014
 * \author Ole Vegard Solberg, SINTEF
 * \author Geir Arne Tangen, SINTEF
 */
class org_custusx_registration_gui_EXPORT RegistrationWidget : public TabbedWidget
{
	Q_OBJECT
public:
	RegistrationWidget(ctkPluginContext *pluginContext, QWidget* parent = 0);
	virtual ~RegistrationWidget() {}

	void selectRegistrationMethod(QString registrationType, QString registrationMethodName);

private slots:
	void onCurrentChanged(int index);
private:
	void initRegistrationTypesWidgets();
	void initServiceListener();

	void onServiceAdded(RegistrationMethodService *service);
	void onServiceRemoved(RegistrationMethodService *service);

	ctkPluginContext* mPluginContext;
	boost::shared_ptr<ServiceTrackerListener<RegistrationMethodService> > mServiceListener;

	StringPropertyPtr mTypeSelector;
	std::map<QString, RegistrationTypeWidget*> mRegistrationTypeMap;
	QStringList mRegistrationTypes;
	XmlOptionFile mOptions;
};

} /* namespace cx */

#endif /* CXREGISTRATIONWIDGET_H_ */
