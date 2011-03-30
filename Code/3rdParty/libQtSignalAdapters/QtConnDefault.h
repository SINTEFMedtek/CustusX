#ifndef _QT_CONN_DEFAULT_H_
#define _QT_CONN_DEFAULT_H_

#include <string>

#include <boost/lexical_cast.hpp>

#include <libQtSignalAdapters/QtConn.h>

namespace QtSignalAdapters
{

class QtConnDefault : public QtConn
{
public:
	QtConnDefault(QObject* qobject, int signalIdx) :
		qobject_(qobject),
		signalIdx_(signalIdx),
		connected_(false)
	{
		id_ = boost::lexical_cast<std::string>(this);
		name_ = id_;
	}

	virtual void setName(const std::string& name)
	{
		name_ = name;
	}

	virtual std::string name() const
	{
		return name_;
	}

	virtual std::string id() const
	{
		return id_;
	}

	virtual bool isConnected() const
	{
		return connected_;
	}

protected:
	QObject* qobject_;
	int signalIdx_;

	std::string name_;
	std::string id_;

	bool connected_;
	int slotIdx_;

};

}

#endif
