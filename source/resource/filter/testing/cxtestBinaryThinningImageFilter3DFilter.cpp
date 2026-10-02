/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <limits>
#include <queue>
#include <set>

#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxImage.h"
#include "cxVector3D.h"
#include "cxtestVisServices.h"
#include "vesselReg/SeansVesselReg.hxx"

/** Level 1 tests for CustusX#54 (remove the ITK dependency): the centerline (thinning)
 *  filter on a synthetic airway tree, so that a replacement implementation can be
 *  checked against the current ITK based one without any test data.
 */
namespace cxtest
{

namespace
{

struct TubeSegment
{
	cx::Vector3D start;
	cx::Vector3D end;
	double radius;
};

/** Trachea, two main bronchi and four segment bronchi (world coordinates, mm).
 *  3 bifurcations and 5 end points (4 leaves + the top of the trachea).
 */
std::vector<TubeSegment> createAirwayTree()
{
	cx::Vector3D tracheaTop(32, 32, 75);
	cx::Vector3D carina(32, 32, 50);
	cx::Vector3D leftMain(18, 32, 35);
	cx::Vector3D rightMain(46, 32, 35);

	std::vector<TubeSegment> retval;
	retval.push_back({tracheaTop, carina, 4.0});
	retval.push_back({carina, leftMain, 3.0});
	retval.push_back({carina, rightMain, 3.0});
	retval.push_back({leftMain, cx::Vector3D(12, 24, 15), 2.0});
	retval.push_back({leftMain, cx::Vector3D(14, 42, 15), 2.0});
	retval.push_back({rightMain, cx::Vector3D(52, 24, 15), 2.0});
	retval.push_back({rightMain, cx::Vector3D(50, 42, 15), 2.0});
	return retval;
}

double distanceToSegment(const cx::Vector3D& p, const TubeSegment& segment)
{
	cx::Vector3D direction = segment.end - segment.start;
	double t = (p - segment.start).dot(direction) / direction.squaredNorm();
	t = std::max(0.0, std::min(1.0, t));
	cx::Vector3D closest = segment.start + t * direction;
	return (p - closest).norm();
}

double distanceToAxes(const cx::Vector3D& p, const std::vector<TubeSegment>& tree)
{
	double retval = std::numeric_limits<double>::max();
	for (const TubeSegment& segment : tree)
	{
		retval = std::min(retval, distanceToSegment(p, segment));
	}
	return retval;
}

/** Binary (0/1) volume covering x,y in [0,64] mm and z in [0,80] mm. */
vtkImageDataPtr createBinaryVolume(const std::vector<TubeSegment>& tree, cx::Vector3D spacing)
{
	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->SetSpacing(spacing.data());
	retval->SetOrigin(0, 0, 0);
	retval->SetDimensions(int(64 / spacing[0]) + 1, int(64 / spacing[1]) + 1, int(80 / spacing[2]) + 1);
	retval->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

	int* dims = retval->GetDimensions();
	unsigned char* voxels = static_cast<unsigned char*>(retval->GetScalarPointer());
	for (int k = 0; k < dims[2]; ++k)
	{
		for (int j = 0; j < dims[1]; ++j)
		{
			for (int i = 0; i < dims[0]; ++i)
			{
				cx::Vector3D p(i * spacing[0], j * spacing[1], k * spacing[2]);
				bool inside = false;
				for (const TubeSegment& segment : tree)
				{
					inside = inside || (distanceToSegment(p, segment) <= segment.radius);
				}
				voxels[i + dims[0] * (j + dims[1] * k)] = inside ? 1 : 0;
			}
		}
	}
	return retval;
}

/** Non-zero voxels as 0/1, independent of the scalar type of the volume. */
std::vector<unsigned char> toMask(vtkImageDataPtr volume)
{
	int* dims = volume->GetDimensions();
	std::vector<unsigned char> retval(size_t(dims[0]) * dims[1] * dims[2], 0);
	for (int k = 0; k < dims[2]; ++k)
	{
		for (int j = 0; j < dims[1]; ++j)
		{
			for (int i = 0; i < dims[0]; ++i)
			{
				bool nonZero = volume->GetScalarComponentAsDouble(i, j, k, 0) != 0;
				retval[i + dims[0] * (j + dims[1] * k)] = nonZero ? 1 : 0;
			}
		}
	}
	return retval;
}

/** Measures of a skeleton volume, used to compare implementations. */
struct SkeletonMeasures
{
	int voxels = 0;
	int voxelsOutsideInput = 0;
	int components = 0;
	int endPoints = 0;
	int junctions = 0; ///< clusters of voxels with 3 or more neighbours
	double maxDistanceToAxes_mm = 0;
	long long indexChecksum = 0; ///< sum of the linear indices of all skeleton voxels
};

std::vector<int> getNeighbours(int index, const int* dims, const unsigned char* voxels)
{
	std::vector<int> retval;
	int i = index % dims[0];
	int j = (index / dims[0]) % dims[1];
	int k = index / (dims[0] * dims[1]);
	for (int dk = -1; dk <= 1; ++dk)
	{
		for (int dj = -1; dj <= 1; ++dj)
		{
			for (int di = -1; di <= 1; ++di)
			{
				int ni = i + di;
				int nj = j + dj;
				int nk = k + dk;
				bool isSelf = (di == 0 && dj == 0 && dk == 0);
				bool inVolume = ni >= 0 && nj >= 0 && nk >= 0 && ni < dims[0] && nj < dims[1] && nk < dims[2];
				if (!isSelf && inVolume)
				{
					int neighbour = ni + dims[0] * (nj + dims[1] * nk);
					if (voxels[neighbour])
					{
						retval.push_back(neighbour);
					}
				}
			}
		}
	}
	return retval;
}

/** Number of 26-connected clusters among the voxels in the given set. */
int countClusters(const std::set<int>& indices, const int* dims, const unsigned char* voxels)
{
	int retval = 0;
	std::set<int> visited;
	for (int start : indices)
	{
		if (visited.count(start))
		{
			continue;
		}
		++retval;
		std::queue<int> queue;
		queue.push(start);
		visited.insert(start);
		while (!queue.empty())
		{
			int current = queue.front();
			queue.pop();
			for (int neighbour : getNeighbours(current, dims, voxels))
			{
				if (indices.count(neighbour) && !visited.count(neighbour))
				{
					visited.insert(neighbour);
					queue.push(neighbour);
				}
			}
		}
	}
	return retval;
}

SkeletonMeasures measureSkeleton(vtkImageDataPtr skeleton, vtkImageDataPtr input, const std::vector<TubeSegment>& tree)
{
	SkeletonMeasures retval;
	int* dims = skeleton->GetDimensions();
	double* spacing = skeleton->GetSpacing();
	std::vector<unsigned char> skeletonMask = toMask(skeleton);
	std::vector<unsigned char> inputMask = toMask(input);
	const unsigned char* voxels = skeletonMask.data();
	const unsigned char* inputVoxels = inputMask.data();
	int numberOfVoxels = dims[0] * dims[1] * dims[2];

	std::set<int> all;
	std::set<int> junctionVoxels;
	for (int index = 0; index < numberOfVoxels; ++index)
	{
		if (!voxels[index])
		{
			continue;
		}
		all.insert(index);
		retval.voxels++;
		retval.indexChecksum += index;
		if (!inputVoxels[index])
		{
			retval.voxelsOutsideInput++;
		}
		size_t neighbours = getNeighbours(index, dims, voxels).size();
		if (neighbours == 1)
		{
			retval.endPoints++;
		}
		if (neighbours >= 3)
		{
			junctionVoxels.insert(index);
		}
		int i = index % dims[0];
		int j = (index / dims[0]) % dims[1];
		int k = index / (dims[0] * dims[1]);
		cx::Vector3D p(i * spacing[0], j * spacing[1], k * spacing[2]);
		retval.maxDistanceToAxes_mm = std::max(retval.maxDistanceToAxes_mm, distanceToAxes(p, tree));
	}
	retval.components = countClusters(all, dims, voxels);
	retval.junctions = countClusters(junctionVoxels, dims, voxels);
	return retval;
}

std::vector<vtkImageDataPtr> runCenterlineFilter(vtkImageDataPtr volume, bool labeledVolume)
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::BinaryThinningImageFilter3DFilter filter(services);
	cx::ImagePtr image(new cx::Image("airways", volume, "airways"));
	return filter.execute(image, labeledVolume);
}

/** Exact output of the current (ITK based) implementation. A port of the same
 *  algorithm must reproduce it voxel by voxel. A different algorithm may change it,
 *  but must still pass the other checks in checkSkeletonOfAirwayTree().
 */
struct ReferenceSkeleton
{
	int voxels;
	long long indexChecksum;
};

void checkSkeletonOfAirwayTree(cx::Vector3D spacing, ReferenceSkeleton reference)
{
	std::vector<TubeSegment> tree = createAirwayTree();
	vtkImageDataPtr volume = createBinaryVolume(tree, spacing);

	std::vector<vtkImageDataPtr> result = runCenterlineFilter(volume, false);
	REQUIRE(result.size() == 1);
	vtkImageDataPtr skeleton = result[0];

	for (int d = 0; d < 3; ++d)
	{
		CHECK(skeleton->GetDimensions()[d] == volume->GetDimensions()[d]);
		CHECK(skeleton->GetSpacing()[d] == Approx(volume->GetSpacing()[d]));
	}

	SkeletonMeasures measures = measureSkeleton(skeleton, volume, tree);
	INFO("voxels: " << measures.voxels << ", checksum: " << measures.indexChecksum
		 << ", end points: " << measures.endPoints << ", junctions: " << measures.junctions
		 << ", max distance to axes: " << measures.maxDistanceToAxes_mm);
	CHECK(measures.voxels > 0);
	CHECK(measures.voxelsOutsideInput == 0);
	CHECK(measures.components == 1);
	CHECK(measures.endPoints == 5);
	CHECK(measures.junctions == 3);
	CHECK(measures.maxDistanceToAxes_mm < 2.0);

	CHECK(measures.voxels == reference.voxels);
	CHECK(measures.indexChecksum == reference.indexChecksum);
}

} // namespace

TEST_CASE("BinaryThinningImageFilter3DFilter: centerline of a synthetic airway tree", "[unit][resource][filter][centerline]")
{
	checkSkeletonOfAirwayTree(cx::Vector3D(1, 1, 1), {135, 20111148});
}

TEST_CASE("BinaryThinningImageFilter3DFilter: centerline of a synthetic airway tree with CT-like anisotropic spacing", "[unit][resource][filter][centerline]")
{
	checkSkeletonOfAirwayTree(cx::Vector3D(0.7, 0.7, 1.25), {116, 29010931});
}

TEST_CASE("BinaryThinningImageFilter3DFilter: labeled volume gives one centerline per label", "[unit][resource][filter][centerline]")
{
	std::vector<TubeSegment> left = {{cx::Vector3D(16, 32, 70), cx::Vector3D(16, 32, 10), 3.0}};
	std::vector<TubeSegment> right = {{cx::Vector3D(48, 32, 70), cx::Vector3D(48, 32, 10), 3.0}};
	vtkImageDataPtr volume = createBinaryVolume(left, cx::Vector3D(1, 1, 1));
	vtkImageDataPtr rightVolume = createBinaryVolume(right, cx::Vector3D(1, 1, 1));
	unsigned char* voxels = static_cast<unsigned char*>(volume->GetScalarPointer());
	const unsigned char* rightVoxels = static_cast<const unsigned char*>(rightVolume->GetScalarPointer());
	int numberOfVoxels = volume->GetNumberOfPoints();
	for (int index = 0; index < numberOfVoxels; ++index)
	{
		if (rightVoxels[index])
		{
			voxels[index] = 2;
		}
	}

	std::vector<vtkImageDataPtr> result = runCenterlineFilter(volume, true);
	REQUIRE(result.size() == 2);
	SkeletonMeasures leftMeasures = measureSkeleton(result[0], volume, left);
	SkeletonMeasures rightMeasures = measureSkeleton(result[1], volume, right);
	CHECK(leftMeasures.components == 1);
	CHECK(rightMeasures.components == 1);
	CHECK(leftMeasures.maxDistanceToAxes_mm < 2.0);
	CHECK(rightMeasures.maxDistanceToAxes_mm < 2.0);
}

TEST_CASE("BinaryThinningImageFilter3DFilter: non-binary input without labeled volume gives no centerline", "[unit][resource][filter][centerline]")
{
	std::vector<TubeSegment> tube = {{cx::Vector3D(32, 32, 70), cx::Vector3D(32, 32, 10), 3.0}};
	vtkImageDataPtr volume = createBinaryVolume(tube, cx::Vector3D(1, 1, 1));
	unsigned char* voxels = static_cast<unsigned char*>(volume->GetScalarPointer());
	voxels[0] = 2;

	CHECK(runCenterlineFilter(volume, false).empty());
}

TEST_CASE("BinaryThinningImageFilter3DFilter: centerline volume converts to a centerline mesh", "[unit][resource][filter][centerline]")
{
	std::vector<TubeSegment> tree = createAirwayTree();
	vtkImageDataPtr volume = createBinaryVolume(tree, cx::Vector3D(1, 1, 1));
	std::vector<vtkImageDataPtr> result = runCenterlineFilter(volume, false);
	REQUIRE(result.size() == 1);
	SkeletonMeasures measures = measureSkeleton(result[0], volume, tree);

	cx::ImagePtr skeletonImage(new cx::Image("skeleton", result[0], "skeleton"));
	vtkPolyDataPtr mesh = cx::SeansVesselReg::extractPolyData(skeletonImage, 1, nullptr);
	REQUIRE(mesh);
	CHECK(mesh->GetNumberOfPoints() == measures.voxels);
}

} // namespace cxtest
