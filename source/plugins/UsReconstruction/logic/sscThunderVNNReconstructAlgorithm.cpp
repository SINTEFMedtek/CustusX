/*
 *  sscThunderVNNReconstructAlgorithm.cpp
 *  Created by Ole Vegard Solberg on 5/6/10.
 */

#include "sscThunderVNNReconstructAlgorithm.h"

#include <QFileInfo>
#include <vtkImageData.h>
#include "recConfig.h"
#ifdef USE_US_RECONSTRUCTION_THUNDER
#include "reconstruct_vnn.h"
#include "utils.h"
#endif // USE_US_RECONSTRUCTION_THUNDER
#include "sscImage.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace ssc
{
ThunderVNNReconstructAlgorithm::ThunderVNNReconstructAlgorithm(QString shaderPath)
{
	mShaderPath = shaderPath;
}

QString ThunderVNNReconstructAlgorithm::getName() const
{
	return "ThunderVNN";
}

std::vector<DataAdapterPtr> ThunderVNNReconstructAlgorithm::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;

#ifdef USE_US_RECONSTRUCTION_THUNDER
	QStringList processors;
	if (ocl_has_device_type("CPU"))
		processors << "CPU";
	if (ocl_has_device_type("GPU"))
		processors << "GPU";

	mProcessorOption = StringDataAdapterXml::initialize("Processor", "", "Which processor to use when reconstructing",
		processors[0], processors, root);
	mDistanceOption
		= DoubleDataAdapterXml::initialize("Distance (mm)", "",
			"Max distance from frame to voxel when filling output volume. mm.", 1, ssc::DoubleRange(0.1, 10, 0.01), 0,
			root);
	mPrintOpenCLInfoOption = BoolDataAdapterXml::initialize("Print OpenCL Info", "",
		"Query OpenCL and print info about CPU to stdout.", false, root);

	retval.push_back(mProcessorOption);
	retval.push_back(mDistanceOption);
	retval.push_back(mPrintOpenCLInfoOption);
#endif

	return retval;
}

void ThunderVNNReconstructAlgorithm::reconstruct(std::vector<TimedPosition> frameInfo,
	USFrameDataPtr frameData, ImagePtr outputData, ImagePtr frameMask, QDomElement settings)
{
#ifdef USE_US_RECONSTRUCTION_THUNDER
	std::cout << "processor: " << mProcessorOption->getValue() << std::endl;
	std::cout << "distance: " << mDistanceOption->getValue() << std::endl;

	if (mPrintOpenCLInfoOption->getValue())
	{
		std::cout << "Printing OpenCL info to stdout..." << std::endl;
		ocl_print_info();
	}

	QStringList paths;
	paths << mShaderPath << THUNDER_KERNEL_PATH << ".";

	QFileInfo path;
	path = QFileInfo(paths[0] + "/kernels.ocl");
	if (!path.exists())
		path = QFileInfo(paths[1] + QString("/kernels.ocl"));
	if (!path.exists())
		path = QFileInfo(paths[2] + "/kernels.ocl");
	if (!path.exists())
	{
		ssc::messageManager()->sendError("Error: Can't find kernels.ocl in any of\n  " + paths.join("  \n"));
		return;
	}

	reconstruct_data data;//TODO change
	//vtkImageDataPtr input = frameData->getBaseVtkImageData();
	//  USFrameDataPtr input = frameData;//TODO: Fix input

	data.frameData = frameData;
	//  data.input = static_cast<unsigned char*>(input->GetScalarPointer());
	//input->GetDimensions(data.input_dim);
	//input->GetSpacing(data.input_spacing);
	//data.input_dim = input->getDimensions();
	//data.input_spacing = input->GetSpacing();

	int* inputDims = frameData->getDimensions();
	//test
	//long size = data.input_dim[0]*data.input_dim[1]*data.input_dim[2];
	long size = inputDims[0] * inputDims[1] * inputDims[2];
	ssc::messageManager()->sendDebug(QString("Reconstruct data input size: %1 Mb").arg(double(size) / 1000 / 1000, 0,
		'f', 1));
	//                                   + qstring_cast(double(size)/1000/1000) + "MB");
	//ssc::messageManager()->sendInfo("input dimensions: "
	//                                + string_cast(data.input_dim[0]) + " "
	//                                + string_cast(data.input_dim[1]) + " "
	//                                + string_cast(data.input_dim[2]));
	//for (int i = 0; i < size; i++)
	//  data.input[i]=255;

	data.input_pos_matrices = new double[frameInfo.size() * 12];
	for (unsigned int i = 0; i < frameInfo.size(); i++)
	{
		boost::array<double, 16> m = frameInfo[i].mPos.flatten();
		for (int j = 0; j < 12; j++)
			data.input_pos_matrices[12 * i + j] = m[j];
	}

	vtkImageDataPtr input_mask = frameMask->getBaseVtkImageData();
	data.input_mask = static_cast<unsigned char*> (input_mask->GetScalarPointer());
	//  data.frameMask = frameMask;

	vtkImageDataPtr output = outputData->getBaseVtkImageData();
	data.output = static_cast<unsigned char*> (output->GetScalarPointer());
	//  data.outputData = outputData;
	output->GetDimensions(data.output_dim);
	output->GetSpacing(data.output_spacing);

	reconstruct_vnn(&data, path.absoluteFilePath().toStdString().c_str(), mProcessorOption->getValue(),
		static_cast<float> (mDistanceOption->getValue()));
	//ssc::messageManager()->sendInfo("ThunderVNNReconstructAlgorithm::reconstruct ***success***");
#endif // USE_US_RECONSTRUCTION_THUNDER
}

}//namespace
