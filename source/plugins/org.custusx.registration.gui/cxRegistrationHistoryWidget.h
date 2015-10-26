/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

  RegServicesPtr mServices;
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXREGISTRATIONHISTORYWIDGET_H_ */
