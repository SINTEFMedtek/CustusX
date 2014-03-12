#ifndef CXSTATUSBAR_H_
#define CXSTATUSBAR_H_

#include <QStatusBar>
#include <map>
#include "sscMessageManager.h"
#include "sscForwardDeclarations.h"
#include "cxLegacySingletons.h"


class QLabel;
class QPixmap;
class QAction;
class QToolButton;

namespace cx
{
/**
 * \class StatusBar
 *
 * \brief Statusbar with extended functionality.
 * \ingroup cx_gui
 *
 * \date Jan 21, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class StatusBar: public QStatusBar
{
  Q_OBJECT

public:
  StatusBar(); ///< connects signals and slots
  virtual ~StatusBar(); ///< empty

private slots:
  void connectToToolSignals(); ///< connect to all available tools
  void disconnectFromToolSignals(); ///< disconnect from all tool
//  void receiveToolVisible(); ///< updates the color label for a tool
//  void receiveToolDominant(); ///< updates the color label for a tool
  void renderingFpsSlot(int numFps); ///< Show rendered frames per seconds
  void grabbingFpsSlot(int numFps); ///< Show grabbed frames per seconds
  void grabberConnectedSlot(bool connected);
  void tpsSlot(int numTps); ///< Show transforms per seconds
  void showMessageSlot(Message message); ///< prints the incomming message to the statusbar
  void updateToolButtons();

private:
  void activateTool(QString uid);
  QString getToolStyle(bool visible, bool initialized, bool dominant);

  QLabel* mRenderingFpsLabel; ///< Label for showing rendering FPS
  QLabel* mGrabbingInfoLabel; ///< Label for showing info about the grabber
  QLabel* mTpsLabel; ///< Label for showing TPS
//  QLabel* mMessageLevelLabel;
  QToolButton* mMessageLevelLabel;

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
