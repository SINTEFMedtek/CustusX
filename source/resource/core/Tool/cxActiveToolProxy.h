/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXACTIVETOOLPROXY_H_
#define CXACTIVETOOLPROXY_H_

#include "cxResourceExport.h"
#include "cxTool.h"

namespace cx
{

/**
 * \file
 * \addtogroup cx_resource_core_tool
 * @{
 */

typedef boost::shared_ptr<class ActiveToolProxy> ActiveToolProxyPtr;
class ToolManager;

/**
 * \brief Helper class for connecting to the active tool.
 * \ingroup cx_resource_core_tool
 *
 * By listening to this class, you will always listen
 * to the active tool.
 *
 *  \date Sep 9, 2011
 *  \author christiana
 */
class cxResource_EXPORT ActiveToolProxy: public QObject
{
Q_OBJECT

public:
	static ActiveToolProxyPtr New(TrackingServicePtr trackingService)
	{
		return ActiveToolProxyPtr(new ActiveToolProxy(trackingService));
	}

	ToolPtr getTool() { return mTool; }

signals:
	// the original tool change signal
	void activeToolChanged(const QString&);

	// forwarding of active tool signals
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
	void toolVisible(bool visible);
	void tooltipOffset(double offset);
	void toolProbeSector();
	void tps(int);


private slots:
	void activeToolChangedSlot(const QString&);
	void trackingStateChanged();
private:
	ActiveToolProxy(TrackingServicePtr trackingService);
	ToolPtr mTool;
	TrackingServicePtr mTrackingService;
};

/**
 * @}
 */
}

#endif /* CXACTIVETOOLPROXY_H_ */
