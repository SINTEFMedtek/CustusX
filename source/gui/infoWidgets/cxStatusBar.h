/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTATUSBAR_H_
#define CXSTATUSBAR_H_

#include "cxGuiExport.h"

#include <QStatusBar>
#include <map>

#include "cxForwardDeclarations.h"
#include "cxReporter.h"
#include "cxLogMessageFilter.h"

class QLabel;
class QPixmap;
class QAction;
class QToolButton;

namespace cx
{

typedef boost::shared_ptr<class ActiveToolProxy> ActiveToolProxyPtr;
typedef boost::shared_ptr<class MessageFilterStatusBar> MessageFilterStatusBarPtr;

/** Filter log messages for display in the StatusBar
 */
class MessageFilterStatusBar : public MessageFilter
{
public:
	static MessageFilterStatusBarPtr create()
	{
		return MessageFilterStatusBarPtr(new MessageFilterStatusBar);
	}
	virtual bool operator()(const Message& msg) const
	{
		LOG_SEVERITY severity = level2severity(msg.getMessageLevel());

		if (severity <= msWARNING)
			return true;
		if (msg.getMessageLevel()==mlVOLATILE)
			return true;
		return false;
	}
	virtual MessageFilterPtr clone()
	{
		return MessageFilterPtr(new MessageFilterStatusBar(*this));
	}
};


/**
 * \class StatusBar
 *
 * \brief Statusbar with extended functionality.
 * \ingroup cx_gui
 *
 * \date Jan 21, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT StatusBar: public QStatusBar
{
  Q_OBJECT

public:
  StatusBar(TrackingServicePtr trackingService, ViewServicePtr viewService, VideoServicePtr videoService); ///< connects signals and slots
  virtual ~StatusBar(); ///< empty

private slots:
  void connectToToolSignals(); ///< connect to all available tools
  void disconnectFromToolSignals(); ///< disconnect from all tool
  void renderingFpsSlot(int numFps); ///< Show rendered frames per seconds
  void grabbingFpsSlot(int numFps); ///< Show grabbed frames per seconds
  void grabberConnectedSlot(bool connected);
  void tpsSlot(int numTps); ///< Show transforms per seconds
  void showMessageSlot(Message message); ///< prints the incomming message to the statusbar
  void updateToolButtons();
  void resetToolManagerConnection();
  void onRecordFullscreenChanged();

private:
  void activateTool(QString uid);
  QString getToolStyle(bool visible, bool initialized, bool active);

  QLabel* mRenderingFpsLabel; ///< Label for showing rendering FPS
  QLabel* mGrabbingInfoLabel; ///< Label for showing info about the grabber
  QLabel* mRecordFullscreenLabel; ///< record screen status
  QLabel* mTpsLabel; ///< Label for showing TPS
//  QLabel* mMessageLevelLabel;
  QToolButton* mMessageLevelLabel;
  ActiveToolProxyPtr mActiveTool;
  MessageListenerPtr mMessageListener;
  TrackingServicePtr mTrackingService;

  struct ToolData
  {
	  boost::shared_ptr<QAction> mAction;
	  boost::shared_ptr<QToolButton> mButton;
	  ToolPtr mTool;
  };
  std::vector<ToolData> mToolData;
};
}

#endif /* CXSTATUSBAR_H_ */
