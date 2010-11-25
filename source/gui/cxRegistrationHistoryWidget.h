#ifndef CXREGISTRATIONHISTORYWIDGET_H_
#define CXREGISTRATIONHISTORYWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscRegistrationTransform.h"


namespace cx
{

/**
 * \class BrowserWidget
 *
 * Buttons for rolling back to previous registrations.
 * Might be enhanced to a graphical timeline.
 *
 * \date 2010.03.17
 * \author: Christian Askeland, SINTEF
 */
class RegistrationHistoryWidget : public QWidget
{
  Q_OBJECT

public:
  RegistrationHistoryWidget(QWidget* parent);
  virtual ~RegistrationHistoryWidget();

signals:

protected slots:
  void rewindSlot();
  void removeSlot();
  void forwardSlot();
  void fastForwardSlot();
  void updateSlot();
  void reconnectSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  RegistrationHistoryWidget();
  void debugDump();

  typedef std::map<QDateTime,QString> TimeMap;
  TimeMap getRegistrationTimes();
  QDateTime getActiveTime();
  void setActiveTime(QDateTime active);
  TimeMap::iterator findCurrentActiveIter(TimeMap& times);
  QGroupBox* mGroup;
  std::vector<ssc::RegistrationHistoryPtr> mHistories;

  QPushButton* mRewindButton;
  QPushButton* mRemoveButton;
  QPushButton* mForwardButton;
  QPushButton* mFastForwardButton;
  std::vector<ssc::RegistrationHistoryPtr> getAllRegistrationHistories();
  std::vector<ssc::RegistrationTransform> mergeHistory(const std::vector<ssc::RegistrationHistoryPtr>& allHistories);
};

}//end namespace cx

#endif /* CXREGISTRATIONHISTORYWIDGET_H_ */
