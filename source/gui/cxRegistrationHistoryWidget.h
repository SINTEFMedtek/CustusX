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
  void showDetailsSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  RegistrationHistoryWidget();
  QString  debugDump();
  template<class T>
  QAction* createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot);
  bool validRegistrationType(QString type) const;

  typedef std::map<QDateTime,QString> TimeMap;
  TimeMap getRegistrationTimes() const;
  QDateTime getActiveTime() const;
  bool setActiveTime(QDateTime active);
  TimeMap::iterator findCurrentActiveIter(TimeMap& times) const;
  QFrame* mGroup;
  std::vector<ssc::RegistrationHistoryPtr> mHistories;
  QAction* mRewindAction;
  QAction* mRemoveAction;
  QAction* mForwardAction;
  QAction* mFastForwardAction;
  QAction* mDetailsAction;
  QAction* mShowAllAction;

  QLabel* mBehindLabel;
  QLabel* mInFrontLabel;
  QTextEdit* mTextEdit;

  TimeMap::iterator findCurrentVisible(TimeMap& times, TimeMap::iterator pos);
  TimeMap::iterator rewindToLastVisible(TimeMap& times, TimeMap::iterator pos);
  TimeMap::iterator forwardToNextVisible(TimeMap& times, TimeMap::iterator pos);
  TimeMap::iterator forwardTowardsNextVisible(TimeMap& times, TimeMap::iterator pos);

  void rewindToPreviousVisible();
  void forwardToNextVisible();
  TimeMap getVisibleRegistrationTimes() const;

  std::vector<ssc::RegistrationHistoryPtr> getAllRegistrationHistories() const;
  std::vector<ssc::RegistrationTransform> mergeHistory(const std::vector<ssc::RegistrationHistoryPtr>& allHistories);
};

}//end namespace cx

#endif /* CXREGISTRATIONHISTORYWIDGET_H_ */






