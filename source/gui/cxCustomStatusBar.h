#ifndef CXCustomStatusBar_H_
#define CXCustomStatusBar_H_

#include <QStatusBar>
#include <map>

class QLabel;
class QPixmap;

namespace cx
{
class MessageManager;
class ToolManager;

/**
 * \class CustomStatusBar
 *
 * \brief Statusbar with extended functionality.
 *
 * \warning Not working yet and functionality still missing.
 *
 * TODO: get the text and color labels representing the tool to work
 * TODO: divide the statusbar into zones
 * TODO: first zone should display normal status text, including the messages from messagemanager
 *
 * \date Jan 21, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class CustomStatusBar: public QStatusBar
{
  Q_OBJECT

public:
  CustomStatusBar(); ///< connects signals and slots
  ~CustomStatusBar(); ///< empty

protected slots:
  void connectToToolSignals(); ///< connect to all available tools
  void disconnectFromToolSignals(); ///< disconnect from all tool
  void receiveToolVisible(bool visible); ///< updates the color label for a tool

protected:
  MessageManager* mMessageManager; ///< takes messages intended for the user
  ToolManager* mToolManager; ///< interface to the navigation system

  std::map<QLabel*, QLabel*> mToolColorMap; ///< maps text- and colorlabel for a tool
};
}

#endif /* CXCustomStatusBar_H_ */
