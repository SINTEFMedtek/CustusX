/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxBinaryThinning3D.h"

#include <cstddef>
#include <cstdlib>
#include <algorithm>
#include <vtkImageData.h>
#include <vtkImageCast.h>

namespace cx
{

namespace
{

/** The 3x3x3 neighbourhood is indexed x fastest: index = (dx+1) + 3*(dy+1) + 9*(dz+1). */
const int neighbourhoodSize = 27;
const int centre = 13;

/** Euler characteristic change [Lee94] of an octant configuration n, indexed by n/2 (n is always odd). */
const int eulerLUT[128] =
{
	 1, -1, -1,  1, -3, -1, -1,  1, -1,  1,  1, -1,  3,  1,  1, -1,
	-3, -1,  3,  1,  1, -1,  3,  1, -1,  1,  1, -1,  3,  1,  1, -1,
	-3,  3, -1,  1,  1,  3, -1,  1, -1,  1,  1, -1,  3,  1,  1, -1,
	 1,  3,  3,  1,  5,  3,  3,  1, -1,  1,  1, -1,  3,  1,  1, -1,
	-7, -1, -1,  1, -3, -1, -1,  1, -1,  1,  1, -1,  3,  1,  1, -1,
	-3, -1,  3,  1,  1, -1,  3,  1, -1,  1,  1, -1,  3,  1,  1, -1,
	-3,  3, -1,  1,  1,  3, -1,  1, -1,  1,  1, -1,  3,  1,  1, -1,
	 1,  3,  3,  1,  5,  3,  3,  1, -1,  1,  1, -1,  3,  1,  1, -1
};

/** Neighbourhood indices of each octant, giving the bits 128, 64, ..., 2 of the Euler LUT index. */
const int eulerOctants[8][7] =
{
	{24, 25, 15, 16, 21, 22, 12}, // SWU
	{26, 23, 17, 14, 25, 22, 16}, // SEU
	{18, 21,  9, 12, 19, 22, 10}, // NWU
	{20, 23, 19, 22, 11, 14, 10}, // NEU
	{ 6, 15,  7, 16,  3, 12,  4}, // SWB
	{ 8,  7, 17, 16,  5,  4, 14}, // SEB
	{ 0,  9,  3, 12,  1, 10,  4}, // NWB
	{ 2,  1, 11, 10,  5,  4, 14}  // NEB
};

/** Neighbourhood index of the 6-neighbour checked for each border type, in the order N, S, E, W, U, B. */
const int borderNeighbours[6] = {10, 16, 14, 12, 22, 4};

bool isEulerInvariant(const unsigned char* neighbourhood)
{
	int eulerChar = 0;
	for (int octant = 0; octant < 8; ++octant)
	{
		int index = 1;
		for (int bit = 0; bit < 7; ++bit)
		{
			if (neighbourhood[eulerOctants[octant][bit]] == 1)
			{
				index |= 128 >> bit;
			}
		}
		eulerChar += eulerLUT[index >> 1];
	}
	return eulerChar == 0;
}

struct Adjacency
{
	Adjacency()
	{
		for (int i = 0; i < neighbourhoodSize; ++i)
		{
			for (int j = 0; j < neighbourhoodSize; ++j)
			{
				bool withinOneVoxel = std::abs(i%3 - j%3) <= 1 && std::abs((i/3)%3 - (j/3)%3) <= 1 && std::abs(i/9 - j/9) <= 1;
				adjacent[i][j] = withinOneVoxel && i != j && i != centre && j != centre;
			}
		}
	}
	bool adjacent[neighbourhoodSize][neighbourhoodSize];
};

const Adjacency& adjacency()
{
	static const Adjacency instance;
	return instance;
}

void visitComponent(const unsigned char* neighbourhood, int start, bool* visited)
{
	const Adjacency& graph = adjacency();
	int stack[neighbourhoodSize];
	int size = 0;
	stack[size++] = start;
	visited[start] = true;
	while (size > 0)
	{
		const int current = stack[--size];
		for (int other = 0; other < neighbourhoodSize; ++other)
		{
			if (graph.adjacent[current][other] && neighbourhood[other] == 1 && !visited[other])
			{
				visited[other] = true;
				stack[size++] = other;
			}
		}
	}
}

/** True if the foreground of the neighbourhood, without the centre, is at most one 26-connected object [Lee94]. */
bool isSimplePoint(const unsigned char* neighbourhood)
{
	bool visited[neighbourhoodSize] = {false};
	int objects = 0;
	for (int i = 0; i < neighbourhoodSize && objects < 2; ++i)
	{
		if (i != centre && neighbourhood[i] == 1 && !visited[i])
		{
			++objects;
			visitComponent(neighbourhood, i, visited);
		}
	}
	return objects < 2;
}

int countForeground(const unsigned char* neighbourhood)
{
	return static_cast<int>(std::count(neighbourhood, neighbourhood + neighbourhoodSize, 1));
}

/** Thinning on a copy of the volume padded with one background voxel on each side. */
class Thinner
{
public:
	Thinner(const std::vector<unsigned char>& voxels, int dimX, int dimY, int dimZ);
	void run();
	void copyTo(std::vector<unsigned char>& voxels) const;

private:
	std::ptrdiff_t paddedIndex(int x, int y, int z) const;
	void getNeighbourhood(std::ptrdiff_t index, unsigned char* neighbourhood) const;
	bool isDeletableBorderPoint(std::ptrdiff_t index, int borderNeighbour) const;
	std::vector<std::ptrdiff_t> findDeletableBorderPoints(int borderNeighbour) const;
	bool deleteSequentially(const std::vector<std::ptrdiff_t>& candidates);
	void removeDeletedFromForeground();

	int mDim[3];
	int mPaddedDim[3];
	std::ptrdiff_t mOffsets[neighbourhoodSize];
	std::vector<unsigned char> mImage;
	std::vector<std::ptrdiff_t> mForeground;
};

Thinner::Thinner(const std::vector<unsigned char>& voxels, int dimX, int dimY, int dimZ) :
	mDim{dimX, dimY, dimZ},
	mPaddedDim{dimX+2, dimY+2, dimZ+2}
{
	mImage.assign(static_cast<size_t>(mPaddedDim[0])*mPaddedDim[1]*mPaddedDim[2], 0);
	for (int i = 0; i < neighbourhoodSize; ++i)
	{
		mOffsets[i] = (i%3 - 1) + std::ptrdiff_t((i/3)%3 - 1)*mPaddedDim[0] + std::ptrdiff_t(i/9 - 1)*mPaddedDim[0]*mPaddedDim[1];
	}
	size_t source = 0;
	for (int z = 0; z < dimZ; ++z)
	{
		for (int y = 0; y < dimY; ++y)
		{
			for (int x = 0; x < dimX; ++x)
			{
				if (voxels[source++])
				{
					const std::ptrdiff_t index = this->paddedIndex(x, y, z);
					mImage[index] = 1;
					mForeground.push_back(index);
				}
			}
		}
	}
}

std::ptrdiff_t Thinner::paddedIndex(int x, int y, int z) const
{
	return (x+1) + std::ptrdiff_t(y+1)*mPaddedDim[0] + std::ptrdiff_t(z+1)*mPaddedDim[0]*mPaddedDim[1];
}

void Thinner::getNeighbourhood(std::ptrdiff_t index, unsigned char* neighbourhood) const
{
	for (int i = 0; i < neighbourhoodSize; ++i)
	{
		neighbourhood[i] = mImage[index + mOffsets[i]];
	}
}

/** A border point that is not the end of an arc, is Euler invariant and is simple [Lee94]. */
bool Thinner::isDeletableBorderPoint(std::ptrdiff_t index, int borderNeighbour) const
{
	unsigned char neighbourhood[neighbourhoodSize];
	this->getNeighbourhood(index, neighbourhood);
	const bool isEndOfArc = countForeground(neighbourhood) == 2;
	return neighbourhood[borderNeighbour] == 0
			&& !isEndOfArc
			&& isEulerInvariant(neighbourhood)
			&& isSimplePoint(neighbourhood);
}

std::vector<std::ptrdiff_t> Thinner::findDeletableBorderPoints(int borderNeighbour) const
{
	std::vector<std::ptrdiff_t> retval;
	for (const std::ptrdiff_t index : mForeground)
	{
		if (mImage[index] == 1 && this->isDeletableBorderPoint(index, borderNeighbour))
		{
			retval.push_back(index);
		}
	}
	return retval;
}

/** Delete the candidates one by one, keeping those whose deletion would now break connectivity. */
bool Thinner::deleteSequentially(const std::vector<std::ptrdiff_t>& candidates)
{
	bool changed = false;
	unsigned char neighbourhood[neighbourhoodSize];
	for (const std::ptrdiff_t index : candidates)
	{
		mImage[index] = 0;
		this->getNeighbourhood(index, neighbourhood);
		if (isSimplePoint(neighbourhood))
		{
			changed = true;
		}
		else
		{
			mImage[index] = 1;
		}
	}
	return changed;
}

void Thinner::removeDeletedFromForeground()
{
	const std::vector<unsigned char>& image = mImage;
	mForeground.erase(std::remove_if(mForeground.begin(), mForeground.end(),
									 [&image](std::ptrdiff_t index) { return image[index] == 0; }),
					  mForeground.end());
}

void Thinner::run()
{
	int unchangedBorders = 0;
	while (unchangedBorders < 6)
	{
		unchangedBorders = 0;
		for (int border = 0; border < 6; ++border)
		{
			const std::vector<std::ptrdiff_t> candidates = this->findDeletableBorderPoints(borderNeighbours[border]);
			if (this->deleteSequentially(candidates))
			{
				this->removeDeletedFromForeground();
			}
			else
			{
				++unchangedBorders;
			}
		}
	}
}

void Thinner::copyTo(std::vector<unsigned char>& voxels) const
{
	size_t target = 0;
	for (int z = 0; z < mDim[2]; ++z)
	{
		for (int y = 0; y < mDim[1]; ++y)
		{
			for (int x = 0; x < mDim[0]; ++x)
			{
				voxels[target++] = mImage[this->paddedIndex(x, y, z)];
			}
		}
	}
}

vtkImageDataPtr createSkeletonImage(vtkImageDataPtr image, const std::vector<unsigned char>& voxels)
{
	const int* extent = image->GetExtent();
	const double* spacing = image->GetSpacing();
	const double* origin = image->GetOrigin();
	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->SetExtent(0, extent[1]-extent[0], 0, extent[3]-extent[2], 0, extent[5]-extent[4]);
	retval->SetSpacing(spacing[0], spacing[1], spacing[2]);
	retval->SetOrigin(origin[0] + extent[0]*spacing[0], origin[1] + extent[2]*spacing[1], origin[2] + extent[4]*spacing[2]);
	retval->AllocateScalars(VTK_SHORT, 1);
	short* values = static_cast<short*>(retval->GetScalarPointer());
	std::copy(voxels.begin(), voxels.end(), values);
	return retval;
}

} // namespace

vtkImageDataPtr BinaryThinning3D::skeleton(vtkImageDataPtr image, int label)
{
	vtkImageCastPtr cast = vtkImageCastPtr::New();
	cast->SetInputData(image);
	cast->SetOutputScalarTypeToShort();
	cast->Update();
	vtkImageDataPtr shortImage = cast->GetOutput();

	const int* dims = shortImage->GetDimensions();
	const size_t numberOfVoxels = static_cast<size_t>(dims[0])*dims[1]*dims[2];
	const int components = shortImage->GetNumberOfScalarComponents();
	const short* values = static_cast<const short*>(shortImage->GetScalarPointer());
	std::vector<unsigned char> voxels(numberOfVoxels);
	for (size_t i = 0; i < numberOfVoxels; ++i)
	{
		voxels[i] = values[i*components] == label ? 1 : 0;
	}

	BinaryThinning3D::thin(voxels, dims[0], dims[1], dims[2]);
	return createSkeletonImage(shortImage, voxels);
}

void BinaryThinning3D::thin(std::vector<unsigned char>& voxels, int dimX, int dimY, int dimZ)
{
	Thinner thinner(voxels, dimX, dimY, dimZ);
	thinner.run();
	thinner.copyTo(voxels);
}

} // namespace cx
