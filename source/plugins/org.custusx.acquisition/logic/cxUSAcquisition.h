/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXUSACQUISITION_H_
#define CXUSACQUISITION_H_

#include "org_custusx_acquisition_Export.h"

#include <vector>
#include "cxForwardDeclarations.h"
#include "cxAcquisitionService.h"

namespace cx
{
struct USReconstructInputData;
}
namespace cx
{
typedef boost::shared_ptr<class UsReconstructionFileMaker> UsReconstructionFileMakerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;
typedef boost::shared_ptr<class USSavingRecorder> USSavingRecorderPtr;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;


/**
 * \file
 * \addtogroup org_custusx_acquisition
 * @{
 */

/**
 * \brief Handles the us acquisition process.
 * \ingroup org_custusx_acquisition
 *
 * The USAcquisition object attaches itself to an
 * input Acquisition object and records ultrasound
 * data when the Acquisiton records.
 *
 * After a successful acquisition, the data is both sent to
 * the reconstructer and saved to disk. saveDataCompleted() is
 * emitted after a successful save of each video stream.
 *
 *  \date May 12, 2011
 *  \author christiana
 */
class org_custusx_acquisition_EXPORT USAcquisition : public QObject
{
	Q_OBJECT
public:
	USAcquisition(AcquisitionPtr base, QObject* parent = 0);
	virtual ~USAcquisition();
	int getNumberOfSavingThreads() const;
	bool isReady(AcquisitionService::TYPES context) const;
	QString getInfoText(AcquisitionService::TYPES context) const;

signals:
	void acquisitionDataReady(); ///< emitted when data is acquired and sent to the reconstruction module
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

	/** Emitted if the readiness of the acq is changed.
	  * Use isReady() and getInfoText() get more info.
	  */
	void readinessChanged();

private slots:
	void checkIfReadySlot();
	void recordStarted();
	void recordStopped();
	void recordCancelled();

private:
	std::vector<VideoSourcePtr> getRecordingVideoSources(ToolPtr tool);
	bool getWriteColor();
	void sendAcquisitionDataToReconstructer();
	void setReady(bool val, QString text);

	VisServicesPtr getServices();
	UsReconstructionServicePtr getReconstructer();

	AcquisitionPtr mBase;
	USSavingRecorderPtr mCore;
	bool mReady;
	QString mInfoText;
};
typedef boost::shared_ptr<USAcquisition> USAcquisitionPtr;

/**
* @}
*/
}

#endif /* CXUSACQUISITION_H_ */
