/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxUr5Connection.h"
#include <QTcpSocket>
#include <iostream>

#include "cxLogger.h"
#include "cxUtilHelpers.h"


namespace cx
{

Ur5Connection::Ur5Connection(QString address, int port) :
	mAddress(address),
	mPort(port)
{
}

void Ur5Connection::connectToRobot()
{
	// Establish Connection
	mSocket.reset(new QTcpSocket());
	connect(mSocket.get(), &QAbstractSocket::readyRead, this, &Ur5Connection::readyReadSlot, Qt::DirectConnection);

	connect(mSocket.get(), &QAbstractSocket::hostFound, this, &Ur5Connection::hostFoundSlot, Qt::DirectConnection);
	connect(mSocket.get(), &QAbstractSocket::connected, this, &Ur5Connection::connectedSlot, Qt::DirectConnection);
	connect(mSocket.get(), &QAbstractSocket::disconnected, this, &Ur5Connection::disconnectedSlot, Qt::DirectConnection);
//	connect(mSocket.get(), &QAbstractSocket::error, this, &Ur5Connection::errorSlot, Qt::DirectConnection);
	connect(mSocket.get(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)),
					Qt::DirectConnection);
	if (!this->tryConnectToHost())
	{
		reportError("Ur5Connection: Failed to connect");
		mSocket.reset();
		return;
	}
}

bool Ur5Connection::multipleTryConnectToHost()
{
	// hold here until all attempts are finished
	int numberOfConnectionAttempts = 5;
	int baseSleep = 300;
	for (int i=0; i<numberOfConnectionAttempts; ++i)
	{
		if (i>0)
			report(QString("[%2] Attempt %1 to connect to host").arg(i+1).arg(this->hostDescription()));
		if (this->tryConnectToHost())
			return true;
		sleep_ms(baseSleep*(i+1));
	}
	reportError(QString("[%1] Timeout connecting to host").arg(this->hostDescription()));
	return false;
}

bool Ur5Connection::tryConnectToHost()
{
	mSocket->connectToHost(mAddress, mPort);

	int timeout = 5000;
	if (!mSocket->waitForConnected(timeout))
	{
		mSocket->disconnectFromHost();
		return false;
	}
	return true;
}

void Ur5Connection::disconnectFromRobot()
{
	if (mSocket)
	{
		mSocket->disconnectFromHost();
		mSocket.reset();
	}
}

bool Ur5Connection::isConnectedToRobot()
{
	return (mSocket && mSocket->isValid());
}

QString Ur5Connection::hostDescription() const
{
	return mAddress + ":" + qstring_cast(mPort);
}

void Ur5Connection::hostFoundSlot()
{
	report(QString("[%1] Found host").arg(this->hostDescription()));
}

//New data available for reading
void Ur5Connection::readyReadSlot()
{
	// How many bytes?
	qint64 maxAvailableBytes = mSocket->bytesAvailable();

	char* inMessage;
	mSocket->read(inMessage, maxAvailableBytes);

	std::cout << "Incoming message: " << QString(inMessage).toStdString() << std::endl;

	//TODO: Do something with received message
}

void Ur5Connection::connectedSlot()
{
	reportSuccess(QString("[%1] Connected to host").arg(this->hostDescription()));
}

void Ur5Connection::disconnectedSlot()
{
	report(QString("[%1] Disconnected from host").arg(this->hostDescription()));
}

void Ur5Connection::errorSlot(QAbstractSocket::SocketError socketError)
{
	report(QString("[%1] Socket error [code=%2]: %3")
		   .arg(this->hostDescription())
		   .arg(QString::number(socketError))
		   .arg(mSocket->errorString()));
}
} // cx
