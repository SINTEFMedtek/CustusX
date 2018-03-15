/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTDIRECTSIGNALLISTENER_H
#define CXTESTDIRECTSIGNALLISTENER_H

#include "cxtestutilities_export.h"
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
class CXTESTUTILITIES_EXPORT DirectSignalListener : public QObject
{
	Q_OBJECT
public:
	DirectSignalListener(QObject* object, const char* signal);
	~DirectSignalListener();

	bool isReceived() const;
	unsigned int getNumberOfRecievedSignals() const;

private slots:
	void receive();

private:
	unsigned int mNumberOfSignalsReceived;
};

} /* namespace cxtest */

#endif // CXTESTDIRECTSIGNALLISTENER_H
