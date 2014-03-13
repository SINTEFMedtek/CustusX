#include "cxStatusBar.h"

#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QAction>
#include <QToolButton>
#include <QPixmap>
#include <QMetaObject>

#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "cxVideoConnectionManager.h"
#include "sscToolManager.h"
#include "cxViewManager.h"
#include "cxVideoService.h"
#include "boost/bind.hpp"
#include <QMetaMethod>
#include "libQtSignalAdapters/Qt2Func.h"
#include "libQtSignalAdapters/ConnectionFactories.h"
#include "cxVideoConnection.h"
#include "sscManualTool.h"
#include "sscTypeConversions.h"
#include "sscDefinitionStrings.h"


namespace cx
{
StatusBar::StatusBar() :
	mRenderingFpsLabel(new QLabel(this)),
	mGrabbingInfoLabel(new QLabel(this)),
	mTpsLabel(new QLabel(this))
//	mMessageLevelLabel(new QToolButton(this))
//	mMessageLevelLabel(new QLabel(this))
{
	connect(messageManager(), SIGNAL(emittedMessage(Message)), this, SLOT(showMessageSlot(Message)));

	connect(toolManager(), SIGNAL(configured()),      this, SLOT(connectToToolSignals()));
	connect(toolManager(), SIGNAL(deconfigured()),    this, SLOT(disconnectFromToolSignals()));
	connect(toolManager(), SIGNAL(trackingStarted()), this, SLOT(updateToolButtons()));
	connect(toolManager(), SIGNAL(trackingStopped()), this, SLOT(updateToolButtons()));

	connect(toolManager(), SIGNAL(tps(int)), this, SLOT(tpsSlot(int)));
	connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(updateToolButtons()));

	connect(viewManager(), SIGNAL(fps(int)), this, SLOT(renderingFpsSlot(int)));

	connect(videoService().get(), SIGNAL(fps(int)), this, SLOT(grabbingFpsSlot(int)));
	connect(videoService()->getVideoConnection().get(), SIGNAL(connected(bool)), this, SLOT(grabberConnectedSlot(bool)));

//	this->addPermanentWidget(mMessageLevelLabel);
	this->addPermanentWidget(mRenderingFpsLabel);
}

StatusBar::~StatusBar()
{
}

void StatusBar::connectToToolSignals()
{
	this->disconnectFromToolSignals(); // avoid duplicates

	this->addPermanentWidget(mTpsLabel);

	ToolManager::ToolMapPtr tools = toolManager()->getTools();
	for (ToolManager::ToolMap::iterator it = tools->begin(); it != tools->end(); ++it)
	{
		ToolPtr tool = it->second;
		if (tool->hasType(Tool::TOOL_MANUAL))
			continue;
		if (tool == toolManager()->getManualTool())
			continue;
		connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolButtons()));

		ToolData current;
		current.mTool = tool;
		current.mAction.reset(new QAction(tool->getName(), NULL));
		current.mAction->setToolTip("Press to set active");

		QtSignalAdapters::connect0<void()>(
			current.mAction.get(),
			SIGNAL(triggered()),
			boost::bind(&StatusBar::activateTool, this, tool->getUid()));

		current.mButton.reset(new QToolButton);
		current.mButton->setDefaultAction(current.mAction.get());
		this->addPermanentWidget(current.mButton.get());
		mToolData.push_back(current);
	}

	this->updateToolButtons();
}

void StatusBar::disconnectFromToolSignals()
{
	this->removeWidget(mTpsLabel);

	for (unsigned i = 0; i < mToolData.size(); ++i)
	{
		ToolData current = mToolData[i];

		disconnect(current.mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolButtons()));
		this->removeWidget(current.mButton.get());
	}
	mToolData.clear();
}


void StatusBar::activateTool(QString uid)
{
	toolManager()->setDominantTool(uid);
}

void StatusBar::updateToolButtons()
{
	ToolPtr dominant = toolManager()->getDominantTool();

	for (unsigned i = 0; i < mToolData.size(); ++i)
	{
		ToolData current = mToolData[i];
		ToolPtr tool = current.mTool;
		QString color = this->getToolStyle(tool->getVisible(), tool->isInitialized(), dominant == tool);
		current.mButton->setStyleSheet(QString("QToolButton { %1; }").arg(color));

		if (!tool->isInitialized())
			current.mAction->setToolTip("Tool is not Initialized");
		else if (dominant == tool)
			current.mAction->setToolTip("Active Tool");
		else if (tool->getVisible())
			current.mAction->setToolTip("Tool not visible/not tracking");
		else
			current.mAction->setToolTip("Tool visible. Press to set as active");
	}
}

QString StatusBar::getToolStyle(bool visible, bool initialized, bool dominant)
{
	if (!initialized)
		return QString("background-color: silver");

	if (visible)
	{
		if (dominant)
			return QString("background-color: lime");
		else
			return QString("background-color: green");
	}

	return QString("background-color: orangered");
}

void StatusBar::renderingFpsSlot(int numFps)
{
	QString fpsString = "FPS: " + QString::number(numFps);
	mRenderingFpsLabel->setText(fpsString);
}

void StatusBar::tpsSlot(int numTps)
{
	QString tpsString = "TPS: " + QString::number(numTps);
	mTpsLabel->setText(tpsString);
}

void StatusBar::grabbingFpsSlot(int numFps)
{
	QString infoString = "VideoConnection-FPS: " + QString::number(numFps);
	mGrabbingInfoLabel->setText(infoString);
}

void StatusBar::grabberConnectedSlot(bool connected)
{
	if (connected)
	{
		this->addPermanentWidget(mGrabbingInfoLabel);
		mGrabbingInfoLabel->show();
	}
	else
		this->removeWidget(mGrabbingInfoLabel);
}

void StatusBar::showMessageSlot(Message message)
{
	QString text = QString("[%1] %4")
			.arg(qstring_cast(message.getMessageLevel()))
			.arg(message.getText());

//	this->showMessage(message.getPrintableMessage(), message.getTimeout());
//	mMessageLevelLabel->setPixmap(QPixmap(":/images/go-home.png"));
//	mMessageLevelLabel->setPixmap(QPixmap(":/icons/screenshot-screen.png"));
//	mMessageLevelLabel->setIcon(QIcon(":/icons/screenshot-screen.png"));
//	mMessageLevelLabel->show();
	this->showMessage(text, message.getTimeout());
}

}//namespace cx
