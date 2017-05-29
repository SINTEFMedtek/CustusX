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

#include "cxTubeSegmentationFilterService.h"

#include "tube-segmentation.hpp"
#include "tsf-config.h"
#include "Exceptions.hpp"

#include <QTimer>

#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <ctkPluginContext.h>

#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxDoubleProperty.h"
#include "cxContourFilter.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringProperty.h"
#include "cxTSFPresets.h"
#include "vtkForwardDeclarations.h"
#include "cxPatientModelServiceProxy.h"
#include "cxVisServices.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxFileManagerService.h"

namespace cx {

TubeSegmentationFilter::TubeSegmentationFilter(ctkPluginContext *pluginContext) :
	FilterImpl(VisServices::create(pluginContext)), mOutput(NULL)
{
	connect(patientService().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));
	mPresets = this->populatePresets();
}

QString TubeSegmentationFilter::getName() const
{
	return "Tube-Segmentation Filter";
}

QString TubeSegmentationFilter::getType() const
{
	return "tube_segmentation_filter";
}

QString TubeSegmentationFilter::getHelp() const
{
	return "<html>"
	        "<h3>Tube-Segmentation.</h3>"
	        "<p><i>Extracts the centerline and creates a segmentation. </br>GPU-based algorithm written by Erik Smistad (NTNU).</i></p>"
	        "</html>";
}

bool TubeSegmentationFilter::hasPresets()
{
	return true;
}

PresetsPtr TubeSegmentationFilter::getPresets()
{
	return mPresets;
}

QDomElement TubeSegmentationFilter::generatePresetFromCurrentlySetOptions(QString name)
{
	std::vector<PropertyPtr> newPresetOptions = this->getNotDefaultOptions();

	std::map<QString, QString> newPresetMap;
	std::vector<PropertyPtr>::iterator it;
	for(it = newPresetOptions.begin(); it != newPresetOptions.end(); ++it){
		PropertyPtr option = *it;
		QString valuename = option->getDisplayName();
		QString value;
		StringPropertyPtr stringOption = boost::dynamic_pointer_cast<StringProperty>(option);
		BoolPropertyPtr boolOption = boost::dynamic_pointer_cast<BoolProperty>(option);
		DoublePropertyPtr doubleOption = boost::dynamic_pointer_cast<DoubleProperty>(option);
		if(stringOption)
			value = stringOption->getValue();
		else if(boolOption)
			value = boolOption->getValue() ? "true" : "false";
		else if(doubleOption)
			value = QString::number(doubleOption->getValue());
		else
			reportError("Could not determine what kind of option to get the value for.");
		newPresetMap[valuename] = value;
	}
	StringPropertyBasePtr centerlineMethod = this->getStringOption("centerline-method");
	newPresetMap[centerlineMethod->getDisplayName()] = centerlineMethod->getValue();

	//create xml
	QDomElement retval = TSFPresets::createPresetElement(name, newPresetMap);

	return retval;
}

void TubeSegmentationFilter::requestSetPresetSlot(QString name)
{
	QString centerLineMethod = "gpu";
	if((name == "<Default preset>") || (name == "none") || (name == "default") || (name == "Default"))
		mParameterFile = "none";
	else
	{
		mParameterFile = name;
	}
	this->loadNewParametersSlot();

	StringPropertyPtr centerlineMethodOption = this->getStringOption("centerline-method");
	centerlineMethodOption->setValue(centerLineMethod);
}

bool TubeSegmentationFilter::execute()
{
    ImagePtr input = this->getCopiedInputImage();
    	if (!input)
    		return false;

	mParameters = this->getParametersFromOptions();
	std::string filename = (patientService()->getActivePatientFolder()+"/"+input->getFilename()).toStdString();

	try {
		std::cout << "=================TSF START====================" << std::endl;
		std::cout << "Input: " <<  input->getName().toStdString() << std::endl;
		std::cout << "Preset: " <<  getParamStr(mParameters, "parameters") << std::endl;
		std::cout << "Centerline-method: " <<  getParamStr(mParameters, "centerline-method") << std::endl;
		QString kernelDir = cx::DataLocations::findConfigFolder("/tsf", KERNELS_DIR);
		QString oulDir = cx::DataLocations::findConfigFolder("/tsf", OUL_DIR);
		std::cout << "Kernel paths: " << kernelDir.toStdString();
		std::cout << " and " << oulDir.toStdString() << std::endl;
		std::cout << "--------------" << std::endl;
		mOutput = run(filename, mParameters, kernelDir.toStdString(), oulDir.toStdString());
		std::cout << "=================TSF END====================" << std::endl;
	} catch(SIPL::SIPLException& e) {
		std::string error = e.what();
		reportError("SIPL::SIPLException: "+qstring_cast(error));

		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	} catch(cl::Error& e) {
		reportError("cl::Error:"+qstring_cast(e.what()));

		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	} catch (std::exception& e){
		reportError("std::exception:"+qstring_cast(e.what()));

		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	} catch (...){
		reportError("Tube segmentation algorithm threw a unknown exception.");

		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	}
 	return true;
}

bool TubeSegmentationFilter::postProcess()
{
	if(!mOutput)
	{
		reportWarning("No output generated from the tube segmentation filter.");
		return false;
	}

	ImagePtr inputImage = this->getCopiedInputImage();
	if (!inputImage)
		return false;

	double inputImageSpacing_x, inputImageSpacing_y, inputImageSpacing_z;
	inputImage->getBaseVtkImageData()->GetSpacing(inputImageSpacing_x, inputImageSpacing_y, inputImageSpacing_z);

	//compensate for cropping
	SIPL::int3 voxelsCropped = mOutput->getShiftVector();
	Vector3D croppingVectorInInpuImageSpace((inputImageSpacing_x*voxelsCropped.x), (inputImageSpacing_y*voxelsCropped.y), (inputImageSpacing_z*voxelsCropped.z));
	Transform3D d_iMd_c = createTransformTranslate(croppingVectorInInpuImageSpace); //dc = data cropped, di = data input
	Transform3D rMd_i = inputImage->get_rMd(); //transform from the volumes coordinate system to our reference coordinate system
	Transform3D rMd_c = rMd_i * d_iMd_c; //translation due to cropping accounted for

	// Centerline (volume)
	//======================================================
	if(mOutput->hasCenterlineVoxels())
	{
		QString uidCenterline = inputImage->getUid() + "_tsf_cl_vol%1";
		QString nameCenterline = inputImage->getName()+"_tsf_cl_vol%1";
		SIPL::int3* size = mOutput->getSize();
		vtkImageDataPtr rawCenterlineResult = this->convertToVtkImageData(mOutput->getCenterlineVoxels(), size->x, size->y, size->z, inputImage);
		if(!rawCenterlineResult)
			return false;

//		ImagePtr outputCenterline = patientService()->createDerivedImage(rawCenterlineResult ,uidCenterline, nameCenterline, inputImage);
		ImagePtr outputCenterline = patientService()->createSpecificData<Image>(uidCenterline, nameCenterline);
		outputCenterline->intitializeFromParentImage(inputImage);
		outputCenterline->setVtkImageData(rawCenterlineResult);

		if (!outputCenterline)
			return false;

		outputCenterline->get_rMd_History()->setRegistration(rMd_c);

		patientService()->insertData(outputCenterline);
//		dataManager()->loadData(outputCenterline);
//		dataManager()->saveImage(outputCenterline, patientService()->getPatientData()->getActivePatientFolder());

		mOutputTypes[0]->setValue(outputCenterline->getUid());
	}

	// Centerline (vtk)
	//======================================================
	boost::unordered_map<std::string, StringParameter>::iterator it = mParameters.strings.find("centerline-vtk-file");
	if(it != mParameters.strings.end() && (it->second.get() != "off"))
	{
		QString tsfVtkFilename = qstring_cast(it->second.get());
		QString uidVtkCenterline = inputImage->getUid() + "_tsf_cl%1";
		QString nameVtkCenterline = inputImage->getName()+"_tsf_cl%1";

		//load vtk file created by tsf into CustusX
		MeshPtr tsfMesh = this->loadVtkFile(tsfVtkFilename, uidVtkCenterline);

		Vector3D inpuImageSpacing(inputImageSpacing_x, inputImageSpacing_y, inputImageSpacing_z);
		Transform3D dMv = createTransformScale(inpuImageSpacing);  // transformation from voxelspace to imagespace

		Transform3D rMv = rMd_c*dMv;

		vtkPolyDataPtr poly = tsfMesh->getTransformedPolyData(rMv);

		//create, load and save mesh
		MeshPtr cxMesh = patientService()->createSpecificData<Mesh>(uidVtkCenterline, nameVtkCenterline);
		cxMesh->setVtkPolyData(poly);
		cxMesh->get_rMd_History()->setParentSpace(inputImage->getUid());
		patientService()->insertData(cxMesh);
//		dataManager()->loadData(cxMesh);
//		dataManager()->saveMesh(cxMesh, patientService()->getPatientData()->getActivePatientFolder());
		QString uid = cxMesh->getUid();

		mOutputTypes[1]->setValue(uid);
	}

	// Segmentation
	//======================================================
	if(mOutput->hasSegmentation())
	{
		//get segmented volume
		SIPL::int3* size = mOutput->getSize();
		vtkImageDataPtr rawSegmentation = this->convertToVtkImageData(mOutput->getSegmentation(), size->x, size->y, size->z, inputImage);

		//make contour of segmented volume
		double threshold = 1;/// because the segmented image is 0..1
		vtkPolyDataPtr rawContour = ContourFilter::execute(rawSegmentation, threshold);

		//add segmentation internally to cx
		QString uidSegmentation = inputImage->getUid() + "_tsf_seg%1";
		QString nameSegmentation = inputImage->getName()+"_tsf_seg%1";

		ImagePtr outputSegmentation = patientService()->createSpecificData<Image>(uidSegmentation, nameSegmentation);
		outputSegmentation->intitializeFromParentImage(inputImage);
		outputSegmentation->setVtkImageData(rawSegmentation);
//		ImagePtr outputSegmentation = dataManager()->createDerivedImage(rawSegmentation,uidSegmentation, nameSegmentation, inputImage);
		if (!outputSegmentation)
			return false;

		outputSegmentation->get_rMd_History()->setRegistration(rMd_c);
		patientService()->insertData(outputSegmentation);
//		dataManager()->loadData(outputSegmentation);
//		dataManager()->saveImage(outputSegmentation, patientService()->getPatientData()->getActivePatientFolder());

		//add contour internally to cx
		MeshPtr contour = ContourFilter::postProcess(patientService(), rawContour, inputImage, QColor("blue"));
		contour->get_rMd_History()->setRegistration(rMd_c);

		//set output
		mOutputTypes[2]->setValue(outputSegmentation->getUid());
		mOutputTypes[3]->setValue(contour->getUid());
	}

	// TDF
	//======================================================
	if(mOutput->hasTDF())
	{
		QString uidTDF = inputImage->getUid() + "_tsf_tdf%1";
		QString nameTDF = inputImage->getName()+"_tsf_tdf%1";
		SIPL::int3* size = mOutput->getSize();

		// convert volume
		vtkImageDataPtr convertedImageData = this->convertToVtkImageData(mOutput->getTDF(), size->x, size->y, size->z, inputImage);

		//scale volume
		if (!convertedImageData)
			return false;

		vtkImageShiftScalePtr cast = vtkImageShiftScalePtr::New();
		cast->SetInputData(convertedImageData);
		cast->ClampOverflowOn();

		//tdfs voxels contains values [0.0,1.0]
		//scaling these to be able to visualize them in CustusX
		int scale = 255; //unsigned char ranges from 0 to 255
		cast->SetScale(scale);
		cast->SetOutputScalarType(VTK_UNSIGNED_CHAR);
		cast->Update();
		convertedImageData = cast->GetOutput();

//		ImagePtr outputTDF = dataManager()->createDerivedImage(convertedImageData,uidTDF, nameTDF, inputImage);
		ImagePtr outputTDF = patientService()->createSpecificData<Image>(uidTDF, nameTDF);
		outputTDF->intitializeFromParentImage(inputImage);
		outputTDF->setVtkImageData(convertedImageData);


		if (!outputTDF)
			return false;

		rMd_i = rMd_i * d_iMd_c; //translation due to cropping accounted for
		outputTDF->get_rMd_History()->setRegistration(rMd_i);
		patientService()->insertData(outputTDF);
//		dataManager()->loadData(outputTDF);
//		dataManager()->saveImage(outputTDF, patientService()->getPatientData()->getActivePatientFolder());

		mOutputTypes[4]->setValue(outputTDF->getUid());
	}

	//clean up
	if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
	}

	return true;
}

void TubeSegmentationFilter::createOptions()
{
	this->createDefaultOptions(mOptions);

	std::vector<StringPropertyPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
		mOptionsAdapters.push_back(*stringIt);

	std::vector<BoolPropertyPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
		mOptionsAdapters.push_back(*boolIt);

	std::vector<DoublePropertyPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
		mOptionsAdapters.push_back(*doubleIt);
}

void TubeSegmentationFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(patientService());
	temp->setValueName("Input");
	temp->setHelp("Select input to run Tube segmentation on.");
	mInputTypes.push_back(temp);

	connect(temp.get(), SIGNAL(changed()), this, SLOT(inputChangedSlot()));
}

void TubeSegmentationFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr tempDataStringAdapter;
	StringPropertySelectMeshPtr tempMeshStringAdapter;

	//0
	tempDataStringAdapter = StringPropertySelectData::New(patientService());
	tempDataStringAdapter->setValueName("Centerline volume");
	tempDataStringAdapter->setHelp("Generated centerline volume.");
	mOutputTypes.push_back(tempDataStringAdapter);

	//1
	tempMeshStringAdapter = StringPropertySelectMesh::New(patientService());
	tempMeshStringAdapter->setValueName("Centerline mesh");
	tempMeshStringAdapter->setHelp("Generated centerline mesh (vtk-format).");
	mOutputTypes.push_back(tempMeshStringAdapter);

	//2
	tempDataStringAdapter = StringPropertySelectData::New(patientService());
	tempDataStringAdapter->setValueName("Segmented centerline");
	tempDataStringAdapter->setHelp("Grown segmentation from the centerline.");
	mOutputTypes.push_back(tempDataStringAdapter);

	//3
	tempMeshStringAdapter = StringPropertySelectMesh::New(patientService());
	tempMeshStringAdapter->setValueName("Segmented centerlines surface");
	tempMeshStringAdapter->setHelp("Generated surface of the segmented volume.");
	mOutputTypes.push_back(tempMeshStringAdapter);

	//4
	tempDataStringAdapter = StringPropertySelectData::New(patientService());
	tempDataStringAdapter->setValueName("TDF volume");
	tempDataStringAdapter->setHelp("Volume showing the probability of a voxel being part of a tubular structure.");
	mOutputTypes.push_back(tempDataStringAdapter);
}

void TubeSegmentationFilter::patientChangedSlot()
{
	QString activePatientFolder = patientService()->getActivePatientFolder()+"/Images/";

	StringPropertyPtr option = this->getStringOption("storage-dir");
	if(option)
		option->setValue(activePatientFolder);
}

void TubeSegmentationFilter::inputChangedSlot()
{
	QString activePatientFolder = patientService()->getActivePatientFolder()+"/Images/";
	QString inputsValue = mInputTypes.front()->getValue();

	StringPropertyPtr option = this->getStringOption("centerline-vtk-file");
	if(option)
		option->setValue(activePatientFolder+inputsValue+QDateTime::currentDateTime().toString(timestampSecondsFormat())+"_tsf_vtk.vtk");
}

void TubeSegmentationFilter::loadNewParametersSlot()
{
	paramList list = this->getDefaultParameters();

	if(mParameterFile != "none")
	{
		try
		{
			setParameter(list, "parameters", mParameterFile.toStdString());
			loadParameterPreset(list, cx::DataLocations::findConfigFolder("/tsf", QString(KERNELS_DIR)).toStdString()+"/parameters");
		} catch (SIPL::SIPLException& e)
		{
			reportWarning("Error when loading a parameter file. Preset is corrupt. "+QString(e.what()));
			return;
		}
	}

	blockSignals(true);
		this->setOptionsSlot(list);
		this->resetOptionsAdvancedSlot();
		//set parameters found in the parameter file as not advanced
		std::vector<std::string> notDefaultOptions = this->getNotDefault(list);
		std::vector<std::string>::iterator it;
		for(it = notDefaultOptions.begin() ;it != notDefaultOptions.end(); ++it)
		{
			this->getOption(qstring_cast(*it))->setAdvanced(false);
		}
	blockSignals(false);

	emit changed();
}

void TubeSegmentationFilter::resetOptionsAdvancedSlot()
{
	std::vector<StringPropertyPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		StringPropertyPtr adapter = *stringIt;
		if(adapter->getDisplayName() == "parameters")
		{
			adapter->setAdvanced(false);
		}
		else
			adapter->setAdvanced(true);
	}

	std::vector<BoolPropertyPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
	{
		BoolPropertyPtr adapter = *boolIt;
		adapter->setAdvanced(true);
	}

	std::vector<DoublePropertyPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
	{
		DoublePropertyPtr adapter = *doubleIt;
		adapter->setAdvanced(true);
	}
}

void TubeSegmentationFilter::resetOptionsSlot()
{
	paramList defaultParameters = this->getDefaultParameters();
	this->resetOptionsAdvancedSlot();
	this->setOptionsSlot(defaultParameters);
}

void TubeSegmentationFilter::setOptionsSlot(paramList& list)
{
	this->setParamtersToOptions(list);
	this->patientChangedSlot();
	this->inputChangedSlot();
	emit changed();
}

vtkImageDataPtr TubeSegmentationFilter::convertToVtkImageData(char * data, int size_x, int size_y, int size_z, ImagePtr input)
{
	if (!input)
		return vtkImageDataPtr::New();

	vtkImageDataPtr retval = this->importRawImageData((void*) data, size_x, size_y, size_z, input, VTK_UNSIGNED_CHAR);
	return retval;
}

vtkImageDataPtr TubeSegmentationFilter::convertToVtkImageData(float * data, int size_x, int size_y, int size_z, ImagePtr input)
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
vtkImageDataPtr TubeSegmentationFilter::importRawImageData(void * data, int size_x, int size_y, int size_z, ImagePtr input, int type)
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

MeshPtr TubeSegmentationFilter::loadVtkFile(QString pathToFile, QString newDatasUid){
	FileManagerServicePtr reader = mServices->file();
	DataPtr data;
	if(reader->canLoad("vtk", pathToFile))
		data = reader->load(newDatasUid, pathToFile);

	MeshPtr retval = boost::dynamic_pointer_cast<Mesh>(data);

    if(!data || !retval)
		reportError("Could not load "+pathToFile);

	return retval;
}

void TubeSegmentationFilter::createDefaultOptions(QDomElement root)
{
	//get list with default options
	paramList defaultOptions = this->getDefaultParameters();

	//generate string adapters
    boost::unordered_map<std::string, StringParameter>::iterator stringIt;
    for(stringIt = defaultOptions.strings.begin(); stringIt != defaultOptions.strings.end(); ++stringIt )
		{
    	StringPropertyPtr option = this->makeStringOption(root, stringIt->first, stringIt->second);
    	option->setAdvanced(true);
    	option->setGroup(qstring_cast(stringIt->second.getGroup()));
    	mStringOptions.push_back(option);
    	if(stringIt->first == "parameters")
    		option->setEnabled(false);
    	option->setAdvanced(true);
    }

	//generate bool adapters
    boost::unordered_map<std::string, BoolParameter>::iterator boolIt;
    for(boolIt = defaultOptions.bools.begin(); boolIt != defaultOptions.bools.end(); ++boolIt )
		{
    	BoolPropertyPtr option = this->makeBoolOption(root, boolIt->first, boolIt->second);
    	option->setAdvanced(true);
    	option->setGroup(qstring_cast(boolIt->second.getGroup()));
    	mBoolOptions.push_back(option);
    }

	//generate double adapters
    boost::unordered_map<std::string, NumericParameter>::iterator numericIt;
    for(numericIt = defaultOptions.numerics.begin(); numericIt != defaultOptions.numerics.end(); ++numericIt )
		{
    	DoublePropertyPtr option = this->makeDoubleOption(root, numericIt->first, numericIt->second);
    	option->setAdvanced(true);
    	option->setGroup(qstring_cast(numericIt->second.getGroup()));
    	mDoubleOptions.push_back(option);
    }
}

paramList TubeSegmentationFilter::getParametersFromOptions()
{
	paramList retval = this->getDefaultParameters();

	std::vector<StringPropertyPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		try{
			setParameter(retval, stringIt->get()->getDisplayName().toStdString(), stringIt->get()->getValue().toStdString());
		}catch(SIPL::SIPLException& e){
			std::string message = "Could not process a string parameter: \""+std::string(e.what())+"\"";
			reportError(qstring_cast(message));
			continue;
		}
	}

	std::vector<BoolPropertyPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
	{
		try{
			std::string value = boolIt->get()->getValue() ? "true" : "false";
			setParameter(retval, boolIt->get()->getDisplayName().toStdString(), value);
		}catch(SIPL::SIPLException& e){
			std::string message = "Could not process a bool parameter: \""+std::string(e.what())+"\"";
			reportError(qstring_cast(message));
			continue;
		}
	}

	std::vector<DoublePropertyPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
	{
		try{
			double dbl = doubleIt->get()->getValue();
			std::string value = boost::lexical_cast<std::string>(dbl);
			setParameter(retval, doubleIt->get()->getDisplayName().toStdString(), value);
		}catch(SIPL::SIPLException& e){
			std::string message = "Could not process a double parameter: \""+std::string(e.what())+"\"";
			reportError(qstring_cast(message));
			continue;
		}
	}
	return retval;
}
void TubeSegmentationFilter::setParamtersToOptions(paramList& parameters)
{
	//set string adapters
    boost::unordered_map<std::string, StringParameter>::iterator stringIt;
    for(stringIt = parameters.strings.begin(); stringIt != parameters.strings.end(); ++stringIt )
    	this->setOptionValue(qstring_cast(stringIt->first),qstring_cast(stringIt->second.get()));

	//set bool adapters
    boost::unordered_map<std::string, BoolParameter>::iterator boolIt;
    for(boolIt = parameters.bools.begin(); boolIt != parameters.bools.end(); ++boolIt )
    	this->setOptionValue(qstring_cast(boolIt->first),qstring_cast(boolIt->second.get()));

	//set double adapters
    boost::unordered_map<std::string, NumericParameter>::iterator numericIt;
    for(numericIt = parameters.numerics.begin(); numericIt != parameters.numerics.end(); ++numericIt )
    	this->setOptionValue(qstring_cast(numericIt->first),qstring_cast(numericIt->second.get()));
}

StringPropertyPtr TubeSegmentationFilter::getStringOption(QString valueName)
{
	StringPropertyPtr retval;
	std::vector<StringPropertyPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		if(stringIt->get()->getDisplayName().compare(valueName) == 0)
		{
			retval = *stringIt;
			return retval;
		}
	}
	return retval;
}

BoolPropertyPtr TubeSegmentationFilter::getBoolOption(QString valueName)
{
	BoolPropertyPtr retval;
	std::vector<BoolPropertyPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
	{
		if(boolIt->get()->getDisplayName().compare(valueName) == 0)
		{
			retval = *boolIt;
			return retval;
		}
	}
	return retval;
}

DoublePropertyPtr TubeSegmentationFilter::getDoubleOption(QString valueName)
{
	DoublePropertyPtr retval;
	std::vector<DoublePropertyPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
	{
		if(doubleIt->get()->getDisplayName().compare(valueName) == 0)
		{
			retval = *doubleIt;
			return retval;
		}
	}
	return retval;
}

PropertyPtr TubeSegmentationFilter::getOption(QString valueName)
{
	PropertyPtr retval;

	retval = getStringOption(valueName);
	if(retval)
		return retval;
	retval = getBoolOption(valueName);
	if(retval)
		return retval;
	retval = getDoubleOption(valueName);
	if(retval)
		return retval;

	return retval;
}

void TubeSegmentationFilter::setOptionAdvanced(QString valueName, bool advanced)
{
	PropertyPtr option = this->getOption(valueName);
	option->setAdvanced(advanced);
}

void TubeSegmentationFilter::setOptionValue(QString valueName, QString value)
{
	PropertyPtr option = this->getOption(valueName);
	if(!option)
		return;

	StringPropertyPtr stringOption = boost::dynamic_pointer_cast<StringProperty>(option);
	BoolPropertyPtr boolOption = boost::dynamic_pointer_cast<BoolProperty>(option);
	DoublePropertyPtr doubleOption = boost::dynamic_pointer_cast<DoubleProperty>(option);
	if(stringOption)
	{
		stringOption->setValue(value);
	}
	else if(boolOption)
	{
		bool boolValue = (value.compare("1") == 0) ? true : false;
		boolOption->setValue(boolValue);
	}
	else if(doubleOption)
	{
		doubleOption->setValue(value.toDouble());
	}
	else
	{
		reportError("Could not determine what kind of option to set the value for.");
		return;
	}
}

std::vector<std::string> TubeSegmentationFilter::getNotDefault(paramList list)
{
	return this->getDifference(this->getDefaultParameters(), list);
}

std::vector<std::string> TubeSegmentationFilter::getDifference(paramList list1, paramList list2)
{
	std::vector<std::string> retval;
	try{
		boost::unordered_map<std::string, StringParameter>::iterator stringIt;
	    for(stringIt = list1.strings.begin(); stringIt != list1.strings.end(); ++stringIt )
	    {
	    	std::string name = stringIt->first;
	    	if(getParamStr(list1, name) != getParamStr(list2, name))
	    		retval.push_back(name);
	    }
		boost::unordered_map<std::string, BoolParameter>::iterator boolIt;
	    for(boolIt = list1.bools.begin(); boolIt != list1.bools.end(); ++boolIt )
	    {
	    	std::string name = boolIt->first;
	    	if(getParamBool(list1, name) != getParamBool(list2, name))
	    		retval.push_back(name);
	    }
		boost::unordered_map<std::string, NumericParameter>::iterator numericIt;
	    for(numericIt = list1.numerics.begin(); numericIt != list1.numerics.end(); ++numericIt )
	    {
	    	std::string name = numericIt->first;
	    	if(getParam(list1, name) != getParam(list2, name))
	    		retval.push_back(name);
	    }

	}catch (SIPL::SIPLException& e){
		reportError(QString(e.what()));
	}
	return retval;

}

std::vector<PropertyPtr> TubeSegmentationFilter::getNotDefaultOptions()
{
	std::vector<PropertyPtr> retval;

	//get list with default options
	paramList defaultOptions = this->getDefaultParameters();

	std::vector<StringPropertyPtr>::iterator stringDAIt;
	for(stringDAIt = mStringOptions.begin(); stringDAIt != mStringOptions.end(); ++stringDAIt)
	{
	    boost::unordered_map<std::string, StringParameter>::iterator stringIt;
	    for(stringIt = defaultOptions.strings.begin(); stringIt != defaultOptions.strings.end(); ++stringIt )
	    {
	    	if(stringDAIt->get()->getDisplayName().toStdString() == stringIt->first)
	    	{
	    		if(stringDAIt->get()->getValue().toStdString() != stringIt->second.get())
	    			retval.push_back(*stringDAIt);
	    	}
	    }
	}

	std::vector<BoolPropertyPtr>::iterator boolDAIt;
	for(boolDAIt = mBoolOptions.begin(); boolDAIt != mBoolOptions.end(); ++boolDAIt)
	{
	    boost::unordered_map<std::string, BoolParameter>::iterator boolIt;
	    for(boolIt = defaultOptions.bools.begin(); boolIt != defaultOptions.bools.end(); ++boolIt )
	    {
	    	if(boolDAIt->get()->getDisplayName().toStdString() == boolIt->first)
	    	{
	    		if(boolDAIt->get()->getValue() != boolIt->second.get())
	    			retval.push_back(*boolDAIt);
	    	}
	    }
	}

	std::vector<DoublePropertyPtr>::iterator doubleDAIt;
	for(doubleDAIt = mDoubleOptions.begin(); doubleDAIt != mDoubleOptions.end(); ++doubleDAIt)
	{
	    boost::unordered_map<std::string, NumericParameter>::iterator numericIt;
	    for(numericIt = defaultOptions.numerics.begin(); numericIt != defaultOptions.numerics.end(); ++numericIt )
			{
				if(doubleDAIt->get()->getDisplayName().toStdString() == numericIt->first)
				{
					if(!similar(doubleDAIt->get()->getValue(), numericIt->second.get()))
						retval.push_back(*doubleDAIt);
				}
	    }
	}

	return retval;
}

paramList TubeSegmentationFilter::getDefaultParameters()
{
	//get list with default options
	paramList defaultOptions;
	try{
		defaultOptions = initParameters(cx::DataLocations::findConfigFolder("/tsf", QString(KERNELS_DIR)).toStdString() + "/parameters");
	} catch (SIPL::SIPLException& e){
		std::string message = "When creating default options, could not init parameters. \""+std::string(e.what())+"\"";
		reportError(qstring_cast(message));
	}
	return defaultOptions;
}

void TubeSegmentationFilter::printParameters(paramList parameters)
{
	std::cout << "\n" << std::endl;
    std::cout << "The following parameters are set: " << std::endl;

    boost::unordered_map<std::string,StringParameter>::iterator itString;
    for(itString = parameters.strings.begin(); itString != parameters.strings.end(); ++itString) {
    	std::cout << itString->first << " " << itString->second.get() << std::endl;
    }

    boost::unordered_map<std::string,BoolParameter>::iterator itBool;
    for(itBool = parameters.bools.begin(); itBool != parameters.bools.end(); ++itBool) {
    	std::string value = itBool->second.get() ? "true" : "false";
    	std::cout << itBool->first << " " << value << std::endl;
    }

    boost::unordered_map<std::string,NumericParameter>::iterator itNumeric;
    for(itNumeric = parameters.numerics.begin(); itNumeric != parameters.numerics.end(); ++itNumeric) {
    	std::cout << itNumeric->first << " " << itNumeric->second.get() << std::endl;
    }

    std::cout << "\n" << std::endl;
}

StringPropertyPtr TubeSegmentationFilter::makeStringOption(QDomElement root, std::string name, StringParameter parameter)
{
	QString helptext = qstring_cast(parameter.getDescription());
	std::vector<std::string> possibilities = parameter.getPossibilities();

	QString value = qstring_cast(parameter.get());

	QStringList list;
	std::vector<std::string>::iterator it;
	for(it = possibilities.begin(); it != possibilities.end(); ++it)
	{
		QString item = qstring_cast(*it);
		if(!item.isEmpty())
			list << item;
	}

	if(!possibilities.empty())
		return StringProperty::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, list, root);
	else
	{
		//NB. hvis en string type ikke har noen possibilities betyr det at det er fritekst
		return  StringProperty::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, root);
	}
}

BoolPropertyPtr TubeSegmentationFilter::makeBoolOption(QDomElement root, std::string name, BoolParameter parameter)
{
	QString helptext = qstring_cast(parameter.getDescription());
	bool value = parameter.get();
	return BoolProperty::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, root);
}

DoublePropertyPtr TubeSegmentationFilter::makeDoubleOption(QDomElement root, std::string name, NumericParameter parameter)
{
	QString helptext = qstring_cast(parameter.getDescription());
	double value = parameter.get();
	double min = parameter.getMin();
	double max = parameter.getMax();
	double step = parameter.getStep();

	DoubleRange range(min, max, step);
	int decimals = 2;

	DoublePropertyPtr retval = DoubleProperty::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, range, decimals, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	return retval;
}

TSFPresetsPtr TubeSegmentationFilter::populatePresets()
{
	TSFPresetsPtr retval(new TSFPresets());
	return retval;
}

TubeSegmentationFilter::~TubeSegmentationFilter() {
	// Cleanup the output from TSF
	if(mOutput != NULL)
		delete mOutput;
}

} /* namespace cx */

