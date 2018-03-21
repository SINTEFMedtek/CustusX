/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSyntheticVolume.h"
#include "vtkImageData.h"
#include "cxImage.h"
#include <cstdlib>
#include <time.h>
#include "cxTypeConversions.h"
#include <QTime>
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxVolumeHelpers.h"


double noiseValue(double noiseSigma,
                         double noiseMean)
{
	double random_value_1 = (rand()+1.0)/(RAND_MAX+1.0);
	double random_value_2 = (rand()+1.0)/(RAND_MAX+1.0);

	double random_normal = sqrt(-2*log(random_value_1)) * cos(2*M_PI*random_value_2);

	return random_normal*noiseSigma + noiseMean;
}

namespace cx {


ProcessedUSInputDataPtr
cxSyntheticVolume::sampleUsData(const std::vector<Transform3D>& planes_rMt,
			 const ProbeDefinition& probe,
			 const Transform3D& output_dMr,
			 const double noiseSigma,
			 const unsigned char noiseMean) const
{
	cx::ProbeSector sector;
	sector.setData(probe);

	std::vector<Transform3D> planes_rMf(planes_rMt.size());
	for (unsigned i=0; i<planes_rMt.size(); ++i)
		planes_rMf[i] = planes_rMt[i] * sector.get_tMu() * sector.get_uMv();

	Eigen::Array2f pixelSpacing = probe.getSpacing().block(0,0,2,1).cast<float>();
	Eigen::Array2i sliceDimension(probe.getSize().width(), probe.getSize().height());

	return this->sampleUsData(planes_rMf,
							  pixelSpacing,
							  sliceDimension,
							  output_dMr,
							  noiseSigma, noiseMean);
}

vtkImageDataPtr
cxSyntheticVolume::sampleUsData(const Transform3D& plane_rMt,
			 const ProbeDefinition& probe,
			 const double noiseSigma,
			 const unsigned char noiseMean) const
{
	cx::ProbeSector sector;
	sector.setData(probe);

	Transform3D rMf = plane_rMt * sector.get_tMu() * sector.get_uMv();
	Eigen::Array2f pixelSpacing = probe.getSpacing().block(0,0,2,1).cast<float>();
	Eigen::Array2i sliceDimension(probe.getSize().width(), probe.getSize().height());

	return this->sampleUsData(rMf,
							  pixelSpacing,
							  sliceDimension,
							  noiseSigma, noiseMean);
}

ProcessedUSInputDataPtr
cxSyntheticVolume::sampleUsData(const std::vector<Transform3D>& planes_rMf,
                                const Eigen::Array2f& pixelSpacing,
                                const Eigen::Array2i& sliceDimension,
								const Transform3D& output_dMr,
								const double noiseSigma, const unsigned char noiseMean) const
{
	std::vector<TimedPosition> positions;
	std::vector<vtkImageDataPtr> images;
	// For each plane
	for(std::vector<Transform3D>::const_iterator i = planes_rMf.begin();
		planes_rMf.end() != i;
			++i)
	{
		const Transform3D rMf = *i;

		vtkImageDataPtr	us_frame;
		us_frame = this->sampleUsData(rMf, pixelSpacing, sliceDimension, noiseSigma, noiseMean);

		// Build the TimedPosition for this frame
		TimedPosition t;
		t.mTime = i - planes_rMf.begin();
		t.mPos = output_dMr*rMf;

		positions.push_back(t);
		images.push_back(us_frame);
	}

	vtkImageDataPtr mask = this->createEmptyMask(sliceDimension);
//	std::cout << "elapsed: " << time.elapsed() << std::endl;

	ProcessedUSInputDataPtr ret;
	ret.reset(new ProcessedUSInputData(images, positions, mask, "VIRTUAL_DATA", "VIRTUAL_DATA_"));
	return ret;
}

vtkImageDataPtr
cxSyntheticVolume::sampleUsData(const Transform3D& rMf,
								const Eigen::Array2f& pixelSpacing,
								const Eigen::Array2i& sliceDimension,
								const double noiseSigma, const unsigned char noiseMean) const
{


	const Vector3D p0 = rMf.coord(Vector3D(0,0,0));
	const Vector3D e_x = rMf.vector(Vector3D(pixelSpacing[0],0,0));
	const Vector3D e_y = rMf.vector(Vector3D(0,pixelSpacing[1],0));

	vtkImageDataPtr us_frame = vtkImageDataPtr::New();
	us_frame->SetExtent(0, sliceDimension[0]-1, 0, sliceDimension[1]-1, 0, 0);
	us_frame->SetSpacing(pixelSpacing[0], pixelSpacing[1], 0.0);
	us_frame->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

	unsigned char* us_data = (unsigned char*)us_frame->GetScalarPointer();
	// For each pixel on that plane
	for(unsigned int px = 0; px < sliceDimension[0]; px++)
	{
		// optimization: use transformed pixel vectors
		const Vector3D px0_vol = p0 + e_x*px;

		for(unsigned int py = 0; py < sliceDimension[1]; py++)
		{
			const Vector3D volume_coords = px0_vol + e_y*py;

			// Evaluate volume at that position
			const unsigned char val = this->evaluate(volume_coords);

			const double noise_val = noiseValue(noiseSigma, noiseMean);
			const int noised_val = noise_val + val;
			unsigned char final_val = this->constrainToUnsignedChar(noised_val);
			// Store that value in the US slice
			us_data[px + py*sliceDimension[0]] = final_val;

		}
	}

	setDeepModified(us_frame);
	return us_frame;
}

vtkImageDataPtr cxSyntheticVolume::createEmptyMask(const Eigen::Array2i& sliceDimension) const
{
	vtkImageDataPtr mask = vtkImageDataPtr::New();
	mask->SetExtent(0, sliceDimension[0]-1, 0, sliceDimension[1]-1, 0, 0);
	mask->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	unsigned char* mask_data = (unsigned char*)mask->GetScalarPointer();
	memset(mask_data, 1, sizeof(unsigned char)*sliceDimension[0]*sliceDimension[1]);
	setDeepModified(mask);
	return mask;
}

unsigned char cxSyntheticVolume::constrainToUnsignedChar(const int val) const
{
	if(val < 0)
	{
		return 0;
	}
	else if(val > 255)
	{
		return 255;
	}
	else
	{
		return (unsigned char)val;
	}
}

float cxSyntheticVolume::computeRMSError(ImagePtr vol)
{
	vtkImageDataPtr input = vol->getBaseVtkImageData();

	vtkImageDataPtr nominal = vtkImageDataPtr::New();
	nominal->DeepCopy(input);
	cx::ImagePtr nominal_img(new cx::Image("nominal", nominal));
	nominal_img->get_rMd_History()->setRegistration(vol->get_rMd());
	this->fillVolume(nominal_img);

	return calculateRMSError(input, nominal);
}

void cxSyntheticVolume::fillVolume(cx::ImagePtr vol)
{
	vtkImageDataPtr raw = vol->getBaseVtkImageData();
	cx::Transform3D rMd = vol->get_rMd();

	Eigen::Array3i dims = Eigen::Array3i(raw->GetDimensions());
	Eigen::Array3d spacing = Eigen::Array3d(raw->GetSpacing());
	unsigned char *pixels = (unsigned char*)raw->GetScalarPointer();

	for(int z = 0; z < dims[2]; ++z)
	{
		for(int y = 0; y < dims[1]; ++y)
		{
			for(int x = 0; x < dims[0]; ++x)
			{
				Vector3D p_d = Vector3D(x, y, z).array()*spacing;
				int index = x + y*dims[0] + z*dims[1]*dims[0];
				pixels[index] = this->evaluate(rMd.coord(p_d));
			}
		}
	}
	setDeepModified(raw);
}

double calculateRMSError(vtkImageDataPtr a, vtkImageDataPtr b)
{
	CX_ASSERT(Eigen::Array3i(a->GetDimensions()).isApprox(Eigen::Array3i(b->GetDimensions())));
	CX_ASSERT(Eigen::Array3d(a->GetSpacing()).isApprox(Eigen::Array3d(b->GetSpacing())));

	float sse = 0.0f;
	Eigen::Array3i dims = Eigen::Array3i(a->GetDimensions());
	unsigned char *pa = (unsigned char*)a->GetScalarPointer();
	unsigned char *pb = (unsigned char*)b->GetScalarPointer();

	for(int z = 0; z < dims[2]; ++z)
	{
		for(int y = 0; y < dims[1]; ++y)
		{
			for(int x = 0; x < dims[0]; ++x)
			{
				int index = x + y*dims[0] + z*dims[1]*dims[0];
				float error = pa[index] - pb[index];
				sse += error*error;
			}
		}
	}

	return sqrt(sse/dims.prod());
}

/** Functor that calculates volume of input voxel sample.
  *
  */
struct MassFunctor
{
	MassFunctor()
	{
		mSum = 0;
		mThreshold = 2;
	}

	void operator()(int x, int y, int z, unsigned char* val)
	{
		if (*val < mThreshold)
			return;
		mSum += *val;
	}

	double getVolume()
	{
		return mSum;
	}

private:
	double mSum;
	unsigned char mThreshold;
};

/** Functor that calculates centroid of input voxel sample.
  *
  */
struct CentroidFunctor
{
	CentroidFunctor()
	{
		mSum = 0;
		mWeight = Vector3D::Zero();
		mThreshold = 2;
	}

	void operator()(int x, int y, int z, unsigned char* val)
	{
		if (*val < mThreshold)
			return;
		mSum += *val;
		mWeight += Vector3D(x,y,z) * (*val);
	}

	Vector3D getCentroid()
	{
		return mWeight/mSum;
	}

private:
	double mSum;
	Vector3D mWeight;
	unsigned char mThreshold;
};

template<class FUNCTOR>
void applyFunctor(cx::ImagePtr image, FUNCTOR& func)
{
	vtkImageDataPtr raw = image->getBaseVtkImageData();
	Eigen::Array3i dims = Eigen::Array3i(raw->GetDimensions());
	unsigned char *pixels = (unsigned char*)raw->GetScalarPointer();

	for(int z = 0; z < dims[2]; ++z)
	{
		for(int y = 0; y < dims[1]; ++y)
		{
			for(int x = 0; x < dims[0]; ++x)
			{
				int index = x + y*dims[0] + z*dims[1]*dims[0];
				func(x,y,z, pixels+index);
			}
		}
	}
}

cx::Vector3D calculateCentroid(cx::ImagePtr image)
{
	vtkImageDataPtr raw = image->getBaseVtkImageData();
	cx::Transform3D rMd = image->get_rMd();
	Eigen::Array3d spacing = Eigen::Array3d(raw->GetSpacing());

	CentroidFunctor func;
	applyFunctor(image, func);
	Vector3D ci = func.getCentroid().array() * spacing;
	return rMd.coord(ci);
}

double calculateMass(cx::ImagePtr image)
{
	MassFunctor func;
	applyFunctor(image, func);
	return func.getVolume();
}

}
