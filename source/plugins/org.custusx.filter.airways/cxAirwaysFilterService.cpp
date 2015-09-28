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
// Test
#include "FAST/Algorithms/BinaryThresholding/BinaryThresholding.hpp"

namespace cx {

AirwaysFilter::AirwaysFilter(ctkPluginContext *pluginContext) :
	FilterImpl(VisServices::create(pluginContext))
{
    // Test
    fast::BinaryThresholding::pointer fastObject = fast::BinaryThresholding::New();
    fastObject->setLowerThreshold(1);
    try {
    fastObject->update();
    } catch(fast::Exception &e) {
        std::cout << "FAST: Works!" << std::endl;
    }
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
	        "<p><i>Extracts the centerline and creates a segmentation. </br>GPU-based algorithm written by Erik Smistad (NTNU).</i></p>"
	        "</html>";
}


bool AirwaysFilter::execute()
{
    ImagePtr input = this->getCopiedInputImage();
    	if (!input)
    		return false;

	std::string filename = (patientService()->getActivePatientFolder()+"/"+input->getFilename()).toStdString();

	try {
		// TODO Run algorithm here

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
		reportError("Tube segmentation algorithm threw a unknown exception.");

		return false;
	}
 	return true;
}

bool AirwaysFilter::postProcess()
{

	// TODO populate the output data channels

	return true;
}

void AirwaysFilter::createOptions()
{

}

void AirwaysFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(patientService());
	temp->setValueName("Input");
	temp->setHelp("Select input to run Tube segmentation on.");
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

vtkImageDataPtr AirwaysFilter::convertToVtkImageData(char * data, int size_x, int size_y, int size_z, ImagePtr input)
{
	if (!input)
		return vtkImageDataPtr::New();

	vtkImageDataPtr retval = this->importRawImageData((void*) data, size_x, size_y, size_z, input, VTK_UNSIGNED_CHAR);
	return retval;
}

vtkImageDataPtr AirwaysFilter::convertToVtkImageData(float * data, int size_x, int size_y, int size_z, ImagePtr input)
{
	if (!input)
		return vtkImageDataPtr::New();

	vtkImageDataPtr retval = this->importRawImageData((void*) data, size_x, size_y, size_z, input, VTK_FLOAT);
	return retval;
}

//From vtkType.h (on Ubuntu 12.04)
//#define VTK_VOID            0
//#define VTK_BIT             1
//#define VTK_CHAR            2
//#define VTK_SIGNED_CHAR    15
//#define VTK_UNSIGNED_CHAR   3
//#define VTK_SHORT           4
//#define VTK_UNSIGNED_SHORT  5
//#define VTK_INT             6
//#define VTK_UNSIGNED_INT    7
//#define VTK_LONG            8
//#define VTK_UNSIGNED_LONG   9
//#define VTK_FLOAT          10
//#define VTK_DOUBLE         11
//#define VTK_ID_TYPE        12
vtkImageDataPtr AirwaysFilter::importRawImageData(void * data, int size_x, int size_y, int size_z, ImagePtr input, int type)
{
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarType(type);
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetDataSpacing(input->getBaseVtkImageData()->GetSpacing());
	imageImport->SetImportVoidPointer(data);
//	imageImport->GetOutput()->Update();
	imageImport->Update();
	imageImport->Modified();

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(imageImport->GetOutput());

	return retval;
}

MeshPtr AirwaysFilter::loadVtkFile(QString pathToFile, QString newDatasUid){
	PolyDataMeshReader reader;
	DataPtr data;
	if(reader.canLoad("vtk", pathToFile))
		data = reader.load(newDatasUid, pathToFile);

	MeshPtr retval = boost::dynamic_pointer_cast<Mesh>(data);

    if(!data || !retval)
		reportError("Could not load "+pathToFile);

	return retval;
}

AirwaysFilter::~AirwaysFilter() {
}

} /* namespace cx */

