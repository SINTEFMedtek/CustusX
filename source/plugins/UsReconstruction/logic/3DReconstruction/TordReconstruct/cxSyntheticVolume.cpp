#include "cxSyntheticVolume.h"
#include "vtkImageData.h"

namespace cx {

ProcessedUSInputDataPtr
cxSyntheticVolume::sampleUsData(const std::vector<Transform3D>& planes,
                                const Eigen::Array2f& pixelSpacing,
                                const Eigen::Array2i& sliceDimension) const
{

	std::vector<TimedPosition> positions;
	std::vector<vtkImageDataPtr> images;
	// For each plane
	for(std::vector<Transform3D>::const_iterator i = planes.begin();
	    planes.end() != i;
	    i++)
	{
		const Transform3D plane = *i;
		vtkImageDataPtr us_frame = vtkImageDataPtr::New();
		us_frame->SetExtent(0, sliceDimension[0]-1, 0, sliceDimension[1]-1, 0, 0);
		us_frame->SetSpacing(pixelSpacing[0], pixelSpacing[1], 0.0);

		unsigned char* us_data = (unsigned char*)us_frame->GetScalarPointer();
		// For each pixel on that plane
		for(unsigned int px = 0; px < sliceDimension[0]; px++)
		{
			for(unsigned int py = 0; py < sliceDimension[1]; py++)
			{
				// Transform it to volume space
				const Vector3D img_coords(pixelSpacing[0]*px, pixelSpacing[1]*py, 0.0);
				const Vector3D volume_coords = plane*img_coords;

				// Evaluate volume at that position
				const unsigned char val = this->evaluate(volume_coords[0],
				                                         volume_coords[1],
				                                         volume_coords[2]);

				// Store that value in the US slice
				us_data[px + py*sliceDimension[0]] = val;
			}
		}

		// Build the TimedPosition for this frame
		TimedPosition t;
		t.mTime = i - planes.begin();
		t.mPos = plane;

		positions.push_back(t);
		images.push_back(us_frame);
	}

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


float cxSyntheticVolume::computeRMSError(vtkImageDataPtr vol)
{
	float sse = 0.0f;
	int* dims = vol->GetDimensions();
	unsigned char *pixels = (unsigned char*)vol->GetScalarPointer();
	for(int z = 0; z < dims[2]; z++)
	{
		for(int y = 0; y < dims[1]; y++)
		{
			for(int x = 0; x < dims[0]; x++)
			{
				unsigned char vol_value = pixels[x + y*dims[0] + z*dims[1]*dims[0]];
				unsigned char our_value = evaluate(x, y, z);
				float error = our_value - vol_value;
				sse += error*error;
			}
		}
	}
	return sqrt(sse/(dims[0]*dims[1]*dims[2]));
}

}
