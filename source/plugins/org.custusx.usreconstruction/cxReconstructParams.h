/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRECONSTRUCTPARAMS_H_
#define CXRECONSTRUCTPARAMS_H_

#include "org_custusx_usreconstruction_Export.h"
#include <memory>

#include "cxXmlOptionItem.h"
#include "cxForwardDeclarations.h"


namespace cx
{

/**
 * \file
 * \addtogroup org_custusx_usreconstruction
 * @{
 */

/** \brief Collection of reconstruction parameters.
 *
 * These parameters are used by the ReconstructionManager, the core reconstruction
 * use the ReconstructCore::InputParams instead.
 */
class org_custusx_usreconstruction_EXPORT ReconstructParams : public QObject
{
	Q_OBJECT

public:
	ReconstructParams(PatientModelServicePtr patientModelService, XmlOptionFile settings);
	virtual ~ReconstructParams();

	PropertyPtr getParameter(QString uid);
	QStringList getParameterUids() const;

	StringPropertyPtr getOrientationAdapter() { this->createParameters(); return mOrientationAdapter; }
	StringPropertyPtr getPresetTFAdapter() { this->createParameters(); return mPresetTFAdapter; }
	StringPropertyPtr getAlgorithmAdapter() { this->createParameters(); return mAlgorithmAdapter; }
	StringPropertyPtr getMaskReduce() { this->createParameters(); return mMaskReduce; }
    StringPropertyPtr getPosFilterStrength() { this->createParameters(); return mPosFilterStrength; }
	StringPropertyPtr getPosFilterType() { this->createParameters(); return mPosFilterType; }
	DoublePropertyPtr getHoltAlpha() { this->createParameters(); return mHoltAlpha; }
	DoublePropertyPtr getHoltBeta() { this->createParameters(); return mHoltBeta; }
	BoolPropertyPtr getHoltBothDirections() { this->createParameters(); return mHoltBothDirections; }
	BoolPropertyPtr getHoltAddTrend() { this->createParameters(); return mHoltAddTrend; }
	BoolPropertyPtr getAlignTimestamps() { this->createParameters(); return mAlignTimestamps; }
    BoolPropertyPtr getPositionThinning() { this->createParameters(); return mPositionThinning; }
	DoublePropertyPtr getTimeCalibration() { this->createParameters(); return mTimeCalibration; }
	DoublePropertyPtr getMaxVolumeSize() { this->createParameters(); return mMaxVolumeSize; }
	BoolPropertyPtr getAngioAdapter() { this->createParameters(); return mAngioAdapter; }
	BoolPropertyPtr getCreateBModeWhenAngio() { this->createParameters(); return mCreateBModeWhenAngio; }

signals:
	void changedInputSettings();
//	void transferFunctionChanged();

private slots:
	void transferFunctionChangedSlot();
	void onPatientChanged();

private:
	std::map<QString, PropertyPtr> mParameters;
	void createParameters();

	StringPropertyPtr mOrientationAdapter;
	StringPropertyPtr mPresetTFAdapter;
	StringPropertyPtr mAlgorithmAdapter;
	StringPropertyPtr mMaskReduce;//Reduce mask size in % in each direction
    StringPropertyPtr mPosFilterStrength; //position noise filter strength (moving average)
	StringPropertyPtr mPosFilterType; ///< Off, Moving average or Holt
	DoublePropertyPtr mHoltAlpha; ///< Holt filter level smoothing
	DoublePropertyPtr mHoltBeta; ///< Holt filter trend smoothing
	BoolPropertyPtr mHoltBothDirections; ///< Holt filter forward and backward
	BoolPropertyPtr mHoltAddTrend; ///< Holt filter output level + trend
	BoolPropertyPtr mAlignTimestamps; ///align track and frame timestamps to each other automatically
    BoolPropertyPtr mPositionThinning; ///remove outlier positions from position sequence
	DoublePropertyPtr mTimeCalibration; ///set a offset in the frame timestamps
	DoublePropertyPtr mMaxVolumeSize; ///< Set max size of output volume.
	BoolPropertyPtr mAngioAdapter; ///US angio data is used as input
	BoolPropertyPtr mCreateBModeWhenAngio; /// If angio requested, create a B-mode reoconstruction based on the same data set.

	PatientModelServicePtr mPatientModelService;
	XmlOptionFile mSettings;
	void add(PropertyPtr param);
};
typedef std::shared_ptr<class ReconstructParams> ReconstructParamsPtr;

/**
 * \}
 */

}//namespace

#endif // CXRECONSTRUCTPARAMS_H_
