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
#include "cxLogicManager.h"
#include "cxStringPropertySelectPointMetric.h"


namespace cx
{

RouteToTargetFilter::RouteToTargetFilter(PatientModelServicePtr patientModelService) :
	FilterImpl(patientModelService)
{
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
	//mOptionsAdapters.push_back(this->getSigma(mOptions));
}

void RouteToTargetFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr centerline;
	centerline = StringPropertySelectMesh::New(mPatientModelService);
	centerline->setValueName("Input");
	centerline->setHelp("Select centerline");
	mInputTypes.push_back(centerline);

	StringPropertySelectPointMetricPtr targetPoint;
	targetPoint = StringPropertySelectPointMetric::New(mPatientModelService);
	targetPoint->setValueName("Input");
	targetPoint->setHelp("Select point metric input");
	connect(targetPoint.get(), SIGNAL(dataChanged(QString)), this, SLOT(pointMetricChangedSlot(QString)));
	mInputTypes.push_back(targetPoint);



}

void RouteToTargetFilter::createOutputTypes()
{
//	SelectDataStringPropertyBasePtr temp;

//	temp = StringPropertySelectData::New(mPatientModelService);
//	temp->setValueName("Output");
//	temp->setHelp("Output smoothed image");
//	mOutputTypes.push_back(temp);

	StringPropertySelectMeshPtr tempMeshStringAdapter;

	tempMeshStringAdapter = StringPropertySelectMesh::New(mPatientModelService);
	tempMeshStringAdapter->setValueName("Centerline mesh");
	tempMeshStringAdapter->setHelp("Generated route to target mesh (vtk-format).");
	mOutputTypes.push_back(tempMeshStringAdapter);

}

bool RouteToTargetFilter::execute()
{
	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mCopiedInput[0])->getMesh();
	vtkPolyDataPtr centerline = mesh->getVtkPolyData();

	//PointMetricPtr targetPoint = PointMetricPtr(new PointMetric());
	PointMetricPtr targetPoint = PointMetric::create("point1    ", "", PatientModelServicePtr(), spaceProvider());
	Vector3D testTargetCoordinate(0, 0, 0);
	targetPoint->setCoordinate(testTargetCoordinate);
	Vector3D targetCoordinate = targetPoint->getCoordinate();

	Transform3D rMd = mesh->get_rMd();
	Transform3D rMpr = mPatientModelService->get_rMpr();
	Transform3D prMd = rMpr.inverse()*rMd;


	mRouteToTarget->processCenterline(centerline, prMd);
	mOutput = mRouteToTarget->findRouteToTarget(targetCoordinate);

	return true;
}

bool RouteToTargetFilter::postProcess()
{

	MeshPtr inputMesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mCopiedInput[0])->getMesh();

	QString uidCenterline = inputMesh->getUid() + "_rtt_cl%1";
	QString nameCenterline = inputMesh->getName()+"_rtt_cl%1";

	MeshPtr outputCenterline = patientService()->createSpecificData<Mesh>(uidCenterline, nameCenterline);
	//outputCenterline->intitializeFromParentMesh(inputMesh);
	outputCenterline->setVtkPolyData(mOutput);

	if (!outputCenterline)
		return false;

	outputCenterline->get_rMd_History()->setRegistration(inputMesh->get_rMd());

	patientService()->insertData(outputCenterline);

	mOutputTypes[0]->setValue(outputCenterline->getUid());


	return true;
}


} // namespace cx

