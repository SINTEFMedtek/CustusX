#include "cxtestTestVideoConnectionWidget.h"

#include <QtTest/QtTest>
#include <QComboBox>
#include <QPushButton>
#include "sscVideoSource.h"
#include "sscStringDataAdapterXml.h"
#include "cxVideoService.h"
#include "cxVideoConnectionManager.h"
#include "cxtestSignalListener.h"
#include "cxtestUtilities.h"
#include "cxDataManager.h"
#include "cxSimulateUSWidget.h"

namespace cxtest
{

TestVideoConnectionWidget::TestVideoConnectionWidget() :
		VideoConnectionWidget(NULL)
{
}

bool TestVideoConnectionWidget::canStream(QString filename, QString streamerType)
{
	this->show();
	QTest::qWaitForWindowShown(this);

	this->setupWidgetToRunStreamer(filename, streamerType);

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //connect

	bool videoConnectionConnected = waitForSignal(this->getVideoConnectionManager().get(), SIGNAL(connected(bool)), 500);
	bool activeVideoSourceChanged = waitForSignal(cx::videoService(), SIGNAL(activeVideoSourceChanged()), 500);
	ssc::VideoSourcePtr stream = cx::videoService()->getActiveVideoSource();
	bool videoSourceReceivedNewFrame = waitForSignal(stream.get(), SIGNAL(newFrame()), 500);
	bool canStream = stream->isStreaming();

	QTest::mouseClick(mConnectButton, Qt::LeftButton); //disconnect

	this->close();

	return canStream;
}

void TestVideoConnectionWidget::setupWidgetToRunStreamer(QString filename, QString streamerType)
{
	ssc::ImagePtr image = Utilities::create3DImage();
	cx::DataManager::getInstance()->setActiveImage(image);
	cx::DataManager::getInstance()->loadData(image);

	QString connectionMethod("Direct Link");
	mConnectionSelector->setValue(connectionMethod);
	QString connectionArguments("--type "+streamerType+" --filename " + filename);
	mSimulationWidget->setImageUidToSimulate(image->getUid());
	mDirectLinkArguments->addItem(connectionArguments);
	mDirectLinkArguments->setCurrentIndex(mDirectLinkArguments->findText(connectionArguments));
}

} /* namespace cxtest */
