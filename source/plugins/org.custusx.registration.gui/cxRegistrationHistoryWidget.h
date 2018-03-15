/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONHISTORYWIDGET_H_
#define CXREGISTRATIONHISTORYWIDGET_H_

#include <vector>
#include <QtWidgets>

#include "cxRegistrationTransform.h"
#include "cxBaseWidget.h"
#include "cxRegServices.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration_gui
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
  RegistrationHistoryWidget(RegServicesPtr servicesPtr, QWidget* parent, bool compact = false);
  virtual ~RegistrationHistoryWidget();
  virtual QString defaultWhatsThis() const;

protected slots:
  void rewindSlot();
  void removeSlot();
  void forwardSlot();
  void fastForwardSlot();
  void reconnectSlot();

  virtual void prePaintEvent();
protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  RegistrationHistoryWidget();
  void createControls(QHBoxLayout* layout);
  QString  debugDump();
  template<class T>
  QAction* createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot);

  typedef std::multimap<QDateTime,QString> TimeMap;
  typedef std::pair<QDateTime,QString> TimeMapPair;
  typedef std::vector<RegistrationHistoryWidget::TimeMap::iterator> TimeMapIterators;
  TimeMap generateRegistrationTimes();
  QDateTime getActiveTime();
  void setActiveTime(QDateTime active);
  RegistrationHistoryWidget::TimeMapIterators findActiveRegistrations(TimeMap& times);

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

  RegServicesPtr mServices;
  RegistrationHistoryWidget::TimeMap::iterator findActiveRegistration(TimeMap &times);
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXREGISTRATIONHISTORYWIDGET_H_ */
