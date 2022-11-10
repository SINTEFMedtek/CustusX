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
#include <string>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include "cxProfile.h"
#include "cxXmlOptionItem.h"
#include "cxDoubleWidgets.h"
#include "cxVisServices.h"
#include "cxHelperWidgets.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxLogger.h"
#include "cxViewService.h"
#include "cxActiveToolWidget.h"
#include "cxTrackingService.h"

namespace cx
{

ShapeSensorWidget::ShapeSensorWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ShapeSensorWidget", "Shape Sensor"),
	mServices(services),
	mVerticalLayout(new QVBoxLayout(this)),
	mSocketConnection(new SocketConnection(this))
{
	SocketConnection::ConnectionInfo info = mSocketConnection->getConnectionInfo();
	info.port = 5001;
	mSocketConnection->setConnectionInfo(info);


	XmlOptionFile mOptions;
	mOptions = profile()->getXmlSettings().descend("shape");
	QDomElement element = mOptions.getElement("shape");

	mIpAddress = this->getIPAddress(element);
	mIpPort = this->getIPPort(element);

	this->setWhatsThis(this->defaultWhatsThis());

	//Is tool selector needed?
	ActiveToolWidget* activeToolWidget = new ActiveToolWidget(services->tracking(), this);
	mSelector = activeToolWidget->getSelector();
	connect(mSelector.get(), &StringPropertyBase::changed, this, &ShapeSensorWidget::activeToolChangedSlot);

	//Connect to tool
	mTool = mServices->tracking()->getActiveTool();
	connect(mTool.get(), &Tool::toolTransformAndTimestamp, this, &ShapeSensorWidget::receiveTransforms);

	mConnectButton = new QPushButton("Connect", this);
	mShowShapeButton = new QPushButton("Show shape", this);
	mTestShapeButton = new QPushButton("Create test shape", this);

	connect(mConnectButton, &QPushButton::clicked, this, &ShapeSensorWidget::connectClickedSlot);
	connect(mShowShapeButton, &QPushButton::clicked, this, &ShapeSensorWidget::showClickedSlot);
	connect(mTestShapeButton, &QPushButton::clicked, this, &ShapeSensorWidget::testShapeClickedSlot);
	connect(mSocketConnection.get(), &SocketConnection::stateChanged, this, &ShapeSensorWidget::connectStateChangedSlot);
	connect(mSocketConnection.get(), &SocketConnection::dataAvailable, this, &ShapeSensorWidget::dataAvailableSlot);

	QWidget* addressWidget = sscCreateDataWidget(this, mIpAddress);
	QWidget* portWidget = sscCreateDataWidget(this, mIpPort);

	mVerticalLayout->addWidget(addressWidget);
	mVerticalLayout->addWidget(portWidget);

	mVerticalLayout->addWidget(mConnectButton);
	mVerticalLayout->addWidget(mShowShapeButton);
	mVerticalLayout->addWidget(activeToolWidget);
	mVerticalLayout->addWidget(this->createHorizontalLine());
	mVerticalLayout->addStretch(1);
	mVerticalLayout->addWidget(mTestShapeButton);
	mVerticalLayout->addStretch();

	this->activeToolChangedSlot();
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
		mSocketConnection->requestConnect();
	else
		mSocketConnection->disconnect();

//	this->adjustSize();
}


void ShapeSensorWidget::showClickedSlot()
{
	vtkPolyDataPtr polydata = mReadFbgsMessage.getPolyData();
	CX_LOG_DEBUG() << "showClickedSlot polydata points: " << polydata->GetPoints()->GetNumberOfPoints();
	if(mShowShape)
	{
		mServices->view()->get3DView()->getRenderer()->RemoveActor(mReadFbgsMessage.getActor());
		mShowShapeButton->setText("Show shape");
	}
	else
	{
		mServices->view()->get3DView()->getRenderer()->AddActor(mReadFbgsMessage.getActor());
		mShowShapeButton->setText("Hide shape");
	}
	mShowShape = !mShowShape;
}

void ShapeSensorWidget::testShapeClickedSlot()
{
	std::vector<double> *xAxis = mReadFbgsMessage.getAxisPosVector(ReadFbgsMessage::axisX);
	std::vector<double> *yAxis = mReadFbgsMessage.getAxisPosVector(ReadFbgsMessage::axisY);
	std::vector<double> *zAxis = mReadFbgsMessage.getAxisPosVector(ReadFbgsMessage::axisZ);
	xAxis->push_back(0);
	yAxis->push_back(0);
	zAxis->push_back(0);
	for(int i = 1; i < 100; ++i)
	{
		xAxis->push_back(i+std::rand()/((RAND_MAX + 1u)/3));
		yAxis->push_back(i+std::rand()/((RAND_MAX + 1u)/3));
		zAxis->push_back(i+std::rand()/((RAND_MAX + 1u)/3));
	}
	mReadFbgsMessage.createPolyData();
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
	mReadFbgsMessage.readBuffer(buffer);
	vtkPolyDataPtr polyData = mReadFbgsMessage.getPolyData();
}

void ShapeSensorWidget::activeToolChangedSlot()
{
	disconnect(mTool.get(), &Tool::toolTransformAndTimestamp, this, &ShapeSensorWidget::receiveTransforms);
	//mTool = mServices->tracking()->getTool(mSelector->getValue());
	mTool = mServices->tracking()->getActiveTool();
	connect(mTool.get(), &Tool::toolTransformAndTimestamp, this, &ShapeSensorWidget::receiveTransforms);
}

void ShapeSensorWidget::receiveTransforms(Transform3D prMt, double timestamp)
{
	mReadFbgsMessage.set_prMt(prMt);
}
} /* namespace cx */
