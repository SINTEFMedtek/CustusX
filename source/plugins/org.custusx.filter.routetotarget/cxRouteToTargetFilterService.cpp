/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxRouteToTargetFilterService.h"

#include <ctkPluginContext.h>

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

RouteToTargetFilter::RouteToTargetFilter(VisServicesPtr services) :
	FilterImpl(services)
  , mTargetName("")
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

}

void RouteToTargetFilter::createOutputTypes()
{
	StringPropertySelectMeshPtr tempRTTMeshStringAdapter;
	tempRTTMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
	tempRTTMeshStringAdapter->setValueName("Route to target mesh");
	tempRTTMeshStringAdapter->setHelp("Generated route to target mesh (vtk-format).");
	mOutputTypes.push_back(tempRTTMeshStringAdapter);

	StringPropertySelectMeshPtr tempRTTEXTMeshStringAdapter;
	tempRTTEXTMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
	tempRTTEXTMeshStringAdapter->setValueName("Route to target extended mesh");
	tempRTTEXTMeshStringAdapter->setHelp("Generated route to target extended mesh (vtk-format).");
	mOutputTypes.push_back(tempRTTEXTMeshStringAdapter);
}


bool RouteToTargetFilter::execute()
{
    mRouteToTarget.reset(new RouteToTarget());

	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
    if (!mesh)
        return false;

	vtkPolyDataPtr centerline_r = mesh->getTransformedPolyDataCopy(mesh->get_rMd());

	PointMetricPtr targetPoint = boost::dynamic_pointer_cast<StringPropertySelectPointMetric>(mInputTypes[1])->getPointMetric();
    if (!targetPoint)
        return false;

    Vector3D targetCoordinate_r = targetPoint->getCoordinate();

    mRouteToTarget->processCenterline(centerline_r);

    //note: mOutput is in reference space
    mOutput = mRouteToTarget->findRouteToTarget(targetCoordinate_r);

	if(mOutput->GetNumberOfPoints() < 1)
		return false;

    mExtendedRoute = mRouteToTarget->findExtendedRoute(targetCoordinate_r);

	return true;
}

bool RouteToTargetFilter::postProcess()
{

	MeshPtr inputMesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
    if (!inputMesh)
        return false;

	QString uidCenterline = inputMesh->getUid() + RouteToTargetFilter::getNameSuffix() + "%1";
	QString nameCenterline = inputMesh->getName()+RouteToTargetFilter::getNameSuffix() + "%1";
	if (!mTargetName.isEmpty())
	{
		uidCenterline.append("_" + mTargetName);
		nameCenterline.append("_" + mTargetName);
	}

    MeshPtr outputCenterline = patientService()->createSpecificData<Mesh>(uidCenterline, nameCenterline);
    outputCenterline->setVtkPolyData(mOutput);
    patientService()->insertData(outputCenterline);

    QString uidCenterlineExt = outputCenterline->getUid() + RouteToTargetFilter::getNameSuffixExtension();
    QString nameCenterlineExt = outputCenterline->getName()+RouteToTargetFilter::getNameSuffixExtension();
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


	return true;
}

void RouteToTargetFilter::setTargetName(QString name)
{
	mTargetName = name;
}


} // namespace cx

