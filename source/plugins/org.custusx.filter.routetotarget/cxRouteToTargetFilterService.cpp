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


namespace cx
{

RouteToTargetFilter::RouteToTargetFilter(VisServicesPtr services) :
	FilterImpl(services)
{
	mRouteToTarget = RouteToTargetPtr(new RouteToTarget());
}

QString RouteToTargetFilter::getName() const
{
	return "Route to target";
}

QString RouteToTargetFilter::getType() const
{
	return "RouteToTargetFilter";
}

QString RouteToTargetFilter::getHelp() const
{
	return "<html>"
			"<h3>Route to target.</h3>"
			"<p>Calculates the route to a selected target in navigated bronchocopy."
			"The rout starts at the top of trachea and ends at the most adjacent airway centerline"
			"from the target.</p>"
			"</html>";
}


void RouteToTargetFilter::createOptions()
{

}

void RouteToTargetFilter::createInputTypes()
{
	StringPropertySelectMeshPtr centerline;
	centerline = StringPropertySelectMesh::New(mServices->patient());
	centerline->setValueName("Centerline");
	centerline->setHelp("Select centerline");
	mInputTypes.push_back(centerline);

	StringPropertySelectPointMetricPtr targetPoint;
	targetPoint = StringPropertySelectPointMetric::New(mServices->patient());
	targetPoint->setValueName("Target point");
	targetPoint->setHelp("Select point metric input");
	mInputTypes.push_back(targetPoint);

}

void RouteToTargetFilter::createOutputTypes()
{

	StringPropertySelectMeshPtr tempMeshStringAdapter;

	tempMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
	tempMeshStringAdapter->setValueName("Centerline mesh");
	tempMeshStringAdapter->setHelp("Generated route to target mesh (vtk-format).");
	mOutputTypes.push_back(tempMeshStringAdapter);

}


bool RouteToTargetFilter::execute()
{
	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();

	vtkPolyDataPtr centerline = mesh->getVtkPolyData();

	PointMetricPtr targetPoint = boost::dynamic_pointer_cast<StringPropertySelectPointMetric>(mInputTypes[1])->getPointMetric();

	Vector3D targetCoordinate = targetPoint->getCoordinate();

	mRouteToTarget->processCenterline(centerline);
	mOutput = mRouteToTarget->findRouteToTarget(targetCoordinate);

	return true;
}

bool RouteToTargetFilter::postProcess()
{

	MeshPtr inputMesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();

	QString uidCenterline = inputMesh->getUid() + "_rtt_cl%1";
	QString nameCenterline = inputMesh->getName()+"_rtt_cl%1";

	MeshPtr outputCenterline = patientService()->createSpecificData<Mesh>(uidCenterline, nameCenterline);

	outputCenterline->setVtkPolyData(mOutput);

	if (!outputCenterline)
		return false;

	outputCenterline->get_rMd_History()->setParentSpace(inputMesh->getUid());
	outputCenterline->get_rMd_History()->setRegistration(inputMesh->get_rMd());

	patientService()->insertData(outputCenterline);
	mServices->view()->autoShowData(outputCenterline);

	mOutputTypes[0]->setValue(outputCenterline->getUid());


	return true;
}


} // namespace cx

