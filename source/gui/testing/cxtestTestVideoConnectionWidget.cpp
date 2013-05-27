#include "cxtestTestVideoConnectionWidget.h"

#include <QComboBox>
#include <QPushButton>
#include "sscVideoSource.h"
#include "sscStringDataAdapterXml.h"
#include "cxVideoService.h"
#include "cxVideoConnectionManager.h"
#include "cxtestSignalListener.h"

namespace cxtest
{

TestVideoConnection::TestVideoConnection() :
		VideoConnectionWidget(NULL)
{}

bool TestVideoConnection::startStopServer()
{
	this->show();
	QTest::qWaitForWindowShown(this);

	QString connectionMethod("Direct Link");
	mConnectionSelector->setValue(connectionMethod);
	QString connectionArguments("--type MHDFile --filename /home/jbake/jbake/data/helix/helix.mhd");
	mDirectLinkArguments->addItem(connectionArguments);
	mDirectLinkArguments->setCurrentIndex(mDirectLinkArguments->findText(connectionArguments));
	QTest::mouseClick(mConnectButton, Qt::LeftButton); //connect

	bool signalArrived = waitForSignal(this->getConnection().get(), SIGNAL(connected(bool)));
	bool connected = this->getConnection()->isConnected();

	bool activeVideoSourceChanged = waitForSignal(cx::videoService(), SIGNAL(activeVideoSourceChanged()));

	ssc::VideoSourcePtr stream = cx::videoService()->getActiveVideoSource();
	bool newFrame = waitForSignal(stream.get(), SIGNAL(newFrame()));
	bool isStreaming = stream->isStreaming();

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //disconnect
	this->close();
	return isStreaming;
}

} /* namespace cxtest */
