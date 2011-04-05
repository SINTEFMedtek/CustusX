#ifndef _QT_CONN_H_
#define _QT_CONN_H_

#include <string>
#include <set>

namespace QtSignalAdapters
{

/**
* Abstract interface for Qt signal/slot adapters. Each adapter
* has a unique id, and can be assigned a name via the setName method.  By
* default, the name is the same as the id. Via the connect and
* disconnect methods, an adapter can start and stop delivery of signals.
*/
class QtConn
{
public:
	virtual ~QtConn() {}

	/**
	* By default, name is the same as the id.  This allows the name to
	* be changed to something more meaningful.
	*/
	virtual void setName(const std::string& name) = 0;

	virtual std::string name() const = 0;

	virtual std::string id() const = 0;

	/**
	* Establishes the connection if it isn't already established.
	*/
	virtual void connect() = 0;

	virtual void disconnect() = 0;

	virtual bool isConnected() const = 0;
};

}

#endif
