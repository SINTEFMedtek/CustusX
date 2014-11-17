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

#include "cxFileSelectWidget.h"
#include "cxTime.h"
#include "cxReporter.h"
#include "cxProbeSector.h"
#include "cxRegistrationTransform.h"
#include "cxStringDataAdapterXml.h"
#include "cxHelperWidgets.h"
#include "cxDataAdapterHelper.h"
#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "cxVideoConnectionManager.h"
#include "cxImageServer.h"
#include "cxVideoServiceOld.h"
#include "cxTrackingService.h"
#include "cxViewManager.h"
#include "cxFileInputWidget.h"
#include "cxLogger.h"
#include "cxOptionsWidget.h"
#include "cxVideoService.h"
#include "cxPatientModelService.h"
#include "cxDetailedLabeledComboBoxWidget.h"

//TODO: remove
#include "cxLegacySingletons.h"
#include "cxLogicManager.h"

namespace cx
{

VideoConnectionWidget::VideoConnectionWidget(VisualizationServicePtr visualizationService, PatientModelServicePtr patientModelService, VideoServicePtr newVideoService, QWidget* parent) :
	BaseWidget(parent, "IGTLinkWidget", "Video Connection"),
	mVisualizationService(visualizationService),
	mPatientModelService(patientModelService),
	mVideoService(newVideoService)
{
	mInitScriptWidget=NULL;

	mOptions = XmlOptionFile(DataLocations::getXmlSettingsFile()).descend("video");

	QStringList connectionOptions;
	QString defaultConnection = this->getVideoConnectionManager()->getConnectionMethod();
	connectionOptions << "Local Server" << "Direct Link" << "Remote Server";
	mConnectionSelector = StringDataAdapterXml::initialize("Connection", "", "Method for connecting to Video Server", defaultConnection, connectionOptions, mOptions.getElement("video"));
	connect(mConnectionSelector.get(), SIGNAL(changed()), this, SLOT(selectGuiForConnectionMethodSlot()));

	connect(this->getVideoConnectionManager().get(), SIGNAL(connected(bool)), this, SLOT(serverStatusChangedSlot()));
	connect(this->getServerProcess(), SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));

	this->initializeScriptWidget();
	mStackedWidget = this->initializeStackedWidget();
	QFrame* frame = this->wrapStackedWidgetInAFrame();
	mConnectButton = this->initializeConnectButton();
	mImportStreamImageButton = this->initializeImportStreamImageButton();
	mActiveVideoSourceSelector = this->initializeActiveVideoSourceSelector();
	mConnectionSelectionWidget = new DetailedLabeledComboBoxWidget(this, mConnectionSelector);

	mToptopLayout = new QVBoxLayout(this);
	mToptopLayout->addWidget(mInitScriptWidget);
	mToptopLayout->addWidget(mConnectionSelectionWidget);
	mToptopLayout->addWidget(frame);
	mToptopLayout->addWidget(mConnectButton);
	mToptopLayout->addWidget(mImportStreamImageButton);
	mToptopLayout->addWidget(sscCreateDataWidget(this, mActiveVideoSourceSelector));
	mToptopLayout->addStretch();

	connect(mVideoService.get(), SIGNAL(StreamerServiceAdded(StreamerService*)), this, SLOT(onServiceAdded(StreamerService*)));
	connect(mVideoService.get(), SIGNAL(StreamerServiceRemoved(StreamerService*)), this, SLOT(onServiceRemoved(StreamerService*)));

	this->addExistingStreamerServices(); //Need to add StreamerServices already existing at this point, since we will only get signals when new Services are added

	this->selectGuiForConnectionMethodSlot();
}

VideoConnectionWidget::~VideoConnectionWidget()
{
	if (mVideoService)
	{
		disconnect(mVideoService.get(), SIGNAL(StreamerServiceAdded(StreamerService*)), this, SLOT(onServiceAdded(StreamerService*)));
		disconnect(mVideoService.get(), SIGNAL(StreamerServiceRemoved(StreamerService*)), this, SLOT(onServiceRemoved(StreamerService*)));
	}
}

void VideoConnectionWidget::addExistingStreamerServices()
{
	QList<StreamerService *> services = mVideoService->getStreamerServices();
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

	OptionsWidget* widget = new OptionsWidget(mVisualizationService, mPatientModelService, this);
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
//	range.removeAll(name);
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

void VideoConnectionWidget::initializeScriptWidget()
{
	mInitScriptWidget = new FileInputWidget(this);
	mInitScriptWidget->setDescription("Init script");
	mInitScriptWidget->setBasePath(DataLocations::getBundlePath());
	mInitScriptWidget->setFilename(getVideoConnectionManager()->getInitScript());
	mInitScriptWidget->setHelp("A script that will be run prior to starting the server. Useful for grabber-specific initialization");
	mInitScriptWidget->setBrowseHelp("Select an init script");
	mInitScriptWidget->setUseRelativePath(true);
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

void VideoConnectionWidget::initScriptSelected(QString filename)
{
	getVideoConnectionManager()->setInitScript(filename);
}

QWidget* VideoConnectionWidget::createDirectLinkWidget()
{
	QWidget* retval = new QWidget();
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);
	layout->addWidget(new QLabel("Arguments", this), 0, 0);
	mDirectLinkArguments = new QComboBox(this);
	mDirectLinkArguments->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	mDirectLinkArguments->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mDirectLinkArguments->setEditable(true);
	mDirectLinkArguments->setInsertPolicy(QComboBox::InsertAtTop);
	mDirectLinkArguments->setToolTip(ImageServer::getArgumentHelpText(""));
	this->updateDirectLinkArgumentHistory();
	layout->addWidget(mDirectLinkArguments, 0, 1);
	return retval;
}

QWidget* VideoConnectionWidget::createLocalServerWidget()
{
	QWidget* retval = new QWidget();
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);
	int line=0;

	mLocalServerFile = new FileInputWidget(this);
	mLocalServerFile->setDescription("Local Server");
	mLocalServerFile->setBasePath(DataLocations::getBundlePath());
	mLocalServerFile->setUseRelativePath(true);
	mLocalServerFile->setFilename(getVideoConnectionManager()->getLocalServerExecutable());
	mLocalServerFile->setHelp(ImageServer::getArgumentHelpText("<executable>"));
	mLocalServerFile->setBrowseHelp("Select a local server application");
	layout->addWidget(mLocalServerFile, line, 0, 1, 2);

	++line;
	layout->addWidget(new QLabel("Arguments", this), line, 0);
	mLocalServerArguments = new QLineEdit(this);
	mLocalServerArguments->setToolTip(ImageServer::getArgumentHelpText("<executable>"));
	mLocalServerArguments->setText(getVideoConnectionManager()->getLocalServerArguments());
	layout->addWidget(mLocalServerArguments, line, 1);

	++line;
	mLaunchServerButton = new QPushButton("Launch Local Server", this);
	connect(mLaunchServerButton, SIGNAL(clicked()), this, SLOT(toggleLaunchServer()));
	mLaunchServerButton->setToolTip("Launch/Close the selected server without connecting to it.");
	layout->addWidget(mLaunchServerButton, line, 0, 2, 0);

	return retval;
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

QWidget* VideoConnectionWidget::createRemoteWidget()
{
	QWidget* retval = new QWidget();
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);
	layout->addWidget(new QLabel("IP Address", this), 0, 0);
	mAddressEdit = new QComboBox(this);
	mAddressEdit->setEditable(true);
	mAddressEdit->setInsertPolicy(QComboBox::InsertAtTop);
	mAddressEdit->setToolTip("Enter TCP/IP address of remote host the Video Server resides on");
	this->updateHostHistory();
	layout->addWidget(mAddressEdit, 0, 1);
	layout->addWidget(new QLabel("Port number", this), 1, 0);
	mPortEdit = new QLineEdit(this);
	mPortEdit->setText(QString::number(getVideoConnectionManager()->getPort()));
	mPortEdit->setToolTip("Enter TCP/IP port that the video server is listening to");
	layout->addWidget(mPortEdit, 1, 1);
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

QProcess* VideoConnectionWidget::getServerProcess()
{
	return this->getVideoConnectionManager()->getLocalVideoServerProcess();
}

bool VideoConnectionWidget::serverIsRunning()
{
	bool isRunning = (this->getServerProcess()) && (this->getServerProcess()->state() == QProcess::Running);
	return isRunning;
}

VideoConnectionManagerPtr VideoConnectionWidget::getVideoConnectionManager()
{
	return videoService()->getVideoConnection();
}

void VideoConnectionWidget::selectGuiForConnectionMethodSlot()
{
	QString name = mConnectionSelector->getValue();
	//Need to set connection method in VideoConneectionManager before calling useDirectLink(), useLocalServer() and useRemoteServer()
	this->getVideoConnectionManager()->setConnectionMethod(mConnectionSelector->getValue());

	if (this->getVideoConnectionManager()->useDirectLink())
		mStackedWidget->setCurrentIndex(0);
	else if (this->getVideoConnectionManager()->useLocalServer())
		mStackedWidget->setCurrentIndex(1);
	else if(this->getVideoConnectionManager()->useRemoteServer())
		mStackedWidget->setCurrentIndex(2);
	else
	{
		QWidget* serviceWidget = mStreamerServiceWidgets[name];
		if(serviceWidget)
			mStackedWidget->setCurrentWidget(serviceWidget);
	}
}

void VideoConnectionWidget::updateHostHistory()
{
	mAddressEdit->blockSignals(true);
	mAddressEdit->clear();
	mAddressEdit->addItems(this->getVideoConnectionManager()->getHostHistory());
	mAddressEdit->blockSignals(false);
}

void VideoConnectionWidget::updateDirectLinkArgumentHistory()
{
	mDirectLinkArguments->blockSignals(true);
	mDirectLinkArguments->clear();
	mDirectLinkArguments->addItems(getVideoConnectionManager()->getDirectLinkArgumentHistory());
	mDirectLinkArguments->blockSignals(false);
}

void VideoConnectionWidget::launchServer()
{
	this->writeSettings();
	this->getVideoConnectionManager()->launchServer();
}

void VideoConnectionWidget::toggleLaunchServer()
{
	if (this->serverIsRunning())
		this->getServerProcess()->close();
	else
		this->launchServer();
}

void VideoConnectionWidget::serverProcessStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
		mLaunchServerButton->setText("Close Local Server");

	if (newState == QProcess::NotRunning)
		mLaunchServerButton->setText("Launch Local Server");

	if (newState == QProcess::Starting)
		mLaunchServerButton->setText("Starting...");
}

void VideoConnectionWidget::toggleConnectServer()
{
	if (!this->getVideoConnectionManager()->isConnected())
		this->connectServer();
	else
		this->disconnectServer();
}

void VideoConnectionWidget::writeSettings()
{
	this->getVideoConnectionManager()->setInitScript(mInitScriptWidget->getFilename());

	//Need to set connection method in VideoConneectionManager before calling useDirectLink(), useLocalServer() and useRemoteServer()
	this->getVideoConnectionManager()->setConnectionMethod(mConnectionSelector->getValue());

	if (this->getVideoConnectionManager()->useDirectLink())
	{
		this->getVideoConnectionManager()->setLocalServerArguments(mDirectLinkArguments->currentText());
		this->updateDirectLinkArgumentHistory();
	}
	else if (this->getVideoConnectionManager()->useLocalServer())
	{
		this->getVideoConnectionManager()->setLocalServerExecutable(mLocalServerFile->getFilename());
		this->getVideoConnectionManager()->setLocalServerArguments(mLocalServerArguments->text());
	}
	else if (this->getVideoConnectionManager()->useRemoteServer())
	{
		this->getVideoConnectionManager()->setHost(mAddressEdit->currentText());
		this->getVideoConnectionManager()->setPort(mPortEdit->text().toInt());
		this->updateHostHistory();
	}
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

QStackedWidget* VideoConnectionWidget::initializeStackedWidget()
{
	QStackedWidget* stackedWidget = new QStackedWidget(this);
	stackedWidget->addWidget(this->wrapVerticalStretch(this->createDirectLinkWidget()));
	stackedWidget->addWidget(this->wrapVerticalStretch(this->createLocalServerWidget()));
	stackedWidget->addWidget(this->wrapVerticalStretch(this->createRemoteWidget()));

	return stackedWidget;
}

void VideoConnectionWidget::connectServer()
{
	if (!this->getVideoConnectionManager()->isConnected())
	{
		this->writeSettings();
		this->getVideoConnectionManager()->launchAndConnectServer(mVideoService, mConnectionSelector->getValue());
	}
}

void VideoConnectionWidget::disconnectServer()
{
	this->getVideoConnectionManager()->disconnectServer();
}

void VideoConnectionWidget::serverStatusChangedSlot()
{
	mImportStreamImageButton->setEnabled(this->getVideoConnectionManager()->isConnected());
	if (this->getVideoConnectionManager()->isConnected())
		mConnectButton->setText("Disconnect Server");
	else
		mConnectButton->setText("Connect Server");

	this->adjustSize();
}

void VideoConnectionWidget::importStreamImageSlot()
{
	if (!this->getVideoConnectionManager())
	{
		reportWarning("No video connection");
		return;
	}
	if (!this->getVideoConnectionManager()->isConnected())
	{
		reportWarning("Video is not connected");
		return;
	}
	Transform3D rMd = Transform3D::Identity();
	ToolPtr probe = trackingService()->getFirstProbe();
	VideoSourcePtr videoSource;
	if (probe)
	{
		videoSource = probe->getProbe()->getRTSource();
		rMd = calculate_rMd_ForAProbeImage(probe);
	}
	else
		videoSource = videoService()->getActiveVideoSource();

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
	Transform3D rMpr = mPatientModelService->get_rMpr();
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

	ImagePtr output = mPatientModelService->createSpecificData<Image>(filename);
	output->setVtkImageData(input);
	output->get_rMd_History()->setRegistration(rMd);
	mPatientModelService->insertData(output);

//	ImagePtr output = mPatientModelService->createImage(copiedImage, filename, filename);
//	output->get_rMd_History()->setRegistration(rMd);
//	QString folder = patientService()->getPatientData()->getActivePatientFolder();
//	mPatientModelService->loadData(output);
//	mPatientModelService->saveImage(output, folder);

	viewManager()->autoShowData(output);
	report(QString("Saved snapshot %1 from active video source").arg(output->getName()));
}

} //end namespace cx
