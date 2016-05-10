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

#include "cxAirwaysFilterService.h"

#include <QTimer>

#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <ctkPluginContext.h>

#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxDataReaderWriter.h"
#include "cxRegistrationTransform.h"
#include "cxDoubleProperty.h"
#include "cxContourFilter.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringProperty.h"
#include "vtkForwardDeclarations.h"
#include "cxPatientModelServiceProxy.h"
#include "cxVisServices.h"
#include "cxUtilHelpers.h"
// Test
#include "FAST/Algorithms/AirwaySegmentation/AirwaySegmentation.hpp"
#include "FAST/Algorithms/CenterlineExtraction/CenterlineExtraction.hpp"
#include "FAST/Importers/ImageFileImporter.hpp"
#include "FAST/Exporters/VTKImageExporter.hpp"
#include "FAST/Exporters/VTKLineSetExporter.hpp"
#include "FAST/Data/Segmentation.hpp"
#include "FAST/SceneGraph.hpp"

namespace cx {

AirwaysFilter::AirwaysFilter(VisServicesPtr services) :
	FilterImpl(services)
{

}

QString AirwaysFilter::getName() const
{
	return "Airway Segmentation Filter";
}

QString AirwaysFilter::getType() const
{
	return "AirwaysFilter";
}

QString AirwaysFilter::getHelp() const
{
	return "<html>"
	        "<h3>Airway Segmentation.</h3>"
	        "<p><i>Extracts segmentation and centerline from a CT volume. If method fails, try to crop volume. </br>Algorithm written by Erik Smistad.</i></p>"
	        "</html>";
}

bool AirwaysFilter::execute()
{
    CX_LOG_INFO() << "EXECUTING AIRWAYS FILTER";
    ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;
	mInputImage = input;
	std::string filename = (patientService()->getActivePatientFolder()+"/"+mInputImage->getFilename()).toStdString();

	try {
        QString kernelDir = cx::DataLocations::findConfigFolder("/FAST", FAST_SOURCE_DIR);
        fast::DeviceManager::getInstance().setKernelRootPath(kernelDir.toStdString());
		// Import image data from disk
		fast::ImageFileImporter::pointer importer = fast::ImageFileImporter::New();
		importer->setFilename(filename);

	    // Need to know the data type
	    importer->update();
	    fast::Image::pointer image = importer->getOutputData<fast::Image>();

	    // Do segmentation
        fast::AirwaySegmentation::pointer segmentation = fast::AirwaySegmentation::New();
	    segmentation->setInputConnection(importer->getOutputPort());

	    // Convert fast segmentation data to VTK data which CX can use
        vtkSmartPointer<fast::VTKImageExporter> vtkExporter = fast::VTKImageExporter::New();
	    vtkExporter->setInputConnection(segmentation->getOutputPort());
	    vtkExporter->Update();
	    mSegmentationOutput = vtkExporter->GetOutput();
        CX_LOG_SUCCESS() << "FINISHED AIRWAY SEGMENTATION";

	    // Get output segmentation data
	    fast::Segmentation::pointer segmentationData = segmentation->getOutputData<fast::Segmentation>(0);

	    // Get the transformation of the segmentation
		Eigen::Affine3f T = fast::SceneGraph::getEigenAffineTransformationFromData(segmentationData);
	    mTransformation.matrix() = T.matrix().cast<double>(); // cast to double

        // Extract centerline
        fast::CenterlineExtraction::pointer centerline = fast::CenterlineExtraction::New();
        centerline->setInputConnection(segmentation->getOutputPort());

        // Get centerline
	    vtkSmartPointer<fast::VTKLineSetExporter> vtkCenterlineExporter = fast::VTKLineSetExporter::New();
	    vtkCenterlineExporter->setInputConnection(centerline->getOutputPort());
	    mCenterlineOutput = vtkCenterlineExporter->GetOutput();
	    vtkCenterlineExporter->Update();

	} catch(fast::Exception& e) {
		std::string error = e.what();
		reportError("fast::Exception: "+qstring_cast(error));

		return false;
	} catch(cl::Error& e) {
		reportError("cl::Error:"+qstring_cast(e.what()));

		return false;
	} catch (std::exception& e){
		reportError("std::exception:"+qstring_cast(e.what()));

		return false;
	} catch (...){
		reportError("Airway segmentation algorithm threw a unknown exception.");

		return false;
	}
 	return true;
}

bool AirwaysFilter::postProcess()
{
	if(!mSegmentationOutput)
		return false;

	std::cout << "POST PROCESS" << std::endl;

	// Make contour of segmented volume
	double threshold = 1; /// because the segmented image is 0..1
	vtkPolyDataPtr rawContour = ContourFilter::execute(
			mSegmentationOutput,
			threshold,
			false, // reduce resolution
			true, // smoothing
			true, // keep topology
			0 // target decimation
	);
	//outputSegmentation->get_rMd_History()->setRegistration(rMd_i);
	//patientService()->insertData(outputSegmentation);

	// Add contour internally to cx
	MeshPtr contour = ContourFilter::postProcess(
			patientService(),
			rawContour,
			mInputImage,
			QColor("green")
	);
	contour->get_rMd_History()->setRegistration(mTransformation);

	// Set output
	mOutputTypes[1]->setValue(contour->getUid());

	// TODO get centerline somehow
	QString uid = mInputImage->getUid() + "_centerline%1";
	QString name = mInputImage->getName() + " centerline%1";
	MeshPtr centerline = patientService()->createSpecificData<Mesh>(uid, name);
	centerline->setVtkPolyData(mCenterlineOutput);
	centerline->get_rMd_History()->setParentSpace(mInputImage->getUid());
	centerline->get_rMd_History()->setRegistration(mTransformation);
	patientService()->insertData(centerline);
	mOutputTypes[0]->setValue(centerline->getUid());

	return true;
}

void AirwaysFilter::createOptions()
{
	//mOptionsAdapters.push_back(getNoiseLevelOption(mOptions));
}

void AirwaysFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(patientService());
	temp->setValueName("Input");
	temp->setHelp("Select input to run airway segmentation on.");
	mInputTypes.push_back(temp);
}

void AirwaysFilter::createOutputTypes()
{
	StringPropertySelectMeshPtr tempMeshStringAdapter;

	//0
	tempMeshStringAdapter = StringPropertySelectMesh::New(patientService());
	tempMeshStringAdapter->setValueName("Centerline");
	tempMeshStringAdapter->setHelp("Generated centerline mesh (vtk-format).");
	mOutputTypes.push_back(tempMeshStringAdapter);

	//1
	tempMeshStringAdapter = StringPropertySelectMesh::New(patientService());
	tempMeshStringAdapter->setValueName("Segmentation");
	tempMeshStringAdapter->setHelp("Generated surface of the segmented volume.");
	mOutputTypes.push_back(tempMeshStringAdapter);

}

/*
DoublePropertyPtr AirwaysFilter::getNoiseLevelOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Noise level",
			"", "Select the amount of noise present in the image", 0.5,
			DoubleRange(0.0, 2, 0.5), 1, root);
	retval->setGuiRepresentation(DoubleProperty::grSLIDER);
	return retval;
}
*/

AirwaysFilter::~AirwaysFilter() {
}

} /* namespace cx */

