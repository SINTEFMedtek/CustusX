#include "cxTubeSegmentationFilter.h"

#ifdef CX_USE_TSF
#include "tube-segmentation.hpp"
#include "openCLUtilities.hpp"
#include "tsf-config.h"
#include "Exceptions.hpp"

#include <QTimer>

#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>

#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscDataManagerImpl.h"
#include "sscRegistrationTransform.h"
#include "sscDoubleDataAdapterXml.h"
#include "cxContourFilter.h"
#include "cxDataLocations.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxTSFPresets.h"

typedef vtkSmartPointer<class vtkImageShiftScale> vtkImageShiftScalePtr;

namespace cx {

TubeSegmentationFilter::TubeSegmentationFilter() :
	FilterImpl(), mOutput(NULL)
{
	connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));
	mPresets = this->populatePresets();
}

QString TubeSegmentationFilter::getName() const
{
	return "Tube-Segmentation Filter";
}

QString TubeSegmentationFilter::getType() const
{
	return "TubeSegmentationFilter";
}

QString TubeSegmentationFilter::getHelp() const
{
	return "<html>"
	        "<h3>Tube-Segmentation.</h3>"
	        "<p><i>Extracts the centerline and creates a segementation. </br>GPU-base algorithm written by Erik Smistad (NTNU).</i></p>"
	        "</html>";
}

bool TubeSegmentationFilter::hasPresets()
{
	return true;
}

ssc::PresetsPtr TubeSegmentationFilter::getPresets()
{
	return mPresets;
}

QDomElement TubeSegmentationFilter::getNewPreset()
{
	std::vector<DataAdapterPtr> newPresetOptions = this->getNotDefaultOptions();

	std::map<QString, QString> newPresetMap;
	std::vector<DataAdapterPtr>::iterator it;
	for(it = newPresetOptions.begin(); it != newPresetOptions.end(); ++it){
		DataAdapterPtr option = *it;
		QString name = option->getValueName();
		QString value;
		ssc::StringDataAdapterXmlPtr stringOption = boost::dynamic_pointer_cast<ssc::StringDataAdapterXml>(option);
		ssc::BoolDataAdapterXmlPtr boolOption = boost::dynamic_pointer_cast<ssc::BoolDataAdapterXml>(option);
		ssc::DoubleDataAdapterXmlPtr doubleOption = boost::dynamic_pointer_cast<ssc::DoubleDataAdapterXml>(option);
		if(stringOption)
			value = stringOption->getValue();
		else if(boolOption)
			value = boolOption->getValue() ? "true" : "false";
		else if(doubleOption)
			value = QString::number(doubleOption->getValue());
		else
			ssc::messageManager()->sendError("Could not determine what kind of option to get the value for.");
		newPresetMap[name] = value;
	}
	ssc::StringDataAdapterPtr centerlineMethod = this->getStringOption("centerline-method");
	newPresetMap[centerlineMethod->getValueName()] = centerlineMethod->getValue();

	//create xml
	QDomDocument doc = mPresets->getCustomFile().getDocument();
	QDomElement retval = doc.createElement("Preset");
	std::map<QString, QString>::iterator it2;
	for(it2 = newPresetMap.begin(); it2 != newPresetMap.end(); ++it2){
		QDomElement newNode = doc.createElement(it2->first);
		newNode.appendChild(doc.createTextNode(it2->second));
		retval.appendChild(newNode);
	}

	return retval;
}

void TubeSegmentationFilter::requestSetPresetSlot(QString name)
{
	QString centerLineMethod = "gpu";
	if((name == "<Default preset>") || (name == "none") || (name == "default"))
		mParameterFile = "none";
	else
	{
		QStringList nameList = name.split(": ", QString::SkipEmptyParts);
		mParameterFile = nameList.at(1);
		QString temp = nameList.at(0);
		centerLineMethod = temp.remove("centerline-");
	}
	this->loadNewParametersSlot();

	ssc::StringDataAdapterXmlPtr centerlineMethodOption = this->getStringOption("centerline-method");
	centerlineMethodOption->setValue(centerLineMethod);
}

bool TubeSegmentationFilter::execute()
{
	//get the image
    ssc::ImagePtr input = this->getCopiedInputImage();
    	if (!input)
    		return false;

	// Parse parameters from program arguments
	mParameters = this->getParametersFromOptions();
	std::string filename = (patientService()->getPatientData()->getActivePatientFolder()+"/"+input->getFilePath()).toStdString();

	try {
		ssc::messageManager()->sendDebug("Looking for TSF files in folder: "+cx::DataLocations::getTSFPath());
		mOutput = run(filename, mParameters, cx::DataLocations::getTSFPath().toStdString());
	} catch(SIPL::SIPLException& e) {
		std::string error = e.what();
		ssc::messageManager()->sendError("SIPL::SIPLException: "+qstring_cast(error));

		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	} catch(cl::Error& e) {
		ssc::messageManager()->sendError("cl::Error:"+qstring_cast(e.what()));

		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	} catch (std::exception& e){
		ssc::messageManager()->sendError("std::exception:"+qstring_cast(e.what()));

		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	} catch (...){
		ssc::messageManager()->sendError("Tube segmentation algorithm threw a unknown exception.");

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
		ssc::messageManager()->sendWarning("No output generated from the tube segmentation filter.");
		return false;
	}

	ssc::ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	//Calculate shift
	double spacing_x, spacing_y, spacing_z;
	input->getBaseVtkImageData()->GetSpacing(spacing_x, spacing_y, spacing_z);

	//compensate for cropping
	SIPL::int3 cropShift = mOutput->getShiftVector();
	ssc::Vector3D translationVector((spacing_x*cropShift.x), (spacing_y*cropShift.y), (spacing_z*cropShift.z));
	ssc::Transform3D cropTranslation = ssc::createTransformTranslate(translationVector);
	ssc::Transform3D rMd = input->get_rMd(); //transform from the volumes coordinate system to our reference coordinate system
	rMd = rMd * cropTranslation; //translation due to cropping accounted for

	// Centerline (volume)
	//======================================================
	if(mOutput->hasCenterlineVoxels())
	{
		QString uidCenterline = input->getUid() + "_tsf_cl_vol%1";
		QString nameCenterline = input->getName()+"_tsf_cl_vol%1";
		SIPL::int3* size = mOutput->getSize();
		vtkImageDataPtr rawCenterlineResult = this->convertToVtkImageData(mOutput->getCenterlineVoxels(), size->x, size->y, size->z, input);
		if(!rawCenterlineResult)
			return false;

		ssc::ImagePtr outputCenterline = ssc::dataManager()->createDerivedImage(rawCenterlineResult ,uidCenterline, nameCenterline, input);
		if (!outputCenterline)
			return false;

		outputCenterline->get_rMd_History()->setRegistration(rMd);

		ssc::dataManager()->loadData(outputCenterline);
		ssc::dataManager()->saveImage(outputCenterline, patientService()->getPatientData()->getActivePatientFolder());

		mOutputTypes[0]->setValue(outputCenterline->getUid());
	}

	// Centerline (vtk)
	//======================================================
	boost::unordered_map<std::string, StringParameter>::iterator it = mParameters.strings.find("centerline-vtk-file");
	if(it != mParameters.strings.end() && (it->second.get() != "off"))
	{
		QString vtkFilename = qstring_cast(it->second.get());
		QString uidVtkCenterline = input->getUid() + "_tsf_cl%1";
		QString nameVtkCenterline = input->getName()+"_tsf_cl%1";

		//load vtk into CustusX
		ssc::PolyDataMeshReader reader;
		ssc::DataPtr data;
		if(reader.canLoad("vtk", vtkFilename))
			data = reader.load(uidVtkCenterline, vtkFilename);

		if(!data)
			false;

		ssc::MeshPtr temp = boost::dynamic_pointer_cast<ssc::Mesh>(data);

		if(data && temp){
			ssc::Vector3D scalingVector(spacing_x, spacing_y, spacing_z);
			ssc::Transform3D scalingTransformation = ssc::createTransformScale(scalingVector);
			ssc::Transform3D dMv = rMd*scalingTransformation;
			vtkPolyDataPtr poly = temp->getTransformedPolyData(dMv);

			//create, load and save mesh
			ssc::MeshPtr mesh = ssc::dataManager()->createMesh(poly, uidVtkCenterline, nameVtkCenterline, "Images");
			mesh->get_rMd_History()->setParentSpace(input->getUid());
			ssc::dataManager()->loadData(mesh);
			ssc::dataManager()->saveMesh(mesh, patientService()->getPatientData()->getActivePatientFolder());
			QString uid = mesh->getUid();

			mOutputTypes[1]->setValue(uid);
		}else{
			ssc::messageManager()->sendError("Could not import vtk centerline: "+vtkFilename);
			return false;
		}
	}

	// Segmentation
	//======================================================
	if(mOutput->hasSegmentation())
	{
		//get segmented volume
		SIPL::int3* size = mOutput->getSize();
		vtkImageDataPtr rawSegmentation = this->convertToVtkImageData(mOutput->getSegmentation(), size->x, size->y, size->z, input);

		//make contour of segmented volume
		double threshold = 1;/// because the segmented image is 0..1
		vtkPolyDataPtr rawContour = ContourFilter::execute(rawSegmentation, threshold);

		//add segmentation internally to cx
		QString uidSegmentation = input->getUid() + "_tsf_seg%1";
		QString nameSegmentation = input->getName()+"_tsf_seg%1";
		ssc::ImagePtr outputSegmentation = ssc::dataManager()->createDerivedImage(rawSegmentation,uidSegmentation, nameSegmentation, input);
		if (!outputSegmentation)
			return false;
		outputSegmentation->get_rMd_History()->setRegistration(rMd);
		ssc::dataManager()->loadData(outputSegmentation);
		ssc::dataManager()->saveImage(outputSegmentation, patientService()->getPatientData()->getActivePatientFolder());

		//add contour internally to cx
		ssc::MeshPtr contour = ContourFilter::postProcess(rawContour, input, QColor("blue"));

		//set output
		mOutputTypes[2]->setValue(outputSegmentation->getUid());
		mOutputTypes[3]->setValue(contour->getUid());
	}

	// TDF
	//======================================================
	if(mOutput->hasTDF())
	{
		QString uidTDF = input->getUid() + "_tsf_tdf%1";
		QString nameTDF = input->getName()+"_tsf_tdf%1";
		SIPL::int3* size = mOutput->getSize();

		// convert volume
		vtkImageDataPtr convertedImageData = this->convertToVtkImageData(mOutput->getTDF(), size->x, size->y, size->z, input);

		//scale volume
		if (!convertedImageData)
			return false;

		vtkImageShiftScalePtr cast = vtkImageShiftScalePtr::New();
		cast->SetInput(convertedImageData);
		cast->ClampOverflowOn();

		//tdfs voxels contains values [0.0,1.0]
		//scaling these to be able to visualize them in CustusX
		int scale = 255; //unsigned char ranges from 0 to 255
		cast->SetScale(scale);
		cast->SetOutputScalarType(VTK_UNSIGNED_CHAR);
		cast->Update();
		convertedImageData = cast->GetOutput();

		ssc::ImagePtr outputTDF = ssc::dataManager()->createDerivedImage(convertedImageData,uidTDF, nameTDF, input);
		if (!outputTDF)
			return false;
		outputTDF->get_rMd_History()->setRegistration(rMd);
		ssc::dataManager()->loadData(outputTDF);
		ssc::dataManager()->saveImage(outputTDF, patientService()->getPatientData()->getActivePatientFolder());

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

	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); stringIt++)
		mOptionsAdapters.push_back(*stringIt);

	std::vector<ssc::BoolDataAdapterXmlPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); boolIt++)
		mOptionsAdapters.push_back(*boolIt);

	std::vector<ssc::DoubleDataAdapterXmlPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); doubleIt++)
		mOptionsAdapters.push_back(*doubleIt);
}

void TubeSegmentationFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select input to run Tube segmentation on.");
	mInputTypes.push_back(temp);

	connect(temp.get(), SIGNAL(changed()), this, SLOT(inputChangedSlot()));
}

void TubeSegmentationFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr tempDataStringAdapter;
	SelectMeshStringDataAdapterPtr tempMeshStringAdapter;

	//0
	tempDataStringAdapter = SelectDataStringDataAdapter::New();
	tempDataStringAdapter->setValueName("Centerline");
	tempDataStringAdapter->setHelp("Generated centerline.");
	mOutputTypes.push_back(tempDataStringAdapter);

	//1
	tempMeshStringAdapter = SelectMeshStringDataAdapter::New();
	tempMeshStringAdapter->setValueName("Centerline (vtk)");
	tempMeshStringAdapter->setHelp("Generated centerline mesh (vtk-format).");
	mOutputTypes.push_back(tempMeshStringAdapter);

	//2
	tempDataStringAdapter = SelectDataStringDataAdapter::New();
	tempDataStringAdapter->setValueName("Segmentation");
	tempDataStringAdapter->setHelp("Generated segmentation.");
	mOutputTypes.push_back(tempDataStringAdapter);

	//3
	tempMeshStringAdapter = SelectMeshStringDataAdapter::New();
	tempMeshStringAdapter->setValueName("Surface");
	tempMeshStringAdapter->setHelp("Generated surface of the segmented volume.");
	mOutputTypes.push_back(tempMeshStringAdapter);

	//4
	tempDataStringAdapter = SelectDataStringDataAdapter::New();
	tempDataStringAdapter->setValueName("TDF");
	tempDataStringAdapter->setHelp("Volume showing the probability of a voxel being part of a tubular structure.");
	mOutputTypes.push_back(tempDataStringAdapter);
}

void TubeSegmentationFilter::patientChangedSlot()
{
	QString activePatientFolder = patientService()->getPatientData()->getActivePatientFolder()+"/Images/";

	ssc::StringDataAdapterXmlPtr option = this->getStringOption("storage-dir");
	if(option)
		option->setValue(activePatientFolder);
}

void TubeSegmentationFilter::inputChangedSlot()
{
	QString activePatientFolder = patientService()->getPatientData()->getActivePatientFolder()+"/Images/";
	QString inputsValue = mInputTypes.front()->getValue();

	ssc::StringDataAdapterXmlPtr option = this->getStringOption("centerline-vtk-file");
	if(option)
		option->setValue(activePatientFolder+inputsValue+QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat())+"_tsf_vtk.vtk");
}

//void TubeSegmentationFilter::parametersFileChanged()
//{
//	if(mParameterFile != this->getStringOption("parameters")->getValue())
//		QTimer::singleShot(0, this, SLOT(loadNewParameters()));
//}

void TubeSegmentationFilter::loadNewParametersSlot()
{
//	ssc::StringDataAdapterXmlPtr parameterOption = this->getStringOption("parameters");
//	if(!parameterOption)
//		return;

//	mParameterFile = parameterOption->getValue();

	paramList list = this->getDefaultParameters();

	if(mParameterFile != "none")
	{
		try
		{
			setParameter(list, "parameters", mParameterFile.toStdString());
			loadParameterPreset(list, cx::DataLocations::getTSFPath().toStdString()+"/parameters");
		} catch (SIPL::SIPLException e)
		{
			ssc::messageManager()->sendWarning("Error when loading a parameter file. Preset is corrupt. "+QString(e.what()));
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
	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		ssc::StringDataAdapterXmlPtr adapter = *stringIt;
		if(adapter->getValueName() == "parameters")
		{
			adapter->setAdvanced(false);
		}
		else
			adapter->setAdvanced(true);
	}

	std::vector<ssc::BoolDataAdapterXmlPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
	{
		ssc::BoolDataAdapterXmlPtr adapter = *boolIt;
		adapter->setAdvanced(true);
	}

	std::vector<ssc::DoubleDataAdapterXmlPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
	{
		ssc::DoubleDataAdapterXmlPtr adapter = *doubleIt;
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

vtkImageDataPtr TubeSegmentationFilter::convertToVtkImageData(char * data, int size_x, int size_y, int size_z, ssc::ImagePtr input)
{
	if (!input)
		return vtkImageDataPtr::New();

	vtkImageDataPtr retval = this->importRawImageData((void*) data, size_x, size_y, size_z, input, VTK_UNSIGNED_CHAR);
	return retval;
}

vtkImageDataPtr TubeSegmentationFilter::convertToVtkImageData(float * data, int size_x, int size_y, int size_z, ssc::ImagePtr input)
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
vtkImageDataPtr TubeSegmentationFilter::importRawImageData(void * data, int size_x, int size_y, int size_z, ssc::ImagePtr input, int type)
{
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarType(type);
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetDataSpacing(input->getBaseVtkImageData()->GetSpacing());
	imageImport->SetImportVoidPointer(data);
	imageImport->GetOutput()->Update();
	imageImport->Modified();

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(imageImport->GetOutput());

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
    	ssc::StringDataAdapterXmlPtr option = this->makeStringOption(root, stringIt->first, stringIt->second);
    	option->setAdvanced(true);
    	option->setGroup(qstring_cast(stringIt->second.getGroup()));
    	mStringOptions.push_back(option);
    	if(stringIt->first == "parameters")
    	{
    		option->setEnabled(false);
//    		connect(option.get(), SIGNAL(changed()), this, SLOT(parametersFileChanged()));
    	}
    	option->setAdvanced(true);
    }

	//generate bool adapters
    boost::unordered_map<std::string, BoolParameter>::iterator boolIt;
    for(boolIt = defaultOptions.bools.begin(); boolIt != defaultOptions.bools.end(); ++boolIt )
    {
    	ssc::BoolDataAdapterXmlPtr option = this->makeBoolOption(root, boolIt->first, boolIt->second);
    	option->setAdvanced(true);
    	option->setGroup(qstring_cast(boolIt->second.getGroup()));
    	mBoolOptions.push_back(option);
    }

	//generate double adapters
    boost::unordered_map<std::string, NumericParameter>::iterator numericIt;
    for(numericIt = defaultOptions.numerics.begin(); numericIt != defaultOptions.numerics.end(); ++numericIt )
    {
    	ssc::DoubleDataAdapterXmlPtr option = this->makeDoubleOption(root, numericIt->first, numericIt->second);
    	option->setAdvanced(true);
    	option->setGroup(qstring_cast(numericIt->second.getGroup()));
    	mDoubleOptions.push_back(option);
    }
}

paramList TubeSegmentationFilter::getParametersFromOptions()
{
	paramList retval = this->getDefaultParameters();

	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		try{
			setParameter(retval, stringIt->get()->getValueName().toStdString(), stringIt->get()->getValue().toStdString());
		}catch(SIPL::SIPLException& e){
			std::string message = "Could not process a string parameter: \""+std::string(e.what())+"\"";
			ssc::messageManager()->sendError(qstring_cast(message));
			continue;
		}
	}

	std::vector<ssc::BoolDataAdapterXmlPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
	{
		try{
			std::string value = boolIt->get()->getValue() ? "true" : "false";
			setParameter(retval, boolIt->get()->getValueName().toStdString(), value);
		}catch(SIPL::SIPLException& e){
			std::string message = "Could not process a bool parameter: \""+std::string(e.what())+"\"";
			ssc::messageManager()->sendError(qstring_cast(message));
			continue;
		}
	}

	std::vector<ssc::DoubleDataAdapterXmlPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
	{
		try{
			double dbl = doubleIt->get()->getValue();
			std::string value = boost::lexical_cast<std::string>(dbl);
			setParameter(retval, doubleIt->get()->getValueName().toStdString(), value);
		}catch(SIPL::SIPLException& e){
			std::string message = "Could not process a double parameter: \""+std::string(e.what())+"\"";
			ssc::messageManager()->sendError(qstring_cast(message));
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

ssc::StringDataAdapterXmlPtr TubeSegmentationFilter::getStringOption(QString valueName)
{
	ssc::StringDataAdapterXmlPtr retval;
	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		if(stringIt->get()->getValueName().compare(valueName) == 0)
		{
			retval = *stringIt;
			return retval;
		}
	}
	return retval;
}

ssc::BoolDataAdapterXmlPtr TubeSegmentationFilter::getBoolOption(QString valueName)
{
	ssc::BoolDataAdapterXmlPtr retval;
	std::vector<ssc::BoolDataAdapterXmlPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
	{
		if(boolIt->get()->getValueName().compare(valueName) == 0)
		{
			retval = *boolIt;
			return retval;
		}
	}
	return retval;
}

ssc::DoubleDataAdapterXmlPtr TubeSegmentationFilter::getDoubleOption(QString valueName)
{
	ssc::DoubleDataAdapterXmlPtr retval;
	std::vector<ssc::DoubleDataAdapterXmlPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
	{
		if(doubleIt->get()->getValueName().compare(valueName) == 0)
		{
			retval = *doubleIt;
			return retval;
		}
	}
	return retval;
}

DataAdapterPtr TubeSegmentationFilter::getOption(QString valueName)
{
	DataAdapterPtr retval;

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
	DataAdapterPtr option = this->getOption(valueName);
	option->setAdvanced(advanced);
}

void TubeSegmentationFilter::setOptionValue(QString valueName, QString value)
{
	DataAdapterPtr option = this->getOption(valueName);
	if(!option)
	{
		return;
	}

	ssc::StringDataAdapterXmlPtr stringOption = boost::dynamic_pointer_cast<ssc::StringDataAdapterXml>(option);
	ssc::BoolDataAdapterXmlPtr boolOption = boost::dynamic_pointer_cast<ssc::BoolDataAdapterXml>(option);
	ssc::DoubleDataAdapterXmlPtr doubleOption = boost::dynamic_pointer_cast<ssc::DoubleDataAdapterXml>(option);
	if(stringOption)
	{
		stringOption->setValue(value);
	}
	else if(boolOption)
	{
		bool boolValue = (value.compare("true") == 0) ? true : false;
		boolOption->setValue(boolValue);
	}
	else if(doubleOption)
	{
		doubleOption->setValue(value.toDouble());
	}
	else
	{
		ssc::messageManager()->sendError("Could not determine what kind of option to set the value for.");
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
		ssc::messageManager()->sendError(QString(e.what()));
	}
	return retval;

}

std::vector<DataAdapterPtr> TubeSegmentationFilter::getNotDefaultOptions()
{
	std::vector<DataAdapterPtr> retval;

	//get list with default options
	paramList defaultOptions = this->getDefaultParameters();

	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringDAIt;
	for(stringDAIt = mStringOptions.begin(); stringDAIt != mStringOptions.end(); ++stringDAIt)
	{
//		if(stringDAIt->get()->getValue().toStdString() == getParamStr(defaultOptions, stringDAIt->get()->getValueName().toStdString()))
//			retval.push_back(*stringDAIt);
	    boost::unordered_map<std::string, StringParameter>::iterator stringIt;
	    for(stringIt = defaultOptions.strings.begin(); stringIt != defaultOptions.strings.end(); ++stringIt )
	    {
	    	if(stringDAIt->get()->getValueName().toStdString() == stringIt->first)
	    	{
	    		if(stringDAIt->get()->getValue().toStdString() != stringIt->second.get())
	    			retval.push_back(*stringDAIt);
	    	}
	    }
	}

	std::vector<ssc::BoolDataAdapterXmlPtr>::iterator boolDAIt;
	for(boolDAIt = mBoolOptions.begin(); boolDAIt != mBoolOptions.end(); ++boolDAIt)
	{
	    boost::unordered_map<std::string, BoolParameter>::iterator boolIt;
	    for(boolIt = defaultOptions.bools.begin(); boolIt != defaultOptions.bools.end(); ++boolIt )
	    {
	    	if(boolDAIt->get()->getValueName().toStdString() == boolIt->first)
	    	{
	    		if(boolDAIt->get()->getValue() != boolIt->second.get())
	    			retval.push_back(*boolDAIt);
	    	}
	    }
	}

	std::vector<ssc::DoubleDataAdapterXmlPtr>::iterator doubleDAIt;
	for(doubleDAIt = mDoubleOptions.begin(); doubleDAIt != mDoubleOptions.end(); ++doubleDAIt)
	{
	    boost::unordered_map<std::string, NumericParameter>::iterator numericIt;
	    for(numericIt = defaultOptions.numerics.begin(); numericIt != defaultOptions.numerics.end(); ++numericIt )
	    {
	    	if(doubleDAIt->get()->getValueName().toStdString() == numericIt->first)
	    	{
	    		if(doubleDAIt->get()->getValue() != numericIt->second.get())
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
		defaultOptions = initParameters(cx::DataLocations::getTSFPath().toStdString()+"/parameters");
	} catch (SIPL::SIPLException& e){
		std::string message = "When creating default options, could not init parameters. \""+std::string(e.what())+"\"";
		ssc::messageManager()->sendError(qstring_cast(message));
	}
	return defaultOptions;
}

void TubeSegmentationFilter::printParameters(paramList parameters)
{
	std::cout << "\n" << std::endl;
    std::cout << "The following parameters are set: " << std::endl;

    boost::unordered_map<std::string,StringParameter>::iterator itString;
    for(itString = parameters.strings.begin(); itString != parameters.strings.end(); itString++) {
    	std::cout << itString->first << " " << itString->second.get() << std::endl;
    }

    boost::unordered_map<std::string,BoolParameter>::iterator itBool;
    for(itBool = parameters.bools.begin(); itBool != parameters.bools.end(); itBool++) {
    	std::string value = itBool->second.get() ? "true" : "false";
    	std::cout << itBool->first << " " << value << std::endl;
    }

    boost::unordered_map<std::string,NumericParameter>::iterator itNumeric;
    for(itNumeric = parameters.numerics.begin(); itNumeric != parameters.numerics.end(); itNumeric++) {
    	std::cout << itNumeric->first << " " << itNumeric->second.get() << std::endl;
    }

    std::cout << "\n" << std::endl;
}

ssc::StringDataAdapterXmlPtr TubeSegmentationFilter::makeStringOption(QDomElement root, std::string name, StringParameter parameter)
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
		return ssc::StringDataAdapterXml::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, list, root);
	else
	{
		//NB. hvis en string type ikke har noen possibilities betyr det at det er fritekst
		return  ssc::StringDataAdapterXml::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, root);
	}
}

ssc::BoolDataAdapterXmlPtr TubeSegmentationFilter::makeBoolOption(QDomElement root, std::string name, BoolParameter parameter)
{
	QString helptext = qstring_cast(parameter.getDescription());
	bool value = parameter.get();
	return ssc::BoolDataAdapterXml::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, root);
}

ssc::DoubleDataAdapterXmlPtr TubeSegmentationFilter::makeDoubleOption(QDomElement root, std::string name, NumericParameter parameter)
{
	QString helptext = qstring_cast(parameter.getDescription());
	double value = parameter.get();
	double min = parameter.getMin();
	double max = parameter.getMax();
	double step = parameter.getStep();

	ssc::DoubleRange range(min, max, step);
	int decimals = 2;

	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, range, decimals, root);
	retval->setAddSlider(true);
	return retval;
}

TSFPresetsPtr TubeSegmentationFilter::populatePresets()
{
//	std::cout << "TubeSegmentationFilter::populatePresets()" << std::endl;
	TSFPresetsPtr retval(new TSFPresets());

	return retval;
}

} /* namespace cx */
#endif //CX_USE_TSF

