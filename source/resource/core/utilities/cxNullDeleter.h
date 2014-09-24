#ifndef CXNULLDELETER_H
#define CXNULLDELETER_H

#include "cxResourceExport.h"

namespace cx
{
struct cxResource_EXPORT null_deleter
{
	void operator()(void const *) const {}
};
}
#endif // CXNULLDELETER_H
