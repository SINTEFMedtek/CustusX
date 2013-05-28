#include "cxtestTestVideoConnectionWidget.h"

#include <QtTest/QtTest>
#include <QComboBox>
#include <QPushButton>
#include "sscVideoSource.h"
#include "sscStringDataAdapterXml.h"
#include "cxVideoService.h"
#include "cxVideoConnectionManager.h"
#include "cxtestSignalListener.h"
#include "cxDataLocations.h"

namespace cxtest
{

TestVideoConnectionWidget::TestVideoConnectionWidget() :
		VideoConnectionWidget(NULL)
{}

bool TestVideoConnectionWidget::startStopServer()
{
	this->show();
	QTest::qWaitForWindowShown(this);

	QString filename = cx::DataLocations::getTestDataPath()+"/testing/TubeSegmentationFramework/Default.mhd";

	QString connectionMethod("Direct Link");
	mConnectionSelector->setValue(connectionMethod);
	QString connectionArguments("--type MHDFile --filename "+filename);
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
