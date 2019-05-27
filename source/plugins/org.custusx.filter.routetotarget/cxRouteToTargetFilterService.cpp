/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRouteToTargetFilterService.h"

#include <ctkPluginContext.h>
#include <QDir>

#include "cxAlgorithmHelpers.h"
#include "cxSelectDataStringProperty.h"

#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"

#include "cxRouteToTarget.h"
#include "cxPatientModelService.h"
#include "cxPointMetric.h"
#include "cxVisServices.h"
#include "cxStringPropertySelectPointMetric.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewService.h"
#include "cxLog.h"

#include <vtkPolyData.h>


namespace cx
{

RouteToTargetFilter::RouteToTargetFilter(VisServicesPtr services, bool createRouteInformationFile) :
    FilterImpl(services),
    mTargetName(""),
    mGenerateFileWithRouteInformation(createRouteInformationFile)
{
}

QString RouteToTargetFilter::getName() const
{
	return "Route to target";
}

QString RouteToTargetFilter::getType() const
{
	return "routetotarget_filter";
}

QString RouteToTargetFilter::getHelp() const
{
	return "<html>"
			"<h3>Route to target.</h3>"
			"<p>Calculates the route to a selected target in navigated bronchocopy. "
			"The route starts at the top of trachea and ends at the most adjacent airway centerline"
			"from the target.</p>"
           "</html>";
}

QString RouteToTargetFilter::getNameSuffix()
{
    return "_rtt_cl";
}

QString RouteToTargetFilter::getNameSuffixExtension()
{
    return "_ext";
}


void RouteToTargetFilter::createOptions()
{

}

void RouteToTargetFilter::createInputTypes()
{
	StringPropertySelectMeshPtr centerline;
	centerline = StringPropertySelectMesh::New(mServices->patient());
	centerline->setValueName("Airways centerline");
	centerline->setHelp("Select airways centerline");
	mInputTypes.push_back(centerline);

	StringPropertySelectPointMetricPtr targetPoint;
	targetPoint = StringPropertySelectPointMetric::New(mServices->patient());
	targetPoint->setValueName("Target point");
	targetPoint->setHelp("Select target point metric");
	mInputTypes.push_back(targetPoint);

	StringPropertySelectMeshPtr bloodVesselCenterline;
	bloodVesselCenterline = StringPropertySelectMesh::New(mServices->patient());
	bloodVesselCenterline->setValueName("Blood vessel centerline");
	bloodVesselCenterline->setHelp("Select blood vessel centerline");
	mInputTypes.push_back(bloodVesselCenterline);



}

void RouteToTargetFilter::createOutputTypes()
{
	StringPropertySelectMeshPtr tempRTTMeshStringAdapter;
	tempRTTMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
	tempRTTMeshStringAdapter->setValueName("Route to target (mesh)");
	tempRTTMeshStringAdapter->setHelp("Generated route to target mesh (vtk-format).");
	mOutputTypes.push_back(tempRTTMeshStringAdapter);

	StringPropertySelectMeshPtr tempRTTEXTMeshStringAdapter;
	tempRTTEXTMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
	tempRTTEXTMeshStringAdapter->setValueName("Route to target extended (mesh)");
	tempRTTEXTMeshStringAdapter->setHelp("Generated route to target extended mesh (vtk-format).");
	mOutputTypes.push_back(tempRTTEXTMeshStringAdapter);

	StringPropertySelectMeshPtr tempRTTVesselMeshStringAdapter;
	tempRTTVesselMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
	tempRTTVesselMeshStringAdapter->setValueName("Route to target along blood vessels (mesh)");
	tempRTTVesselMeshStringAdapter->setHelp("Generated route to target along blood vessels mesh (vtk-format).");
	mOutputTypes.push_back(tempRTTVesselMeshStringAdapter);

    StringPropertySelectMeshPtr tempAirwaysModelMeshStringAdapter;
    tempAirwaysModelMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
    tempAirwaysModelMeshStringAdapter->setValueName("Airways along blood vessels surface model (mesh)");
    tempAirwaysModelMeshStringAdapter->setHelp("Generated airways surface model mesh (vtk-format).");
    mOutputTypes.push_back(tempAirwaysModelMeshStringAdapter);
}


bool RouteToTargetFilter::execute()
{
	mRouteToTarget.reset(new RouteToTarget());

	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
	if (!mesh)
		return false;

	PointMetricPtr targetPoint = boost::dynamic_pointer_cast<StringPropertySelectPointMetric>(mInputTypes[1])->getPointMetric();
	if (!targetPoint)
		return false;

	mRouteToTarget->processCenterline(mesh);

    //note: mOutput is in reference space
	mOutput = mRouteToTarget->findRouteToTarget(targetPoint);

	if(mOutput->GetNumberOfPoints() < 1)
		return false;

	mExtendedRoute = mRouteToTarget->findExtendedRoute(targetPoint);

	if (mGenerateFileWithRouteInformation)
		mRouteToTarget->addRouteInformationToFile(mServices);

	return true;
}

bool RouteToTargetFilter::postProcess()
{

	MeshPtr inputMesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
	if (!inputMesh)
		return false;

	PointMetricPtr targetPoint = boost::dynamic_pointer_cast<StringPropertySelectPointMetric>(mInputTypes[1])->getPointMetric();
	if (!targetPoint)
		return false;

	QString uidOutputCenterline = inputMesh->getName() + "_" + targetPoint->getName() + RouteToTargetFilter::getNameSuffix();
	QString nameOutputCenterline = inputMesh->getName() + "_" + targetPoint->getName() + RouteToTargetFilter::getNameSuffix();
	if (!mTargetName.isEmpty())
	{
		uidOutputCenterline.append("_" + mTargetName);
		nameOutputCenterline.append("_" + mTargetName);
	}

	MeshPtr outputCenterline = patientService()->createSpecificData<Mesh>(uidOutputCenterline, nameOutputCenterline);
	outputCenterline->setVtkPolyData(mOutput);
	patientService()->insertData(outputCenterline);

	QString uidCenterlineExt = outputCenterline->getUid() + RouteToTargetFilter::getNameSuffixExtension();
	QString nameCenterlineExt = outputCenterline->getName() + RouteToTargetFilter::getNameSuffixExtension();
	MeshPtr outputCenterlineExt = patientService()->createSpecificData<Mesh>(uidCenterlineExt, nameCenterlineExt);
	outputCenterlineExt->setVtkPolyData(mExtendedRoute);
	outputCenterlineExt->setColor(QColor(0, 0, 255, 255));
	patientService()->insertData(outputCenterlineExt);

	//note: mOutput and outputCenterline is in reference(r) space


	//Meshes are expected to be in data(d) space
	outputCenterline->get_rMd_History()->setParentSpace(inputMesh->getUid());
    outputCenterlineExt->get_rMd_History()->setParentSpace(inputMesh->getUid());

	mServices->view()->autoShowData(outputCenterline);

	if(mOutputTypes.size() > 0)
		mOutputTypes[0]->setValue(outputCenterline->getUid());
	if(mOutputTypes.size() > 1)
		mOutputTypes[1]->setValue(outputCenterlineExt->getUid());

//	//Create Ceetron route-to-target file
//	QString CeetronPath = mServices->patient()->getActivePatientFolder() + "/Images/MarianaRTT/";
//	QDir CeetronDirectory(CeetronPath);
//	if (!CeetronDirectory.exists()) // Creating MarianaRTT folder if it does not exist
//		CeetronDirectory.mkpath(CeetronPath);
//	QString filePathCeetron = CeetronPath + outputCenterline->getUid() + ".txt";
//	mRouteToTarget->makeMarianaCenterlineFile(filePathCeetron);

	return true;
}

void RouteToTargetFilter::setTargetName(QString name)
{
	mTargetName = name;
}


} // namespace cx

