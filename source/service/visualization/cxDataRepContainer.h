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

#ifndef CXDATAREPCONTAINER_H
#define CXDATAREPCONTAINER_H

#include "cxForwardDeclarations.h"
#include "cxSettings.h"

namespace cx
{
typedef boost::shared_ptr<class SphereMetric> SphereMetricPtr;

/** Creates and manages a list of reps based on input Data objects.
 *
 *
 * \ingroup cx_service_visualization
 * \date 2014-03-27
 * \author christiana
 */
typedef boost::shared_ptr<class DataRepContainer> DataRepContainerPtr;
/** Creates and manages a list of reps based on input Data objects.
  *
  */
class DataRepContainer
{
public:
	void setSliceProxy(SliceProxyPtr sliceProxy);
	void setView(QPointer<ViewWidget> view);

	void updateSettings();

	void addData(DataPtr data);
	void removeData(QString uid);
private:
	virtual void meshAdded(MeshPtr mesh);
	virtual void pointMetricAdded(PointMetricPtr mesh);
	void updateSettings(RepPtr rep);
	void sphereMetricAdded(SphereMetricPtr mesh);

	SliceProxyPtr mSliceProxy;
	QPointer<ViewWidget> mView;

	typedef std::map<QString, RepPtr> RepMap;
	RepMap mDataReps;
};


} // namespace cx

#endif // CXDATAREPCONTAINER_H
