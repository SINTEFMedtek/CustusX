/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <algorithm>
#include <vector>
#include <vtkImageData.h>
#include "cxBinaryThinning3D.h"

namespace cxtest
{

namespace
{

struct Volume
{
	Volume(int x, int y, int z) :
		dimX(x), dimY(y), dimZ(z), voxels(static_cast<size_t>(x)*y*z, 0)
	{
	}
	int index(int x, int y, int z) const
	{
		return x + y*dimX + z*dimX*dimY;
	}
	bool inside(int x, int y, int z) const
	{
		return x >= 0 && y >= 0 && z >= 0 && x < dimX && y < dimY && z < dimZ;
	}
	bool isSet(int x, int y, int z) const
	{
		return this->inside(x, y, z) && voxels[this->index(x, y, z)] != 0;
	}
	void set(int x, int y, int z)
	{
		voxels[this->index(x, y, z)] = 1;
	}
	void fill(int x0, int x1, int y0, int y1, int z0, int z1)
	{
		for (int z = z0; z <= z1; ++z)
		{
			for (int y = y0; y <= y1; ++y)
			{
				for (int x = x0; x <= x1; ++x)
				{
					this->set(x, y, z);
				}
			}
		}
	}
	int count() const
	{
		return static_cast<int>(std::count(voxels.begin(), voxels.end(), 1));
	}
	void thin()
	{
		cx::BinaryThinning3D::thin(voxels, dimX, dimY, dimZ);
	}

	int numberOfNeighbours(int x, int y, int z) const
	{
		int retval = 0;
		for (int i = 0; i < 27; ++i)
		{
			if (i != 13 && this->isSet(x + i%3 - 1, y + (i/3)%3 - 1, z + i/9 - 1))
			{
				++retval;
			}
		}
		return retval;
	}

	int numberOfObjects() const
	{
		std::vector<bool> visited(voxels.size(), false);
		int retval = 0;
		for (size_t start = 0; start < voxels.size(); ++start)
		{
			if (voxels[start] && !visited[start])
			{
				++retval;
				this->visit(static_cast<int>(start), visited);
			}
		}
		return retval;
	}

	void visit(int start, std::vector<bool>& visited) const
	{
		std::vector<int> stack(1, start);
		visited[start] = true;
		while (!stack.empty())
		{
			const int current = stack.back();
			stack.pop_back();
			const int x = current % dimX;
			const int y = (current / dimX) % dimY;
			const int z = current / (dimX*dimY);
			for (int i = 0; i < 27; ++i)
			{
				const int nx = x + i%3 - 1;
				const int ny = y + (i/3)%3 - 1;
				const int nz = z + i/9 - 1;
				if (this->isSet(nx, ny, nz) && !visited[this->index(nx, ny, nz)])
				{
					visited[this->index(nx, ny, nz)] = true;
					stack.push_back(this->index(nx, ny, nz));
				}
			}
		}
	}

	int dimX;
	int dimY;
	int dimZ;
	std::vector<unsigned char> voxels;
};

} // namespace

TEST_CASE("BinaryThinning3D: empty volume stays empty", "[unit][resource][core][centerline]")
{
	Volume volume(5, 5, 5);
	volume.thin();
	CHECK(volume.count() == 0);
}

TEST_CASE("BinaryThinning3D: single voxel is kept, also at the volume border", "[unit][resource][core][centerline]")
{
	Volume volume(3, 3, 3);
	volume.set(0, 0, 0);
	volume.thin();
	CHECK(volume.count() == 1);
	CHECK(volume.isSet(0, 0, 0));
}

TEST_CASE("BinaryThinning3D: square rod becomes a line along its axis", "[unit][resource][core][centerline]")
{
	Volume volume(13, 5, 5);
	volume.fill(1, 11, 1, 3, 1, 3);
	volume.thin();

	REQUIRE(volume.count() >= 5);
	CHECK(volume.numberOfObjects() == 1);
	for (int z = 0; z < volume.dimZ; ++z)
	{
		for (int y = 0; y < volume.dimY; ++y)
		{
			for (int x = 0; x < volume.dimX; ++x)
			{
				if (volume.isSet(x, y, z))
				{
					CHECK(y == 2);
					CHECK(z == 2);
				}
			}
		}
	}
}

TEST_CASE("BinaryThinning3D: separate objects stay separate", "[unit][resource][core][centerline]")
{
	Volume volume(13, 11, 5);
	volume.fill(1, 11, 1, 3, 1, 3);
	volume.fill(1, 11, 7, 9, 1, 3);
	volume.thin();
	CHECK(volume.numberOfObjects() == 2);
}

TEST_CASE("BinaryThinning3D: thick ring becomes a closed loop", "[unit][resource][core][centerline]")
{
	Volume volume(15, 15, 5);
	volume.fill(1, 13, 1, 13, 1, 3);
	for (int z = 1; z <= 3; ++z)
	{
		for (int y = 4; y <= 10; ++y)
		{
			for (int x = 4; x <= 10; ++x)
			{
				volume.voxels[volume.index(x, y, z)] = 0;
			}
		}
	}
	volume.thin();

	REQUIRE(volume.count() > 0);
	CHECK(volume.numberOfObjects() == 1);
	for (int z = 0; z < volume.dimZ; ++z)
	{
		for (int y = 0; y < volume.dimY; ++y)
		{
			for (int x = 0; x < volume.dimX; ++x)
			{
				if (volume.isSet(x, y, z))
				{
					CHECK(volume.numberOfNeighbours(x, y, z) >= 2);
				}
			}
		}
	}
}

TEST_CASE("BinaryThinning3D: skeleton uses only the given label and keeps the geometry", "[unit][resource][core][centerline]")
{
	vtkImageDataPtr image = vtkImageDataPtr::New();
	image->SetExtent(2, 14, 1, 5, 0, 4);
	image->SetSpacing(0.5, 0.7, 1.25);
	image->SetOrigin(10, 20, 30);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	unsigned char* values = static_cast<unsigned char*>(image->GetScalarPointer());
	std::fill(values, values + 13*5*5, 0);
	for (int z = 1; z <= 3; ++z)
	{
		for (int y = 1; y <= 3; ++y)
		{
			for (int x = 1; x <= 11; ++x)
			{
				values[x + y*13 + z*13*5] = y == 1 ? 1 : 2;
			}
		}
	}

	vtkImageDataPtr skeleton = cx::BinaryThinning3D::skeleton(image, 2);

	CHECK(skeleton->GetScalarType() == VTK_SHORT);
	const int* extent = skeleton->GetExtent();
	CHECK(extent[0] == 0);
	CHECK(extent[1] == 12);
	CHECK(extent[2] == 0);
	CHECK(extent[3] == 4);
	CHECK(extent[4] == 0);
	CHECK(extent[5] == 4);
	CHECK(skeleton->GetSpacing()[2] == Approx(1.25));
	CHECK(skeleton->GetOrigin()[0] == Approx(11));
	CHECK(skeleton->GetOrigin()[1] == Approx(20.7));
	CHECK(skeleton->GetOrigin()[2] == Approx(30));

	const short* result = static_cast<const short*>(skeleton->GetScalarPointer());
	int numberOfSkeletonVoxels = 0;
	for (int i = 0; i < 13*5*5; ++i)
	{
		CHECK((result[i] == 0 || result[i] == 1));
		if (result[i] == 1)
		{
			++numberOfSkeletonVoxels;
			CHECK(values[i] == 2);
		}
	}
	CHECK(numberOfSkeletonVoxels > 0);
}

TEST_CASE("BinaryThinning3D: skeleton of a multi-component image uses the first component", "[unit][resource][core][centerline]")
{
	vtkImageDataPtr image = vtkImageDataPtr::New();
	image->SetExtent(0, 12, 0, 4, 0, 4);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 2);
	unsigned char* values = static_cast<unsigned char*>(image->GetScalarPointer());
	std::fill(values, values + 13*5*5*2, 1);
	for (int z = 1; z <= 3; ++z)
	{
		for (int y = 1; y <= 3; ++y)
		{
			for (int x = 1; x <= 11; ++x)
			{
				values[2*(x + y*13 + z*13*5)] = 0;
			}
		}
	}

	vtkImageDataPtr skeleton = cx::BinaryThinning3D::skeleton(image, 0);

	const short* result = static_cast<const short*>(skeleton->GetScalarPointer());
	int numberOfSkeletonVoxels = 0;
	for (int i = 0; i < 13*5*5; ++i)
	{
		if (result[i] == 1)
		{
			++numberOfSkeletonVoxels;
			CHECK(values[2*i] == 0);
		}
	}
	CHECK(numberOfSkeletonVoxels > 0);
}

} // namespace cxtest
