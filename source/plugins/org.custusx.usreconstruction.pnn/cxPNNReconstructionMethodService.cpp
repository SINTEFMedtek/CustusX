/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPNNReconstructionMethodService.h"

#include <QFileInfo>
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxVolumeHelpers.h"
#include "cxTimeKeeper.h"
#include "cxUSFrameData.h"
#include <vtkImageData.h>
#include "cxImage.h"
#include "cxDoubleProperty.h"

namespace cx
{
PNNReconstructionMethodService::PNNReconstructionMethodService(ctkPluginContext* context)
{
}

PNNReconstructionMethodService::~PNNReconstructionMethodService()
{}

QString PNNReconstructionMethodService::getName() const
{
	return "pnn";
}

std::vector<PropertyPtr> PNNReconstructionMethodService::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;
	retval.push_back(this->getInterpolationStepsOption(root));
	return retval;
}

DoublePropertyPtr PNNReconstructionMethodService::getInterpolationStepsOption(QDomElement root)
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("interpolationSteps", "Distance (voxels)",
		"Interpolation steps in voxels", 3, DoubleRange(1, 10, 1), 0, root);
	return retval;
}

void optimizedCoordTransform(Vector3D* p, boost::array<double, 16> tt)
{
	double* t = tt.begin();
	double x = (*p)[0];
	double y = (*p)[1];
	double z = (*p)[2];
	(*p)[0] = t[0] * x + t[1] * y + t[2] * z + t[3];
	(*p)[1] = t[4] * x + t[5] * y + t[6] * z + t[7];
	(*p)[2] = t[8] * x + t[9] * y + t[10] * z + t[11];
}

bool PNNReconstructionMethodService::reconstruct(ProcessedUSInputDataPtr input,
		vtkImageDataPtr outputData, QDomElement settings)
{
	input->validate();

	std::vector<TimedPosition> frameInfo = input->getFrames();
	if (frameInfo.empty())
		return false;
	if (input->getDimensions()[2]==0)
		return false;

	vtkImageDataPtr target = outputData;

	Eigen::Array3i inputDims = input->getDimensions();

	Eigen::Array3i targetDims(target->GetDimensions());
	Vector3D targetSpacing(target->GetSpacing());

	//Create temporary volume
	vtkImageDataPtr tempOutput = generateVtkImageData(targetDims, targetSpacing, 0);
	ImagePtr tempOutputData = ImagePtr(new Image("tempOutput", tempOutput, "tempOutput"));

	int* outputDims = tempOutput->GetDimensions();
	//int* outputDims = target->GetDimensions();

	if (inputDims[2] != static_cast<int> (frameInfo.size()))
		reportWarning("inputDims[2] != frameInfo.size()" + qstring_cast(inputDims[2]) + " != "
			+ qstring_cast(frameInfo.size()));

	Vector3D inputSpacing(input->getSpacing());
	Vector3D outputSpacing(tempOutput->GetSpacing());

	//Get raw data pointers
	unsigned char *outputPointer = static_cast<unsigned char*> (tempOutput->GetScalarPointer());
	unsigned char* maskPointer = static_cast<unsigned char*> (input->getMask()->GetScalarPointer());

	// Traverse all input pixels
	for (int record = 0; record < inputDims[2]; record++)
	{
		unsigned char *inputPointer = input->getFrame(record);
		boost::array<double, 16> recordTransform = frameInfo[record].mPos.flatten();

		for (int beam = 0; beam < inputDims[0]; beam++)
		{
			for (int sample = 0; sample < inputDims[1]; sample++)
			{
				if (!validPixel(beam, sample, inputDims, maskPointer))
					continue;
				Vector3D inputPoint(beam * inputSpacing[0], sample * inputSpacing[1], 0.0);
				Vector3D outputPoint = inputPoint;
				optimizedCoordTransform(&outputPoint, recordTransform);
				int outputVoxelX = static_cast<int> ((outputPoint[0] / outputSpacing[0]) + 0.5);
				int outputVoxelY = static_cast<int> ((outputPoint[1] / outputSpacing[1]) + 0.5);
				int outputVoxelZ = static_cast<int> ((outputPoint[2] / outputSpacing[2]) + 0.5);

				if (validVoxel(outputVoxelX, outputVoxelY, outputVoxelZ, outputDims))
				{
					int outputIndex = outputVoxelX + outputVoxelY * outputDims[0] + outputVoxelZ * outputDims[0]
						* outputDims[1];
					int inputIndex = beam + sample * inputDims[0];

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

	setDeepModified(outputData);
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
vtkImageDataPtr PNNReconstructionMethodService::createMask(vtkImageDataPtr inputData)
{
	Eigen::Array3i dim(inputData->GetDimensions());
	Vector3D spacing(inputData->GetSpacing());
	vtkImageDataPtr mask = generateVtkImageData(dim, spacing, 0);
	unsigned char *inputPtr = static_cast<unsigned char*> (inputData->GetScalarPointer());
	unsigned char *maskPtr = static_cast<unsigned char*> (mask->GetScalarPointer());

	// mask along all 3 dimensions
	maskAlongDim(dim[0], dim[1], dim[2], dim, inputPtr, maskPtr, &getIndex_z_last);
	maskAlongDim(dim[1], dim[2], dim[0], dim, inputPtr, maskPtr, &getIndex_x_last);
	maskAlongDim(dim[2], dim[0], dim[1], dim, inputPtr, maskPtr, &getIndex_y_last);

	return mask;
}

void PNNReconstructionMethodService::interpolate(ImagePtr inputData, vtkImageDataPtr outputData, QDomElement settings)
{
	TimeKeeper timer;
	DoublePropertyPtr interpolationStepsOption = this->getInterpolationStepsOption(settings);
	int interpolationSteps = static_cast<int> (interpolationStepsOption->getValue());

	vtkImageDataPtr input = inputData->getBaseVtkImageData();
	vtkImageDataPtr output = outputData;
	vtkImageDataPtr mask = this->createMask(input);

	Eigen::Array3i outputDims(output->GetDimensions());

	Eigen::Array3i inputDims(input->GetDimensions());

	unsigned char *inputPointer = static_cast<unsigned char*> (input->GetScalarPointer());
	unsigned char *outputPointer = static_cast<unsigned char*> (output->GetScalarPointer());
	unsigned char *maskPointer = static_cast<unsigned char*> (mask->GetScalarPointer());

	if ((outputDims[0] != inputDims[0]) || (outputDims[1] != inputDims[1]) || (outputDims[2] != inputDims[2]))
		reportWarning("outputDims != inputDims. output: " + qstring_cast(outputDims[0]) + " "
			+ qstring_cast(outputDims[1]) + " " + qstring_cast(outputDims[2]) + " input: " + qstring_cast(inputDims[0])
			+ " " + qstring_cast(inputDims[1]) + " " + qstring_cast(inputDims[2]));

	int total = outputDims[0] * outputDims[1] * outputDims[2];
	int removed = 0;
	int ignored = 0;
	// Traverse all voxels
	for (int x = 0; x < outputDims[0]; x++)
	{
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
	}//x

	int valid = 100*double(ignored)/double(total);
	int outside = 100*double(removed)/double(total);
	int holes = 100*double(total-ignored-removed)/double(total);
	reportDebug(
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
void PNNReconstructionMethodService::fillHole(unsigned char *inputPointer, unsigned char *outputPointer, int x, int y, int z, const Eigen::Array3i& dim, int interpolationSteps)
{
	int outputIndex = x + y * dim[0] + z * dim[0] * dim[1];
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
