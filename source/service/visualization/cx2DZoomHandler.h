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

#ifndef CX2DZOOMHANDLER_H
#define CX2DZOOMHANDLER_H

#include "cxViewGroupData.h"
#include <QMenu>

namespace cx
{
typedef boost::shared_ptr<class Zoom2DHandler> Zoom2DHandlerPtr;

/** 
 *
 * \ingroup cx_service_visualization
 * \date 2014-02-26
 * \author christiana
 */
class Zoom2DHandler : public QObject
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
	void addConnectivityAction(QString type, QString text, QMenu *contextMenu);
	void set(SyncedValuePtr value);

	SyncedValuePtr mZoom2D;
	ViewGroupDataPtr mGroupData;
};


} // namespace cx



#endif // CX2DZOOMHANDLER_H
