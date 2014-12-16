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

#include "cxVideoConnectionWidget.h"

#include <boost/bind.hpp>

#include <QDir>
#include <QStackedWidget>
#include <QPushButton>
#include <QFileDialog>

#include "vtkImageData.h"

#include "cxTime.h"
#include "cxLogger.h"
#include "cxProbeSector.h"
#include "cxRegistrationTransform.h"
#include "cxStringDataAdapterXml.h"
#include "cxHelperWidgets.h"
#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "cxTrackingService.h"
#include "cxOptionsWidget.h"
#include "cxVideoService.h"
#include "cxPatientModelService.h"
#include "cxDetailedLabeledComboBoxWidget.h"
#include "cxStreamerService.h"
#include "cxVideoSource.h"
#include "cxVisualizationService.h"
#include "cxImage.h"

namespace cx
{

VideoConnectionWidget::VideoConnectionWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "IGTLinkWidget", "Video Connection"),
	mServices(services)
{
	mOptions = XmlOptionFile(DataLocations::getXmlSettingsFile()).descend("video");

	QString defaultConnection = mServices->videoService->getConnectionMethod();
	mConnectionSelector = StringDataAdapterXml::initialize("Connection", "", "Method for connecting to Video Server", defaultConnection, QStringList(), mOptions.getElement("video"));
	connect(mConnectionSelector.get(), SIGNAL(changed()), this, SLOT(selectGuiForConnectionMethodSlot()));

	connect(mServices->videoService.get(), &VideoService::connected, this, &VideoConnectionWidget::serverStatusChangedSlot);

	mStackedWidget = new QStackedWidget(this);
	QFrame* frame = this->wrapStackedWidgetInAFrame();
	mConnectButton = this->initializeConnectButton();
	mImportStreamImageButton = this->initializeImportStreamImageButton();
	mActiveVideoSourceSelector = this->initializeActiveVideoSourceSelector();
	mConnectionSelectionWidget = new DetailedLabeledComboBoxWidget(this, mConnectionSelector);

	mToptopLayout = new QVBoxLayout(this);
	mToptopLayout->addWidget(mConnectionSelectionWidget);
	mToptopLayout->addWidget(frame);
	mToptopLayout->addWidget(mConnectButton);
	mToptopLayout->addWidget(mImportStreamImageButton);
	mToptopLayout->addWidget(sscCreateDataWidget(this, mActiveVideoSourceSelector));
	mToptopLayout->addStretch();

	connect(mServices->videoService.get(), SIGNAL(StreamerServiceAdded(StreamerService*)), this, SLOT(onServiceAdded(StreamerService*)));
	connect(mServices->videoService.get(), SIGNAL(StreamerServiceRemoved(StreamerService*)), this, SLOT(onServiceRemoved(StreamerService*)));

	this->addExistingStreamerServices(); //Need to add StreamerServices already existing at this point, since we will only get signals when new Services are added

	this->selectGuiForConnectionMethodSlot();
}

VideoConnectionWidget::~VideoConnectionWidget()
{
	if (mServices->videoService)
	{
		disconnect(mServices->videoService.get(), SIGNAL(StreamerServiceAdded(StreamerService*)), this, SLOT(onServiceAdded(StreamerService*)));
		disconnect(mServices->videoService.get(), SIGNAL(StreamerServiceRemoved(StreamerService*)), this, SLOT(onServiceRemoved(StreamerService*)));
	}
}

void VideoConnectionWidget::addExistingStreamerServices()
{
	QList<StreamerService *> services = mServices->videoService->getStreamerServices();
	foreach(StreamerService* service, services)
	{
		this->onServiceAdded(service);
	}
}

void VideoConnectionWidget::onServiceAdded(StreamerService* service)
{
	QWidget* widget = this->createStreamerWidget(service);
	QWidget* serviceWidget = this->wrapVerticalStretch(widget);
	mStackedWidget->addWidget(serviceWidget);
	mStreamerServiceWidgets[service->getName()] = serviceWidget;

	this->addServiceToSelector(service->getName());
}

QWidget* VideoConnectionWidget::createStreamerWidget(StreamerService* service)
{
	QString serviceName = service->getName();
	QDomElement element = mOptions.getElement("video");
	std::vector<DataAdapterPtr> adapters = service->getSettings(element);

	OptionsWidget* widget = new OptionsWidget(mServices->visualizationService, mServices->patientModelService, this);
	widget->setOptions(serviceName, adapters, false);

	connect(mConnectionSelectionWidget, SIGNAL(detailsTriggered()), widget, SLOT(toggleAdvanced()));

	return widget;
}

void VideoConnectionWidget::onServiceRemoved(StreamerService *service)
{
	this->removeServiceFromSelector(service->getName());
	this->removeServiceWidget(service->getName());
}

void VideoConnectionWidget::addServiceToSelector(QString name)
{
	QStringList range = mConnectionSelector->getValueRange();
	range.append(name);
	range.removeDuplicates();
	mConnectionSelector->setValueRange(range);
}

void VideoConnectionWidget::removeServiceFromSelector(QString name)
{
	QStringList range = mConnectionSelector->getValueRange();
	int index = range.indexOf(name);
	if(mConnectionSelector->getValue() == name)
		mConnectionSelector->setValue(range[0]);
	range.removeAt(index);
	mConnectionSelector->setValueRange(range);
}

void VideoConnectionWidget::removeServiceWidget(QString name)
{
	QWidget* serviceWidget = mStreamerServiceWidgets[name];
	mStackedWidget->removeWidget(serviceWidget);
	delete serviceWidget;
	mStreamerServiceWidgets.erase(name);
}

ActiveVideoSourceStringDataAdapterPtr VideoConnectionWidget::initializeActiveVideoSourceSelector()
{
	return ActiveVideoSourceStringDataAdapter::New();
}

QFrame* VideoConnectionWidget::wrapStackedWidgetInAFrame()
{
	QFrame* frame = new QFrame(this);
	frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	frame->setSizePolicy(frame->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	QVBoxLayout* frameLayout = new QVBoxLayout(frame);
	frameLayout->addWidget(mStackedWidget);

	return frame;
}

QWidget* VideoConnectionWidget::wrapVerticalStretch(QWidget* input)
{
	QWidget* retval = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(retval);
	layout->addWidget(input);
	layout->addStretch();
	layout->setMargin(0);
	layout->setSpacing(0);
	return retval;
}

QString VideoConnectionWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3><Setup IGTLink connection.</h3>"
			"<p>Lets you set up a connection to a streaming server using IGTLink.</p>"
			"<p><i></i></p>"
			"</html>";
}

void VideoConnectionWidget::selectGuiForConnectionMethodSlot()
{
	QString name = mConnectionSelector->getValue();
	//Need to set connection method in VideoConneectionManager before calling useDirectLink(), useLocalServer() and useRemoteServer()
	mServices->videoService->setConnectionMethod(mConnectionSelector->getValue());

	QWidget* serviceWidget = mStreamerServiceWidgets[name];
	if(serviceWidget)
		mStackedWidget->setCurrentWidget(serviceWidget);
}

void VideoConnectionWidget::toggleConnectServer()
{
	if (mServices->videoService->isConnected())
		mServices->videoService->closeConnection();
	else
		mServices->videoService->openConnection();
}

QPushButton* VideoConnectionWidget::initializeConnectButton()
{
	QPushButton* connectButton = new QPushButton("Connect", this);
	connectButton->setToolTip("Connect/disconnect to the video server using the seleted method");
	connect(connectButton, SIGNAL(clicked()), this, SLOT(toggleConnectServer()));
	return connectButton;
}

QPushButton* VideoConnectionWidget::initializeImportStreamImageButton()
{
	QPushButton* importstreamimagebutton = new QPushButton("Import image from stream", this);
	importstreamimagebutton->setToolTip("Import a single image/volume from the real time stream");
	importstreamimagebutton->setDisabled(true);
	connect(importstreamimagebutton, SIGNAL(clicked()), this, SLOT(importStreamImageSlot()));

	return importstreamimagebutton;
}

void VideoConnectionWidget::serverStatusChangedSlot()
{
	mImportStreamImageButton->setEnabled(mServices->videoService->isConnected());
	if (mServices->videoService->isConnected())
		mConnectButton->setText("Disconnect Server");
	else
		mConnectButton->setText("Connect Server");

	this->adjustSize();
}

void VideoConnectionWidget::importStreamImageSlot()
{
	if (!mServices->videoService->isConnected())
	{
		reportWarning("Video is not connected");
		return;
	}
	Transform3D rMd = Transform3D::Identity();
	ToolPtr probe = mServices->trackingService->getFirstProbe();
	VideoSourcePtr videoSource;
	if (probe)
	{
		videoSource = probe->getProbe()->getRTSource();
		rMd = calculate_rMd_ForAProbeImage(probe);
	}
	else
		videoSource = mServices->videoService->getActiveVideoSource();

	if (!videoSource)
	{
		reportWarning("No Video data source");
		return;
	}
	if (!videoSource->validData())
	{
		reportWarning("No valid video data");
		return;
	}

	vtkImageDataPtr input;
	input = videoSource->getVtkImageData();
	if (!input)
	{
		reportWarning("No Video data");
		return;
	}
	QString filename = generateFilename(videoSource);

	this->saveAndImportSnapshot(input, filename, rMd);

}

Transform3D VideoConnectionWidget::calculate_rMd_ForAProbeImage(ToolPtr probe)
{
	Transform3D rMd = Transform3D::Identity();
	Transform3D rMpr = mServices->patientModelService->get_rMpr();
	Transform3D prMt = probe->get_prMt();
	Transform3D tMu = probe->getProbe()->getSector()->get_tMu();
	Transform3D uMv = probe->getProbe()->getSector()->get_uMv();
	rMd = rMpr * prMt * tMu * uMv;
	return rMd;
}

QString VideoConnectionWidget::generateFilename(VideoSourcePtr videoSource)
{
	vtkImageDataPtr input = videoSource->getVtkImageData();
	int* extent = input->GetExtent();
	QString filename;
	QString format = timestampSecondsFormat();
	if (extent[5] - extent[4] > 0)
		filename = "3DRTSnapshot_";
	else
		filename = "2DRTSnapshot_";

	filename += videoSource->getName() + QDateTime::currentDateTime().toString(format);
	return filename;
}

void VideoConnectionWidget::saveAndImportSnapshot(vtkImageDataPtr input, QString filename, Transform3D rMd)
{
	vtkImageDataPtr copiedImage = vtkImageDataPtr::New();
	copiedImage->DeepCopy(input);

	ImagePtr output = mServices->patientModelService->createSpecificData<Image>(filename);
	output->setVtkImageData(input);
	output->get_rMd_History()->setRegistration(rMd);
	mServices->patientModelService->insertData(output);

	mServices->visualizationService->autoShowData(output);
	report(QString("Saved snapshot %1 from active video source").arg(output->getName()));
}

} //end namespace cx
