/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXPNNRECONSTRUCTIONSERVICE_H_
#define CXPNNRECONSTRUCTIONSERVICE_H_

#include "cxReconstructionService.h"
#include "org_custusx_pnnreconstruction_Export.h"
#include "cxTransform3D.h"
class ctkPluginContext;

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
	static ReconstructionServicePtr create(ctkPluginContext* context) { return ReconstructionServicePtr(new PNNReconstructionService(context)); }

	PNNReconstructionService(ctkPluginContext* context);
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

