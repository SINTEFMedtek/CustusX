// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 *  sscThunderVNNReconstructAlgorithm.cpp
 *  Created by Ole Vegard Solberg on 5/6/10.
 */

#include "sscThunderVNNReconstructAlgorithm.h"

#include <QFileInfo>
#include <vtkImageData.h>
#include "recConfig.h"
#include "sscImage.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscUSFrameData.h"

#ifdef SSC_USE_OpenCL
#include "reconstruct_vnn.h"
#include "Thunder/utils.h"
#endif // SSC_USE_OpenCL

namespace ssc
{

ReconstructAlgorithmPtr ThunderVNNReconstructAlgorithm::create(QString shaderPath)
{
#ifdef SSC_USE_OpenCL
	return ReconstructAlgorithmPtr(new ThunderVNNReconstructAlgorithm(shaderPath));
#else
	return ReconstructAlgorithmPtr();
#endif
}

ThunderVNNReconstructAlgorithm::~ThunderVNNReconstructAlgorithm()
{
}

ThunderVNNReconstructAlgorithm::ThunderVNNReconstructAlgorithm(QString shaderPath)
{
	mShaderPath = shaderPath;
}

QString ThunderVNNReconstructAlgorithm::getName() const
{
#ifdef SSC_USE_OpenCL
	return "ThunderVNN";
#else
	return "";
#endif
}

std::vector<DataAdapterPtr> ThunderVNNReconstructAlgorithm::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
#ifdef SSC_USE_OpenCL
	retval.push_back(this->getProcessorOption(root));
	retval.push_back(this->getDistanceOption(root));
	retval.push_back(this->getPrintOpenCLInfoOption(root));
#endif
	return retval;
}

StringDataAdapterXmlPtr ThunderVNNReconstructAlgorithm::getProcessorOption(QDomElement root)
{
	QStringList processors;
#ifdef SSC_USE_OpenCL
	if (ocl_has_device_type("CPU"))
		processors << "CPU";
	if (ocl_has_device_type("GPU"))
		processors << "GPU";
#endif
	return StringDataAdapterXml::initialize("Processor", "", "Which processor to use when reconstructing",
		processors[0], processors, root);
}

DoubleDataAdapterXmlPtr ThunderVNNReconstructAlgorithm::getDistanceOption(QDomElement root)
{
	return  DoubleDataAdapterXml::initialize("Distance (mm)", "",
            "Max distance from frame to voxel when filling output volume. mm.", 1, ssc::DoubleRange(0.1, 10, 0.1), 1,
			root);
}

BoolDataAdapterPtr ThunderVNNReconstructAlgorithm::getPrintOpenCLInfoOption(QDomElement root)
{
	return BoolDataAdapterXml::initialize("Print OpenCL Info", "",
		"Query OpenCL and print info about CPU to stdout.", false, root);
}

bool ThunderVNNReconstructAlgorithm::reconstruct(ProcessedUSInputDataPtr input,
												 vtkImageDataPtr outputData, QDomElement settings)
{
	bool success = false;


	if (input->getFrames().empty())
		return false;
	if (input->getDimensions()[2]==0)
		return false;

#ifdef SSC_USE_OpenCL
	std::cout << "processor: " << this->getProcessorOption(settings)->getValue() << std::endl;
	std::cout << "distance: " << this->getDistanceOption(settings)->getValue() << std::endl;

	if (this->getPrintOpenCLInfoOption(settings)->getValue())
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
		return false;
	}

	reconstruct_data data;//TODO change
	//vtkImageDataPtr input = frameData->getBaseVtkImageData();
	//  USFrameDataPtr input = frameData;//TODO: Fix input

	data.frameData = input;
	//  data.input = static_cast<unsigned char*>(input->GetScalarPointer());
	//input->GetDimensions(data.input_dim);
	//input->GetSpacing(data.input_spacing);
	//data.input_dim = input->getDimensions();
	//data.input_spacing = input->GetSpacing();

	Eigen::Array3i inputDims = input->getDimensions();
	//test
	//long size = data.input_dim[0]*data.input_dim[1]*data.input_dim[2];
	double size = double(inputDims[0] * inputDims[1] * inputDims[2]) / 1024 / 1024;
	ssc::messageManager()->sendDebug(QString("Reconstruct data input size: %1 Mb (compare to CL_DEVICE_GLOBAL_MEM_SIZE)").arg(size, 0, 'f', 1));
	ssc::messageManager()->sendDebug(QString("Reconstruct data chunk size: %1 Mb (compare to CL_DEVICE_MAX_MEM_ALLOC_SIZE)").arg(size/10, 0, 'f', 1));
	//                                   + qstring_cast(double(size)/1000/1000) + "MB");
	//ssc::messageManager()->sendInfo("input dimensions: "
	//                                + string_cast(data.input_dim[0]) + " "
	//                                + string_cast(data.input_dim[1]) + " "
	//                                + string_cast(data.input_dim[2]));
	//for (int i = 0; i < size; i++)
	//  data.input[i]=255;

	std::vector<ssc::TimedPosition> frameInfo = input->getFrames();
	data.input_pos_matrices = new double[frameInfo.size() * 12];
	for (unsigned int i = 0; i < frameInfo.size(); i++)
	{
		boost::array<double, 16> m = frameInfo[i].mPos.flatten();
		for (int j = 0; j < 12; j++)
			data.input_pos_matrices[12 * i + j] = m[j];
	}

	vtkImageDataPtr input_mask = input->getMask()->getBaseVtkImageData();
	data.input_mask = static_cast<unsigned char*> (input_mask->GetScalarPointer());
	//  data.frameMask = frameMask;

	vtkImageDataPtr output = outputData;
	data.output = static_cast<unsigned char*> (output->GetScalarPointer());
	//  data.outputData = outputData;
	output->GetDimensions(data.output_dim);
	output->GetSpacing(data.output_spacing);

	success = reconstruct_vnn(&data,
			path.absoluteFilePath().toStdString().c_str(),
			this->getProcessorOption(settings)->getValue(),
		static_cast<float> (this->getDistanceOption(settings)->getValue()));
	//ssc::messageManager()->sendInfo("ThunderVNNReconstructAlgorithm::reconstruct ***success***");
#endif // SSC_USE_OpenCL
	return success;
}

}//namespace
