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

#ifndef CXSELECTIONGROUPBOX_H_
#define CXSELECTIONGROUPBOX_H_

#include <QGroupBox>

class QButtonGroup;
class QLayout;

namespace cx
{
/**
 * \ingroup cx_resource_widgets
 *
 * \date Mar 30, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class SelectionGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  SelectionGroupBox(QString title, QStringList selectionList, Qt::Orientation orientation, bool exclusive = false, QWidget *parent=NULL);
  virtual ~SelectionGroupBox();

  virtual QStringList getSelected(); ///< get a list of selected button text
  virtual void setSelected(QStringList selectedlist); ///< set the selected buttons

  void setEnabledButtons(bool value); ///< enables or disables all buttons

signals:
  void selectionChanged(); ///< emitted when the selection of application changed
  void userClicked(); ///< emitted when a user has clicked a button inside the groupbox

private:
  void populate(bool exclusive); ///< Populates the group box
  void filter(QStringList filter); ///< Selects the buttons with text matching the incoming list

//  QVBoxLayout* mLayout;
  QLayout* mLayout;

  QStringList     mSelectionList;
  QButtonGroup*   mButtonGroup;
};
}//namespace cx
#endif /* CXSELECTIONGROUPBOX_H_ */
