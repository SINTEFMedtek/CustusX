/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDATAREPCONTAINER_H
#define CXDATAREPCONTAINER_H

#include "org_custusx_core_view_Export.h"

#include "cxForwardDeclarations.h"
#include "cxSettings.h"

namespace cx
{
typedef boost::shared_ptr<class SphereMetric> SphereMetricPtr;

/** Creates and manages a list of reps based on input Data objects.
 *
 *
 * \ingroup org_custusx_core_view
 * \date 2014-03-27
 * \author christiana
 */
typedef boost::shared_ptr<class DataRepContainer> DataRepContainerPtr;
/** Creates and manages a list of reps based on input Data objects.
  *
  */
class org_custusx_core_view_EXPORT DataRepContainer
{
public:
	void setSliceProxy(SliceProxyPtr sliceProxy);
	void setView(ViewPtr view);

	void updateSettings();

	void addData(DataPtr data);
	void removeData(QString uid);
private:
	virtual void meshAdded(MeshPtr mesh);
	virtual void pointMetricAdded(PointMetricPtr mesh);
	void updateSettings(RepPtr rep);
	void sphereMetricAdded(SphereMetricPtr mesh);

	SliceProxyPtr mSliceProxy;
	ViewPtr mView;

	typedef std::map<QString, RepPtr> RepMap;
	RepMap mDataReps;
};


} // namespace cx

#endif // CXDATAREPCONTAINER_H
