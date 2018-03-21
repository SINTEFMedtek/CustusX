/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	ViewGroupDataPtr mCurrentViewGroupData;
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
