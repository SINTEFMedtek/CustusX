#include "cxTubeSegmentationFilter.h"

#ifdef CX_USE_TSF
#include "tube-segmentation.hpp"
#include "openCLUtilities.hpp"
#include "tsf-config.h"

#include <vtkImageImport.h>
#include <vtkImageData.h>

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
void TubeSegmentationFilter::createOptions(QDomElement root)
{
 //TODO
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

bool TubeSegmentationFilter::execute()
{
    //get the image
    ssc::ImagePtr input = this->getCopiedInputImage();
    	if (!input)
    		return false;

	//======================================================
	char *argv[]{
		"/home/jbake/jbake/workspace/Tube-Segmenation-Framework/build_Debug/tubeSegmentation",
		"/home/jbake/jbake/data/helix/helix.mhd",
		"--mode",
		"white",
		"--storage-dir",
		"/home/jbake/jbake/workspace/",
		"--timing",
		"NULL"
	};
	int argc = sizeof argv/sizeof argv[0] - 1;

    // Parse parameters from program arguments
	boost::unordered_map<std::string, std::string> parameters = this->getParameters(argc, argv);

    // Write out parameter list
    std::cout << "The following parameters are set: " << std::endl;
    boost::unordered_map<std::string, std::string>::iterator it;
    for(it = parameters.begin(); it != parameters.end(); it++) {
    	std::cout << it->first << " " << it->second << std::endl;
    }

	OpenCL ocl;
	ocl.context = createCLContextFromArguments(argc, argv);

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

/**
 * Just needed for development/debugging!!!
 */
boost::unordered_map<std::string, std::string> TubeSegmentationFilter::getParameters(int argc, char ** argv) {
	boost::unordered_map<std::string, std::string> parameters;
    // Go through each parameter, first parameter is filename
    for(int i = 2; i < argc; i++) {
        std::string token = argv[i];
        if(token.substr(0,2) == "--") {
            // Check to see if the parameter has a value
            std::string nextToken;
            if(i+1 < argc) {
                nextToken = argv[i+1];
            } else {
                nextToken = "--";
            }
            if(nextToken.substr(0,2) == "--") {
                // next token is not a value
                parameters[token.substr(2)] = "dummy-value";
            } else {
                // next token is a value, store the value
                parameters[token.substr(2)] = nextToken;
                i++;
            }
        }
    }

    return parameters;
}

vtkImageDataPtr TubeSegmentationFilter::convertToVtkImageData(char * data, int size_x, int size_y, int size_z)
{
	int size[3]; // image dimension

	//======================================================
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarTypeToUnsignedChar();
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetImportVoidPointer((void*)data);
	imageImport->GetOutput()->Update();
	imageImport->Modified();
	//======================================================

	vtkImageDataPtr retval = imageImport->GetOutput();

	return retval;
}

} /* namespace cx */
#endif //CX_USE_TSF

