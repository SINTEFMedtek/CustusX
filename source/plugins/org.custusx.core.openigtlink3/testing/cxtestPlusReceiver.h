#ifndef PLUSRECEIVER_H
#define PLUSRECEIVER_H
#include "cxtest_org_custusx_core_openigtlink3_export.h"

#include "catch.hpp"

#include "cxtestReceiver.h"

namespace cxtest
{

class CXTEST_ORG_CUSTUSX_CORE_OPENIGTLINK3_EXPORT PlusReceiver : public Receiver
{
public:
	PlusReceiver(igtlio::LogicPointer logic);

	void send_RequestChannelIDs();

};

} //namespace cxtest

#endif // PLUSRECEIVER_H
