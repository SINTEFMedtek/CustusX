#ifndef _QT_2_FUNC_5_H_
#define _QT_2_FUNC_5_H_

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
class Qt2FuncSlot5
{
public:
	typedef boost::function<SIGNATURE> FuncType;
	typedef typename boost::function_traits<SIGNATURE>::arg1_type ParmType1;
	typedef typename boost::function_traits<SIGNATURE>::arg2_type ParmType2;
	typedef typename boost::function_traits<SIGNATURE>::arg3_type ParmType3;
	typedef typename boost::function_traits<SIGNATURE>::arg4_type ParmType4;
	typedef typename boost::function_traits<SIGNATURE>::arg5_type ParmType5;
	
	Qt2FuncSlot5(const FuncType& func) :
		func_(func)
	{
	}

	void call(QObject* sender, void **arguments)
	{
		ParmType1* a1 = reinterpret_cast<ParmType1*>(arguments[1]);
		ParmType2* a2 = reinterpret_cast<ParmType2*>(arguments[2]);
		ParmType3* a3 = reinterpret_cast<ParmType3*>(arguments[3]);
		ParmType4* a4 = reinterpret_cast<ParmType4*>(arguments[4]);
		ParmType5* a5 = reinterpret_cast<ParmType5*>(arguments[5]);

		if ( func_ )
			func_(*a1,*a2, *a3, *a4, *a5);
	}

private:
	FuncType func_;
};
/**
* \endcond
*/

template<typename SIGNATURE>
class Qt2Func5 : public QObject, public QtConnDefault
{
public:
	typedef boost::function<SIGNATURE> FuncType;
	typedef typename boost::function_traits<SIGNATURE>::arg1_type ParmType;

	Qt2Func5(QObject* qobject, int signalIdx, const FuncType& func,
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
		slot_ = new Qt2FuncSlot5<SIGNATURE>(func);

		if ( initiallyConnected )
			connect();
	}

	~Qt2Func5()
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
	Qt2FuncSlot5<SIGNATURE>* slot_;
};

}

#endif
