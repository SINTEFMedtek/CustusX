/*
 * cxCustomStatusBar.h
 *
 *  Created on: Jan 21, 2009
 *      Author: Janne Beate Bakeng, SINTEF
 */

#ifndef CXCustomStatusBar_H_
#define CXCustomStatusBar_H_

#include <QStatusBar>
//#include <map.h>

class QLabel;
class QPixmap;

namespace cx
{
class MessageManager;
class ToolManager;

//typedef std::map<std::string, QPixmap*> ToolColorMap;

class CustomStatusBar: public QStatusBar
{
  Q_OBJECT

public:
  CustomStatusBar();
  ~CustomStatusBar();

protected slots:
  void connectToToolSignals();
  void disconnectFromToolSignals();
  void receiveToolVisible(bool visible);

protected:
  MessageManager& mMessageManager;
  ToolManager* mToolManager;

  //ToolColorMap mToolColorMap;
  std::vector<QLabel*> mToolLabelVector;
};
}

#endif /* CXCustomStatusBar_H_ */
