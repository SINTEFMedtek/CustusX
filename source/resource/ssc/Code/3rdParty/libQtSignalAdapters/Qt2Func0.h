#ifndef _QT_2_FUNC_0_H_
#define _QT_2_FUNC_0_H_

#include <iostream>

#include <boost/function.hpp>
#include <boost/type_traits.hpp>

#include <QObject>

#include <libQtSignalAdapters/QtConnDefault.h>

//using namespace boost;

namespace QtSignalAdapters
{

/**
* \mainpage
*
* This set of classes serve to connect QObject signals to C++ functions,
* either standalone or member functions. Let's say we have a custom widget,
* MyWidget, with a signal called "invokeWithInt(int)", and let's also
* say we have a plain old C++ class, MyObject, with a member function
* called "receiveInt". So, you can do:
*
* \code
*
* MyObject* mo = new MyObject;
* MyWidget* widget = new MyWidget();
*
* // Qt setup stuff (must have Qt running for this to work).
*
* connect1<void(int)>(widget, SIGNAL(invokeWithInt(int)),
*    boost::bind(&MyObject::receiveInt, mo, _1));
*
* \endcode
*
* From now on, when "invokeWithInt" is emitted:
*
* \code
* emit invokeWithInt(42);
* \endcode
*
* the receiveInt member function will be invoked on the MyObject instance.
* In this example, widget is the parent of the adapter.  The adapter will
* automatically be deleted when widget is deleted.
*
* As created above, the connection exists on the heap and will not be
* deleted until the QObject (widget) is deleted; the widget is the parent
* of the adapter.  If you want to be able to disconnect and re-connect
* the signal adapter, you would assign the return value of connect1 to a
* variable:
*
* \code
*
* QtConn* conn_var = connect1<void(int)>(widget, SIGNAL(invokeWithInt(int)),
*    boost::bind(&MyObject::receiveInt, mo, _1));
*
* \endcode
*
*
* Then you can do:
*
* \code
*
* conn_var->connect();
* conn_var->disconnect();
* if ( !conn_var->isConnected() )
*    conn_var->connect();
*
* \endcode
*
* There are connect functions for up to 5 parameters: connect0, connect1,
* connect2, connect3, connect4, and connect5.
*
* So, if MyObject had a class member 'void method(int i, double d)',
* and widget had a signal 'void invokeTwo(int, double)', you would
* do:
*
* \code
*
* connect2<void(int,double)>(qobject, SIGNAL(invokeTwo(int, double)),
*     boost::bind(&MyObject::method, mo, _1, _2));
*
* \endcode
*
*
* The latest version of libQtSignalAdapters is found in
* http://redwood:3690/svn/repos/common/lib/libQtSignalAdapters/trunk
*
* This Qt Quarterly article was very helpful:
* http://doc.trolltech.com/qq/qq16-dynamicqobject.html
*/

/**
* \cond
*/
template<typename SIGNATURE>
class Qt2FuncSlot0
{
public:
	typedef boost::function<SIGNATURE> FuncType;
	
	Qt2FuncSlot0(const FuncType& func) :
		func_(func)
	{
	}

	void call(QObject* sender, void **arguments)
	{
		if ( func_ )
			func_();
	}

	~Qt2FuncSlot0()
	{
	}

private:
	FuncType func_;
};
/**
* \endcond
*/

template<typename SIGNATURE>
class Qt2Func0 : public QObject, public QtConnDefault
{
public:
	typedef boost::function<SIGNATURE> FuncType;

	Qt2Func0(QObject* qobject, int signalIdx, const FuncType& func,
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
		slot_ = new Qt2FuncSlot0<SIGNATURE>(func);

		if ( initiallyConnected )
			connect();
	}

	~Qt2Func0()
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
	Qt2FuncSlot0<SIGNATURE>* slot_;
};

}

#endif
