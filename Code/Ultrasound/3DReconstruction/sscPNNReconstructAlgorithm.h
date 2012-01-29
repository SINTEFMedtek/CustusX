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

#ifndef SSCPNNRECONSTRUCTALGORITHM_H_
#define SSCPNNRECONSTRUCTALGORITHM_H_

#include <vtkImageData.h>
#include "sscReconstructAlgorithm.h"
#include "sscImage.h"
#include "sscDoubleDataAdapterXml.h"
//#include "sscStringDataAdapterXml.h"


namespace ssc
{

/**
 * \addtogroup sscUSReconstruction
 * \{
 */

/**\brief PNN US reconstruction.
 *
 * A specialization of ReconstructAlgorithm that implements
 * a simple PNN (pixel-nearest-neighbour) algorithm.
 *
 *  \date June 23, 2010
 *  \author Ole Vegard Solberg
 */
class PNNReconstructAlgorithm: public ReconstructAlgorithm
{
public:
	PNNReconstructAlgorithm();
	virtual ~PNNReconstructAlgorithm()
	{
	}
	virtual QString getName() const
	{
		return "PNN";
	}
	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
	virtual void reconstruct(std::vector<TimedPosition> frameInfo, USFrameDataPtr frameData, ImagePtr outputData,
		ImagePtr frameMask, QDomElement settings);
	DoubleDataAdapterXmlPtr mInterpolationStepsOption;
private:
	//DoubleDataAdapterXmlPtr mInterpolationDistanceOption;
	bool validPixel(int x, int y, int* dims, unsigned char* rawPointer)
	{
		return (x >= 0) && (x < dims[0]) && (y >= 0) && (y < dims[1]) && (rawPointer[x + y * dims[0]] != 0);
	}

	bool validVoxel(int x, int y, int z, int* dims)
	{
		return (x >= 0) && (x < dims[0]) && (y >= 0) && (y < dims[1]) && (z >= 0) && (z < dims[2]);
	}

	void interpolate(ImagePtr inputData, ImagePtr outputData);
};

/**
 * \}
 */


}//namespace ssc
#endif //SSCPNNRECONSTRUCTALGORITHM_H_
