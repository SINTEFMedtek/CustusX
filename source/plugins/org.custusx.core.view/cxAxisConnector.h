/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXAXISCONNECTOR_H
#define CXAXISCONNECTOR_H

#include "org_custusx_core_view_Export.h"

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
 * \ingroup org_custusx_core_view
 * \date 5 Sep 2013, 2013
 * \author christiana
 */
class org_custusx_core_view_EXPORT AxisConnector : public QObject
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
