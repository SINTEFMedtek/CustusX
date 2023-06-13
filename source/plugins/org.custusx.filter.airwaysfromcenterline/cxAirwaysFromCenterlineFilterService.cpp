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
#include "cxImage.h"
#include "cxVolumeHelpers.h"

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

BoolPropertyPtr AirwaysFromCenterlineFilter::getSaveOutputVolumeOption(QDomElement root)
{
	return BoolProperty::initialize("Save airways volume", "",
																	"Save volume of airways model after filtering.",
																	 false, root);
}

void AirwaysFromCenterlineFilter::createOptions()
{
	mSaveOutputVolumeOption = this->getSaveOutputVolumeOption(mOptions);
	mOptionsAdapters.push_back(mSaveOutputVolumeOption);

}

void AirwaysFromCenterlineFilter::createInputTypes()
{
	StringPropertySelectMeshPtr centerline;
	centerline = StringPropertySelectMesh::New(mServices->patient());
	centerline->setValueName("Airways centerline");
	centerline->setHelp("Select airways centerline");
	mInputTypes.push_back(centerline);
	StringPropertySelectImagePtr segmentedVolume;
	segmentedVolume = StringPropertySelectImage::New(mServices->patient());
	segmentedVolume->setValueName("Segmented volume (optional)");
	segmentedVolume->setHelp("Select segmented airways volume");
	mInputTypes.push_back(segmentedVolume);

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

	StringPropertySelectImagePtr tempAirwaysModelVolumeStringAdapter;
	tempAirwaysModelVolumeStringAdapter = StringPropertySelectImage::New(mServices->patient());
	tempAirwaysModelVolumeStringAdapter->setValueName("Airways surface model volume");
	tempAirwaysModelVolumeStringAdapter->setHelp("Generated airways surface model volume.");
	mOutputTypes.push_back(tempAirwaysModelVolumeStringAdapter);
}


bool AirwaysFromCenterlineFilter::execute()
{
	mAirwaysFromCenterline.reset(new AirwaysFromCenterline());

	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
	if (!mesh)
		return false;

	vtkPolyDataPtr centerline_r = mesh->getTransformedPolyDataCopy(mesh->get_rMd());

	mAirwaysFromCenterline->processCenterline(centerline_r);

	ImagePtr segmentedVolume = boost::dynamic_pointer_cast<StringPropertySelectImage>(mInputTypes[1])->getImage();

	if(segmentedVolume)
	{
		mAirwaysFromCenterline->setSegmentedVolume(segmentedVolume->getBaseVtkImageData(), segmentedVolume->get_rMd());
		mOutputAirwayMesh = mAirwaysFromCenterline->generateTubes(0, true);
	}
	else
	mOutputAirwayMesh = mAirwaysFromCenterline->generateTubes();
	//note: mOutputAirwayMesh is in reference space

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
		ImagePtr segmentedinputVolume = boost::dynamic_pointer_cast<StringPropertySelectImage>(mInputTypes[1])->getImage();
		if(segmentedinputVolume)
		{
			outputMesh->get_rMd_History()->setParentSpace(segmentedinputVolume->getUid());
			outputMesh->get_rMd_History()->setRegistration(segmentedinputVolume->get_rMd());
		}
		else
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

		if(mSaveOutputVolumeOption && segmentedinputVolume)
		{
			vtkImageDataPtr segmentedOutputVolume = mAirwaysFromCenterline->getFilteredSegmentedVolume();
			if(segmentedOutputVolume)
			{
				QString uidOutputVolume = segmentedinputVolume->getUid() + AirwaysFromCenterlineFilter::getNameSuffix() + "%1";
				QString nameOutputVolume = segmentedinputVolume->getName() + AirwaysFromCenterlineFilter::getNameSuffix() + "%1";
				ImagePtr outputVolume = createDerivedImage(mServices->patient(),
																						 uidOutputVolume, nameOutputVolume,
																						 segmentedOutputVolume, segmentedinputVolume);
				outputVolume->mergevtkSettingsIntosscTransform();
				patientService()->insertData(outputVolume);

				if(mOutputTypes.size() > 2)
						mOutputTypes[2]->setValue(outputVolume->getUid());
			}
		}

    return true;
}

} // namespace cx

