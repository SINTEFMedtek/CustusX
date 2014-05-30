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

#include "cxVideoConnectionWidget.h"

#include <boost/bind.hpp>

#include <QDir>
#include <QStackedWidget>
#include <QPushButton>
#include <QFileDialog>

#include "vtkImageData.h"

#include "cxFileSelectWidget.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDataManager.h"
#include "cxTime.h"
#include "cxReporter.h"
#include "cxProbeSector.h"
#include "cxRegistrationTransform.h"
#include "cxStringDataAdapterXml.h"
#include "cxHelperWidgets.h"

#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "cxVideoConnectionManager.h"
#include "cxImageServer.h"
#include "cxVideoService.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
//#include "cxSimulateUSWidget.h"
#include "cxFileInputWidget.h"
#include "cxLogger.h"
#include "cxLogicManager.h"

namespace cx
{

VideoConnectionWidget::VideoConnectionWidget(QWidget* parent) :
		BaseWidget(parent, "IGTLinkWidget", "Video Connection")//, mSimulationWidget(NULL)
{
	mInitScriptWidget=NULL;

	connect(this->getVideoConnectionManager().get(), SIGNAL(connected(bool)), this, SLOT(serverStatusChangedSlot()));
	connect(this->getVideoConnectionManager().get(), SIGNAL(connectionMethodChanged()), this, SLOT(selectGuiForConnectionMethodSlot()));
	connect(this->getServerProcess(), SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));

	this->initializeScriptWidget();
	mConnectionSelector = this->initializeConnectionSelector();
	mStackedWidget = this->initializeStackedWidget();
	QFrame* frame = this->wrapStackedWidgetInAFrame();
	mConnectButton = this->initializeConnectButton();
	mImportStreamImageButton = this->initializeImportStreamImageButton();
	mActiveVideoSourceSelector = this->initializeActiveVideoSourceSelector();

	mToptopLayout = new QVBoxLayout(this);
	mToptopLayout->addWidget(mInitScriptWidget);
	mToptopLayout->addWidget(new LabeledComboBoxWidget(this, mConnectionSelector));
	mToptopLayout->addWidget(frame);
	mToptopLayout->addWidget(mConnectButton);
	mToptopLayout->addWidget(mImportStreamImageButton);
	mToptopLayout->addWidget(sscCreateDataWidget(this, mActiveVideoSourceSelector));
	mToptopLayout->addStretch();

	mServiceListener.reset(new ServiceTrackerListener<StreamerService>(
			LogicManager::getInstance()->getPluginFramework(),
			boost::bind(&VideoConnectionWidget::onServiceAdded, this),
			boost::function<void ()>(),
			boost::bind(&VideoConnectionWidget::onServiceRemoved, this)
	));

	this->selectGuiForConnectionMethodSlot();
}

VideoConnectionWidget::~VideoConnectionWidget()
{}

void VideoConnectionWidget::onServiceAdded()
{
	std::cout << "VideoConnectionWidget::Added!!!" << std::endl;
//	mStackedWidget->addWidget(this->wrapVerticalStretch(service->createWidget));
}
void VideoConnectionWidget::onServiceRemoved()
{
	std::cout << "VideoConnectionWidget::Removed!!!" << std::endl;
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

StringDataAdapterXmlPtr VideoConnectionWidget::initializeConnectionSelector()
{
	return this->getVideoConnectionManager()->getConnectionMethod();
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

//TODO: This commented out code is a start of a fix of #884. Not finisehd yet
//QWidget* VideoConnectionWidget::createDirectLinkWidget()
//{
//	QWidget* retval = new QWidget();
//	QGridLayout* layout = new QGridLayout(retval);
//	layout->setMargin(0);

//	cx::ImageStreamerFactory factory;
//	QString selectedSender = factory.getDefaultSenderType();
//	QStringList senderTypes = factory.getSenderTypes();
//	mSenderType = StringDataAdapterXml::initialize("Grabber type", "",
//																								 "Video grabber/sender type",
//																								 selectedSender, senderTypes);
//	connect(mSenderType.get(), SIGNAL(changed()), this, SLOT(senderTypeChanged()));


//	layout->addWidget(new LabeledComboBoxWidget(this, mSenderType), 0, 1);
//	return retval;
//}

//When type is changed generate/use widget with arguments for this grabber/sender
//void VideoConnectionWidget::senderTypeChanged()
//{
//	cx::ImageStreamerFactory factory;
//	StreamerPtr streamer = factory.getImageSender(mSenderType->getValue());

//	//TODO: Get available arguments from streamer
////	QStringList args = streamer->getArgumentList();
////	for (int i = 0; i < args.size(); ++i)
////	{
////		//TODO: Generate correct GUI item based on argument type. Can this be done similarly to the TSF GUI?
////		QStringList values = streamer->getArgumentValues(args.at(i));
////	}

//}

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

//QWidget* VideoConnectionWidget::createSimulationWidget()
//{
//	mSimulationWidget = new SimulateUSWidget();
//	return mSimulationWidget;
//}

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
	if (this->getVideoConnectionManager()->useDirectLink())
		mStackedWidget->setCurrentIndex(0);
	else if (this->getVideoConnectionManager()->useLocalServer())
		mStackedWidget->setCurrentIndex(1);
	else if(this->getVideoConnectionManager()->useRemoteServer())
		mStackedWidget->setCurrentIndex(2);
	else if(this->getVideoConnectionManager()->useSimulatedServer())
		mStackedWidget->setCurrentIndex(3);
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
	else if (this->getVideoConnectionManager()->useSimulatedServer())
	{
		this->getVideoConnectionManager()->setLocalServerArguments("--type SimulatedImageStreamer");
	}
}

QPushButton* VideoConnectionWidget::initializeConnectButton()
{
	QPushButton* connectButton = new QPushButton("Connect Server", this);
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
//	stackedWidget->addWidget(this->wrapVerticalStretch(this->createSimulationWidget()));

	//TODO: Get widget from StreamerService (plugin)

	return stackedWidget;
}

void VideoConnectionWidget::connectServer()
{
	if (!this->getVideoConnectionManager()->isConnected())
	{
		this->writeSettings();
		this->getVideoConnectionManager()->launchAndConnectServer();
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
	ToolPtr probe = toolManager()->findFirstProbe();
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
	Transform3D rMpr = dataManager()->get_rMpr();
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
	ImagePtr output = dataManager()->createImage(copiedImage, filename, filename);
	output->get_rMd_History()->setRegistration(rMd);
	QString folder = patientService()->getPatientData()->getActivePatientFolder();
	dataManager()->loadData(output);
	dataManager()->saveImage(output, folder);
	viewManager()->autoShowData(output);
	report(QString("Saved snapshot %1 from active video source").arg(output->getName()));
}

} //end namespace cx
