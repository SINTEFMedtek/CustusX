/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSELECTIONGROUPBOX_H_
#define CXSELECTIONGROUPBOX_H_

#include "cxResourceWidgetsExport.h"

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

class cxResourceWidgets_EXPORT SelectionGroupBox : public QGroupBox
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
