#include "cxVideoConnectionWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QDir>

#include "vtkRenderWindow.h"
#include <vtkImageData.h>

#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscVideoRep.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"
#include "sscToolManager.h"
#include "sscTime.h"
#include "sscMessageManager.h"
#include "cxVideoConnectionManager.h"
//#include "cxStateService.h"
#include "cxImageServer.h"
#include "cxVideoService.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "sscStringDataAdapterXml.h"
#include "cxVideoConnection.h"
#include "sscHelperWidgets.h"
#include "cxDataInterface.h"

namespace cx
{

VideoConnectionWidget::VideoConnectionWidget(QWidget* parent) :
		BaseWidget(parent, "IGTLinkWidget", "Video Connection")
{
	connect(this->getConnection().get(), SIGNAL(connected(bool)), this, SLOT(serverStatusChangedSlot()));
	connect(getServer(), SIGNAL(stateChanged(QProcess::ProcessState)), this,
			SLOT(serverProcessStateChanged(QProcess::ProcessState)));

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	mToptopLayout = toptopLayout;

	QHBoxLayout* initScriptLayout = new QHBoxLayout();
	toptopLayout->addLayout(initScriptLayout);
	initScriptLayout->addWidget(new QLabel("Init script", this));
	mInitScriptWidget = new ssc::FileSelectWidget(this);
	QString path = QDir::cleanPath(DataLocations::getBundlePath() + "/" + getConnection()->getInitScript());
	QStringList nameFilters;
	nameFilters << "*.*";
	mInitScriptWidget->setNameFilter(nameFilters);

	if (!getConnection()->getInitScript().isEmpty())
		mInitScriptWidget->setFilename(path);
//	mParameterFileWidget0->setFilename(mElastixManager->getActiveParameterFile0());
	connect(mInitScriptWidget, SIGNAL(fileSelected(QString)), this, SLOT(initScriptSelected(QString)));
	mInitScriptWidget->setSizePolicy(QSizePolicy::Expanding, mInitScriptWidget->sizePolicy().verticalPolicy());
	initScriptLayout->addWidget(mInitScriptWidget);

	mToptopLayout->addWidget(new ssc::LabeledComboBoxWidget(this, this->getConnection()->getConnectionMethod()));
	connect(this->getConnection().get(), SIGNAL(settingsChanged()), this, SLOT(dataChanged()));

	mStackedWidget = new QStackedWidget(this);
	mStackedWidget->addWidget(this->wrapVerticalStretch(this->createDirectLinkWidget()));
	mStackedWidget->addWidget(this->wrapVerticalStretch(this->createLocalServerWidget()));
	mStackedWidget->addWidget(this->wrapVerticalStretch(this->createRemoteWidget()));

	// wrap stackedwidget in a frame:
	QFrame* frame = new QFrame(this);
	frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	frame->setSizePolicy(frame->sizePolicy().horizontalPolicy(),QSizePolicy::Fixed);
	QVBoxLayout* frameLayout = new QVBoxLayout(frame);
	frameLayout->addWidget(mStackedWidget);
	toptopLayout->addWidget(frame);

	mConnectButton = new QPushButton("Connect Server", this);
	mConnectButton->setToolTip(""
			"Connect/disconnect to the video server using the seleted method");
	connect(mConnectButton, SIGNAL(clicked()), this, SLOT(toggleConnectServer()));
	toptopLayout->addWidget(mConnectButton);

	mSnapshotButton = new QPushButton("Snapshot", this);
	mSnapshotButton->setToolTip(""
			"Save snapshot of real time image/volume in the snapshot folder");
	mSnapshotButton->setDisabled(true);
	connect(mSnapshotButton, SIGNAL(clicked()), this, SLOT(saveSnapshotSlot()));
	toptopLayout->addWidget(mSnapshotButton);

	toptopLayout->addWidget(createDataWidget(this, ActiveVideoSourceStringDataAdapter::New()));

	toptopLayout->addStretch();

	this->dataChanged();
}

void VideoConnectionWidget::initScriptSelected(QString filename)
{
	getConnection()->setInitScript(filename);
}

QWidget* VideoConnectionWidget::createDirectLinkWidget()
{
	QWidget* retval = new QWidget();
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);

	layout->addWidget(new QLabel("Arguments", this), 0, 0);
	mDirectLinkArguments = new QComboBox(this);
	//Reduce size as mDirectLinkArguments can contain too much info for the widget
	mDirectLinkArguments->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	//Set size expanding in horizontal direction
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

	layout->addWidget(new QLabel("Local Server", this), 0, 0);
	mLocalServerEdit = new QLineEdit(this);
	mLocalServerEdit->setText(getConnection()->getLocalServerExecutable());
	mLocalServerEdit->setToolTip(ImageServer::getArgumentHelpText("<executable>"));
	layout->addWidget(mLocalServerEdit, 0, 1);

	QAction* browseLocalServerAction = new QAction(QIcon(":/icons/open.png"), "Browse", this);
	browseLocalServerAction->setStatusTip("Select a local server application");
	connect(browseLocalServerAction, SIGNAL(triggered()), this, SLOT(browseLocalServerSlot()));
	QToolButton* button = new QToolButton();
	button->setDefaultAction(browseLocalServerAction);
	layout->addWidget(button, 0, 2);

	layout->addWidget(new QLabel("Arguments", this), 1, 0);
	mLocalServerArguments = new QLineEdit(this);
	mLocalServerArguments->setToolTip(ImageServer::getArgumentHelpText("<executable>"));
	mLocalServerArguments->setText(getConnection()->getLocalServerArguments());
	layout->addWidget(mLocalServerArguments, 1, 1);

	mLaunchServerButton = new QPushButton("Launch Local Server", this);
	connect(mLaunchServerButton, SIGNAL(clicked()), this, SLOT(toggleLaunchServer()));
	mLaunchServerButton->setToolTip("Launch/Close the selected server without connecting to it.");
	layout->addWidget(mLaunchServerButton, 2, 0, 2, 0);

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
	mPortEdit->setText(QString::number(getConnection()->getPort()));
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

QProcess* VideoConnectionWidget::getServer()
{
	return getConnection()->getProcess();
}

//GrabberVideoSourcePtr IGTLinkWidget::getRTSource()
//{
//	return getConnection()->getVideoSource();
//}

VideoConnectionManagerPtr VideoConnectionWidget::getConnection()
{
	return videoService()->getVideoConnection();
}

VideoConnectionWidget::~VideoConnectionWidget()
{
}

//void IGTLinkWidget::useLocalServerChanged()
//{
//	if (mUseLocalServer->isChecked())
//	{
//		mAddressEdit->addItem("Localhost");
//		mAddressEdit->setCurrentIndex(mAddressEdit->count() - 1);
//	}
//
//	this->writeSettings();
//	this->dataChanged();
//}
//
//void IGTLinkWidget::useDirectLinkChanged()
//{
//	this->writeSettings();
//	this->dataChanged();
//}

void VideoConnectionWidget::dataChanged()
{
	if (getConnection()->getUseDirectLink2())
		mStackedWidget->setCurrentIndex(0);
	else if (getConnection()->getUseLocalServer2())
		mStackedWidget->setCurrentIndex(1);
	else
		mStackedWidget->setCurrentIndex(2);

//	bool direct = getConnection()->getUseDirectLink2();
//	bool local = getConnection()->getUseLocalServer2();
//
//	mAddressEdit->setEnabled(!local && !direct);
//	mLocalServerEdit->setEnabled(local);
//	mLaunchServerButton->setEnabled(local && !direct);
//	mUseLocalServer->setEnabled(!direct);
}

void VideoConnectionWidget::updateHostHistory()
{
	mAddressEdit->blockSignals(true);
	mAddressEdit->clear();
	mAddressEdit->addItems(getConnection()->getHostHistory());
	mAddressEdit->blockSignals(false);
}

void VideoConnectionWidget::updateDirectLinkArgumentHistory()
{
	mDirectLinkArguments->blockSignals(true);
	mDirectLinkArguments->clear();
	mDirectLinkArguments->addItems(getConnection()->getDirectLinkArgumentHistory());
	mDirectLinkArguments->blockSignals(false);
}

void VideoConnectionWidget::browseLocalServerSlot()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Server"), "~");
	if (fileName.isEmpty())
		return;
	mLocalServerEdit->setText(fileName);
}

void VideoConnectionWidget::showEvent(QShowEvent* event)
{
}

void VideoConnectionWidget::hideEvent(QHideEvent* event)
{
}

void VideoConnectionWidget::launchServer()
{
	this->writeSettings();
	getConnection()->launchServer();
}

void VideoConnectionWidget::toggleLaunchServer()
{
	if ((getServer()) && (getServer()->state() == QProcess::Running))
		getServer()->close();
	else
		this->launchServer();
}

void VideoConnectionWidget::serverProcessStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
	{
		mLaunchServerButton->setText("Close Local Server");
	}
	if (newState == QProcess::NotRunning)
	{
		mLaunchServerButton->setText("Launch Local Server");
	}
	if (newState == QProcess::Starting)
	{
		mLaunchServerButton->setText("Starting...");
	}
}

void VideoConnectionWidget::toggleConnectServer()
{
	if (!this->getConnection()->isConnected())
	{
		this->connectServer();
	}
	else
	{
		this->getConnection()->disconnectServer();
	}
}

void VideoConnectionWidget::writeSettings()
{
	if (this->getConnection()->getUseDirectLink2())
	{
		getConnection()->setLocalServerArguments(mDirectLinkArguments->currentText());
		this->updateDirectLinkArgumentHistory();
	}
	else if (this->getConnection()->getUseLocalServer2())
	{
		getConnection()->setLocalServerExecutable(mLocalServerEdit->text());
		getConnection()->setLocalServerArguments(mLocalServerArguments->text());
	}
	else
	{
		getConnection()->setHost(mAddressEdit->currentText());
		getConnection()->setPort(mPortEdit->text().toInt());
		this->updateHostHistory();
	}

////  std::cout << "guiChanged" << std::endl;
//	getConnection()->setLocalServerCommandLine(mLocalServerEdit->text());
//	getConnection()->setHost(mAddressEdit->currentText());
//	getConnection()->setPort(mPortEdit->text().toInt());
////	getConnection()->setUseLocalServer(mUseLocalServer->isChecked());
////	getConnection()->setUseDirectLink(mUseDirectLink->isChecked());
//	this->updateHostHistory();
}

void VideoConnectionWidget::connectServer()
{
	if (!this->getConnection()->isConnected())
	{
		this->writeSettings();
		getConnection()->launchAndConnectServer();
	}
}

void VideoConnectionWidget::serverStatusChangedSlot()
{
	mSnapshotButton->setEnabled(this->getConnection()->isConnected());

	if (this->getConnection()->isConnected())
	{
//		mSnapshotButton->setEnabled(getRTSource()->isStreaming());
		mConnectButton->setText("Disconnect Server");
	}
	else
	{
//		mSnapshotButton->setEnabled(getRTSource()->isStreaming());
		mConnectButton->setText("Connect Server");
	}

	this->adjustSize();
}

void VideoConnectionWidget::saveSnapshotSlot()
{
	ssc::messageManager()->sendInfo("IGTLinkWidget::saveSnapshotSlot()");

	if(!this->getConnection())
	{
		ssc::messageManager()->sendWarning("No video connection");
		return;
	}
	if(!this->getConnection()->isConnected())
	{
		ssc::messageManager()->sendWarning("Video is not connected");
		return;
	}
	vtkImageDataPtr input = videoService()->getActiveVideoSource()->getVtkImageData();
	if(!input)
	{
		ssc::messageManager()->sendWarning("No Video data");
		return;
	}
	int* extent = input->GetExtent();
	QString filename;
	QString format = ssc::timestampSecondsFormat();
	if(extent[5]- extent[4] > 0) //3D
		filename = "3DRTSnapshot" + QDateTime::currentDateTime().toString(format);
	else //2D
		filename = "2DRTSnapshot" + QDateTime::currentDateTime().toString(format);

	ssc::ImagePtr output = ssc::dataManager()->createImage(input, filename, filename);
	QString folder = patientService()->getPatientData()->getActivePatientFolder() + "/Screenshots/";
	ssc::dataManager()->saveImage(output, folder); //Always sets type as 8 bit, even when 24 bit
}

} //end namespace cx
