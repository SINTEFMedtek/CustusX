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
#ifndef CXAXISCONNECTOR_H
#define CXAXISCONNECTOR_H

#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxCoordinateSystemHelpers.h"

namespace cx
{
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class SpaceListener> SpaceListenerPtr;

/** 
 * Ad-hoc class for connecting axis reps to coord spaces.
 * Used by ViewWrapper3D.
 *
 * \ingroup cx_service_visualization
 * \date 5 Sep 2013, 2013
 * \author christiana
 */
class AxisConnector : public QObject
{
	Q_OBJECT
	public:
		AxisConnector(CoordinateSystem space, SpaceProviderPtr spaceProvider);
		void connectTo(ToolPtr tool);
		void mergeWith(SpaceListenerPtr base);
		AxesRepPtr mRep; ///< axis
		SpaceListenerPtr mListener;
	private slots:
		void changedSlot();
	private:
		SpaceListenerPtr mBase;
		ToolPtr mTool;
		SpaceProviderPtr mSpaceProvider;
};
typedef boost::shared_ptr<class AxisConnector> AxisConnectorPtr;


} // namespace cx




#endif // CXAXISCONNECTOR_H
