#ifndef CXREGISTRATIONHISTORYWIDGET_H_
#define CXREGISTRATIONHISTORYWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscRegistrationTransform.h"
#include "cxBaseWidget.h"
#include "cxLegacySingletons.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_plugin_registration
 * @{
 */

/**
 * \class RegistrationHistoryWidget
 *
 * Buttons for rolling back to previous registrations.
 * Might be enhanced to a graphical timeline.
 *
 * \date 2010.03.17
 * \\author Christian Askeland, SINTEF
 */
class RegistrationHistoryWidget : public BaseWidget
{
  Q_OBJECT

public:
  RegistrationHistoryWidget(QWidget* parent, bool compact = false);
  virtual ~RegistrationHistoryWidget();
  virtual QString defaultWhatsThis() const;

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
  void createControls(QHBoxLayout* layout);
  QString  debugDump();
  template<class T>
  QAction* createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot);

  typedef std::map<QDateTime,QString> TimeMap;
  TimeMap getRegistrationTimes();
  QDateTime getActiveTime();
  void setActiveTime(QDateTime active);
  TimeMap::iterator findCurrentActiveIter(TimeMap& times);

  QFrame* mGroup;
  std::vector<RegistrationHistoryPtr> mHistories;
  QAction* mRewindAction;
  QAction* mRemoveAction;
  QAction* mForwardAction;
  QAction* mFastForwardAction;

  QLabel* mBehindLabel;
  QLabel* mInFrontLabel;
  QTextEdit* mTextEdit;

  std::vector<RegistrationHistoryPtr> getAllRegistrationHistories();
  std::vector<RegistrationTransform> mergeHistory(const std::vector<RegistrationHistoryPtr>& allHistories);
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXREGISTRATIONHISTORYWIDGET_H_ */
