/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImageAlgorithms.h"

#include <algorithm>
#include <cmath>
#include <vtkImageData.h>
#include <vtkImageAccumulate.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>

#include <vtkImageClip.h>
#include <vtkImageChangeInformation.h>

#include "cxImage.h"
#include "cxDefinitions.h"
#include "cxPatientModelService.h"
#include "cxUtilHelpers.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxRegistrationTransform.h"

#include "cxTime.h"
#include "cxVolumeHelpers.h"

#include "cxSlicedImageProxy.h"
#include "cxSliceProxy.h"
#include "cxLogger.h"
#include "cxEnumConversion.h"


namespace cx
{

/** Return an image that is resampled into space q.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImage(PatientModelServicePtr dataManager, ImagePtr image, Transform3D qMd)
{
	//TODO: fix error:
	// There is an error in the transfer functions of the returned image from this function

	// provide a resampled volume for algorithms requiring that (such as PickerRep)
	vtkMatrix4x4Ptr orientatorMatrix = vtkMatrix4x4Ptr::New();
	vtkImageReslicePtr orientator = vtkImageReslicePtr::New();
	orientator->SetInputData(image->getBaseVtkImageData());
	orientator->SetInterpolationModeToLinear();
	orientator->SetOutputDimensionality(3);
	orientator->SetResliceAxes(qMd.inv().getVtkMatrix());
	orientator->AutoCropOutputOn();
	orientator->Update();
	vtkImageDataPtr rawResult = orientator->GetOutput();

//  rawResult->Update();

	QString uid = image->getUid() + "_or%1";
	QString name = image->getName()+" or%1";
//  ImagePtr oriented = dataManager->createDerivedImage(rawResult, uid, name, image);

	ImagePtr oriented = createDerivedImage(dataManager,
										 uid, name,
										 rawResult, image);

	oriented->get_rMd_History()->setRegistration(image->get_rMd() * qMd.inv());
	oriented->mergevtkSettingsIntosscTransform();

	return oriented;
}

/** Return an image that is resampled with a new output spacing.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImage(PatientModelServicePtr dataManager, ImagePtr image, const Vector3D spacing, QString uid, QString name)
{
	vtkImageDataPtr input = image->getBaseVtkImageData();
	double* inputOrigin = input->GetOrigin();
	double* inputSpacing = input->GetSpacing();
	int* inputDims = input->GetDimensions();

	int outputExtent[6];
	double outputOrigin[3];
	for (int axis = 0; axis < 3; ++axis)
	{
		double physicalExtent = (inputDims[axis] - 1) * inputSpacing[axis];
		int outputDim = std::max(1, static_cast<int>(std::round(physicalExtent / spacing[axis])) + 1);
		double outputPhysicalExtent = (outputDim - 1) * spacing[axis];
		double slack = physicalExtent - outputPhysicalExtent;

		outputExtent[axis*2] = 0;
		outputExtent[axis*2+1] = outputDim - 1;
		outputOrigin[axis] = inputOrigin[axis] + slack / 2.0;
	}

	vtkImageReslicePtr resampler = vtkImageReslicePtr::New();
	resampler->SetInputData(input);
	resampler->SetInterpolationModeToLinear();
	resampler->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
	resampler->SetOutputOrigin(outputOrigin);
	resampler->SetOutputExtent(outputExtent);
	resampler->Update();
	vtkImageDataPtr rawResult = resampler->GetOutput();

	if (uid.isEmpty())
	{
		uid = image->getUid() + "_res%1";
		name = image->getName()+" res%1";
	}

	ImagePtr retval = createDerivedImage(dataManager,
										 uid, name,
										 rawResult, image);
	return retval;
}

/** Return an image resampled so its in-plane (x/y) resolution is capped at
 *  maxInPlaneDimension pixels, keeping the z spacing unchanged.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImageToMaxInPlaneResolution(PatientModelServicePtr dataManager, ImagePtr image, int maxInPlaneDimension, QString uid, QString name)
{
	vtkImageDataPtr vtkImageGrayscale = image->getGrayScaleVtkImageData();
	if (!vtkImageGrayscale)
		return image;

	double* spacing = vtkImageGrayscale->GetSpacing();
	int* dim = vtkImageGrayscale->GetDimensions();

	Vector3D newSpacing;
	newSpacing[0] = (double)dim[0] / maxInPlaneDimension * spacing[0];
	newSpacing[1] = (double)dim[1] / maxInPlaneDimension * spacing[1];
	newSpacing[2] = spacing[2];

	return resampleImage(dataManager, image, newSpacing, uid, name);
}

/** Return an image resampled so its total voxel count is capped at maxVoxelCount,
 *  scaling all three axes uniformly. Returns the input unchanged if it is already
 *  at or below maxVoxelCount.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImageToMaxVoxelCount(PatientModelServicePtr dataManager, ImagePtr image, double maxVoxelCount, QString uid, QString name)
{
	vtkImageDataPtr vtkImageGrayscale = image->getGrayScaleVtkImageData();
	if (!vtkImageGrayscale)
		return image;

	double* spacing = vtkImageGrayscale->GetSpacing();
	int* dim = vtkImageGrayscale->GetDimensions();
	double voxelCount = (double)dim[0] * dim[1] * dim[2];
	if (voxelCount <= maxVoxelCount)
		return image;

	double scale = std::cbrt(voxelCount / maxVoxelCount);
	Vector3D newSpacing;
	newSpacing[0] = spacing[0] * scale;
	newSpacing[1] = spacing[1] * scale;
	newSpacing[2] = spacing[2] * scale;

	return resampleImage(dataManager, image, newSpacing, uid, name);
}

/** Otsu's method: given a histogram, find the threshold (bin index) that
 *  maximizes the between-class variance of the two classes it splits the
 *  histogram into. Standard textbook algorithm.
 */
namespace
{
int otsuThresholdBin(const std::vector<double>& histogram)
{
	double total = 0;
	for (size_t i = 0; i < histogram.size(); ++i)
		total += histogram[i];
	if (total <= 0)
		return 0;

	double sumAll = 0;
	for (size_t i = 0; i < histogram.size(); ++i)
		sumAll += i * histogram[i];

	double weightBackground = 0;
	double sumBackground = 0;
	double bestVariance = -1;
	int bestBin = 0;

	for (size_t i = 0; i < histogram.size(); ++i)
	{
		weightBackground += histogram[i];
		if (weightBackground <= 0 || weightBackground >= total)
			continue;
		sumBackground += i * histogram[i];

		double weightForeground = total - weightBackground;
		double meanBackground = sumBackground / weightBackground;
		double meanForeground = (sumAll - sumBackground) / weightForeground;
		double meanDiff = meanBackground - meanForeground;
		double variance = weightBackground * weightForeground * meanDiff * meanDiff;

		if (variance > bestVariance)
		{
			bestVariance = variance;
			bestBin = static_cast<int>(i);
		}
	}
	return bestBin;
}
} // namespace

double computeOtsuThreshold(vtkImageDataPtr image)
{
	double range[2];
	image->GetScalarRange(range);
	if (range[1] <= range[0])
		return range[0];

	const int numBins = 256;
	vtkSmartPointer<vtkImageAccumulate> histogramFilter = vtkSmartPointer<vtkImageAccumulate>::New();
	histogramFilter->SetInputData(image);
	histogramFilter->SetComponentExtent(0, numBins - 1, 0, 0, 0, 0);
	histogramFilter->SetComponentOrigin(range[0], 0, 0);
	histogramFilter->SetComponentSpacing((range[1] - range[0]) / numBins, 0, 0);
	histogramFilter->Update();

	vtkImageDataPtr histogramImage = histogramFilter->GetOutput();
	std::vector<double> histogram(numBins);
	for (int i = 0; i < numBins; ++i)
		histogram[i] = histogramImage->GetScalarComponentAsDouble(i, 0, 0, 0);

	int bin = otsuThresholdBin(histogram);
	return range[0] + (bin + 0.5) * (range[1] - range[0]) / numBins;
}

namespace
{
/** Scan the raw voxel buffer for the tight index range containing all
 *  voxels at or above threshold. bounds is {xmin,xmax,ymin,ymax,zmin,zmax};
 *  a xmin>xmax (etc.) result on return means nothing matched.
 */
template <class T>
void findThresholdVoxelBounds(T* data, const int dims[3], double threshold, int bounds[6])
{
	bounds[0] = dims[0]; bounds[1] = -1;
	bounds[2] = dims[1]; bounds[3] = -1;
	bounds[4] = dims[2]; bounds[5] = -1;

	vtkIdType idx = 0;
	for (int z = 0; z < dims[2]; ++z)
	{
		for (int y = 0; y < dims[1]; ++y)
		{
			for (int x = 0; x < dims[0]; ++x, ++idx)
			{
				if (static_cast<double>(data[idx]) < threshold)
					continue;
				if (x < bounds[0]) bounds[0] = x;
				if (x > bounds[1]) bounds[1] = x;
				if (y < bounds[2]) bounds[2] = y;
				if (y > bounds[3]) bounds[3] = y;
				if (z < bounds[4]) bounds[4] = z;
				if (z > bounds[5]) bounds[5] = z;
			}
		}
	}
}

/** Per-slice (z) count of voxels at or above threshold - the in-body
 *  cross-sectional area profile. Already robust to arbitrary surrounding
 *  background padding: background is by construction below threshold, so
 *  it never contributes regardless of how much of it there is.
 */
template <class T>
std::vector<int> countVoxelsAboveThresholdPerSlice(T* data, const int dims[3], double threshold)
{
	std::vector<int> profile(dims[2], 0);
	vtkIdType idx = 0;
	for (int z = 0; z < dims[2]; ++z)
	{
		int count = 0;
		for (int y = 0; y < dims[1]; ++y)
			for (int x = 0; x < dims[0]; ++x, ++idx)
				if (static_cast<double>(data[idx]) >= threshold)
					++count;
		profile[z] = count;
	}
	return profile;
}

/** Per-slice (z) count of voxels in [lower, upper] that are "enclosed" -
 *  unreachable, via a 4-connected flood fill of below-bodyThreshold voxels,
 *  from that slice's own border. A true internal air pocket (lungs, bowel
 *  gas) is surrounded by tissue and so cannot reach the border this way,
 *  unlike surrounding background air, or a background gap trapped between
 *  two limbs (e.g. the arms, raised alongside the head) that is still
 *  connected to the outside air around/above them - a per-slice bounding
 *  box cannot tell these apart, since both fall inside the tight rectangle
 *  around the slice's own tissue; actual reachability can.
 */
template <class T>
std::vector<int> countEnclosedVoxelsInRangePerSlice(T* data, const int dims[3], double bodyThreshold,
                                                      double lower, double upper)
{
	std::vector<int> profile(dims[2], 0);
	int sliceSize = dims[0] * dims[1];
	std::vector<unsigned char> reachedFromBorder(sliceSize);
	std::vector<int> stack;
	stack.reserve(sliceSize);

	for (int z = 0; z < dims[2]; ++z)
	{
		std::fill(reachedFromBorder.begin(), reachedFromBorder.end(), 0);
		stack.clear();
		vtkIdType sliceBase = static_cast<vtkIdType>(z) * sliceSize;

		auto isBackground = [&](int x, int y) {
			return static_cast<double>(data[sliceBase + y * dims[0] + x]) < bodyThreshold;
		};
		auto seed = [&](int x, int y) {
			int i = y * dims[0] + x;
			if (!reachedFromBorder[i] && isBackground(x, y))
			{
				reachedFromBorder[i] = 1;
				stack.push_back(i);
			}
		};

		for (int x = 0; x < dims[0]; ++x)
		{
			seed(x, 0);
			seed(x, dims[1] - 1);
		}
		for (int y = 0; y < dims[1]; ++y)
		{
			seed(0, y);
			seed(dims[0] - 1, y);
		}

		while (!stack.empty())
		{
			int i = stack.back();
			stack.pop_back();
			int x = i % dims[0];
			int y = i / dims[0];
			if (x > 0) seed(x - 1, y);
			if (x < dims[0] - 1) seed(x + 1, y);
			if (y > 0) seed(x, y - 1);
			if (y < dims[1] - 1) seed(x, y + 1);
		}

		int count = 0;
		for (int y = 0; y < dims[1]; ++y)
			for (int x = 0; x < dims[0]; ++x)
			{
				int i = y * dims[0] + x;
				if (reachedFromBorder[i])
					continue;
				double v = static_cast<double>(data[sliceBase + i]);
				if (v < bodyThreshold && v >= lower && v <= upper)
					++count;
			}
		profile[z] = count;
	}
	return profile;
}

std::vector<int> computeLungAirVoxelProfile(vtkImageDataPtr image, const int dims[3], double bodyThreshold)
{
	std::vector<int> profile;
	void* ptr = image->GetScalarPointer();
	switch (image->GetScalarType())
	{
		// Standard air/aerated-lung HU band.
		vtkTemplateMacro(profile = countEnclosedVoxelsInRangePerSlice(static_cast<VTK_TT*>(ptr), dims, bodyThreshold,
		                                                               -1000.0, -400.0));
	}
	return profile;
}

std::vector<int> computeInBodyAreaProfile(vtkImageDataPtr image, const int dims[3], double threshold)
{
	std::vector<int> profile;
	void* ptr = image->GetScalarPointer();
	switch (image->GetScalarType())
	{
		vtkTemplateMacro(profile = countVoxelsAboveThresholdPerSlice(static_cast<VTK_TT*>(ptr), dims, threshold));
	}
	return profile;
}

/** Average of profile[lo..hi] inclusive, clamped to a valid, non-empty range. 0 if none. */
double averageProfileInRange(const std::vector<int>& profile, int lo, int hi)
{
	lo = std::max(0, lo);
	hi = std::min(static_cast<int>(profile.size()) - 1, hi);
	if (hi < lo)
		return 0;
	double sum = 0;
	for (int i = lo; i <= hi; ++i)
		sum += profile[i];
	return sum / (hi - lo + 1);
}

} // namespace

DoubleBoundingBox3D computeAutoCropBox(ImagePtr image, int paddingVoxels)
{
	vtkImageDataPtr vtkImage = image->getGrayScaleVtkImageData();
	DoubleBoundingBox3D fullVolume = image->boundingBox();
	if (!vtkImage)
		return fullVolume;

	double threshold = computeOtsuThreshold(vtkImage);

	int dims[3];
	vtkImage->GetDimensions(dims);
	int bounds[6];
	void* ptr = vtkImage->GetScalarPointer();
	switch (vtkImage->GetScalarType())
	{
		vtkTemplateMacro(findThresholdVoxelBounds(static_cast<VTK_TT*>(ptr), dims, threshold, bounds));
	}

	if (bounds[1] < bounds[0] || bounds[3] < bounds[2] || bounds[5] < bounds[4])
		return fullVolume;

	bounds[0] = std::max(0, bounds[0] - paddingVoxels);
	bounds[1] = std::min(dims[0] - 1, bounds[1] + paddingVoxels);
	bounds[2] = std::max(0, bounds[2] - paddingVoxels);
	bounds[3] = std::min(dims[1] - 1, bounds[3] + paddingVoxels);
	bounds[4] = std::max(0, bounds[4] - paddingVoxels);
	bounds[5] = std::min(dims[2] - 1, bounds[5] + paddingVoxels);

	// Matches the inverse of cropImage(PatientModelServicePtr, ImagePtr)'s
	// mm-to-voxel conversion exactly (round(mm/spacing) <-> voxel*spacing),
	// so a box built here round-trips to the same inclusive voxel extent.
	double* spacing = vtkImage->GetSpacing();
	return DoubleBoundingBox3D(
				bounds[0] * spacing[0], bounds[1] * spacing[0],
				bounds[2] * spacing[1], bounds[3] * spacing[1],
				bounds[4] * spacing[2], bounds[5] * spacing[2]);
}

namespace
{
/** Longest contiguous run of indices where profile[i] >= areaProfile[i] *
 *  minFraction (that slice's own in-body area, not a fixed constant - a
 *  narrower slice, e.g. the neck, should not be held to the same absolute
 *  count as a wider one, e.g. the thorax). Returns false if no run of at
 *  least minLength is found; runLo/runHi are left undefined in that case.
 */
bool findLongestRunAboveFraction(const std::vector<int>& profile, const std::vector<int>& areaProfile,
                                  double minFraction, int minLength, int& runLo, int& runHi)
{
	int bestLo = -1, bestHi = -1;
	int curLo = -1;
	for (int i = 0; i < static_cast<int>(profile.size()); ++i)
	{
		double minCount = areaProfile[i] * minFraction;
		if (profile[i] >= minCount)
		{
			if (curLo < 0)
				curLo = i;
			if (bestLo < 0 || (i - curLo) > (bestHi - bestLo))
			{
				bestLo = curLo;
				bestHi = i;
			}
		}
		else
		{
			curLo = -1;
		}
	}
	if (bestLo < 0 || (bestHi - bestLo + 1) < minLength)
		return false;
	runLo = bestLo;
	runHi = bestHi;
	return true;
}
} // namespace

DoubleBoundingBox3D computeLungBaseCropBox(ImagePtr image)
{
	DoubleBoundingBox3D fullVolume = image->boundingBox();
	if (image->getModality() != imCT)
		return fullVolume;

	vtkImageDataPtr vtkImage = image->getGrayScaleVtkImageData();
	if (!vtkImage)
		return fullVolume;

	int dims[3];
	vtkImage->GetDimensions(dims);
	if (dims[2] < 3)
		return fullVolume;

	double* spacing = vtkImage->GetSpacing();
	if (spacing[2] <= 0)
		return fullVolume;

	// computeLungAirVoxelProfile() restricts itself to each slice's own body
	// silhouette (not a fixed box), so this stays meaningful whether called
	// on a raw, not-yet-cropped volume (e.g. via the CroppingWidget button)
	// or an already-cropped one.
	double threshold = computeOtsuThreshold(vtkImage);
	std::vector<int> lungProfile = computeLungAirVoxelProfile(vtkImage, dims, threshold);
	std::vector<int> areaProfile = computeInBodyAreaProfile(vtkImage, dims, threshold);

	// A lung-bearing slice has a substantial fraction of its own (in-body)
	// area reading as air - well above what a stray bowel-gas pocket would
	// contribute. The run also has to persist over a physically plausible
	// craniocaudal distance (real lungs span ~200-300mm) - a brief, few-slice
	// air pocket trapped between limbs (e.g. arms raised alongside the head,
	// pinched shut in a particular slice or two even though open a slice
	// above/below - the flood fill above is only 2D per-slice, so it can
	// still be fooled by this) is much shorter-lived than that.
	const double minLungFractionOfSliceArea = 0.08;
	const double minLungRunMm = 100;
	int minLungRunVoxels = static_cast<int>(minLungRunMm / spacing[2]);
	int lungLo, lungHi;
	if (!findLongestRunAboveFraction(lungProfile, areaProfile, minLungFractionOfSliceArea,
	                                  minLungRunVoxels, lungLo, lungHi))
		return fullVolume;

	// Tell the abdomen side (wide) from the neck side (narrow) by comparing
	// in-body cross-section just past each end of the lung range - this
	// works regardless of which index direction is head-ward vs foot-ward.
	const int sideBandVoxels = 10;
	double areaBelowLo = averageProfileInRange(areaProfile, lungLo - sideBandVoxels, lungLo - 1);
	double areaAboveHi = averageProfileInRange(areaProfile, lungHi + 1, lungHi + sideBandVoxels);
	bool abdomenIsAboveHi = areaAboveHi >= areaBelowLo;

	// Margin back into the lung range, for diaphragm-level variability, and
	// a generous margin into the abdomen side, comfortably covering the
	// adult liver's ~150-200mm craniocaudal span.
	const double diaphragmMarginMm = 40;
	const double abdomenSpanMarginMm = 280;
	int diaphragmMarginVoxels = static_cast<int>(diaphragmMarginMm / spacing[2]);
	int abdomenSpanMarginVoxels = static_cast<int>(abdomenSpanMarginMm / spacing[2]);

	int zLo, zHi;
	if (abdomenIsAboveHi)
	{
		zLo = lungHi - diaphragmMarginVoxels;
		zHi = lungHi + abdomenSpanMarginVoxels;
	}
	else
	{
		zLo = lungLo - abdomenSpanMarginVoxels;
		zHi = lungLo + diaphragmMarginVoxels;
	}
	zLo = std::max(0, zLo);
	zHi = std::min(dims[2] - 1, zHi);

	// Sanity check on the result itself, not just the inputs that produced
	// it: the designed window is ~320mm: if what's left after clamping to
	// the volume's own extent is much smaller than that, the "lung range"
	// this was anchored to was most likely near the very edge of the volume
	// (e.g. a false positive close to the top) rather than a real thorax
	// with genuine abdomen below it - discard rather than return a
	// thin sliver, and let the caller fall back to a cheaper estimate.
	const double minPlausibleResultZRangeMm = 150;
	if ((zHi - zLo) * spacing[2] < minPlausibleResultZRangeMm)
		return fullVolume;

	return DoubleBoundingBox3D(
				fullVolume[0], fullVolume[1],
				fullVolume[2], fullVolume[3],
				zLo * spacing[2], zHi * spacing[2]);
}

DoubleBoundingBox3D computeWidestCrossSectionCropBox(ImagePtr image)
{
	DoubleBoundingBox3D fullVolume = image->boundingBox();
	vtkImageDataPtr vtkImage = image->getGrayScaleVtkImageData();
	if (!vtkImage)
		return fullVolume;

	int dims[3];
	vtkImage->GetDimensions(dims);
	if (dims[2] < 1)
		return fullVolume;

	double threshold = computeOtsuThreshold(vtkImage);
	std::vector<int> areaProfile = computeInBodyAreaProfile(vtkImage, dims, threshold);

	int peakSlice = static_cast<int>(std::max_element(areaProfile.begin(), areaProfile.end()) - areaProfile.begin());
	if (areaProfile[peakSlice] <= 0)
		return fullVolume;

	double* spacing = vtkImage->GetSpacing();
	if (spacing[2] <= 0)
		return fullVolume;

	// Wider than computeLungBaseCropBox()'s margins: this is a cruder
	// estimate (e.g. the peak may be at the shoulders rather than the
	// abdomen if the arms are down at the sides), so it needs more slack to
	// stay safe.
	const double halfWindowMm = 300;
	int halfWindowVoxels = static_cast<int>(halfWindowMm / spacing[2]);
	int zLo = std::max(0, peakSlice - halfWindowVoxels);
	int zHi = std::min(dims[2] - 1, peakSlice + halfWindowVoxels);

	return DoubleBoundingBox3D(
				fullVolume[0], fullVolume[1],
				fullVolume[2], fullVolume[3],
				zLo * spacing[2], zHi * spacing[2]);
}

DoubleBoundingBox3D computeAutoCropBoxAbdomen(ImagePtr image)
{
	DoubleBoundingBox3D fullVolume = image->boundingBox();
	DoubleBoundingBox3D lungBaseBox = computeLungBaseCropBox(image);
	if (!similar(lungBaseBox, fullVolume))
	{
		DoubleBoundingBox3D crossSectionBox = computeWidestCrossSectionCropBox(image);
		CX_LOG_INFO() << "computeAutoCropBoxAbdomen: lung-base z range [" << lungBaseBox[4] << ", " << lungBaseBox[5]
		              << "] mm, cross-section z range [" << crossSectionBox[4] << ", " << crossSectionBox[5]
		              << "] mm (for comparison - using the lung-base result)";
		return lungBaseBox;
	}
	return computeWidestCrossSectionCropBox(image);
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr duplicateImage(PatientModelServicePtr dataManager, ImagePtr image)
{
	Vector3D spacing(image->getBaseVtkImageData()->GetSpacing());
	return resampleImage(dataManager, image, spacing, image->getUid()+"_copy%1", image->getName()+" copy%1");
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
vtkImageDataPtr cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox)
{
	vtkImageClipPtr clip = vtkImageClipPtr::New();
	clip->SetInputData(input);
	clip->SetOutputWholeExtent(cropbox.begin());
	clip->ClipDataOn();
	clip->Update();
	vtkImageDataPtr rawResult = clip->GetOutput();

//  rawResult->Update();
//  rawResult->UpdateInformation();
	rawResult->ComputeBounds();
	return rawResult;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr cropImage(PatientModelServicePtr dataManager, ImagePtr image, DoubleBoundingBox3D bb, QString uid, QString name)
{
	double* sp = image->getBaseVtkImageData()->GetSpacing();
	IntBoundingBox3D cropbox(
				static_cast<int>(bb[0]/sp[0]+0.5), static_cast<int>(bb[1]/sp[0]+0.5),
				static_cast<int>(bb[2]/sp[1]+0.5), static_cast<int>(bb[3]/sp[1]+0.5),
				static_cast<int>(bb[4]/sp[2]+0.5), static_cast<int>(bb[5]/sp[2]+0.5));
	vtkImageDataPtr rawResult = cropImage(image->getBaseVtkImageData(), cropbox);

	if (uid.isEmpty())
	{
		uid = image->getUid() + "_crop%1";
		name = image->getName()+" crop%1";
	}
	ImagePtr result = createDerivedImage(dataManager,
										 uid, name,
										 rawResult, image);
	result->mergevtkSettingsIntosscTransform();

	return result;
}

ImagePtr cropImage(PatientModelServicePtr dataManager, ImagePtr image)
{
	return cropImage(dataManager, image, image->getCroppingBox());
}

/**
 */
QDateTime extractTimestamp(QString text)
{
	// retrieve timestamp as
	QRegExp tsReg("[0-9]{8}T[0-9]{6}");
	if (tsReg.indexIn(text)>0)
	{
		QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
		return datetime;
	}
	return QDateTime();
}

//From https://www.vtk.org/Wiki/VTK/Examples/Cxx/Qt/ImageDataToQImage
QImage vtkImageDataToQImage(vtkImageDataPtr imageData, bool overlay, QColor overlayColor)
{
	if ( !imageData ) { return QImage(); }

	int width = imageData->GetDimensions()[0];
	int height = imageData->GetDimensions()[1];

	QImage image( width, height, QImage::Format_ARGB32 ); //32 bit
	QRgb *rgbPtr = reinterpret_cast<QRgb *>( image.bits() ) + width * ( height - 1 );
	unsigned char *colorsPtr = reinterpret_cast<unsigned char *>( imageData->GetScalarPointer() );

	// Loop over the vtkImageData contents.
	for ( int row = 0; row < height; row++ )
	{
		for ( int col = 0; col < width; col++ )
		{
			// Swap the vtkImageData RGB values with an equivalent QColor
			*( rgbPtr++ ) = convertToQColor(colorsPtr, overlay, overlayColor);

			colorsPtr += imageData->GetNumberOfScalarComponents();
		}

		rgbPtr -= width * 2;
	}

	return image;
}

QRgb convertToQColor(unsigned char *colorsPtr, bool overlay, QColor overlayColor)
{
	if(overlay)
		return modifyOverlayColor(colorsPtr, overlayColor);

	//32 bit
	QRgb rgb;
	rgb = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2], colorsPtr[3] ).rgba();
	//	rgb = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2], opacity ).rgba(); //Don't use alpha for now

	return rgb;
}

bool isDark(unsigned char *colorsPtr)
{
	unsigned char threshold = 10;
	if (colorsPtr[0] < threshold &&
			colorsPtr[1] < threshold &&
			colorsPtr[2] < threshold)
		return true;
	return false;
}

QRgb modifyOverlayColor(unsigned char *colorsPtr, QColor overlayColor)
{
	if(isDark(colorsPtr))
		overlayColor.setAlpha(0);

	QRgb retval = overlayColor.rgba();
	return retval;
}

vtkImageDataPtr createSlice(ImagePtr image, PLANE_TYPE planeType, Vector3D outputSpacing, Eigen::Array3i outputDimensions, ToolPtr sliceTool, PatientModelServicePtr patientModel, bool applyLUT)
{
	vtkImageDataPtr retval = vtkImageDataPtr::New();

	if(!image || !patientModel)
	{
		return retval;
	}

	cx::SliceProxyPtr proxy = cx::SliceProxy::create(patientModel);
	SlicedImageProxyPtr imageSlicer(new SlicedImageProxy);

	proxy->setTool(sliceTool);

	imageSlicer->setSliceProxy(proxy);
	imageSlicer->setImage(image);

	proxy->initializeFromPlane(planeType, false, false, 1, 0);
	proxy->setClinicalApplicationToFixedValue(mdRADIOLOGICAL);//Always create slices in radiological view

	// Using these values centers image in view, but seems to lock the manual image movement in some directions.
	double screenX = outputDimensions[0]*outputSpacing[0] / 2;
	double screenY = outputDimensions[1]*outputSpacing[1] / 2;

	imageSlicer->setOutputFormat(Vector3D(-screenX,-screenY,0), outputDimensions, outputSpacing);

	imageSlicer->update();
	if (applyLUT)
	{
		imageSlicer->getOutputPort()->Update();
		retval->DeepCopy(imageSlicer->getOutput());
	}
	else //Don't use LUT
	{
		imageSlicer->getOutputPortWithoutLUT()->Update();
		retval->DeepCopy(imageSlicer->getOutputWithoutLUT());

	}

	return retval;
}

vtkImageDataPtr createSlice(ImagePtr image, PLANE_TYPE planeType, Vector3D position_r, Vector3D target_r, double offset, bool applyLUT)
{
	vtkImageDataPtr slicedImage = vtkImageDataPtr::New();

	vtkImageReslicePtr imageReslicer = vtkImageReslicePtr::New();

	imageReslicer->SetInputData(image->getBaseVtkImageData());
	imageReslicer->SetBackgroundLevel(image->getMin());

	imageReslicer->SetInterpolationModeToLinear();
	imageReslicer->SetOutputDimensionality(2);


	Eigen::Array3d inputSpacing = image->getSpacing();

	//imageReslicer->SetOutputOrigin(image->getBaseVtkImageData()->GetOrigin()); // set to [0, 0, 0] ??
//	double origin[3];
//	image->getBaseVtkImageData()->GetOrigin(origin);

//	Eigen::Array3d spacing = image->getSpacing();
	int extent[6];
	image->getBaseVtkImageData()->GetExtent(extent);

	Transform3D rMd = image->get_rMd();

	//Eigen::Array3i dim(image->getBaseVtkImageData()->GetDimensions());

	Transform3D positionTransform_d = rMd.inv() * createTransformTranslate(position_r);
	vtkSmartPointer<vtkMatrix4x4> resliceAxes = vtkSmartPointer<vtkMatrix4x4>::New();

	Transform3D targetTransform_d;
	Vector3D direction;
	Vector3D up(0, -1, 0);
	Vector3D xAxis;
	Vector3D yAxis;


	switch (planeType)
	{
	case ptAXIAL:
		resliceAxes->SetElement(0, 0, 1);
		resliceAxes->SetElement(0, 1, 0);
		resliceAxes->SetElement(0, 2, 0);
		resliceAxes->SetElement(0, 3, 0);
		resliceAxes->SetElement(1, 0, 0);
		resliceAxes->SetElement(1, 1, -1);
		resliceAxes->SetElement(1, 2, 0);
		resliceAxes->SetElement(1, 3, 0);
		resliceAxes->SetElement(2, 0, 0);
		resliceAxes->SetElement(2, 1, 0);
		resliceAxes->SetElement(2, 2, 1);
		resliceAxes->SetElement(2, 3, positionTransform_d(2,3) + offset);
		resliceAxes->SetElement(3, 0, 0);
		resliceAxes->SetElement(3, 1, 0);
		resliceAxes->SetElement(3, 2, 0);
		resliceAxes->SetElement(3, 3, 1);
		imageReslicer->SetResliceAxes(resliceAxes);
		imageReslicer->SetOutputExtent(extent[0], extent[1], extent[2], extent[3], 0, 0);
		imageReslicer->SetOutputSpacing(inputSpacing[0], inputSpacing[1], 0);
		break;
	case ptCORONAL:
		resliceAxes->SetElement(0, 0, 1);
		resliceAxes->SetElement(0, 1, 0);
		resliceAxes->SetElement(0, 2, 0);
		resliceAxes->SetElement(0, 3, 0);
		resliceAxes->SetElement(1, 0, 0);
		resliceAxes->SetElement(1, 1, 0);
		resliceAxes->SetElement(1, 2, 1);
		resliceAxes->SetElement(1, 3, positionTransform_d(1,3) + offset);
		resliceAxes->SetElement(2, 0, 0);
		resliceAxes->SetElement(2, 1, 1);
		resliceAxes->SetElement(2, 2, 0);
		resliceAxes->SetElement(2, 3, 0);
		resliceAxes->SetElement(3, 0, 0);
		resliceAxes->SetElement(3, 1, 0);
		resliceAxes->SetElement(3, 2, 0);
		resliceAxes->SetElement(3, 3, 1);
		imageReslicer->SetResliceAxes(resliceAxes);
		imageReslicer->SetOutputExtent(extent[0], extent[1], extent[4], extent[5], 0, 0);
		imageReslicer->SetOutputSpacing(inputSpacing[0], inputSpacing[2], 0);
		break;
	case ptSAGITTAL:
		resliceAxes->SetElement(0, 0, 0);
		resliceAxes->SetElement(0, 1, 0);
		resliceAxes->SetElement(0, 2, -1);
		resliceAxes->SetElement(0, 3, positionTransform_d(0,3));
		resliceAxes->SetElement(1, 0, 1);
		resliceAxes->SetElement(1, 1, 0);
		resliceAxes->SetElement(1, 2, 0);
		resliceAxes->SetElement(1, 3, 0);
		resliceAxes->SetElement(2, 0, 0);
		resliceAxes->SetElement(2, 1, 1);
		resliceAxes->SetElement(2, 2, 0);
		resliceAxes->SetElement(2, 3, 0);
		resliceAxes->SetElement(3, 0, 0);
		resliceAxes->SetElement(3, 1, 0);
		resliceAxes->SetElement(3, 2, 0);
		resliceAxes->SetElement(3, 3, 1);
		imageReslicer->SetResliceAxes(resliceAxes);
		imageReslicer->SetOutputExtent(extent[2], extent[3], extent[4], extent[5], 0, 0);
		imageReslicer->SetOutputSpacing(inputSpacing[1], inputSpacing[2], 0);
		break;
	case ptRADIALPLANE:
		targetTransform_d = rMd.inv() * createTransformTranslate(target_r);
		direction(0) = targetTransform_d(0,3) - positionTransform_d(0,3);
		direction(1) = targetTransform_d(1,3) - positionTransform_d(1,3);
		direction(2) = targetTransform_d(2,3) - positionTransform_d(2,3);
		direction = direction.normalized();
		xAxis = up.cross(direction).normalized();
		yAxis = direction.cross(xAxis).normalized();

		resliceAxes->SetElement(0, 0, xAxis(0));
		resliceAxes->SetElement(0, 1, yAxis(0));
		resliceAxes->SetElement(0, 2, direction(0));
		resliceAxes->SetElement(0, 3, positionTransform_d(0,3) + offset*direction(0));
		resliceAxes->SetElement(1, 0, xAxis(1));
		resliceAxes->SetElement(1, 1, yAxis(1));
		resliceAxes->SetElement(1, 2, direction(1));
		resliceAxes->SetElement(1, 3, positionTransform_d(1,3) + offset*direction(1));
		resliceAxes->SetElement(2, 0, xAxis(2));
		resliceAxes->SetElement(2, 1, yAxis(2));
		resliceAxes->SetElement(2, 2, direction(2));
		resliceAxes->SetElement(2, 3, positionTransform_d(2,3) + offset*direction(2));
		resliceAxes->SetElement(3, 0, 0);
		resliceAxes->SetElement(3, 1, 0);
		resliceAxes->SetElement(3, 2, 0);
		resliceAxes->SetElement(3, 3, 1);
		imageReslicer->SetResliceAxes(resliceAxes);
		imageReslicer->SetOutputExtent(extent[0], extent[1], extent[2], extent[3], 0, 0);
		imageReslicer->SetOutputSpacing(inputSpacing[0], inputSpacing[1], 0);
		break;
	default:
		CX_LOG_WARNING() << "Not a valid plane type." << enum2string(planeType);
	}

	imageReslicer->Update();

	if (applyLUT)
	{
		ApplyLUTToImage2DProxyPtr imageWithLUTProxyPtr = ApplyLUTToImage2DProxyPtr(new ApplyLUTToImage2DProxy());
		vtkImageChangeInformationPtr redirecterPtr = vtkImageChangeInformationPtr::New();
		redirecterPtr->SetInputConnection(imageReslicer->GetOutputPort());

		imageWithLUTProxyPtr->setInput(redirecterPtr, image->getLookupTable2D()->getOutputLookupTable());

		imageWithLUTProxyPtr->getOutputPort()->Update();
		slicedImage->DeepCopy(imageWithLUTProxyPtr->getOutput());
	}
	else
			slicedImage = imageReslicer->GetOutput();

	return slicedImage;
}

std::vector<int> getSliceVoxelFrom3Dposition(ImagePtr image, PLANE_TYPE planeType, Vector3D position_r)
{
	std::vector<int> voxel(2,0);

	Transform3D rMd = image->get_rMd();
	Eigen::Array3d spacing = image->getSpacing();
	Transform3D positionTransform_d = rMd.inv() * createTransformTranslate(position_r);
	int xVoxel = std::round( positionTransform_d(0,3) / spacing(0) );
	int yVoxel = std::round( positionTransform_d(1,3) / spacing(1) );
	int zVoxel = std::round( positionTransform_d(2,3) / spacing(2) );

	switch (planeType)
	{
	case ptAXIAL:
		voxel[0] = xVoxel;
		voxel[1] = yVoxel;
		break;
	case ptCORONAL:
		voxel[0] = xVoxel;
		voxel[1] = zVoxel;
		break;
	case ptSAGITTAL:
		voxel[0] = yVoxel;
		voxel[1] = zVoxel;
		break;
	default:
		CX_LOG_WARNING() << "Not a valid plane type." << enum2string(planeType);
	}
	return voxel;
}

int getSliceNumberFrom3Dposition(ImagePtr image, PLANE_TYPE planeType, Vector3D position_r)
{
	int sliceNumber;

	Transform3D rMd = image->get_rMd();
	Eigen::Array3d spacing = image->getSpacing();
	Transform3D positionTransform_d = rMd.inv() * createTransformTranslate(position_r);

	switch (planeType)
	{
	case ptAXIAL:
		sliceNumber = std::round( positionTransform_d(2,3) / spacing(2) );
		break;
	case ptCORONAL:
		sliceNumber = std::round( positionTransform_d(1,3) / spacing(1) );
		break;
	case ptSAGITTAL:
		sliceNumber = std::round( positionTransform_d(0,3) / spacing(0) );
		break;
	default:
		CX_LOG_WARNING() << "Not a valid plane type." << enum2string(planeType);
	}
	return sliceNumber;
}

Vector3D get3DpositionFromSliceVoxel(ImagePtr image, PLANE_TYPE planeType, std::vector<int> voxel, int sliceNumber)
{
	Vector3D position_r;

	Transform3D rMd = image->get_rMd();
	Eigen::Array3d spacing = image->getSpacing();
	Vector3D position_d;

	switch (planeType)
	{
	case ptAXIAL:
		position_d(0) = voxel[0]*spacing(0);
		position_d(1) = voxel[1]*spacing(1);
		position_d(2) = sliceNumber*spacing(2);
		break;
	case ptCORONAL:
		position_d(0) = voxel[0]*spacing(0);
		position_d(1) = sliceNumber*spacing(1);
		position_d(2) = voxel[1]*spacing(2);
		break;
	case ptSAGITTAL:
		position_d(0) = sliceNumber*spacing(0);
		position_d(1) = voxel[0]*spacing(1);
		position_d(2) = voxel[1]*spacing(2);
		break;
	default:
		CX_LOG_WARNING() << "Not a valid plane type." << enum2string(planeType);
		return position_r;
	}

	Transform3D positionTransform_r = rMd * createTransformTranslate(position_d);
	position_r(0) = positionTransform_r(0,3);
	position_r(1) = positionTransform_r(1,3);
	position_r(2) = positionTransform_r(2,3);

	return position_r;
}

} // namespace cx
