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
#ifndef CXDATASELECTWIDGET_H
#define CXDATASELECTWIDGET_H

#include "cxResourceWidgetsExport.h"

#include "boost/shared_ptr.hpp"
#include "cxBaseWidget.h"
#include <QMouseEvent>


namespace cx
{
typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;
typedef boost::shared_ptr<class SelectDataStringPropertyBase> SelectDataStringPropertyBasePtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;
class EraseDataToolButton;

/** Widget for selecting/showing/removing a Data.
  *
  * The widget wraps a SelectDataStringPropertyBasePtr,
  * and adds utility buttons such as showing the data in the
  * View, and removing it altogether.
  *
  * \ingroup cx_gui
  * \date Nov 25, 2012
  * \author christiana
  */
class cxResourceWidgets_EXPORT DataSelectWidget : public BaseWidget
{
    Q_OBJECT
public:
	DataSelectWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent, SelectDataStringPropertyBasePtr data, QGridLayout* gridLayout = NULL, int row=0);
	~DataSelectWidget();

private slots:
    void toggleShowData();
    void updateDataVisibility();
    void viewGroupChangedSlot();
private:
	void eraseData();
    void setShowIcon();
    ViewGroupDataPtr getActiveViewGroupData();
	SelectDataStringPropertyBasePtr mData;
    ViewGroupDataPtr mCurrentViewGroup;
    QAction* mToggleShowAction;
	EraseDataToolButton* mRemoveButton;
	ViewServicePtr mViewService;
	PatientModelServicePtr mPatientModelService;
};

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

/** Helper button for DataSelectWidget.
  * Emits a signal upon right-click
  *
  * \ingroup cx_gui
  * \date Nov 25, 2012
  * \author christiana
  */
class cxResourceWidgets_EXPORT EraseDataToolButton : public CXSmallToolButton
{
    Q_OBJECT
public:
	EraseDataToolButton(QWidget* parent);
	void reset(); ///< reset button to initial state
signals:
	void eraseData(); ///< indicates that erase has been requested by user.
private:
	void cancelRemovalSlot();
	void setRemoveIcon();
	void requestEraseData();
	void mousePressEvent(QMouseEvent* e);
	QAction* mRemoveAction;
};

} // namespace cx

#endif // CXDATASELECTWIDGET_H
