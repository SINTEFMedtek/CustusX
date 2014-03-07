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
#ifndef CXSPACELISTENERIMPL_H
#define CXSPACELISTENERIMPL_H

#include "cxSpaceListener.h"

namespace cx
{
class ToolManager;
class DataManager;

/**\brief Class that listens to changes in a coordinate system,
 * and emits a signal if that system changes.
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
 */
class SpaceListenerImpl: public SpaceListener
{
Q_OBJECT

public:
	SpaceListenerImpl(TrackingServicePtr toolManager, DataManager* dataManager);
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

	TrackingServicePtr mToolManager;
	DataManager* mDataManager;
};

} // namespace cx


#endif // CXSPACELISTENERIMPL_H
