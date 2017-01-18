#ifndef CXOPERATINGTABLETAB_H
#define CXOPERATINGTABLETAB_H

#include "cxPreferenceTab.h"
#include "cxCoreServices.h"

namespace cx
{

/** Wrap TableWidget in a PreferenceTab
 */
class cxGui_EXPORT OperatingTableTab : public PreferenceTab
{
	Q_OBJECT

public:
	OperatingTableTab(CoreServicesPtr services, QWidget *parent = 0);
	void init();

public slots:
	void saveParametersSlot() {}

protected:
	CoreServicesPtr mServices;

private slots:
};

} /* namespace cx */

#endif // CXOPERATINGTABLETAB_H
