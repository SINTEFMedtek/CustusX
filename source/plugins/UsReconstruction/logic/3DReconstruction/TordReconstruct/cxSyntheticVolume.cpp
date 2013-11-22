#include "cxSyntheticVolume.h"
#include "vtkImageData.h"
#include "sscImage.h"
#include <cstdlib>
#include <time.h>
#include "sscTypeConversions.h"
#include <QTime>

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
cxSyntheticVolume::sampleUsData(const std::vector<Transform3D>& planes_rMf,
                                const Eigen::Array2f& pixelSpacing,
                                const Eigen::Array2i& sliceDimension,
								const Transform3D& output_dMr,
								const double noiseSigma, const unsigned char noiseMean) const
{
	// Seed the random number generator
	srand(time(NULL));

	QTime time;
	time.start();

	std::vector<TimedPosition> positions;
	std::vector<vtkImageDataPtr> images;
	// For each plane
	for(std::vector<Transform3D>::const_iterator i = planes_rMf.begin();
		planes_rMf.end() != i;
	    i++)
	{
		const Transform3D rMf = *i;
		const Vector3D p0 = rMf.coord(Vector3D(0,0,0));
		const Vector3D e_x = rMf.vector(Vector3D(pixelSpacing[0],0,0));
		const Vector3D e_y = rMf.vector(Vector3D(0,pixelSpacing[1],0));

		vtkImageDataPtr us_frame = vtkImageDataPtr::New();
		us_frame->SetExtent(0, sliceDimension[0]-1, 0, sliceDimension[1]-1, 0, 0);
		us_frame->SetSpacing(pixelSpacing[0], pixelSpacing[1], 0.0);

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

				unsigned char final_val;
				if(noised_val < 0)
				{
					final_val = 0;
				}
				else if(noised_val > 255)
				{
					final_val = 255;
				}
				else
				{
					final_val = (unsigned char)noised_val;
				}

				// Store that value in the US slice
				us_data[px + py*sliceDimension[0]] = final_val;

			}
		}

		// Build the TimedPosition for this frame
		TimedPosition t;
		t.mTime = i - planes_rMf.begin();
		t.mPos = output_dMr*rMf;

		positions.push_back(t);
		images.push_back(us_frame);
	}

	std::cout << "elapsed: " << time.elapsed() << std::endl;
	// Make an empty mask
	vtkImageDataPtr mask = vtkImageDataPtr::New();
	mask->SetExtent(0, sliceDimension[0]-1, 0, sliceDimension[1]-1, 0, 0);
	unsigned char* mask_data = (unsigned char*)mask->GetScalarPointer();
	memset(mask_data, 1,
	       sizeof(unsigned char)*sliceDimension[0]*sliceDimension[1]);

	ProcessedUSInputDataPtr ret(new ProcessedUSInputData(images,
	                                                     positions,
	                                                     mask,
	                                                     "VIRTUAL_DATA",
	                                                     "VIRTUAL_DATA_"));
	return ret;

}


float cxSyntheticVolume::computeRMSError(ImagePtr vol)
{
	vtkImageDataPtr raw = vol->getBaseVtkImageData();

	float sse = 0.0f;
	int* dims = raw->GetDimensions();
	unsigned char *pixels = (unsigned char*)raw->GetScalarPointer();
	for(int z = 0; z < dims[2]; z++)
	{
		for(int y = 0; y < dims[1]; y++)
		{
			for(int x = 0; x < dims[0]; x++)
			{
				unsigned char vol_value = pixels[x + y*dims[0] + z*dims[1]*dims[0]];
				unsigned char our_value = evaluate(Vector3D(x, y, z));
				float error = our_value - vol_value;
//				if (our_value>0.5)
//				std::cout << QString("[%1,%2,%3] = %4 - %5 = %6")
//							 .arg(x).arg(y).arg(z)
//							 .arg(our_value).arg(vol_value).arg(error) << std::endl;
				sse += error*error;
			}
		}
	}
//	std::cout << "sse: " << sse << std::endl;
//	std::cout << "tot: " << dims[0]*dims[1]*dims[2] << std::endl;
	return sqrt(sse/(dims[0]*dims[1]*dims[2]));
}

}
