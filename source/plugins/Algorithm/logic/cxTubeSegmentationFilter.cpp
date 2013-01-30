#include "cxTubeSegmentationFilter.h"

#ifdef CX_USE_TSF
#include "tube-segmentation.hpp"
#include "openCLUtilities.hpp"
#include "tsf-config.h"
#include "Exceptions.hpp"

#include <vtkImageImport.h>
#include <vtkImageData.h>

#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

namespace cx {

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

	//======================================================

    /*
	char  arg0[] = "not needed?"; //"/home/jbake/jbake/workspace/Tube-Segmenation-Framework/build_Debug/tubeSegmentation"; // executable
	char  arg1[] = "/home/jbake/jbake/data/helix/helix.mhd"; // dataset
	char  arg2[] = "--mode";
	char  arg3[] = "white"; // modes value
	char  arg4[] = "--storage-dir";
	char  arg5[] = "/home/jbake/jbake/workspace/"; // storage-dirs value
	char  arg6[] = "--timing";
	//char  arg2[] = "--parameters";
	//char  arg3[] = "us-accuracy";
	char  arg7[] = "NULL"; // end of list
	char* argv[] = { &arg0[0], &arg1[0], &arg2[0], &arg3[0], &arg4[0], &arg5[0], &arg6[0], &arg7[0], NULL };
	//char* argv[] = { &arg0[0], &arg1[0], &arg2[0], &arg3[0], &arg7[0], NULL };

    //char** argv = this->generateParameterList();
	int argc = sizeof argv/sizeof argv[0] - 1;
	*/
    /*
	QStringList stringlist = generateParametersFromOptions();
	QByteArray temp = stringlist.simplified().toLocal8Bit();
	QList<QByteArray> list = temp.split(' ');
	int argc = list.count();
	char **argv = new char*[argc];
	for (int i = 0; i < argc; ++i)
	    argv[i] = list[i].data();
	*/

    /*
    QStringList stringlist = this->generateParametersFromOptions();
    for (int i = 0; i < stringlist.size(); ++i)
         std::cout << stringlist.at(i).toLocal8Bit().constData() << std::endl;

    char** argv = this->convertValue(this->generateParametersFromOptions());
    int argc = sizeof argv/sizeof argv[0] - 1;
    std::cout << "argc " << argc << std::endl;

    // Parse parameters from program arguments
	boost::unordered_map<std::string, std::string> parameters = getParameters(argc, argv);
	*/
try{
    mParameters = this->getParameters();

    // Write out parameter list
    this->printParameters(mParameters);

    //TODO remove
    return true;

    /*
    std::cout << "The following parameters are set: " << std::endl;
    boost::unordered_map<std::string, std::string>::iterator it;
    for(it = mParameters.begin(); it != mParameters.end(); it++) {
    	std::cout << it->first << " " << it->second << std::endl;
    }
    */

    OpenCL ocl;
	//ocl.context = createCLContextFromArguments(argc, argv);
    ocl.context = this->createCLContextFromArguments(mParameters);

    // Select first device
    cl::vector<cl::Device> devices = ocl.context.getInfo<CL_CONTEXT_DEVICES>();
    std::cout << "Using device: " << devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
    ocl.queue = cl::CommandQueue(ocl.context, devices[0], CL_QUEUE_PROFILING_ENABLE);

    // Query the size of available memory
    unsigned int memorySize = devices[0].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
    std::cout << "Available memory on selected device " << (double)memorySize/(1024*1024) << " MB "<< std::endl;

    try{
	    // Compile and create program
	    std::string kernelFile;
	    if(!getParamBool(mParameters, "buffers-only") && (int)devices[0].getInfo<CL_DEVICE_EXTENSIONS>().find("cl_khr_3d_image_writes") > -1) {
	    	kernelFile = std::string(KERNELS_DIR) + "/kernels.cl";
	        ocl.program = buildProgramFromSource(ocl.context, kernelFile);
	        BoolParameter v = mParameters.bools["3d_write"];
			v.set(true);
			mParameters.bools["3d_write"] = v;
	    } else {
	    	kernelFile = std::string(KERNELS_DIR) + "/kernels_no_3d_write.cl";
	        BoolParameter v = mParameters.bools["3d_write"];
	        v.set(false);
	        mParameters.bools["3d_write"] = v;
	        ocl.program = buildProgramFromSource(ocl.context, kernelFile);
	        std::cout << "Writing to 3D textures is not supported on the selected device." << std::endl;
	    }
    }
    catch(cl::Error error) {
        if(error.err() == CL_BUILD_PROGRAM_FAILURE) {
            std::cout << "Build log:" << std::endl << ocl.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
            return false;
        }
    }

    SIPL::int3 size;
    TubeSegmentation TS;
    try {
    	std::string filename = (patientService()->getPatientData()->getActivePatientFolder()+"/"+input->getFilePath()).toStdString();
    	//std::cout << "reading and transfering: " << filename << std::endl;
        // Read dataset and transfer to device
        cl::Image3D dataset = readDatasetAndTransfer(ocl, filename, mParameters, &size);

        // Run specified method on dataset
        if(getParamStr(mParameters, "centerline-method") == "ridge") {
            TS = runCircleFittingAndRidgeTraversal(ocl, dataset, size, mParameters);
        } else {
            TS = runCircleFittingAndNewCenterlineAlg(ocl, dataset, size, mParameters);
        }
    } catch(cl::Error e) {
        std::cout << "OpenCL error: " << getCLErrorString(e.err()) << std::endl;
        return 0;
    }
    ocl.queue.finish();

    mRawCenterlineResult = this->convertToVtkImageData(TS.centerline, size.x, size.y, size.z);
    mRawSegmentationResult = this->convertToVtkImageData(TS.segmentation, size.x, size.y, size.z);


    //TODO Clean up!!!
    //TODO cannot clean up yet, because need this data to make the file...
    /*if(mParameters.count("display") > 0 || mParameters.count("storage-dir") > 0 || mParameters["centerline-method"] == "ridge") {
        // Cleanup transferred data
        delete[] TS.centerline;
        delete[] TS.TDF;
        if(mParameters.count("no-segmentation") == 0)
            delete[] TS.segmentation;
        if(mParameters["centerline-method"] == "ridge")
            delete[] TS.radius;
    }*/
} catch(std::exception& s){
	printf("std::exception: %s \n",s.what());
} catch(SIPL::SIPLException& e){
	printf("SIPL::SIPLException: %s \n",e.what());
} catch(...){
	std::cout << "Caught unhandled exception." << std::endl;
}
	//======================================================
 	return true;
}

void TubeSegmentationFilter::postProcess()
{
	if (!mRawCenterlineResult || !mRawSegmentationResult)
		return;

	ssc::ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return;

	// Centerline
	//======================================================
	QString uidCenterline = input->getUid() + "_tsf_cl%1";
	QString nameCenterline = input->getName()+"_tsf_cl%1";

	//mRawCenterlineResult->Print(std::cout);

	ssc::ImagePtr outputCenterline = ssc::dataManager()->createDerivedImage(mRawCenterlineResult,uidCenterline, nameCenterline, input);
	if (!outputCenterline)
		return;

	ssc::dataManager()->loadData(outputCenterline);
	ssc::dataManager()->saveImage(outputCenterline, patientService()->getPatientData()->getActivePatientFolder());

	mOutputTypes.front()->setValue(outputCenterline->getUid());

	// Centerline (vtk)
	//======================================================
	boost::unordered_map<std::string, StringParameter>::iterator it = mParameters.strings.find("centerline-vtk-file");
	if(it != mParameters.strings.end())
	{
		QString vtkFilename = qstring_cast(it->second.get());
		//load vtk into CustusX
		ssc::DataPtr data = patientService()->getPatientData()->importData(vtkFilename);
		if(data){
			QString uid = data->getUid();
			mOutputTypes[1]->setValue(uid);
		}else{
			ssc::messageManager()->sendError("Could not import vtk centerline: "+vtkFilename);
		}
	}

	// Segmentation
	//======================================================
	QString uidSegmentation = input->getUid() + "_tsf_seg%1";
	QString nameSegmentation = input->getName()+"_tsf_seg%1";

	ssc::ImagePtr outputSegmentaion = ssc::dataManager()->createDerivedImage(mRawSegmentationResult,uidSegmentation, nameSegmentation, input);
	if (!outputSegmentaion)
		return;

	ssc::dataManager()->loadData(outputSegmentaion);
	ssc::dataManager()->saveImage(outputSegmentaion, patientService()->getPatientData()->getActivePatientFolder());

	mOutputTypes.back()->setValue(outputSegmentaion->getUid());
}

void TubeSegmentationFilter::createOptions(QDomElement root)
{
	this->createDefaultOptions(root);

	//TODO load saved options, done automatically because this is xml adapters???

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

	/*
	mDeviceOption = this->makeDeviceOption(root);
	mOptionsAdapters.push_back(mDeviceOption);
	mBufferOnlyOption = this->makeBuffersOnlyOption(root);
	mOptionsAdapters.push_back(mBufferOnlyOption);
	mAutoMinimumOption = this->makeAutoMinimumOption(root);
	mOptionsAdapters.push_back(mAutoMinimumOption);
	mMinimumOption = this->makeMinimumOption(root);
	mOptionsAdapters.push_back(mMinimumOption);
	mAutoMaximumOption = this->makeAutoMaximumOption(root);
	mOptionsAdapters.push_back(mAutoMaximumOption);
	mMaximumOption = this->makeMaximumOption(root);
	mOptionsAdapters.push_back(mMaximumOption);
	mModeOption = this->makeModeOption(root);
	mOptionsAdapters.push_back(mModeOption);
	mNoSegmentationOption = this->makeNoSegmentationOption(root);
	mOptionsAdapters.push_back(mNoSegmentationOption);
	mCenterlineMethodOption = this->makeCenterlineMethodOption(root);
	mOptionsAdapters.push_back(mCenterlineMethodOption);
	mTimingOption = this->makeTimingOption(root);
	mOptionsAdapters.push_back(mTimingOption);

	connect(mAutoMinimumOption.get(), SIGNAL(changed()), this, SLOT(toggleAutoMinimum()));
	this->toggleAutoMinimum();
	connect(mAutoMaximumOption.get(), SIGNAL(changed()), this, SLOT(toggleAutoMaximum()));
	this->toggleAutoMaximum();
	*/
}

void TubeSegmentationFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select input to run Tube segmentation on.");
	mInputTypes.push_back(temp);
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

void TubeSegmentationFilter::toggleAutoMinimum()
{
	//mMinimumOption->setEnabled(!mAutoMinimumOption->getValue());
}

void TubeSegmentationFilter::toggleAutoMaximum()
{
	//mMaximumOption->setEnabled(!mAutoMaximumOption->getValue());
}

vtkImageDataPtr TubeSegmentationFilter::convertToVtkImageData(char * data, int size_x, int size_y, int size_z)
{
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarTypeToUnsignedShort();
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetImportVoidPointer((void*)data);
	imageImport->GetOutput()->Update();
	imageImport->Modified();

	vtkImageDataPtr retval = imageImport->GetOutput();

	return retval;
}

void TubeSegmentationFilter::createDefaultOptions(QDomElement root)
{
	paramList defaultOptions = initParameters();

	this->printParameters(defaultOptions); //debugging

	//TODO add vendor and device manually...

	//TODO skip parameters we don't want:
	// display, storage-dir ...
	std::vector<std::string> hideParameter;
	hideParameter.push_back("display");
	hideParameter.push_back("storage-dir");

	//TODO set storage-dir untill problem is fixed
	setParameter(defaultOptions, "storage-dir", "/home/jbake/jbake/workspace/");

	//generate string adapters
    boost::unordered_map<std::string, StringParameter>::iterator stringIt;
    for(stringIt = defaultOptions.strings.begin(); stringIt != defaultOptions.strings.end(); ++stringIt )
    {
    	if(std::find(hideParameter.begin(), hideParameter.end(), stringIt->first) == hideParameter.end())
    		mStringOptions.push_back(this->makeStringOption(root, stringIt->first, stringIt->second));
    }

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

paramList TubeSegmentationFilter::getParameters()
{
	//TODO rydd opp...

	paramList retval = initParameters();

	std::vector<ssc::StringDataAdapterXmlPtr>::iterator stringIt;
	for(stringIt = mStringOptions.begin(); stringIt != mStringOptions.end(); ++stringIt)
	{
		//TODO hvordan vet vi om det gikk?
		try{
			std::cout << "Setter et string parameter: " << stringIt->get()->getValueName().toStdString() << " " << stringIt->get()->getValue().toStdString() << std::endl;
			retval = setParameter(retval, stringIt->get()->getValueName().toStdString(), stringIt->get()->getValue().toStdString());
		} catch (std::exception e)
		{
			ssc::messageManager()->sendError("Could not process a parameter needed for tube segementation.");
			//TODO read what()
			//TODO throw!!!
		}
	}

	std::vector<ssc::BoolDataAdapterXmlPtr>::iterator boolIt;
	for(boolIt = mBoolOptions.begin(); boolIt != mBoolOptions.end(); ++boolIt)
	{
		//TODO hvordan vet vi om det gikk?
		try{
			std::string value = boolIt->get()->getValue() ? "true" : "false";
			std::cout << "Setter et bool parameter: " << boolIt->get()->getValueName().toStdString() << " " << boolIt->get()->getValue() << std::endl;
			retval = setParameter(retval, boolIt->get()->getValueName().toStdString(), value);
		} catch (std::exception e)
		{
			ssc::messageManager()->sendError("Could not process a parameter needed for tube segementation.");
			//TODO read what()
			//TODO throw!!!
		}
	}

	std::vector<ssc::DoubleDataAdapterXmlPtr>::iterator doubleIt;
	for(doubleIt = mDoubleOptions.begin(); doubleIt != mDoubleOptions.end(); ++doubleIt)
	{
		//TODO hvordan vet vi om det gikk?
		try{
			double dbl = doubleIt->get()->getValue();
			std::string value = boost::lexical_cast<std::string>(dbl);
			//std::ostringstream strs;
			//strs << dbl;
			//std::string value = strs.str();
			std::cout << "Setter et double parameter: " << doubleIt->get()->getValueName().toStdString() << " " << doubleIt->get()->getValue() << std::endl;
			retval = setParameter(retval, doubleIt->get()->getValueName().toStdString(), value);
		} catch (std::exception e)
		{
			ssc::messageManager()->sendError("Could not process a parameter needed for tube segementation.");
			//TODO read what()
			//TODO throw!!!
		}
	}
	return retval;

	/*
	//TODO BEGIN
	std::string  storageDir = "storage-dir";
	std::string storageDirPath = "/home/jbake/jbake/workspace/"; // storage-dirs value
	retval[storageDir] = storageDirPath;


	//std::string parameters = "parameters";
	//std::string parametersFile = "us-accuracy";
	//retval[parameters] = parametersFile;

	std::string centerlineVtkFile = "centerline-vtk-file";
	std::string centerlineVtkFileFile = "/home/jbake/Desktop/vtkCenterline.vtk";
	retval[centerlineVtkFile] = centerlineVtkFileFile;
	//TODO END

	//ssc::BoolDataAdapterXmlPtr mTimingOption;
	if(mTimingOption->getValue()){
		std::string timing = "timing";
		retval[timing] = "dummy-value";
	}

	//ssc::StringDataAdapterXmlPtr mDeviceOption;
	std::string device = "device";
	std::string deviceType = mDeviceOption->getValue().toStdString();
	retval[device] = deviceType;

	//ssc::BoolDataAdapterXmlPtr mBufferOnlyOption;
	std::string bufferOnly = "buffers-only";
	if(mBufferOnlyOption->getValue())
		retval[bufferOnly] = "dummy-value";

	//ssc::BoolDataAdapterXmlPtr mAutoMinimumOption;
	//ssc::DoubleDataAdapterXmlPtr mMinimumOption;
	if(!mAutoMinimumOption->getValue())
	{
		std::string minimum = "minimum";
		std::string minumumValue = boost::lexical_cast<std::string>(mMinimumOption->getValue());
		retval[minimum] = minumumValue;
	}

	//ssc::BoolDataAdapterXmlPtr mAutoMaximumOption;
	//ssc::DoubleDataAdapterXmlPtr mMaximumOption;
	if(!mAutoMaximumOption->getValue())
	{
		std::string maximum = "maximum";
		std::string maximumValue = boost::lexical_cast<std::string>(mMaximumOption->getValue());
		retval[maximum] = maximumValue;
	}

	//ssc::StringDataAdapterXmlPtr mModeOption;
	std::string mode = "mode";
	std::string modeType = mModeOption->getValue().toStdString();
	retval[mode] = modeType;

	//ssc::BoolDataAdapterXmlPtr mNoSegmentationOption;
	std::string noSegmentation = "";
	if(mNoSegmentationOption->getValue())
	{
		noSegmentation = "no-segmentation";
		retval[noSegmentation] = "dummy-value";
	}

	//ssc::StringDataAdapterXmlPtr mCenterlineMethodOption;
	std::string centerlineMethod = "centerline-method";
	std::string centerlineMethodType = mCenterlineMethodOption->getValue().toStdString();
	retval[centerlineMethod] = centerlineMethodType;

	return retval;
	*/
}

cl::Context TubeSegmentationFilter::createCLContextFromArguments(paramList parameters)
{
    cl_device_type type = CL_DEVICE_TYPE_ALL;
    cl_vendor vendor = VENDOR_ANY;

    boost::unordered_map<std::string, StringParameter>::iterator deviceIt = parameters.strings.find("device");
    if(deviceIt != parameters.strings.end())
    {
    	if(deviceIt->second.get() == "cpu")
    		type = CL_DEVICE_TYPE_CPU;
    	else if(deviceIt->second.get() == "gpu")
    		type = CL_DEVICE_TYPE_GPU;
    }

    boost::unordered_map<std::string, StringParameter>::iterator vendorIt = parameters.strings.find("vendor");
    if(vendorIt != parameters.strings.end())
    {
    	if(vendorIt->second.get() == "amd")
    		type = VENDOR_AMD;
    	else if(vendorIt->second.get() == "intel")
    		type = VENDOR_INTEL;
    	else if(vendorIt->second.get() == "nvidia")
    	    type = VENDOR_NVIDIA;
    }

    return createCLContext(type, vendor);
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
	//TODO fix when available from tsf lib
	QString helptext = qstring_cast("Coming!"); //parameter.getHelpText();
	std::vector<std::string> possibilities; //parameter.getPossibilities();
	possibilities.push_back("todo1");
	possibilities.push_back("todo2");
	possibilities.push_back("todo3");

	QString value = qstring_cast(parameter.get());

	QStringList list;
	std::vector<std::string>::iterator it;
	for(it = possibilities.begin(); it != possibilities.end(); ++it)
		list << qstring_cast(*it);

	return ssc::StringDataAdapterXml::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, list, root);
}

ssc::BoolDataAdapterXmlPtr TubeSegmentationFilter::makeBoolOption(QDomElement root, std::string name, BoolParameter parameter)
{
	//TODO fix when available from tsf lib
	QString helptext = qstring_cast("Coming!"); //parameter.getHelpText();
	bool value = parameter.get();
	return ssc::BoolDataAdapterXml::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, root);
}

ssc::DoubleDataAdapterXmlPtr TubeSegmentationFilter::makeDoubleOption(QDomElement root, std::string name, NumericParameter parameter)
{
	//TODO fix when available from tsf lib
	QString helptext = qstring_cast("Coming!"); //parameter.getHelpText();
	double value = parameter.get();
	double min = 0;
	double max = 10;
	double step = 1;

	ssc::DoubleRange range(min, max, step);
	int decimals = 2;

	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize(qstring_cast("tsf_"+name), qstring_cast(name), helptext, value, range, decimals, root);
	retval->setAddSlider(true);
	return retval;
}
/*
ssc::StringDataAdapterXmlPtr TubeSegmentationFilter::makeDeviceOption(QDomElement root)
{
	QStringList list;
	list << "gpu" << "cpu";
	return ssc::StringDataAdapterXml::initialize("tsf_device", "Device", "Which type of device to run calculations on",
	                                             list[0], list, root);
}

ssc::BoolDataAdapterXmlPtr TubeSegmentationFilter::makeBuffersOnlyOption(QDomElement root)
{
	return ssc::BoolDataAdapterXml::initialize("tsf_buffers-only", "Buffers only",
	                                           "Disable writing to 3D images", false, root);
}

ssc::BoolDataAdapterXmlPtr TubeSegmentationFilter::makeAutoMinimumOption(QDomElement root)
{
	return ssc::BoolDataAdapterXml::initialize("tsf_auto-minimum", "Auto minimum",
	                                           "Find minimum automatically", false, root);
}

ssc::DoubleDataAdapterXmlPtr TubeSegmentationFilter::makeMinimumOption(QDomElement root)
{
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("tsf_minimum", "Minimum",
		                                                                        "Set minimum threshold",
		                                                                        0, ssc::DoubleRange(-1000, 1000, 1), 0, root);
		retval->setAddSlider(true);
		return retval;
}

ssc::BoolDataAdapterXmlPtr TubeSegmentationFilter::makeAutoMaximumOption(QDomElement root)
{
	return ssc::BoolDataAdapterXml::initialize("tsf_auto-maximum", "Auto maximum",
	                                           "Find maximum automatically", false, root);
}

ssc::DoubleDataAdapterXmlPtr TubeSegmentationFilter::makeMaximumOption(QDomElement root)
{
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("tsf_maximum", "Maximum",
		                                                                        "Set maximum threshold",
		                                                                        100.0, ssc::DoubleRange(-1000, 1000, 1), 0, root);
		retval->setAddSlider(true);
		return retval;
}

ssc::StringDataAdapterXmlPtr TubeSegmentationFilter::makeModeOption(QDomElement root)
{
	QStringList list;
	list << "black" << "white";
	return ssc::StringDataAdapterXml::initialize("tsf_mode", "Mode", "Look for black or white tubes",
	                                             list[0], list, root);
}

ssc::BoolDataAdapterXmlPtr TubeSegmentationFilter::makeNoSegmentationOption(QDomElement root)
{
	return ssc::BoolDataAdapterXml::initialize("tsf_no-segmentation", "Segmentation",
	                                           "Turns off segmentation and returns centerline only", false, root);
}

ssc::BoolDataAdapterXmlPtr TubeSegmentationFilter::makeTimingOption(QDomElement root)
{
	return ssc::BoolDataAdapterXml::initialize("tsf_timing", "Timing",
	                                           "Prints timing information.", false, root);
}

ssc::StringDataAdapterXmlPtr TubeSegmentationFilter::makeCenterlineMethodOption(QDomElement root)
{
	QStringList list;
	list << "gpu" << "cpu";
	return ssc::StringDataAdapterXml::initialize("tsf_centerline-method", "Centerline Method", "Specify which centerline method to use",
	                                             list[0], list, root);
}
*/
} /* namespace cx */
#endif //CX_USE_TSF

