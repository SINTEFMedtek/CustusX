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
#include "sscTimeKeeper.h"
#include "sscUSFrameData.h"

namespace ssc
{
PNNReconstructAlgorithm::PNNReconstructAlgorithm()
{
}

std::vector<DataAdapterPtr> PNNReconstructAlgorithm::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	retval.push_back(this->getInterpolationStepsOption(root));
	return retval;
}

DoubleDataAdapterXmlPtr PNNReconstructAlgorithm::getInterpolationStepsOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval;
	retval = DoubleDataAdapterXml::initialize("interpolationSteps", "Distance (voxels)",
		"Interpolation steps in voxels", 3, ssc::DoubleRange(1, 10, 1), 0, root);
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

bool PNNReconstructAlgorithm::reconstruct(ProcessedUSInputDataPtr input,
		vtkImageDataPtr outputData, QDomElement settings)
{
	std::vector<TimedPosition> frameInfo = input->getFrames();
	//std::vector<Planes> planes = generate_planes(frameInfo, frameData);
	if (frameInfo.empty())
		return false;
	if (input->getDimensions()[2]==0)
		return false;

	//vtkImageDataPtr input = frameData->getBaseVtkImageData();
//	USFrameDataPtr input = frameData;
	vtkImageDataPtr target = outputData;

	Eigen::Array3i inputDims = input->getDimensions();

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
	unsigned char* maskPointer = static_cast<unsigned char*> (input->getMask()->getBaseVtkImageData()->GetScalarPointer());

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

					// assign the max value found from all frames hitting this voxel. This removes black areas where (some of) multiple sweeps contains shadows.
					outputPointer[outputIndex] = std::max<unsigned char>(inputPointer[inputIndex], outputPointer[outputIndex]);
					// set minimum intensity value to 1. This separates "zero intensity" from "no intensity".
					outputPointer[outputIndex] = std::max<unsigned char>(inputPointer[inputIndex], 1); //
				}//validVoxel

			}//sample
		}//beam
	}//record

	// Fill holes
	this->interpolate(tempOutputData, outputData, settings);

	return true;
}

namespace
{
/**Used in createMask()
 */
inline int getIndex_z_last(int x, int y, int z, const Eigen::Array3i& dim)
{
	return x + y*dim[0] + z*dim[0]*dim[1];
}
/**Used in createMask()
 */
inline int getIndex_x_last(int y, int z, int x, const Eigen::Array3i& dim)
{
	return x + y*dim[0] + z*dim[0]*dim[1];
}
/**Used in createMask()
 */
inline int getIndex_y_last(int z, int x, int y, const Eigen::Array3i& dim)
{
	return x + y*dim[0] + z*dim[0]*dim[1];
}


/**Used in createMask()
 *
 * Seach along a given dimension (x,y or z). Mask out
 * all values outside the first and last nonzero values.
 */
template <class FUNCTION>
void maskAlongDim(int a_dim, int b_dim, int c_dim, const Eigen::Array3i& dim, unsigned char *inputPtr, unsigned char *maskPtr, FUNCTION getIndex)
{
	for (int a = 0; a < a_dim; a++)
	{
		for (int b = 0; b < b_dim; b++)
		{
			int start = c_dim;
			int stop = -1;
			for (int c = 0; c < c_dim; c++)
			{
				int index = getIndex(a, b, c, dim);
				if (inputPtr[index]>0)
				{
					start = c;
					break;
				}
			}
			for (int c = c_dim-1; c >=0; c--)
			{
				int index = getIndex(a, b, c, dim);
				if (inputPtr[index]>0)
				{
					stop = c;
					break;
				}
			}
			for (int c = start; c <= stop; c++)
			{
				int index = getIndex(a, b, c, dim);
				maskPtr[index] = 1;
			}
		}
	}
}
} // unnamed namespace

/**Create a mask enclosing the data in input,
 * but excluding the outer zeroed parts.
 *
 * Use to exclude hole filling at the edges.
 *
 * Optimized code: Change with care!
 *
 */
vtkImageDataPtr PNNReconstructAlgorithm::createMask(vtkImageDataPtr inputData)
{
//	QTime startTime = QTime::currentTime();
	Eigen::Array3i dim(inputData->GetDimensions());
	ssc::Vector3D spacing(inputData->GetSpacing());
	vtkImageDataPtr mask = generateVtkImageData(dim, spacing, 0);
	unsigned char *inputPtr = static_cast<unsigned char*> (inputData->GetScalarPointer());
	unsigned char *maskPtr = static_cast<unsigned char*> (mask->GetScalarPointer());

	// mask along all 3 dimensions
	maskAlongDim(dim[0], dim[1], dim[2], dim, inputPtr, maskPtr, &getIndex_z_last);
	maskAlongDim(dim[1], dim[2], dim[0], dim, inputPtr, maskPtr, &getIndex_x_last);
	maskAlongDim(dim[2], dim[0], dim[1], dim, inputPtr, maskPtr, &getIndex_y_last);

//	std::cout << QString("mask generation: %1ms").arg(startTime.msecsTo(QTime::currentTime())) << std::endl;
	return mask;
}

void PNNReconstructAlgorithm::interpolate(ImagePtr inputData, vtkImageDataPtr outputData, QDomElement settings)
{
	TimeKeeper timer;
	DoubleDataAdapterXmlPtr interpolationStepsOption = this->getInterpolationStepsOption(settings);
	int interpolationSteps = static_cast<int> (interpolationStepsOption->getValue());

//	messageManager()->sendInfo(QString("PNN hole filling [steps=%1] ...").arg(interpolationSteps));

	vtkImageDataPtr input = inputData->getBaseVtkImageData();
	vtkImageDataPtr output = outputData;
	vtkImageDataPtr mask = this->createMask(input);

	//int* inputDims = input->GetDimensions();
	Eigen::Array3i outputDims(output->GetDimensions());

	Eigen::Array3i inputDims(input->GetDimensions());
	//ssc::Vector3D outputDims(output->GetDimensions());


	unsigned char *inputPointer = static_cast<unsigned char*> (input->GetScalarPointer());
	unsigned char *outputPointer = static_cast<unsigned char*> (output->GetScalarPointer());
	unsigned char *maskPointer = static_cast<unsigned char*> (mask->GetScalarPointer());

	if ((outputDims[0] != inputDims[0]) || (outputDims[1] != inputDims[1]) || (outputDims[2] != inputDims[2]))
		messageManager()->sendWarning("outputDims != inputDims. output: " + qstring_cast(outputDims[0]) + " "
			+ qstring_cast(outputDims[1]) + " " + qstring_cast(outputDims[2]) + " input: " + qstring_cast(inputDims[0])
			+ " " + qstring_cast(inputDims[1]) + " " + qstring_cast(inputDims[2]));

	//ssc::Vector3D spacing(output->GetSpacing());
	// Assume output spacing is equal in all directions
	//int interpolationSteps = static_cast<int>((mInterpolationDistanceOption->getValue() / spacing[0]) + 0.5);
//	DoubleDataAdapterXmlPtr interpolationStepsOption = this->getInterpolationStepsOption(settings);

//	int interpolationSteps = static_cast<int> (interpolationStepsOption->getValue());
//	messageManager()->sendInfo("interpolationSteps: " + qstring_cast(interpolationSteps));

	int total = outputDims[0] * outputDims[1] * outputDims[2];
	int removed = 0;
	int ignored = 0;
	// Traverse all voxels
	for (int x = 0; x < outputDims[0]; x++)
	{
		//messageManager()->sendDebug("x: " + string_cast(x));
		for (int y = 0; y < outputDims[1]; y++)
		{
			for (int z = 0; z < outputDims[2]; z++)
			{
				int outputIndex = x + y * outputDims[0] + z * outputDims[0] * outputDims[1];

				// ignore if outside volume of interest
				if (maskPointer[outputIndex]==0)
				{
					removed++;
				}
				// copy if value already exists
				else if (inputPointer[outputIndex]>0)
				{
					outputPointer[outputIndex] = inputPointer[outputIndex];
					ignored++;
				}
				// fill hole otherwise (empty space within the volume)
				else
				{
					this->fillHole(inputPointer, outputPointer, x, y, z, outputDims, interpolationSteps);
				}
			}//z
		}//y
//		std::cout << QString("[%1/%2]").arg(x).arg(outputDims[0]);
//		std::cout.flush();
	}//x
//	std::cout << std::endl;
	int valid = 100*double(ignored)/double(total);
	int outside = 100*double(removed)/double(total);
	int holes = 100*double(total-ignored-removed)/double(total);
	ssc::messageManager()->sendDebug(
				QString("PNN: Size: %1Mb, Valid voxels: %2\%, Outside mask: %3\%  Filled holes [steps=%4, %5s]: %6\%")
				.arg(total/1024/1024)
				.arg(valid)
				.arg(outside)
				.arg(interpolationSteps)
				.arg(timer.getElapsedSecondsAsString())
				.arg(holes));
}

/**Fill the empty voxel (x,y,z) with the average value of the surrounding box.
 * The box is as small a possible, up to a maximum of 2*interpolationSteps+1.
 *
 */
void PNNReconstructAlgorithm::fillHole(unsigned char *inputPointer, unsigned char *outputPointer, int x, int y, int z, const Eigen::Array3i& dim, int interpolationSteps)
{
	int outputIndex = x + y * dim[0] + z * dim[0] * dim[1];
//	this->fillHole(inputPointer, outputPointer, x, y, z, outputDims);
	bool interpolated = false;
	int localArea = 0;

	int count = 0;
	double tempVal = 0;

	do
	{
		for (int i = -localArea; i < localArea + 1; i++)
		{
			for (int j = -localArea; j < localArea + 1; j++)
			{
				for (int k = -localArea; k < localArea + 1; k++)
				{
					int localIndex = outputIndex + i + j*dim[0] + k*dim[0]*dim[1];

					if (validVoxel(x + i, y + j, z + k, dim.data()) && inputPointer[localIndex] > 0.1)
					{
						tempVal += inputPointer[localIndex];
						count++;
					}
				}//local voxel area
			}
		}

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
}

}//namespace
