#ifndef CXSTATUSBAR_H_
#define CXSTATUSBAR_H_

#include <QStatusBar>
#include <map>
#include "sscMessageManager.h"
#include "sscForwardDeclarations.h"

class QLabel;
class QPixmap;

namespace cx
{
/**
 * \class StatusBar
 *
 * \brief Statusbar with extended functionality.
 *
 * \date Jan 21, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class StatusBar: public QStatusBar
{
  Q_OBJECT

  typedef ssc::Message Message;

public:
  StatusBar(); ///< connects signals and slots
  ~StatusBar(); ///< empty

private slots:
  void connectToToolSignals(); ///< connect to all available tools
  void disconnectFromToolSignals(); ///< disconnect from all tool
  void receiveToolVisible(); ///< updates the color label for a tool
  void receiveToolDominant(); ///< updates the color label for a tool
  void renderingFpsSlot(int numFps); ///< Show rendered frames per seconds
  void grabbingFpsSlot(int numFps); ///< Show grabbed frames per seconds
  void grabberConnectedSlot(bool connected);
  void tpsSlot(int numTps); ///< Show transforms per seconds
  void showMessageSlot(Message message); ///< prints the incomming message to the statusbar

private:
  void colorTool(ssc::Tool* tool);
  void setToolLabelColor(QLabel* label, bool visible, bool dominant);

  std::vector<QLabel*> mToolLabels; ///< labels indicating the tools visibility
  QLabel* mRenderingFpsLabel; ///< Label for showing rendering FPS
  QLabel* mGrabbingInfoLabel; ///< Label for showing info about the grabber
  QLabel* mTpsLabel; ///< Label for showing TPS
};
}

#endif /* CXSTATUSBAR_H_ */
