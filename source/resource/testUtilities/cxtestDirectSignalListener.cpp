/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxtestDirectSignalListener.h"

namespace cxtest
{

DirectSignalListener::DirectSignalListener(QObject* object, const char* signal) :
	mNumberOfSignalsReceived(0)
{
	connect(object, signal, this, SLOT(receive()));
}

DirectSignalListener::~DirectSignalListener()
{
}

void DirectSignalListener::receive()
{
	mNumberOfSignalsReceived +=1;
}

bool DirectSignalListener::isReceived() const
{
	return  mNumberOfSignalsReceived > 0;
}

unsigned int DirectSignalListener::getNumberOfRecievedSignals() const
{
	return mNumberOfSignalsReceived;
}

} // namespace cxtest

