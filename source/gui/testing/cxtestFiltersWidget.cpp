/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QApplication>
#include <QComboBox>
#include "cxLogicManager.h"
#include "cxVisServices.h"
#include "cxFilterImpl.h"
#include "cxFiltersWidget.h"
#include "cxFilterSetupWidget.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegisteredService.h"

namespace cxtest
{

namespace
{

/** A filter registered as a service, shared by all the FiltersWidgets, like the filters in plugins. */
class SharedTestFilter : public cx::FilterImpl
{
public:
	SharedTestFilter(cx::VisServicesPtr services) : cx::FilterImpl(services) {}

	QString getType() const override { return "shared_test_filter"; }
	QString getName() const override { return "Shared Test Filter"; }
	QString getHelp() const override { return "Help for the shared test filter"; }
	bool execute() override { return true; }
	bool postProcess() override { return true; }

protected:
	void createOptions() override {}
	void createInputTypes() override {}
	void createOutputTypes() override {}
};

/** Select a filter by name in the widget's filter selector, as the user does. */
void selectFilter(cx::FiltersWidget* widget, QString name)
{
	cx::LabeledComboBoxWidget* selector = widget->findChildren<cx::LabeledComboBoxWidget*>(QString(), Qt::FindDirectChildrenOnly).front();
	QComboBox* combo = selector->findChild<QComboBox*>();
	widget->show();
	widget->repaint(); // the selector gets its items when painted
	qApp->processEvents();
	int index = combo->findText(name);
	REQUIRE(index >= 0);
	combo->setCurrentIndex(index);
}

QString getSelectedFilterType(cx::FiltersWidget* widget)
{
	return widget->findChild<cx::FilterSetupWidget*>()->objectName();
}

} // namespace

TEST_CASE("FiltersWidget: a filter service is selectable while another FiltersWidget also has it", "[unit][gui][widget][not_win32]")
{
	cx::LogicManager::initialize();
	{
		cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());
		cx::RegisteredServicePtr registration = cx::RegisteredService::create(cx::logicManager()->getPluginContext(),
																			  new SharedTestFilter(services), FilterService_iid);

		cx::FiltersWidget* allFilters = new cx::FiltersWidget(services, NULL, QStringList(), "test_all_filters");
		cx::FiltersWidget* singleFilter = new cx::FiltersWidget(services, NULL, QStringList("Shared Test Filter"), "test_single_filter");

		selectFilter(allFilters, "Shared Test Filter");
		CHECK(getSelectedFilterType(allFilters) == "shared_test_filter");
		CHECK(allFilters->findChild<cx::FilterSetupWidget*>()->toolTip().contains("Help for the shared test filter"));
		CHECK(getSelectedFilterType(singleFilter) == "shared_test_filter");

		delete singleFilter;
		delete allFilters;
		registration.reset();
	}
	cx::LogicManager::shutdown();
}

} // namespace cxtest
