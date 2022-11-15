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
	mShapePointLock = this->getShapePointLock(element);

	this->setWhatsThis(this->defaultWhatsThis());

	//Is tool selector needed?
	ActiveToolWidget* activeToolWidget = new ActiveToolWidget(services->tracking(), this);
	mSelector = activeToolWidget->getSelector();
	connect(mSelector.get(), &StringPropertyBase::changed, this, &ShapeSensorWidget::activeToolChangedSlot);
	//connect(services->tracking().get(), &TrackingService::activeToolChanged,this, &ShapeSensorWidget::activeToolChangedSlot);

	//Connect to tool
	mTool = mServices->tracking()->getActiveTool();
	connect(mTool.get(), &Tool::toolTransformAndTimestamp, this, &ShapeSensorWidget::receiveTransforms);

	mConnectButton = new QPushButton("Connect", this);
	mShowShapeButton = new QPushButton("Hide shape", this);
	mTestShapeButton = new QPushButton("Create test shape", this);

	connect(mConnectButton, &QPushButton::clicked, this, &ShapeSensorWidget::connectClickedSlot);
	connect(mShowShapeButton, &QPushButton::clicked, this, &ShapeSensorWidget::showClickedSlot);
	connect(mTestShapeButton, &QPushButton::clicked, this, &ShapeSensorWidget::testShapeClickedSlot);
	connect(mSocketConnection.get(), &SocketConnection::stateChanged, this, &ShapeSensorWidget::connectStateChangedSlot);
	connect(mSocketConnection.get(), &SocketConnection::dataAvailable, this, &ShapeSensorWidget::dataAvailableSlot);
	connect(mShapePointLock.get(), &Property::changed, this, &ShapeSensorWidget::shapePointLockChangedSlot);

	QWidget* addressWidget = sscCreateDataWidget(this, mIpAddress);
	QWidget* portWidget = sscCreateDataWidget(this, mIpPort);
	QWidget* shapePointLockWidget = sscCreateDataWidget(this, mShapePointLock);

	mVerticalLayout->addWidget(addressWidget);
	mVerticalLayout->addWidget(portWidget);

	mVerticalLayout->addWidget(mConnectButton);
	mVerticalLayout->addWidget(this->createHorizontalLine());
	mVerticalLayout->addWidget(mShowShapeButton);
	mVerticalLayout->addWidget(shapePointLockWidget);
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

DoublePropertyPtr ShapeSensorWidget::getShapePointLock(QDomElement root)
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("shape_point_lock", "Lock shape point number to tool",
										"Lock a specific point in the shape to the position of the active tool ",
										0, DoubleRange(0, 1000, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
	retval->setAdvanced(true);
	retval->setGroup("Connection");
	return retval;
}

void ShapeSensorWidget::shapePointLockChangedSlot()
{
	mReadFbgsMessage.setShapePointLock(mShapePointLock->getValue());
}

void ShapeSensorWidget::updateRange()
{
	int rangeMax = mReadFbgsMessage.getRangeMax();
	if(rangeMax > 0)
	{
		DoubleRange range = mShapePointLock->getValueRange();
		if (range.max() != rangeMax)
		{
			range.mMax = rangeMax;
			mShapePointLock->setValueRange(range);
		}
		if (mShapePointLock->getValue() > rangeMax)
			mShapePointLock->setValue(rangeMax);
	}
}

void ShapeSensorWidget::connectStateChangedSlot(CX_SOCKETCONNECTION_STATE status)
{
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

	SocketConnection::ConnectionInfo info = mSocketConnection->getConnectionInfo();
	info.port = mIpPort->getValue();
	info.host = mIpAddress->getValue();
	mSocketConnection->setConnectionInfo(info);

	if(mSocketConnection->getState() != scsCONNECTED)
		mSocketConnection->requestConnect();
	else
		mSocketConnection->requestDisconnect();

	this->showShape();
//	this->adjustSize();
}


void ShapeSensorWidget::showClickedSlot()
{
	vtkPolyDataPtr polydata = mReadFbgsMessage.getPolyData();
//	CX_LOG_DEBUG() << "showClickedSlot polydata points: " << polydata->GetPoints()->GetNumberOfPoints();
	if(mShowShape)
		mShowShapeButton->setText("Show shape");
	else
		mShowShapeButton->setText("Hide shape");
	mShowShape = !mShowShape;
	this->showShape();
}

void ShapeSensorWidget::showShape()
{
	if(mShowShape)
		mServices->view()->get3DView()->getRenderer()->AddActor(mReadFbgsMessage.getActor());
	else
		mServices->view()->get3DView()->getRenderer()->RemoveActor(mReadFbgsMessage.getActor());
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
	bool ok = true;
	if(mDataLenght == 0)//Use previously read value
	{
		unsigned char charSize[4];
		ok = mSocketConnection->socketReceive(&charSize, 4);
		if(!ok)
		{
			CX_LOG_WARNING() << "Cannot read 4 characters from TCP socket";
			return;
		}
		//CX_LOG_DEBUG() << "convert 4 bytes to int: " << int(charSize[0]) << " " << int(charSize[1]) << " " << int(charSize[2]) << " " << int(charSize[3]);
		mDataLenght = int( ( (unsigned char)(charSize[0]) << 24 )
				| ( (unsigned char)(charSize[1]) << 16 )
				| ( (unsigned char)(charSize[2]) << 8 )
				| ( (unsigned char)(charSize[3]) ) );
		//CX_LOG_DEBUG() << "mDataLenght: " << mDataLenght;
	}
//	else
//		CX_LOG_DEBUG() << "Another try to read " << mDataLenght << " data";

	char *charBuffer = (char*)malloc(mDataLenght);
	ok = mSocketConnection->socketReceive(charBuffer, mDataLenght);
	if(!ok)
	{
		//CX_LOG_DEBUG() << "Cannot read " << mDataLenght << " characters from TCP socket. Waiting...";
		return;
	}
	else
		mDataLenght = 0;
	QString buffer(charBuffer);
	free(charBuffer);
	mReadFbgsMessage.readBuffer(buffer);

	this->updateRange();
}

void ShapeSensorWidget::activeToolChangedSlot()
{
//	disconnect(mTool.get(), &Tool::toolTransformAndTimestamp, this, &ShapeSensorWidget::receiveTransforms);
//	mTool = mServices->tracking()->getActiveTool();
//	connect(mTool.get(), &Tool::toolTransformAndTimestamp, this, &ShapeSensorWidget::receiveTransforms);

	disconnect(mSelector.get(), &StringPropertyBase::changed, this, &ShapeSensorWidget::activeToolChangedSlot);
	mTool = mServices->tracking()->getTool(mSelector->getValue());
	connect(mSelector.get(), &StringPropertyBase::changed, this, &ShapeSensorWidget::activeToolChangedSlot);
}

void ShapeSensorWidget::receiveTransforms(Transform3D prMt, double timestamp)
{
	mReadFbgsMessage.set_prMt(prMt);
}
} /* namespace cx */
