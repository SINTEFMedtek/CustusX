#include "cxTubeSegmentationFilter.h"

#ifdef CX_USE_TSF
//#include "tube-segmentation.hpp"
//#include "openCLUtilities.hpp"
//#include <map>

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

//ssc::StringDataAdapterPtr TubeSegmentationFilter::getStringOption(QDomElement root)
//{
//	QStringList list;
//	list << "String0" << "String1";
//	return ssc::StringDataAdapterXml::initialize("String", "", "dummy string option",
//	                                             list[0], list, root);
//}
//
//ssc::DoubleDataAdapterPtr TubeSegmentationFilter::getDoubleOption(QDomElement root)
//{
//	return ssc::DoubleDataAdapterXml::initialize("Value", "",
//	                                             "dummy double value.", 1, ssc::DoubleRange(0.1, 10, 0.01), 2,
//	                                             root);
//}
//
//ssc::BoolDataAdapterPtr TubeSegmentationFilter::getBoolOption(QDomElement root)
//{
//	return ssc::BoolDataAdapterXml::initialize("Bool0", "",
//	                                           "Dummy bool value.", false, root);
//}

void TubeSegmentationFilter::createOptions(QDomElement root)
{
//	mOptionsAdapters.push_back(this->getStringOption(root));
//	mOptionsAdapters.push_back(this->getDoubleOption(root));
//	mOptionsAdapters.push_back(this->getBoolOption(root));
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
	temp->setValueName("Output");
	temp->setHelp("Generated centerline.");
	mOutputTypes.push_back(temp);
}

bool TubeSegmentationFilter::execute()
{
	ssc::ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	//======================================================
	//char ** argv;
//	char *argv[]{
//		"/home/jbake/jbake/workspace/Tube-Segmenation-Framework/build_Debug/tubeSegmentation",
//		"/home/jbake/jbake/data/helix/helix.mhd",
//		"--mode",
//		"white",
//		"--display",
//		"NULL"
//	};
//	int argc = sizeof argv/sizeof argv[0] - 1;
//
//    // Parse parameters from program arguments
//	std::map<std::string, std::string> parameters = this->getParameters(argc, argv);
//
//    // Write out parameter list
//    std::cout << "The following parameters are set: " << std::endl;
//    std::map<std::string, std::string>::iterator it;
//    for(it = parameters.begin(); it != parameters.end(); it++) {
//    	std::cout << it->first << " " << it->second << std::endl;
//    }
//
//	OpenCL ocl;
//	ocl.context = createCLContextFromArguments(argc, argv);

//    // Select first device
//    cl::vector<cl::Device> devices = ocl.context.getInfo<CL_CONTEXT_DEVICES>();
//    std::cout << "Using device: " << devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
//    ocl.queue = cl::CommandQueue(ocl.context, devices[0], CL_QUEUE_PROFILING_ENABLE);
//
//    // Query the size of available memory
//    unsigned int memorySize = devices[0].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
//    std::cout << "Available memory on selected device " << (double)memorySize/(1024*1024) << " MB "<< std::endl;
//
//    // Compile and create program
//    if(parameters.count("buffers-only") == 0 && (int)devices[0].getInfo<CL_DEVICE_EXTENSIONS>().find("cl_khr_3d_image_writes") > -1) {
//        ocl.program = buildProgramFromSource(ocl.context, "kernels.cl");
//        parameters["3d_write"] = "true";
//    } else {
//        ocl.program = buildProgramFromSource(ocl.context, "kernels_no_3d_write.cl");
//        std::cout << "Writing to 3D textures is not supported on the selected device." << std::endl;
//    }


	//======================================================

	std::cout << "NOT IMPLEMENTED: TubeSegmentationFilter::execute()" << std::endl;
	return false;
}

void TubeSegmentationFilter::postProcess()
{
	/*
	//TODO: add stuff such as saving to dataManager here.
	std::cout << "TubeSegmentationFilter::postProcess " << mCopiedInput.size() << std::endl;

	if (mInputTypes.front()->getData())
		mOutputTypes.front()->setValue(mInputTypes.front()->getData()->getUid());

	//    return mInput;
	 */
	std::cout << "NOT IMPLEMENTED: TubeSegmentationFilter::postProcess()" << std::endl;
}

/**
 * Just needed for development/debugging!!!
 */
//std::map<std::string, std::string> TubeSegmentationFilter::getParameters(int argc, char ** argv) {
//	std::map<std::string, std::string> parameters;
//    // Go through each parameter, first parameter is filename
//    for(int i = 2; i < argc; i++) {
//        std::string token = argv[i];
//        if(token.substr(0,2) == "--") {
//            // Check to see if the parameter has a value
//            std::string nextToken;
//            if(i+1 < argc) {
//                nextToken = argv[i+1];
//            } else {
//                nextToken = "--";
//            }
//            if(nextToken.substr(0,2) == "--") {
//                // next token is not a value
//                parameters[token.substr(2)] = "dummy-value";
//            } else {
//                // next token is a value, store the value
//                parameters[token.substr(2)] = nextToken;
//                i++;
//            }
//        }
//    }
//
//    return parameters;
//}

} /* namespace cx */
#endif //CX_USE_TSF

