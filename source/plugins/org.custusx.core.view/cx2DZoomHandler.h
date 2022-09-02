/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CX2DZOOMHANDLER_H
#define CX2DZOOMHANDLER_H

#include "org_custusx_core_view_Export.h"

#include "cxViewGroupData.h"
#include <QMenu>

namespace cx
{
typedef boost::shared_ptr<class Zoom2DHandler> Zoom2DHandlerPtr;

/** 
 *
 * \ingroup org_custusx_core_view
 * \date 2014-02-26
 * \author christiana
 */
class org_custusx_core_view_EXPORT Zoom2DHandler : public QObject
{
	Q_OBJECT
public:
	Zoom2DHandler();

	void addActionsToMenu(QMenu* contextMenu);
	void setGroupData(ViewGroupDataPtr group);
	double getFactor();
	void setFactor(double factor);

private slots:
	void zoom2DActionSlot();
signals:
	void zoomChanged();
private:
	void setConnectivityFromType(QString type);
	QString getConnectivityType();
	void addConnectivityAction(QString type, QString text, QString toolTip, QMenu *contextMenu);
	void set(SyncedValuePtr value);

	SyncedValuePtr mZoom2D;
	ViewGroupDataPtr mGroupData;
};


} // namespace cx



#endif // CX2DZOOMHANDLER_H
