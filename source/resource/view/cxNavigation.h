/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXNAVIGATION_H
#define CXNAVIGATION_H

#include "cxResourceVisualizationExport.h"

#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxViewGroupData.h"

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/** Functions for navigating in the visualization scene(s).
 *
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT Navigation
{
public:
	enum VIEW_TYPE { v2D = 0x01, v3D=0x02, vBOTH=0x03 };
	Navigation(VisServicesPtr services, CameraControlPtr camera3D=CameraControlPtr());
	void centerToTooltip();
	void centerToPosition(Vector3D p_r, QFlags<VIEW_TYPE> viewType=vBOTH);

	void centerToDataInActiveViewGroup(DataViewProperties properties=DataViewProperties::createFull());
	void centerToDataInViewGroup(ViewGroupDataPtr group, DataViewProperties properties=DataViewProperties::createFull());
private:
	void moveManualToolToPosition(Vector3D& p_r);
	Vector3D findDataCenter(const std::vector<DataPtr> &data);
	void centerToData(DataPtr image);
	void centerToData(const std::vector<DataPtr>& images);

	VisServicesPtr mServices;
	CameraControlPtr mCamera3D;
};


} // namespace cx

#endif // CXNAVIGATION_H
