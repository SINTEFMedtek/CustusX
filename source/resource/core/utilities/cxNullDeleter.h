#ifndef CXNULLDELETER_H
#define CXNULLDELETER_H

namespace cx
{
struct null_deleter
{
	void operator()(void const *) const {}
};
}
#endif // CXNULLDELETER_H
