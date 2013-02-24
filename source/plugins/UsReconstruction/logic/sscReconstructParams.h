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
#ifndef SSCRECONSTRUCTPARAMS_H
#define SSCRECONSTRUCTPARAMS_H

#include "sscXmlOptionItem.h"
#include "cxForwardDeclarations.h"

namespace ssc
{

/**
 * \file
 * \addtogroup cxPluginUsReconstruction
 * @{
 */

/** \brief Collection of reconstruction parameters.
 *
 * These parameters are used by the ReconstructionManager, the core reconstruction
 * use the ssc::ReconstructCore::InputParams instead.
 */
class ReconstructParams : public QObject
{
	Q_OBJECT

public:
	ReconstructParams(XmlOptionFile settings);
	virtual ~ReconstructParams();

	StringDataAdapterXmlPtr mOrientationAdapter;
	StringDataAdapterXmlPtr mPresetTFAdapter;
	StringDataAdapterXmlPtr mAlgorithmAdapter;
	StringDataAdapterXmlPtr mMaskReduce;//Reduce mask size in % in each direction
	BoolDataAdapterXmlPtr mAlignTimestamps; ///align track and frame timestamps to each other automatically
	DoubleDataAdapterXmlPtr mTimeCalibration; ///set a offset in the frame timestamps
	DoubleDataAdapterXmlPtr mMaxVolumeSize; ///< Set max size of output volume.
	BoolDataAdapterXmlPtr mAngioAdapter; ///US angio data is used as input
	BoolDataAdapterXmlPtr mCreateBModeWhenAngio; /// If angio requested, create a B-mode reoconstruction based on the same data set.

	XmlOptionFile mSettings;

signals:
	void changedInputSettings();
	void transferFunctionChanged();
};
typedef boost::shared_ptr<class ReconstructParams> ReconstructParamsPtr;

/**
 * \}
 */

}//namespace

#endif // SSCRECONSTRUCTPARAMS_H
