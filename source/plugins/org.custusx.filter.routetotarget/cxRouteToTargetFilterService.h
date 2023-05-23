/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXROUTETOTARGETFILTER_H
#define CXROUTETOTARGETFILTER_H

#include "org_custusx_filter_routetotarget_Export.h"
#include "cxForwardDeclarations.h"
#include "cxPatientModelService.h"
#include "cxFilterImpl.h"

class ctkPluginContext;

namespace cx
{

/** Filter to calculates the route to a selected target in navigated bronchocopy.
 * The rout starts at the top of trachea and ends at the most adjacent airway centerline
 *  from the target.</p>
 * \ingroup cx_module_algorithm
 * \date Jan 29, 2015
 * \author Erlend Fagertun Hofstad
 */


typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;

class org_custusx_filter_routetotarget_EXPORT RouteToTargetFilter : public FilterImpl
{
	Q_OBJECT
	Q_INTERFACES(cx::Filter)

public:
	RouteToTargetFilter(VisServicesPtr services, bool createRouteInformationFile = false);
	virtual ~RouteToTargetFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	static QString getNameSuffix();
	static QString getNameSuffixExtension();
	static QString getNameSuffixBloodVessel();
	static QString getNameSuffixAirwayModel();
	static QString getNameSuffixAirwayAndVesselRTT();

	std::vector< Eigen::Vector3d > getRoutePositions(bool extendedRoute = true);
	std::vector<BranchPtr> getRouteBranches();
	std::vector< double > getCameraRotation();
	std::vector< int > getGenerationNumbers();
	std::vector< int > getBranchingIndex();
	BranchListPtr getBranchList();
	void setBranchList(BranchListPtr branchList);
	void setReprocessCenterline(bool reprocess);

	virtual bool execute();
	virtual bool postProcess();
	virtual bool postProcessBloodVessels();
    void setSmoothing(bool smoothing = true);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
	RouteToTargetPtr mRouteToTarget;
	vtkPolyDataPtr mOutput;
    vtkPolyDataPtr mExtendedRoute;
    vtkPolyDataPtr 	mBloodVesselRoute;
    vtkPolyDataPtr mAirwaysFromBloodVessel;
    vtkPolyDataPtr mAirwayAndBloodVesselRoute;
    BranchListPtr mBranchListPtr;
    bool mGenerateFileWithRouteInformation;
    bool mSmoothing;
		bool mReprocessCenterline = true;
    BoolPropertyPtr getBloodVesselOption(QDomElement root);
};
typedef boost::shared_ptr<class RouteToTargetFilter> RouteToTargetFilterPtr;


} // namespace cx



#endif // CXROUTETOTARGETFILTER_H
