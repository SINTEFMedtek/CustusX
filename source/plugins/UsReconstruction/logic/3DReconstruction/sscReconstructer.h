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

#ifndef SSCRECONSTRUCTER_H_
#define SSCRECONSTRUCTER_H_

#include <QObject>
#include <QThread>
#include <math.h>
#include "sscReconstructAlgorithm.h"
#include "sscBoundingBox3D.h"
#include "sscReconstructedOutputVolumeParams.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
#include "sscXmlOptionItem.h"
#include "sscProbeSector.h"

#include "sscReconstructCore.h"

namespace ssc
{

/**
 * \addtogroup sscUSReconstruction
 * \{
 */

/**\brief Collection of reconstruction parameters.
 *
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

typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;

/**
 * \brief Manager for the us reconstruction process.
 *
 * Create a 3D volume based on a set of 2D images with 3D position.
 *
 * \todo The interface is large and fuzzy - tighten it.
 *
 * Used coordinate systems:
 *  - u  = raw input Ultrasound frames (in x, y. Origin lower left.)
 *  - t  = Tool space for probe as defined in ssc:Tool (z in ray direction, y to the left)
 *  - s  = probe localizer Sensor.
 *  - pr = Patient Reference localizer sensor.
 *  - d  = Output Data space
 *
 * \ingroup sscUSReconstruction
 *
 * \author Ole Vegard Solberg
 * \author Christian Askeland
 * \date May 4, 2010
 */
class Reconstructer: public QObject
{
	Q_OBJECT

public:
	Reconstructer(XmlOptionFile settings, QString shaderPath);
	virtual ~Reconstructer();

	void setInputData(USReconstructInputData fileData);
	ReconstructCorePtr createCore(); ///< used for threaded reconstruction
	ReconstructCorePtr createDualCore(); ///< core version for B-mode in case of angio recording.

	ReconstructParamsPtr mParams;
	std::vector<DataAdapterPtr> mAlgoOptions;

	OutputVolumeParams getOutputVolumeParams() const;
	void setOutputVolumeParams(const OutputVolumeParams& par);
	void setOutputRelativePath(QString path);
	void setOutputBasePath(QString path);
	void clearAll();

public slots:
	void setSettings();
	void transferFunctionChangedSlot();

signals:
	void paramsChanged();
	void algorithmChanged();
	void inputDataSelected(QString mhdFileName);

private:
	USReconstructInputData mFileData;
	USReconstructInputData mOriginalFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.
//	bool mIsReconstructing; ///< use for blocking of parameter setting while reconstructing.
	OutputVolumeParams mOutputVolumeParams;
	XmlOptionFile mSettings;
	QString mOutputRelativePath;///< Relative path to the output image
	QString mOutputBasePath;///< Global path where the relative path starts, for the output image
	QString mShaderPath; ///< name of shader folder
	double mMaxTimeDiff; ///< The largest allowed time deviation for the positions used in the frame interpolations

	void createAlgorithm();
	void updateFromOriginalFileData();
	ReconstructCore::InputParams createCoreParameters();

	bool validInputData() const;///< checks if internal states is valid (that it actually has frames to reconstruct)
};

/**
 * \}
 */

}//namespace
#endif //SSCRECONSTRUCTER_H_
