/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxAirwaysFromCenterlineFilterService.h"

#include <ctkPluginContext.h>

#include "cxAlgorithmHelpers.h"
#include "cxSelectDataStringProperty.h"

#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"

#include "cxAirwaysFromCenterline.h"
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

AirwaysFromCenterlineFilter::AirwaysFromCenterlineFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString AirwaysFromCenterlineFilter::getName() const
{
    return "Airways from centerline";
}

QString AirwaysFromCenterlineFilter::getType() const
{
    return "airwaysfromcenterline_filter";
}

QString AirwaysFromCenterlineFilter::getHelp() const
{
	return "<html>"
            "<h3>Airways from centerline.</h3>"
            "<p>Generates artificial surface models of airways around input centerline.</p>"
           "</html>";
}

QString AirwaysFromCenterlineFilter::getNameSuffix()
{
    return "_AirwaysModel";
}

QString AirwaysFromCenterlineFilter::getNameSuffixCenterline()
{
    return "_SmoothedCenterline";
}

void AirwaysFromCenterlineFilter::createOptions()
{

}

void AirwaysFromCenterlineFilter::createInputTypes()
{
	StringPropertySelectMeshPtr centerline;
	centerline = StringPropertySelectMesh::New(mServices->patient());
	centerline->setValueName("Airways centerline");
	centerline->setHelp("Select airways centerline");
	mInputTypes.push_back(centerline);

}

void AirwaysFromCenterlineFilter::createOutputTypes()
{
    StringPropertySelectMeshPtr tempAirwaysModelMeshStringAdapter;
    tempAirwaysModelMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
    tempAirwaysModelMeshStringAdapter->setValueName("Airways surface model mesh");
    tempAirwaysModelMeshStringAdapter->setHelp("Generated airways surface model mesh (vtk-format).");
    mOutputTypes.push_back(tempAirwaysModelMeshStringAdapter);

    StringPropertySelectMeshPtr tempSmoothedCenterlineMeshStringAdapter;
    tempSmoothedCenterlineMeshStringAdapter = StringPropertySelectMesh::New(mServices->patient());
		tempSmoothedCenterlineMeshStringAdapter->setValueName("Smoothed centerline");
    tempSmoothedCenterlineMeshStringAdapter->setHelp("Smoothed centerline (vtk-format).");
    mOutputTypes.push_back(tempSmoothedCenterlineMeshStringAdapter);
}


bool AirwaysFromCenterlineFilter::execute()
{
    mAirwaysFromCenterline.reset(new AirwaysFromCenterline());

	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
    if (!mesh)
        return false;

	vtkPolyDataPtr centerline_r = mesh->getTransformedPolyDataCopy(mesh->get_rMd());

    mAirwaysFromCenterline->processCenterline(centerline_r);

    //note: mOutputAirwayMesh is in reference space
    mOutputAirwayMesh = mAirwaysFromCenterline->generateTubes();

    //if(mOutputAirwayMesh->GetNumberOfPoints() < 1)
    //    return false;

	return true;
}

bool AirwaysFromCenterlineFilter::postProcess()
{

    MeshPtr inputMesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
    if (!inputMesh)
        return false;

		QString uidSurfaceModel = inputMesh->getUid() + AirwaysFromCenterlineFilter::getNameSuffix() + "%1";
		QString nameSurfaceModel = inputMesh->getName() + AirwaysFromCenterlineFilter::getNameSuffix() + "%1";

		MeshPtr outputMesh = patientService()->createSpecificData<Mesh>(uidSurfaceModel, nameSurfaceModel);
		outputMesh->setVtkPolyData(mOutputAirwayMesh);
		outputMesh->setColor(QColor(253, 173, 136, 255));
		patientService()->insertData(outputMesh);

		//Meshes are expected to be in data(d) space
		outputMesh->get_rMd_History()->setParentSpace(inputMesh->getUid());

		mServices->view()->autoShowData(outputMesh);

		QString uidCenterline = inputMesh->getUid() + AirwaysFromCenterlineFilter::getNameSuffixCenterline() + "%1";
		QString nameCenterline = inputMesh->getName() + AirwaysFromCenterlineFilter::getNameSuffixCenterline() + "%1";

		MeshPtr outputCenterline = patientService()->createSpecificData<Mesh>(uidCenterline, nameCenterline);
		outputCenterline->setVtkPolyData(mAirwaysFromCenterline->getVTKPoints());
		outputCenterline->setColor(QColor(0, 200, 0, 255));
		patientService()->insertData(outputCenterline);

		if(mOutputTypes.size() > 0)
				mOutputTypes[0]->setValue(outputMesh->getUid());
		if(mOutputTypes.size() > 1)
				mOutputTypes[1]->setValue(outputCenterline->getUid());

    return true;
}

} // namespace cx

