#include "cxOperatingTableTab.h"
#include "cxOperatingTableWidget.h"
#include <QVBoxLayout>


namespace cx
{

OperatingTableTab::OperatingTableTab(CoreServicesPtr services, QWidget *parent) :
	PreferenceTab(parent),
	mServices(services)
{

}

void OperatingTableTab::init()
{
	OperatingTableWidget* tw = new OperatingTableWidget(mServices);
	mTopLayout->addWidget(tw);
}


} // namespace cx
