#ifndef CXCustomStatusBar_H_
#define CXCustomStatusBar_H_

#include <QStatusBar>
#include <map>
#include "sscMessageManager.h"

class QLabel;
class QPixmap;

namespace cx
{
/**
 * \class CustomStatusBar
 *
 * \brief Statusbar with extended functionality.
 *
 * \date Jan 21, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class CustomStatusBar: public QStatusBar
{
  Q_OBJECT

  typedef ssc::Message Message;

public:
  CustomStatusBar(); ///< connects signals and slots
  ~CustomStatusBar(); ///< empty

private slots:
  void connectToToolSignals(); ///< connect to all available tools
  void disconnectFromToolSignals(); ///< disconnect from all tool
  void receiveToolVisible(bool visible); ///< updates the color label for a tool
  void fpsSlot(int numFps); ///< Show frames per seconds
  void tpsSlot(int numTps); ///< Show transforms per seconds
  void showMessageSlot(Message message); ///< prints the incomming message to the statusbar

private:
  void setToolLabelColor(QLabel* label, bool visible);

  std::vector<QLabel*> mToolLabels; ///< labels indicating the tools visibility
  QLabel* mFpsLabel; ///< Label for showing FPS
  QLabel* mTpsLabel; ///< Label for showing TPS
};
}

#endif /* CXCustomStatusBar_H_ */
