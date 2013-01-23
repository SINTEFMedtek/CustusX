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

/*
 *  sscThunderVNNReconstructAlgorithm.h
 *
 *  Created by Ole Vegard Solberg on 5/6/10.
 *
 */

#ifndef SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_
#define SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_

#include "sscReconstructAlgorithm.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"

namespace ssc
{

/**
 * \addtogroup sscUSReconstruction
 * \{
 */

/**\brief VNN US reconstruction using Thunder GPU algo.
 *
 * A specialization of ReconstructAlgorithm that glues the sscThunder
 * module together with the \ref sscUSReconstruction module.
 *
 * It calls the VNN (voxel-nearest-neighbour) algorithm.
 *
 *  \date May 6, 2010
 *  \author Ole Vegard Solberg
 */
class ThunderVNNReconstructAlgorithm: public ReconstructAlgorithm
{
public:
	ThunderVNNReconstructAlgorithm(QString shaderPath);
	virtual ~ThunderVNNReconstructAlgorithm()
	{
	}
	virtual QString getName() const;
	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
	virtual bool reconstruct(ProcessedUSInputDataPtr input,
							 vtkImageDataPtr outputData, QDomElement settings);

	StringDataAdapterXmlPtr getProcessorOption(QDomElement root);
	DoubleDataAdapterXmlPtr getDistanceOption(QDomElement root);
	BoolDataAdapterPtr getPrintOpenCLInfoOption(QDomElement root);
//	StringDataAdapterXmlPtr mProcessorOption;///< Select the processor to run the ocl code on (CPU/GPU)
private:
//	DoubleDataAdapterXmlPtr mDistanceOption;
//	BoolDataAdapterXmlPtr mPrintOpenCLInfoOption;
	QString mShaderPath;
};

/**
 * \}
 */


}//namespace

#endif //SSCTHUNDERVNNRECONSTRUCTALGORITHM_H_
