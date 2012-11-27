// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXDATASELECTWIDGET_H
#define CXDATASELECTWIDGET_H

#include "cxBaseWidget.h"
#include "cxViewWrapper.h"
#include "cxDataInterface.h"
#include <QMouseEvent>


namespace cx
{

/** Widget for selecting/showing/removing a ssc::Data.
  *
  * The widget wraps a SelectDataStringDataAdapterBasePtr,
  * and adds utility buttons such as showing the data in the
  * View, and removing it altogether.
  *
  * \ingroup cxGUI
  * \date Nov 25, 2012
  * \author christiana
  */
class DataSelectWidget : public BaseWidget
{
    Q_OBJECT
public:
    DataSelectWidget(QWidget* parent, SelectDataStringDataAdapterBasePtr data);
    QString defaultWhatsThis() const;

signals:

private slots:
    void requestEraseData();
    void cancelRemovalSlot();
    void toggleShowData();
    void updateDataVisibility();
    void viewGroupChangedSlot();
private:
    void setRemoveIcon();
    void setShowIcon();
    ViewGroupDataPtr getActiveViewGroupData();
    SelectDataStringDataAdapterBasePtr mData;
    ViewGroupDataPtr mCurrentViewGroup;
    QAction* mToggleShowAction;
    QAction* mRemoveAction;
};

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

/** Helper button for DataSelectWidget.
  * Emits a signal upon right-click
  *
  * \ingroup cxGUI
  * \date Nov 25, 2012
  * \author christiana
  */
class EraseDataToolButton : public CXSmallToolButton
{
    Q_OBJECT
public:
    EraseDataToolButton(QWidget* parent) : CXSmallToolButton(parent) {}
signals:
    void rightClick();
private:
    void mousePressEvent(QMouseEvent* e)
    {
        CXSmallToolButton::mousePressEvent(e);
        if (e->button() == Qt::RightButton)
            emit rightClick();
    }
};

} // namespace cx

#endif // CXDATASELECTWIDGET_H
