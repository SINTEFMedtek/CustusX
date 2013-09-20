#ifndef _QT_CONNECTION_FACTORIES_H_
#define _QT_CONNECTION_FACTORIES_H_

#include <iostream>

#include <libQtSignalAdapters/Qt2Func.h>

//using namespace std;

namespace QtSignalAdapters
{

/**
* Checks that signal is actually in the proper form, i.e.  SIGNAL(clicked()),
* that the signal actually exists on qobject, and that the number of
* parameters of signal match with func.  Returns the signal id to be
* used to invoke the func.
*/
template <typename SIGNATURE>
int checkConnection(QObject* qobject, const char* signal,
	const boost::function<SIGNATURE>& func)
{
	QByteArray norm = QMetaObject::normalizedSignature(signal+1);

	int code = (signal[0] - '0') & 0x03;
	if ( code != QSIGNAL_CODE )
	{
		cerr << "Error connecting " << norm.constData() << "; not a signal" <<
			endl;
		return -1;
	}

	int signalId;
	if ( (signalId =qobject->metaObject()->indexOfSignal(norm.constData())) <
		0 )
	{
		cerr << "Error connecting " << norm.constData() << "; signal does not exist on object " << qobject->objectName().toStdString() << endl;
		return -1;
	}

	int arity = boost::function_traits<SIGNATURE>::arity;
	QMetaMethod qmm = qobject->metaObject()->method(signalId);
	QList<QByteArray> pn = qmm.parameterNames();

	if ( arity != pn.size() )
	{
		cerr << "Error connecting " << norm.constData() << "; # of arguments do not match" << endl;
		return -1;
	}

	return signalId;
}


/**
* Creates a Qt signal to boost::function adapter object that causes
* signals with 0 parameters to invoke boost::function objects with the
* signature void(void).
*/
template <typename SIGNATURE>
QtConn* connect0(QObject* qobject, const char* signal,
	const boost::function<SIGNATURE>& func)
{
	int signalId = checkConnection(qobject, signal,func);

	if( signalId < 0 )
		return NULL;

	return new Qt2Func0<SIGNATURE>(qobject, signalId, func);
}

/**
* Creates a Qt signal to boost::function adapter object that causes
* signals with 1 parameter to invoke boost::function objects with the
* signature void(T), where T is the type of the argument.
*/
template <typename SIGNATURE>
QtConn* connect1(QObject* qobject, const char* signal,
	const boost::function<SIGNATURE>& func)
{
	int signalId = checkConnection(qobject, signal,func);

	if( signalId < 0 )
		return NULL;

	return new Qt2Func1<SIGNATURE>(qobject, signalId, func);
}


/**
* Creates a Qt signal to boost::function adapter object that causes
* signals with 2 parameters to invoke boost::function objects with the
* signature void(T1, T2), where T1 and T2 are the types of the arguments.
*/
template <typename SIGNATURE>
QtConn* connect2(QObject* qobject, const char* signal,
	const boost::function<SIGNATURE>& func)
{
	int signalId = checkConnection(qobject, signal, func);

	if ( signalId < 0 )
		return NULL;

	return new Qt2Func2<SIGNATURE>(qobject, signalId, func);
}


/**
* Creates a Qt signal to boost::function adapter object that causes
* signals with 3 parameters to invoke boost::function objects with the
* signature void(T1, T2, T3), where T1, T2, and T3 are the types of the
* arguments.
*/
template <typename SIGNATURE>
QtConn* connect3(QObject* qobject, const char* signal,
	const boost::function<SIGNATURE>& func)
{
	int signalId = checkConnection(qobject, signal, func);

	if ( signalId < 0 )
		return NULL;

	return new Qt2Func3<SIGNATURE>(qobject, signalId, func);
}


/**
* Creates a Qt signal to boost::function adapter object that causes
* signals with 4 parameters to invoke boost::function objects with the
* signature void(T1, T2, T3, T4), where T1, T2, T3, and T4 are the types of the
* arguments.
*/
template <typename SIGNATURE>
QtConn* connect4(QObject* qobject, const char* signal,
	const boost::function<SIGNATURE>& func)
{
	int signalId = checkConnection(qobject, signal, func);

	if ( signalId < 0 )
		return NULL;

	return new Qt2Func4<SIGNATURE>(qobject, signalId, func);
}

/**
* Creates a Qt signal to boost::function adapter object that causes
* signals with 5 parameters to invoke boost::function objects with the
* signature void(T1, T2, T3, T4, T5), where T1, T2, T3, T4, and T5
* are the types of the arguments.
*/
template <typename SIGNATURE>
QtConn* connect5(QObject* qobject, const char* signal,
	const boost::function<SIGNATURE>& func)
{
	int signalId = checkConnection(qobject, signal, func);

	if ( signalId < 0 )
		return NULL;

	return new Qt2Func5<SIGNATURE>(qobject, signalId, func);
}

}

#endif
