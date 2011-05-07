#ifndef _QT_2_FUNC_2_H_
#define _QT_2_FUNC_2_H_

#include <iostream>

#include <boost/function.hpp>
#include <boost/type_traits.hpp>

#include <QObject>

#include <libQtSignalAdapters/QtConnDefault.h>

//using namespace boost;

namespace QtSignalAdapters
{

/**
* \cond
*/
template<typename SIGNATURE>
class Qt2FuncSlot2
{
public:
	typedef boost::function<SIGNATURE> FuncType;
	typedef typename boost::function_traits<SIGNATURE>::arg1_type ParmType1;
	typedef typename boost::function_traits<SIGNATURE>::arg2_type ParmType2;
	
	Qt2FuncSlot2(const FuncType& func) :
		func_(func)
	{
	}

	void call(QObject* sender, void **arguments)
	{
		ParmType1* a1 = reinterpret_cast<ParmType1*>(arguments[1]);
		ParmType2* a2 = reinterpret_cast<ParmType2*>(arguments[2]);
		if ( func_ )
			func_(*a1,*a2);
	}

private:
	FuncType func_;
};
/**
* \endcond
*/

template<typename SIGNATURE>
class Qt2Func2 : public QObject, public QtConnDefault
{
public:
	typedef boost::function<SIGNATURE> FuncType;
	typedef typename boost::function_traits<SIGNATURE>::arg1_type ParmType;

	Qt2Func2(QObject* qobject, int signalIdx, const FuncType& func,
			bool initiallyConnected=true) :
		QObject(qobject),
		QtConnDefault(qobject, signalIdx),
		func_(func)
	{
		//
		// Get the next usable slot ID on this...
		//
		slotIdx_ = metaObject()->methodCount();

		//
		// Create a slot to handle invoking the boost::function object.
		//
		slot_ = new Qt2FuncSlot2<SIGNATURE>(func);

		if ( initiallyConnected )
			connect();
	}

	~Qt2Func2()
	{
		delete slot_;
	}

	int qt_metacall(QMetaObject::Call c, int id, void **arguments)
	{
		id = QObject::qt_metacall(c, id, arguments);
		if ( id < 0 || c != QMetaObject::InvokeMetaMethod )
			return id;
		
		slot_->call(sender(), arguments);
		return -1;
	}

	void connect()
	{
		connect_();
	}

	void disconnect()
	{
		disconnect_();
	}

private:
	void connect_()
	{
		connected_ =
			QMetaObject::connect(qobject_, signalIdx_, this, slotIdx_);
	}

	void disconnect_()
	{
		connected_ =
			!QMetaObject::disconnect(qobject_, signalIdx_, this, slotIdx_);
	}


	FuncType func_;
	Qt2FuncSlot2<SIGNATURE>* slot_;
};

}

#endif
