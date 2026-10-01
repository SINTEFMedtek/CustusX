/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDilationFilter.h"

#include <vtkImageData.h>

#include <vtkPolyData.h>

#include "cxDoubleProperty.h"
#include "cxColorProperty.h"
#include "cxBoolProperty.h"
#include "cxStringProperty.h"
#include "cxSelectDataStringProperty.h"

#include <vtkImageThreshold.h>
#include <algorithm>
#include <limits>
#include <vector>
#include "cxUtilHelpers.h"
#include "cxContourFilter.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"


namespace cx {

DilationFilter::DilationFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString DilationFilter::getName() const
{
	return "Dilation";
}

QString DilationFilter::getType() const
{
	return "dilation_filter";
}

QString DilationFilter::getHelp() const
{
	return "<html>"
	        "<h3>Dilation Filter.</h3>"
	        "<p>This filter dilates a binary volume with a given radius in mm.<p>"
	        "<p>The dilation is performed using a ball structuring element<p>"
	        "</html>";
}

DoublePropertyPtr DilationFilter::getDilationRadiusOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Dilation radius (mm)", "",
    "Set dilation radius in mm", 1, DoubleRange(1, 20, 1), 0,
                    root);
	retval->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	return retval;
}

BoolPropertyPtr DilationFilter::getGenerateSurfaceOption(QDomElement root)
{
	BoolPropertyPtr retval = BoolProperty::initialize("Generate Surface", "",
	                                                                        "Generate a surface of the output volume", true,
	                                                                            root);
	return retval;
}

ColorPropertyPtr DilationFilter::getColorOption(QDomElement root)
{
	return ColorProperty::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}


void DilationFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getDilationRadiusOption(mOptions));
	mOptionsAdapters.push_back(this->getGenerateSurfaceOption(mOptions));
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void DilationFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select segmentation input for dilation");
	mInputTypes.push_back(temp);
}

void DilationFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Dilated segmentation image");
	mOutputTypes.push_back(temp);

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Contour");
	temp->setHelp("Output contour generated from dilated segmentation image.");
	mOutputTypes.push_back(temp);
}

bool DilationFilter::preProcess() {

    return FilterImpl::preProcess();
}

bool DilationFilter::execute() {
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	double radius = this->getDilationRadiusOption(mCopiedOptions)->getValue();

	mRawResult = dilate(input->getBaseVtkImageData(), radius);

	BoolPropertyPtr generateSurface = this->getGenerateSurfaceOption(mCopiedOptions);
	if (generateSurface->getValue())
	{
        double threshold = 1;/// because the segmented image is 0..1
        mRawContour = ContourFilter::execute(mRawResult, threshold);
	}

    return true;
}

namespace
{

const double farAway = 1e30;
const double infinity = std::numeric_limits<double>::infinity();

double intersection(const std::vector<double>& f, double weight, int q, int p)
{
	return ((f[q] + weight*q*q) - (f[p] + weight*p*p)) / (2*weight*(q-p));
}

/** In place: f[q] = min over p of weight*(q-p)^2 + f[p], in linear time [Felzenszwalb and Huttenlocher 2012]. */
void distanceTransformLine(std::vector<double>& f, double weight)
{
	const int n = static_cast<int>(f.size());
	std::vector<int> v(n);
	std::vector<double> z(n+1);
	std::vector<double> d(n);
	int k = 0;
	v[0] = 0;
	z[0] = -infinity;
	z[1] = infinity;
	for (int q = 1; q < n; ++q)
	{
		double s = intersection(f, weight, q, v[k]);
		while (s <= z[k])
		{
			--k;
			s = intersection(f, weight, q, v[k]);
		}
		++k;
		v[k] = q;
		z[k] = s;
		z[k+1] = infinity;
	}
	k = 0;
	for (int q = 0; q < n; ++q)
	{
		while (z[k+1] < q)
		{
			++k;
		}
		d[q] = weight*(q-v[k])*(q-v[k]) + f[v[k]];
	}
	f = d;
}

void distanceTransformAlongAxis(std::vector<double>& values, const int* dims, int axis, double weight)
{
	const size_t strides[3] = {1, size_t(dims[0]), size_t(dims[0])*dims[1]};
	const int a1 = (axis+1)%3;
	const int a2 = (axis+2)%3;
	std::vector<double> line(dims[axis]);
	for (int i2 = 0; i2 < dims[a2]; ++i2)
	{
		for (int i1 = 0; i1 < dims[a1]; ++i1)
		{
			const size_t start = i1*strides[a1] + i2*strides[a2];
			for (int i = 0; i < dims[axis]; ++i)
			{
				line[i] = values[start + i*strides[axis]];
			}
			distanceTransformLine(line, weight);
			for (int i = 0; i < dims[axis]; ++i)
			{
				values[start + i*strides[axis]] = line[i];
			}
		}
	}
}

/** Index bounds of the voxels equal to 1, expanded by margin and clipped to the image. False if there are none. */
bool foregroundBounds(const unsigned char* voxels, const int* dims, const int* margin, int* bounds)
{
	for (int i = 0; i < 3; ++i)
	{
		bounds[2*i] = dims[i];
		bounds[2*i+1] = -1;
	}
	for (int z = 0; z < dims[2]; ++z)
	{
		for (int y = 0; y < dims[1]; ++y)
		{
			for (int x = 0; x < dims[0]; ++x)
			{
				if (voxels[x + size_t(dims[0])*(y + size_t(dims[1])*z)])
				{
					const int index[3] = {x, y, z};
					for (int i = 0; i < 3; ++i)
					{
						bounds[2*i] = std::min(bounds[2*i], index[i]);
						bounds[2*i+1] = std::max(bounds[2*i+1], index[i]);
					}
				}
			}
		}
	}
	const bool found = bounds[1] >= 0;
	for (int i = 0; i < 3; ++i)
	{
		bounds[2*i] = std::max(0, bounds[2*i] - margin[i]);
		bounds[2*i+1] = std::min(dims[i]-1, bounds[2*i+1] + margin[i]);
	}
	return found;
}

/** Copies between the voxels inside bounds and values: 0 for foreground and farAway else, or back as values <= 1. */
void copyBounds(unsigned char* voxels, const int* dims, const int* bounds, std::vector<double>& values, bool toValues)
{
	size_t i = 0;
	for (int z = bounds[4]; z <= bounds[5]; ++z)
	{
		for (int y = bounds[2]; y <= bounds[3]; ++y)
		{
			unsigned char* row = voxels + size_t(dims[0])*(y + size_t(dims[1])*z);
			for (int x = bounds[0]; x <= bounds[1]; ++x)
			{
				if (toValues)
				{
					values[i] = row[x] ? 0 : farAway;
				}
				else
				{
					row[x] = values[i] <= 1 ? 1 : 0;
				}
				++i;
			}
		}
	}
}

/** Binary dilation in place with an ellipsoid of semi-axes radius+0.5 voxels, as a weighted squared distance transform. */
void dilateWithBall(unsigned char* voxels, const int* dims, const int* radius)
{
	int bounds[6];
	if (foregroundBounds(voxels, dims, radius, bounds))
	{
		const int box[3] = {bounds[1]-bounds[0]+1, bounds[3]-bounds[2]+1, bounds[5]-bounds[4]+1};
		std::vector<double> values(size_t(box[0])*box[1]*box[2]);
		copyBounds(voxels, dims, bounds, values, true);
		for (int axis = 0; axis < 3; ++axis)
		{
			distanceTransformAlongAxis(values, box, axis, 1.0 / ((radius[axis]+0.5)*(radius[axis]+0.5)));
		}
		copyBounds(voxels, dims, bounds, values, false);
	}
}

} // namespace

vtkImageDataPtr DilationFilter::dilate(vtkImageDataPtr image, double radius)
{
	vtkSmartPointer<vtkImageThreshold> binary = vtkSmartPointer<vtkImageThreshold>::New();
	binary->SetInputData(image);
	binary->ThresholdBetween(1, 1);
	binary->SetInValue(1);
	binary->SetOutValue(0);
	binary->ReplaceInOn();
	binary->ReplaceOutOn();
	binary->SetOutputScalarTypeToUnsignedChar();
	binary->Update();
	vtkImageDataPtr retval = binary->GetOutput();

	const double* spacing = retval->GetSpacing();
	int radiusInVoxels[3];
	for (int i = 0; i < 3; ++i)
	{
		radiusInVoxels[i] = static_cast<int>(radius/spacing[i]);
	}
	dilateWithBall(static_cast<unsigned char*>(retval->GetScalarPointer()), retval->GetDimensions(), radiusInVoxels);
	return retval;
}

bool DilationFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	QString uid = input->getUid() + "_seg%1";
	QString name = input->getName()+" seg%1";
	ImagePtr output = createDerivedImage(mServices->patient(),
										 uid, name,
										 mRawResult, input);
	mRawResult = NULL;
	if (!output)
		return false;

	output->resetTransferFunctions();
	mServices->patient()->insertData(output);

	// set output
	mOutputTypes.front()->setValue(output->getUid());

	// set contour output
	if (mRawContour!=NULL)
	{
		ColorPropertyPtr colorOption = this->getColorOption(mOptions);
		MeshPtr contour = ContourFilter::postProcess(mServices->patient(), mRawContour, output, colorOption->getValue());
		mOutputTypes[1]->setValue(contour->getUid());
		mRawContour = vtkPolyDataPtr();
	}

    return true;
}



} // namespace cx
