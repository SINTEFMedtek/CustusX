/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxShapeSensorWidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStringRef>
#include <string>
#include "cxProfile.h"
#include "cxXmlOptionItem.h"
//#include "cxStringPropertyBase.h"
//#include "cxSocketConnection.h"

//#include "cxStringPropertyBase.h"
//#include "cxDoublePropertyBase.h"
#include "cxDoubleWidgets.h"
#include "cxVisServices.h"
#include "cxHelperWidgets.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
//#include "cxDataSelectWidget.h"
#include "cxLogger.h"

namespace cx
{

ShapeSensorWidget::ShapeSensorWidget(VisServicesPtr services, QWidget* parent) :
	QWidget(parent),
	mVerticalLayout(new QVBoxLayout(this)),
	mSocketConnection(new SocketConnection(this))
{
	this->setObjectName("ShapeSensorWidget");
	this->setWindowTitle("Shape Sensor");

	SocketConnection::ConnectionInfo info = mSocketConnection->getConnectionInfo();
	info.port = 5001;
	mSocketConnection->setConnectionInfo(info);


	XmlOptionFile mOptions;
	mOptions = profile()->getXmlSettings().descend("shape");
	QDomElement element = mOptions.getElement("shape");

	mIpAddress = this->getIPAddress(element);
	mIpPort = this->getIPPort(element);

	this->setWhatsThis(this->defaultWhatsThis());

	//mVerticalLayout->addWidget(new QLabel("Hello Plugin!"));


	mConnectButton = new QPushButton("Connect", this);

	connect(mConnectButton, &QPushButton::clicked, this, &ShapeSensorWidget::connectClickedSlot);
	connect(mSocketConnection.get(), &SocketConnection::stateChanged, this, &ShapeSensorWidget::connectStateChangedSlot);
	connect(mSocketConnection.get(), &SocketConnection::dataAvailable, this, &ShapeSensorWidget::dataAvailableSlot);

	QWidget* addressWidget = sscCreateDataWidget(this, mIpAddress);
	//QWidget* portWidget = new SliderGroupWidget(this, mIpPort);
	QWidget* portWidget = sscCreateDataWidget(this, mIpPort);
	mVerticalLayout->addWidget(addressWidget);
	mVerticalLayout->addWidget(portWidget);

	mVerticalLayout->addWidget(mConnectButton);
	mVerticalLayout->addStretch();
}

ShapeSensorWidget::~ShapeSensorWidget()
{
}

QString ShapeSensorWidget::defaultWhatsThis() const
{
	return "<html>"
		   "<h3>Shape Sensor plugin.</h3>"
		   "<p>Widget for connecting to Fiber Bragg Shape Sensor/p>"
		   "</html>";
}

StringPropertyBasePtr ShapeSensorWidget::getIPAddress(QDomElement root)
{
	StringPropertyPtr retval;
	QString defaultValue = "127.0.0.1";
	retval = StringProperty::initialize("ip_address", "Address", "TCP/IP Address",
												defaultValue, root);
	retval->setGroup("Connection");
	return retval;
}

DoublePropertyBasePtr ShapeSensorWidget::getIPPort(QDomElement root)
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("ip_port", "Port", "TCP/IP Port (default 5001)",
												5001, DoubleRange(1024, 49151, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
	retval->setAdvanced(true);
	retval->setGroup("Connection");
	return retval;
}

void ShapeSensorWidget::connectStateChangedSlot(CX_SOCKETCONNECTION_STATE status)
{
	CX_LOG_DEBUG() << "connectStateChangedSlot: " << status;
	if(status == scsCONNECTED)
	{
		mConnectButton->setText("Disconnect");
	}
	else
	{
		mConnectButton->setText("Connect");
	}
}

void ShapeSensorWidget::connectClickedSlot()
{
	if(mSocketConnection->getState() != scsCONNECTED)
	{
		mSocketConnection->requestConnect();
		//mConnectButton->setText("Disconnect");
	}
	else
	{
		mSocketConnection->disconnect();
		//mConnectButton->setText("Connect");
	}

	this->adjustSize();
}

void ShapeSensorWidget::dataAvailableSlot()
{
	char charSize[4];
	bool ok = mSocketConnection->socketReceive(&charSize, 4);
	if(!ok)
	{
		CX_LOG_WARNING() << "Cannot read 4 characters from TCP socket";
		return;
	}
	int dataLength = std::stoi(charSize);
	CX_LOG_DEBUG() << "dataLength: " << dataLength;

	char *charBuffer = (char*)malloc(dataLength);
	ok = mSocketConnection->socketReceive(charBuffer, dataLength);
	if(!ok)
	{
		CX_LOG_WARNING() << "Cannot read " << dataLength << " characters from TCP socket";
		return;
	}
	QString buffer(charBuffer);//TODO: Need terminator?
	free(charBuffer);
	CX_LOG_DEBUG() << "Read buffer: " << buffer;
	this->readBuffer(buffer);
}

void ShapeSensorWidget::readBuffer(QString buffer)
{
	QString xString("Shape x [cm]");
	QString yString("Shape y [cm]");
	QString zString("Shape z [cm]");
	QStringList axisStrings;
	axisStrings<< xString << yString << zString;
	for(int i = 0; i < axisStrings.size(); ++i)
	{
		if(!this->readShape(axisStrings[i], buffer))
		{
			CX_LOG_WARNING() << "Error reading " << axisStrings[i] << " values from TCP socket";
			return;
		}
	}
}

bool ShapeSensorWidget::readShape(QString axisString, QString buffer)
{
	int bufferPos = 0;
	bufferPos = buffer.indexOf(axisString, bufferPos, Qt::CaseInsensitive);
	if(bufferPos == -1)
	{
		CX_LOG_WARNING() << "Cannot read " << axisString << " from TCP socket";
		return false;
	}
	bufferPos += axisString.size();
	return this->readPositions(buffer, bufferPos);
}

bool ShapeSensorWidget::readPositions(QString buffer, int bufferPos)
{
	bool ok;
	QStringRef numberString(&buffer, bufferPos, sizeof(int));
	int numValues = numberString.toInt(&ok);
	if(!ok)
		return false;
	for(int i = 0; i < numValues; ++i)
	{
		//Use sizeof(float) instead?
		numberString = QStringRef(&buffer, bufferPos, sizeof(double));
		double value = numberString.toDouble(&ok);
		//TODO: use values
		//Create a vtkPolyData, see cxToolTracer?
		if(!ok)
			return false;
		bufferPos += sizeof(double);
	}
	return true;
}

} /* namespace cx */
