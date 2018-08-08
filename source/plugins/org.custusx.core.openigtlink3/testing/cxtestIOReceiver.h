#ifndef IORECEIVER_H
#define IORECEIVER_H

#include "cxtest_org_custusx_core_openigtlink3_export.h"

#include "catch.hpp"

#include "cxtestReceiver.h"

namespace cxtest
{

class CXTEST_ORG_CUSTUSX_CORE_OPENIGTLINK3_EXPORT IOReceiver : public Receiver
{
public:
	IOReceiver(igtlioLogicPointer logic);

	void sendCommand_Get_Parameter_Depth();

private:
	void sendCommand_Get_Parameter(std::string parameter_name);

};

} //namespace cxtest

#endif // IORECEIVER_H
