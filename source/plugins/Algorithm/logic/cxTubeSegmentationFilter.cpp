#include "cxTubeSegmentationFilter.h"

#ifdef CX_USE_TSF
#include "tube-segmentation.hpp"
#include "openCLUtilities.hpp"
#include "tsf-config.h"
#include "Exceptions.hpp"

#include <vtkImageImport.h>
#include <vtkImageData.h>

#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscDataManagerImpl.h"
#include "sscRegistrationTransform.h"
#include "cxDataLocations.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxSelectDataStringDataAdapter.h"
#include "sscDoubleDataAdapterXml.h"

namespace cx {

TubeSegmentationFilter::TubeSegmentationFilter() :
	FilterImpl(), mOutput(NULL)
{
	connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));
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
	        "<p><i>Extracts the centerline and creates a segementation. </br>GPU-base algorithm wrtiten by Erik Smistad (NTNU).</i></p>"
	        "</html>";
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

		mParameters = loadParameterPreset(mParameters, cx::DataLocations::getTSFPath().toStdString()+"/parameters");

		//this->printParameters(mParameters);

		mOutput = run(filename, mParameters, cx::DataLocations::getTSFPath().toStdString());
	} catch(SIPL::SIPLException e) {
		std::string error = e.what();
		ssc::messageManager()->sendError(qstring_cast(error));

		//TODO free data
		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	} catch (std::exception e){
		ssc::messageManager()->sendError("Tube segmentation algorithm threw a std::exception.");

		//TODO free data
		if(mOutput != NULL){
			delete mOutput;
			mOutput = NULL;
		}
		return false;
	} catch (...){
		ssc::messageManager()->sendError("Tube segmentation algorithm threw a unknown exception.");

		//free data
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
	ssc::Transform3D rMd = input->get_rMd();
	rMd = rMd * cropTranslation;

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
	if(mOutput->hasTDF() && it != mParameters.strings.end() && (it->second.get() != "off"))
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
		QString uidSegmentation = input->getUid() + "_tsf_seg%1";
		QString nameSegmentation = input->getName()+"_tsf_seg%1";
		SIPL::int3* size = mOutput->getSize();
		vtkImageDataPtr rawSegmentationResult = this->convertToVtkImageData(mOutput->getSegmentation(), size->x, size->y, size->z, input);

		ssc::ImagePtr outputSegmentaion = ssc::dataManager()->createDerivedImage(rawSegmentationResult,uidSegmentation, nameSegmentation, input);
		if (!outputSegmentaion)
			return false;

		outputSegmentaion->get_rMd_History()->setRegistration(rMd);

		ssc::dataManager()->loadData(outputSegmentaion);
		ssc::dataManager()->saveImage(outputSegmentaion, patientService()->getPatientData()->getActivePatientFolder());

		mOutputTypes[2]->setValue(outputSegmentaion->getUid());
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

	//TODO temporary fix to be able to reset options
	mOptionsAdapters.push_back(mResetOption);

	//TODO options are automatically populated with saved data, because these are xml adapters
	//BUT these options are saved on a system level (NOT patient level!)

	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); stringIt++)
		mOptionsAdapters.push_back(*stringIt);

	std::vector<ssc::BoolDataAdapterXmlPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); boolIt++)
		mOptionsAdapters.push_back(*boolIt);

	std::vector<ssc::DoubleDataAdapterXmlPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); doubleIt++)
		mOptionsAdapters.push_back(*doubleIt);

	//TODO connect options that should be connected
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
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Centerline");
	temp->setHelp("Generated centerline.");
	mOutputTypes.push_back(temp);

	SelectMeshStringDataAdapterPtr vtkCenterline = SelectMeshStringDataAdapter::New();
	vtkCenterline->setValueName("Centerline (vtk)");
	vtkCenterline->setHelp("Generated centerline mesh (vtk-format).");
	mOutputTypes.push_back(vtkCenterline);

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Segmentation");
	temp->setHelp("Generated segmentation.");
	mOutputTypes.push_back(temp);
}

void TubeSegmentationFilter::patientChangedSlot()
{
	QString activePatientFolder = patientService()->getPatientData()->getActivePatientFolder()+"/Images/";

	for(std::vector<ssc::StringDataAdapterXmlPtr>::iterator it = mStringOptions.begin(); it != mStringOptions.end(); ++it)
	{
		//TODO set storage-dir until problem is fixed
		if(it->get()->getValueName().compare("storage-dir") == 0)
			it->get()->setValue(activePatientFolder);
	}
}

void TubeSegmentationFilter::inputChangedSlot()
{
	QString activePatientFolder = patientService()->getPatientData()->getActivePatientFolder()+"/Images/";
	QString inputsValue = mInputTypes.front()->getValue();

	for(std::vector<ssc::StringDataAdapterXmlPtr>::iterator it = mStringOptions.begin(); it != mStringOptions.end(); ++it)
	{
		if(it->get()->getValueName().compare("centerline-vtk-file") == 0)
			it->get()->setValue(activePatientFolder+inputsValue+QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat())+"_tsf_vtk.vtk");
	}
}
void TubeSegmentationFilter::parametersFileChanged()
{
	std::string parameterFile = "none";
	for(std::vector<ssc::StringDataAdapterXmlPtr>::iterator it = mStringOptions.begin(); it != mStringOptions.end(); ++it)
	{
		if(it->get()->getValueName().compare("parameters") == 0)
			parameterFile = it->get()->getValue().toStdString();
	}
	paramList temp = this->getParametersFromOptions();
	try
	{
		temp = loadParameterPreset(temp, cx::DataLocations::getTSFPath().toStdString()+"/parameters");
	} catch (SIPL::SIPLException e)
	{
		ssc::messageManager()->sendWarning("Error when loading parameter file "+qstring_cast(parameterFile)+". Preset is corrupt.");
	}
	this->setParamtersToOptions(temp);
}

void TubeSegmentationFilter::resetOptions()
{
	if(mResetOption->getValue() == "reset")
	{
		try{
			paramList defaultParameters = initParameters(cx::DataLocations::getTSFPath().toStdString()+"/parameters");
			this->setParamtersToOptions(defaultParameters);
			this->patientChangedSlot();
			this->inputChangedSlot();
			mResetOption->setValue("not reset");
		} catch (SIPL::SIPLException& e){
			std::string message = "When resettting options, could not init parameters. \""+std::string(e.what())+"\"";
			ssc::messageManager()->sendError(qstring_cast(message));
		}
	}
}

//TODO later...
//void TubeSegmentationFilter::centerlineMethodChanged()
//{
//
//}

vtkImageDataPtr TubeSegmentationFilter::convertToVtkImageData(char * data, int size_x, int size_y, int size_z, ssc::ImagePtr input)
{
	if (!input)
		return vtkImageDataPtr::New();

	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarTypeToUnsignedChar();
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetDataSpacing(input->getBaseVtkImageData()->GetSpacing());
	imageImport->SetImportVoidPointer((void*)data);
	imageImport->GetOutput()->Update();
	imageImport->Modified();

//	vtkImageDataPtr retval = imageImport->GetOutput();
	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(imageImport->GetOutput());

	return retval;
}

void TubeSegmentationFilter::createDefaultOptions(QDomElement root)
{
	paramList defaultOptions;
	try{
		defaultOptions = initParameters(cx::DataLocations::getTSFPath().toStdString()+"/parameters");
	} catch (SIPL::SIPLException& e){
		std::string message = "When creating default options, could not init parameters. \""+std::string(e.what())+"\"";
		ssc::messageManager()->sendError(qstring_cast(message));
	}

	// skip parameters we don't want:
	// display, storage-dir ...
	//TODO
	std::vector<std::string> hideParameter;
	//hideParameter.push_back("display");
	//hideParameter.push_back("storage-dir");
	//hideParameter.push_back("centerline-vtk-file");

	//generate string adapters
    boost::unordered_map<std::string, StringParameter>::iterator stringIt;
    for(stringIt = defaultOptions.strings.begin(); stringIt != defaultOptions.strings.end(); ++stringIt )
    {
    	//skip some parameters
    	if(std::find(hideParameter.begin(), hideParameter.end(), stringIt->first) != hideParameter.end())
    		continue;

    	ssc::StringDataAdapterXmlPtr option = this->makeStringOption(root, stringIt->first, stringIt->second);
    	mStringOptions.push_back(option);
    	if(stringIt->first == "parameters")
    		connect(option.get(), SIGNAL(changed()), this, SLOT(parametersFileChanged()));
//    	if(stringIt->first == "centerline-method")
//    		connect(option.get(), SIGNAL(changed()), this, SLOT(centerlineMethodChanged()));
    }

    //Manuelly adding option for resetting.
    QStringList list;
    list << "not reset";
    list << "reset";
//	mResetOption = this->makeStringOption(root, "RESET TO DEFAULT", list);
    mResetOption = ssc::StringDataAdapterXml::initialize("tsf_reset_to_default", "RESET TO DEFAULT", "Used to reset options to default values.", "not reset", list, root);
	//mStringOptions.push_back(mResetOption);
	connect(mResetOption.get(), SIGNAL(changed()), this, SLOT(resetOptions()));

	//generate bool adapters
    boost::unordered_map<std::string, BoolParameter>::iterator boolIt;
    for(boolIt = defaultOptions.bools.begin(); boolIt != defaultOptions.bools.end(); ++boolIt )
    {
    	if(std::find(hideParameter.begin(), hideParameter.end(), boolIt->first) == hideParameter.end())
    		mBoolOptions.push_back(this->makeBoolOption(root, boolIt->first, boolIt->second));
    }

	//generate double adapters
    boost::unordered_map<std::string, NumericParameter>::iterator numericIt;
    for(numericIt = defaultOptions.numerics.begin(); numericIt != defaultOptions.numerics.end(); ++numericIt )
    {
    	if(std::find(hideParameter.begin(), hideParameter.end(), numericIt->first) == hideParameter.end())
    		mDoubleOptions.push_back(this->makeDoubleOption(root, numericIt->first, numericIt->second));
    }

}

paramList TubeSegmentationFilter::getParametersFromOptions()
{
	paramList retval;
	try{
		retval = initParameters(cx::DataLocations::getTSFPath().toStdString()+"/parameters");
	} catch (SIPL::SIPLException& e){
		std::string message = "When getting parameters from options, could not init parameters. \""+std::string(e.what())+"\"";
		ssc::messageManager()->sendError(qstring_cast(message));
	}

	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		try{
			retval = setParameter(retval, stringIt->get()->getValueName().toStdString(), stringIt->get()->getValue().toStdString());
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
			retval = setParameter(retval, boolIt->get()->getValueName().toStdString(), value);
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
			retval = setParameter(retval, doubleIt->get()->getValueName().toStdString(), value);
		}catch(SIPL::SIPLException& e){
			std::string message = "Could not process a double parameter: \""+std::string(e.what())+"\"";
			ssc::messageManager()->sendError(qstring_cast(message));
			continue;
		}
	}
	return retval;
}
void TubeSegmentationFilter::setParamtersToOptions(paramList parameters)
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

void TubeSegmentationFilter::setOptionValue(QString valueName, QString value)
{
	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		if(stringIt->get()->getValueName().compare(valueName) == 0)
		{
			stringIt->get()->setValue(value);
			return;
		}
	}

	std::vector<ssc::BoolDataAdapterXmlPtr>::iterator boolIt;
	bool boolValue = (value.compare("true") == 0) ? true : false;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
	{
		if(boolIt->get()->getValueName().compare(valueName) == 0)
		{
			boolIt->get()->setValue(boolValue);
			return;
		}
	}

	std::vector<ssc::DoubleDataAdapterXmlPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
	{
		if(doubleIt->get()->getValueName().compare(valueName) == 0)
		{
			doubleIt->get()->setValue(value.toDouble());
			return;
		}
	}
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
	QString helptext = qstring_cast(parameter.getDescription()); //parameter.getHelpText();
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

} /* namespace cx */
#endif //CX_USE_TSF

