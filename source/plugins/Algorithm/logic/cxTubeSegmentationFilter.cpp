#include "cxTubeSegmentationFilter.h"

#ifdef CX_USE_TSF
#include "tube-segmentation.hpp"
#include "openCLUtilities.hpp"
#include "tsf-config.h"

#include <vtkImageImport.h>
#include <vtkImageData.h>

#include "sscTypeConversions.h"
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

    boost::unordered_map<std::string, std::string> parameters = this->generateParametersFromOptions();

    // Write out parameter list
    std::cout << "The following parameters are set: " << std::endl;
    boost::unordered_map<std::string, std::string>::iterator it;
    for(it = parameters.begin(); it != parameters.end(); it++) {
    	std::cout << it->first << " " << it->second << std::endl;
    }

    OpenCL ocl;
	//ocl.context = createCLContextFromArguments(argc, argv);
    ocl.context = this->createCLContextFromArguments(parameters);

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
	    if(parameters.count("buffers-only") == 0 && (int)devices[0].getInfo<CL_DEVICE_EXTENSIONS>().find("cl_khr_3d_image_writes") > -1) {
	    	kernelFile = std::string(KERNELS_DIR) + "/kernels.cl";
	        ocl.program = buildProgramFromSource(ocl.context, kernelFile);
	        parameters["3d_write"] = "true";
	    } else {
	    	kernelFile = std::string(KERNELS_DIR) + "/kernels_no_3d_write.cl";
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
    catch (...){
    	std::cout << "Caught unexpected exception in TubeSegmentationFilter::execute(), contact programmer." << std::endl;
    	return false;
    }

    SIPL::int3 size;
    TubeSegmentation TS;
    try {
    	std::string filename = (patientService()->getPatientData()->getActivePatientFolder()+"/"+input->getFilePath()).toStdString();
    	std::cout << "reading and transfering: " << filename << std::endl;
        // Read dataset and transfer to device
        cl::Image3D dataset = readDatasetAndTransfer(ocl, filename, parameters, &size);

        // Run specified method on dataset
        if(parameters.count("centerline-method") && parameters["centerline-method"] == "ridge") {
            TS = runCircleFittingAndRidgeTraversal(ocl, dataset, size, parameters);
        } else {
            TS = runCircleFittingAndNewCenterlineAlg(ocl, dataset, size, parameters);
        }
    } catch(cl::Error e) {
        std::cout << "OpenCL error: " << getCLErrorString(e.err()) << std::endl;
        return 0;
    }
    ocl.queue.finish();

    mRawCenterlineResult = this->convertToVtkImageData(TS.centerline, size.x, size.y, size.z);
    mRawSegmentationResult = this->convertToVtkImageData(TS.segmentation, size.x, size.y, size.z);


    //TODO Clean up!!!
    if(parameters.count("display") > 0 || parameters.count("storage-dir") > 0 || parameters["centerline-method"] == "ridge") {
        // Cleanup transferred data
        delete[] TS.centerline;
        delete[] TS.TDF;
        if(parameters.count("no-segmentation") == 0)
            delete[] TS.segmentation;
        if(parameters["centerline-method"] == "ridge")
            delete[] TS.radius;
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

	// Segmentation
	//======================================================
	QString uidSegmentation = input->getUid() + "_tsf_seg%1";
	QString nameSegmentation = input->getName()+"_tsf_seg%1";

	ssc::ImagePtr outputSegmentaion = ssc::dataManager()->createDerivedImage(mRawSegmentationResult,uidSegmentation, nameSegmentation, input);
	if (!outputSegmentaion)
		return;

	ssc::dataManager()->loadData(outputSegmentaion);
	ssc::dataManager()->saveImage(outputSegmentaion, patientService()->getPatientData()->getActivePatientFolder());

	mOutputTypes[1]->setValue(outputSegmentaion->getUid());
}

void TubeSegmentationFilter::createOptions(QDomElement root)
{
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

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Segmentation");
	temp->setHelp("Generated segmentation.");
	mOutputTypes.push_back(temp);
}

void TubeSegmentationFilter::toggleAutoMinimum()
{
	mMinimumOption->setEnabled(!mAutoMinimumOption->getValue());
}

void TubeSegmentationFilter::toggleAutoMaximum()
{
	mMaximumOption->setEnabled(!mAutoMaximumOption->getValue());
}

vtkImageDataPtr TubeSegmentationFilter::convertToVtkImageData(char * data, int size_x, int size_y, int size_z)
{
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarTypeToUnsignedChar();
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetImportVoidPointer((void*)data);
	imageImport->GetOutput()->Update();
	imageImport->Modified();

	vtkImageDataPtr retval = imageImport->GetOutput();

	return retval;
}

boost::unordered_map<std::string, std::string> TubeSegmentationFilter::generateParametersFromOptions()
{
	boost::unordered_map<std::string, std::string> retval;

	//TODO BEGIN
	//std::string  storageDir = "storage-dir";
	//std::string storageDirPath = "/home/jbake/jbake/workspace/"; // storage-dirs value
	//retval[storageDir] = storageDirPath;


	//std::string parameters = "--parameters";
	//std::string parametersFile = "us-accuracy";
	//retval[parameters] = parametersFile;
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
}

cl::Context TubeSegmentationFilter::createCLContextFromArguments(boost::unordered_map<std::string, std::string> parameters) {
    cl_device_type type = CL_DEVICE_TYPE_ALL;
    cl_vendor vendor = VENDOR_ANY;

    boost::unordered_map<std::string, std::string>::iterator deviceIt = parameters.find("device");
    if(deviceIt != parameters.end())
    {
    	if(deviceIt->second == "cpu")
    		type = CL_DEVICE_TYPE_CPU;
    	else if(deviceIt->second == "gpu")
    		type = CL_DEVICE_TYPE_GPU;
    }

    boost::unordered_map<std::string, std::string>::iterator vendorIt = parameters.find("vendor");
    if(vendorIt != parameters.end())
    {
    	if(vendorIt->second == "amd")
    		type = VENDOR_AMD;
    	else if(vendorIt->second == "intel")
    		type = VENDOR_INTEL;
    	else if(vendorIt->second == "nvidia")
    	    type = VENDOR_NVIDIA;
    }

    return createCLContext(type, vendor);
}

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
} /* namespace cx */
#endif //CX_USE_TSF

