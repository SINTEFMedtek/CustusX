/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSPACELISTENERIMPL_H
#define CXSPACELISTENERIMPL_H

#include "cxResourceExport.h"

#include "cxSpaceListener.h"

namespace cx
{
typedef boost::shared_ptr<class ActiveToolProxy> ActiveToolProxyPtr;


/**\brief Class that listens to changes in a coordinate system,
 * and emits a signal if that system changes.
 *
 * \ingroup cx_resource_core_utilities
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT SpaceListenerImpl: public SpaceListener
{
Q_OBJECT

public:
	SpaceListenerImpl(TrackingServicePtr trackingService, PatientModelServicePtr dataManager);
//	SpaceListenerImpl(CoordinateSystem space);
	virtual ~SpaceListenerImpl();
	void setSpace(CoordinateSystem space);
	CoordinateSystem getSpace() const;
//signals:
//	void changed();
private slots:
	void reconnect();
private:
	void doConnect();
	void doDisconnect();
	CoordinateSystem mSpace;

	TrackingServicePtr mTrackingService;
	PatientModelServicePtr mDataManager;
	ActiveToolProxyPtr mActiveTool;
};

} // namespace cx


#endif // CXSPACELISTENERIMPL_H
