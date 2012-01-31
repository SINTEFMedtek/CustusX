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

#include "sscPNNReconstructAlgorithm.h"

#include <QFileInfo>
#include "recConfig.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscVolumeHelpers.h"

namespace ssc
{
PNNReconstructAlgorithm::PNNReconstructAlgorithm()
{
}

std::vector<DataAdapterPtr> PNNReconstructAlgorithm::getSettings(QDomElement root)
{
	//	mProcessorOption = StringOptionItem::initialize("Processor", "",
	//		      "Which processor to use when reconstructing",
	//		      "CPU", QString("CPU GPU").split(" "),
	//		      root);
	//	mInterpolationDistanceOption = DoubleDataAdapterXml::initialize("Distance (mm)", "",
	//		      "Interpolation distance in mm",
	//		      0.5, ssc::DoubleRange(0.1, 10, 0.01), 0,
	//          root);
	mInterpolationStepsOption = DoubleDataAdapterXml::initialize("Distance (voxels)", "",
		"Interpolation steps in voxels", 3, ssc::DoubleRange(1, 10, 1), 0, root);

	std::vector<DataAdapterPtr> retval;
	//	retval.push_back(mProcessorOption);
	//	retval.push_back(mInterpolationDistanceOption);
	retval.push_back(mInterpolationStepsOption);
	return retval;
}

void optimizedCoordTransform(ssc::Vector3D* p, boost::array<double, 16> tt)
{
	double* t = tt.begin();
	double x = (*p)[0];
	double y = (*p)[1];
	double z = (*p)[2];
	(*p)[0] = t[0] * x + t[1] * y + t[2] * z + t[3];
	(*p)[1] = t[4] * x + t[5] * y + t[6] * z + t[7];
	(*p)[2] = t[8] * x + t[9] * y + t[10] * z + t[11];
}

void PNNReconstructAlgorithm::reconstruct(std::vector<TimedPosition> frameInfo, USFrameDataPtr frameData,
	ImagePtr outputData, ImagePtr frameMask, QDomElement settings)
{
	//std::vector<Planes> planes = generate_planes(frameInfo, frameData);

	//vtkImageDataPtr input = frameData->getBaseVtkImageData();
	USFrameDataPtr input = frameData;
	vtkImageDataPtr target = outputData->getBaseVtkImageData();

	int* inputDims = frameData->getDimensions();

	Eigen::Array3i targetDims(target->GetDimensions());
	ssc::Vector3D targetSpacing(target->GetSpacing());

	//Create temporary volume
	vtkImageDataPtr tempOutput = generateVtkImageData(targetDims, targetSpacing, 0);
	ImagePtr tempOutputData = ImagePtr(new Image("tempOutput", tempOutput, "tempOutput"));

	int* outputDims = tempOutput->GetDimensions();
	//int* outputDims = target->GetDimensions();

	if (inputDims[2] != static_cast<int> (frameInfo.size()))
		messageManager()->sendWarning("inputDims[2] != frameInfo.size()" + qstring_cast(inputDims[2]) + " != "
			+ qstring_cast(frameInfo.size()));

	ssc::Vector3D inputSpacing(input->getSpacing());
	ssc::Vector3D outputSpacing(tempOutput->GetSpacing());

	//Get raw data pointers
	//unsigned char *inputPointer = static_cast<unsigned char*>( input->GetScalarPointer() );
	unsigned char *outputPointer = static_cast<unsigned char*> (tempOutput->GetScalarPointer());
	//unsigned char *outputPointer = static_cast<unsigned char*>(target->GetScalarPointer());
	unsigned char* maskPointer = static_cast<unsigned char*> (frameMask->getBaseVtkImageData()->GetScalarPointer());

	// Traverse all input pixels
	for (int record = 0; record < inputDims[2]; record++)
	{
		unsigned char *inputPointer = input->getFrame(record);
		//messageManager()->sendDebug("record: " + string_cast(record));
		boost::array<double, 16> recordTransform = frameInfo[record].mPos.flatten();

		for (int beam = 0; beam < inputDims[0]; beam++)
		{
			for (int sample = 0; sample < inputDims[1]; sample++)
			{
				if (!validPixel(beam, sample, inputDims, maskPointer))
					continue;
				ssc::Vector3D inputPoint(beam * inputSpacing[0], sample * inputSpacing[1], 0.0);
				//ssc::Vector3D outputPoint = frameInfo[record].mPos.coord(inputPoint);
				ssc::Vector3D outputPoint = inputPoint;
				optimizedCoordTransform(&outputPoint, recordTransform);
				//ssc::Vector3D outputVoxel;
				int outputVoxelX = static_cast<int> ((outputPoint[0] / outputSpacing[0]) + 0.5);
				int outputVoxelY = static_cast<int> ((outputPoint[1] / outputSpacing[1]) + 0.5);
				int outputVoxelZ = static_cast<int> ((outputPoint[2] / outputSpacing[2]) + 0.5);

				if (validVoxel(outputVoxelX, outputVoxelY, outputVoxelZ, outputDims))
				{
					int outputIndex = outputVoxelX + outputVoxelY * outputDims[0] + outputVoxelZ * outputDims[0]
						* outputDims[1];
					int inputIndex = beam + sample * inputDims[0];
					//+ record*inputDims[0]*inputDims[1];//get new pointer for each record
//					outputPointer[outputIndex] = inputPointer[inputIndex];
					outputPointer[outputIndex] = std::max<unsigned char>(inputPointer[inputIndex], 1); //  // set minimum intensity value to 1. This separates "zero intensity" from "no intensity".
				}//validVoxel

			}//sample
		}//beam
	}//record

	// Fill holes
	this->interpolate(tempOutputData, outputData);
}

void PNNReconstructAlgorithm::interpolate(ImagePtr inputData, ImagePtr outputData)
{
	messageManager()->sendInfo("Interpolating...");

	vtkImageDataPtr input = inputData->getBaseVtkImageData();
	vtkImageDataPtr output = outputData->getBaseVtkImageData();

	//int* inputDims = input->GetDimensions();
	Eigen::Array3i outputDims(output->GetDimensions());

	Eigen::Array3i inputDims(input->GetDimensions());
	//ssc::Vector3D outputDims(output->GetDimensions());


	unsigned char *inputPointer = static_cast<unsigned char*> (input->GetScalarPointer());
	unsigned char *outputPointer = static_cast<unsigned char*> (output->GetScalarPointer());

	if ((outputDims[0] != inputDims[0]) || (outputDims[1] != inputDims[1]) || (outputDims[2] != inputDims[2]))
		messageManager()->sendWarning("outputDims != inputDims. output: " + qstring_cast(outputDims[0]) + " "
			+ qstring_cast(outputDims[1]) + " " + qstring_cast(outputDims[2]) + " input: " + qstring_cast(inputDims[0])
			+ " " + qstring_cast(inputDims[1]) + " " + qstring_cast(inputDims[2]));

	//ssc::Vector3D spacing(output->GetSpacing());
	// Assume output spacing is equal in all directions
	//int interpolationSteps = static_cast<int>((mInterpolationDistanceOption->getValue() / spacing[0]) + 0.5);
	int interpolationSteps = static_cast<int> (mInterpolationStepsOption->getValue());
	messageManager()->sendInfo("interpolationSteps: " + qstring_cast(interpolationSteps));

	// Traverse all voxels
	for (int x = 0; x < outputDims[0]; x++)
	{
		//messageManager()->sendDebug("x: " + string_cast(x));
		for (int y = 0; y < outputDims[1]; y++)
		{
			for (int z = 0; z < outputDims[2]; z++)
			{
				int outputIndex = x + y * outputDims[0] + z * outputDims[0] * outputDims[1];
				bool interpolated = false;
				int localArea = 0;

				int count = 0;
				double tempVal = 0;
				do
				{
					for (int i = -localArea; i < localArea + 1; i++)
						for (int j = -localArea; j < localArea + 1; j++)
							for (int k = -localArea; k < localArea + 1; k++)
							{
								//optimize? - The if is to expensive
								/*if((i == -localArea || i == localArea ||
								 j == -localArea || j == localArea ||
								 k == -localArea || k == localArea))*/
								//continue;
								/*if(i != -localArea || i != localArea ||
								 j != -localArea || j != localArea ||
								 k != -localArea || k != localArea)*/
								{

									int localIndex = outputIndex + i + j * outputDims[0] + k * outputDims[0]
										* outputDims[1];

									if (validVoxel(x + i, y + j, z + k, outputDims.data()) && inputPointer[localIndex]
										> 0.1)
									{
										tempVal += inputPointer[localIndex];
										count++;
									}
								}// if optimize
							}//local voxel area
					if (count > 0)
					{
						interpolated = true;
						if (tempVal == 0)
						{
							// keep noneness of index
						}
						else
						{
							outputPointer[outputIndex] = static_cast<int> ((tempVal / count) + 0.5);
							outputPointer[outputIndex] = std::max<unsigned char>(1, outputPointer[outputIndex]);
						}
					}
					localArea++;
				} while (localArea <= interpolationSteps && !interpolated);

			}//z
		}//y
	}//x
}

}//namespace
