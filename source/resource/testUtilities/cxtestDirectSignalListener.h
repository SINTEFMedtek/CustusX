// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXTESTDIRECTSIGNALLISTENER_H
#define CXTESTDIRECTSIGNALLISTENER_H

#include <QObject>

namespace cxtest
{

/**
 * \brief Listen for signals using Qt::DirectConnection.
 *
 * \author Ole Vegard Solberg, SINTEF
 * \author Christian Askeland, SINTEF
 * \date Sep 5, 2013
 */
class DirectSignalListener : public QObject
{
	Q_OBJECT
public:
	DirectSignalListener(QObject* object, const char* signal);
	~DirectSignalListener(){}
	bool isReceived();
private slots:
	void slot();
private:
	bool mReceived;
};

} /* namespace cxtest */

#endif // CXTESTDIRECTSIGNALLISTENER_H
