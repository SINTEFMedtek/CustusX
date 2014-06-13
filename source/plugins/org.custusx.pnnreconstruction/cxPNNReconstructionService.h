// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXPNNRECONSTRUCTIONSERVICE_H_
#define CXPNNRECONSTRUCTIONSERVICE_H_

#include "cxReconstructionService.h"
#include "org_custusx_pnnreconstruction_Export.h"
#include "cxTransform3D.h"

namespace cx
{

/**
 * Implementation of PNN reconstruction service.
 * A specialization of ReconstructAlgorithm that implements
 * a simple PNN (pixel-nearest-neighbour) algorithm.
 *
 * \ingroup org_custusx_pnnreconstruction
 *
 * \date 2014-06-12
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_pnnreconstruction_EXPORT PNNReconstructionService : public ReconstructionService
{
	Q_INTERFACES(cx::ReconstructionService)
public:
	static ReconstructionServicePtr create() { return ReconstructionServicePtr(new PNNReconstructionService()); }

	PNNReconstructionService();
	virtual ~PNNReconstructionService();

	virtual QString getName() const;

	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
	virtual bool reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings);


private:
	DoubleDataAdapterXmlPtr getInterpolationStepsOption(QDomElement root);
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
typedef boost::shared_ptr<PNNReconstructionService> PNNReconstructionServicePtr;

} /* namespace cx */

#endif /* CXPNNRECONSTRUCTIONSERVICE_H_ */

