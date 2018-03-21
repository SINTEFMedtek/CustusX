/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPNNRECONSTRUCTIONMETHODSERVICE_H_
#define CXPNNRECONSTRUCTIONMETHODSERVICE_H_

#include "cxReconstructionMethodService.h"
#include "org_custusx_usreconstruction_pnn_Export.h"
#include "cxTransform3D.h"
class ctkPluginContext;

namespace cx
{

/**
 * Implementation of PNN reconstruction service.
 * A specialization of ReconstructAlgorithm that implements
 * a simple PNN (pixel-nearest-neighbour) algorithm.
 *
 * \ingroup org_custusx_usreconstruction_pnn
 *
 * \date 2014-06-12
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_usreconstruction_pnn_EXPORT PNNReconstructionMethodService : public ReconstructionMethodService
{
	Q_INTERFACES(cx::ReconstructionMethodService)
public:
	PNNReconstructionMethodService(ctkPluginContext* context);
	virtual ~PNNReconstructionMethodService();

	virtual QString getName() const;

	virtual std::vector<PropertyPtr> getSettings(QDomElement root);
	virtual bool reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings);


private:
	DoublePropertyPtr getInterpolationStepsOption(QDomElement root);
	bool validPixel(int x, int y, const Eigen::Array3i& dims, unsigned char* rawPointer)
	{
		return (x >= 0) && (x < dims[0]) && (y >= 0) && (y < dims[1]) && (rawPointer[x + y * dims[0]] != 0);
	}

	bool validVoxel(int x, int y, int z, const int* dims)
	{
		return (x >= 0) && (x < dims[0]) && (y >= 0) && (y < dims[1]) && (z >= 0) && (z < dims[2]);
	}

	void interpolate(ImagePtr inputData, vtkImageDataPtr outputData, QDomElement settings);
	vtkImageDataPtr createMask(vtkImageDataPtr inputData);
	void fillHole(unsigned char *inputPointer, unsigned char *outputPointer, int x, int y, int z, const Eigen::Array3i& dim, int interpolationSteps);


};
//typedef boost::shared_ptr<PNNReconstructionMethodService> PNNReconstructionMethodService*;

} /* namespace cx */

#endif /* CXPNNRECONSTRUCTIONMETHODSERVICE_H_ */

