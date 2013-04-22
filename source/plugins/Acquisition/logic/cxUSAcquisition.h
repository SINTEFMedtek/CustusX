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
#ifndef CXUSACQUISITION_H_
#define CXUSACQUISITION_H_

#include <vector>
#include "cxForwardDeclarations.h"

namespace ssc
{
class USReconstructInputData;
}
namespace cx
{
typedef boost::shared_ptr<class UsReconstructionFileMaker> UsReconstructionFileMakerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;
typedef boost::shared_ptr<class USSavingRecorder> USSavingRecorderPtr;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;


/**
 * \file
 * \addtogroup cxPluginAcquisition
 * @{
 */

/**
 * \brief Handles the us acquisition process.
 * \ingroup cxPluginAcquisition
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
class USAcquisition : public QObject
{
	Q_OBJECT
public:
	USAcquisition(AcquisitionPtr base, QObject* parent = 0);
	virtual ~USAcquisition();
	int getNumberOfSavingThreads() const;

signals:
	void acquisitionDataReady(); ///< emitted when data is acquired and sent to the reconstruction module
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

private slots:
	void checkIfReadySlot();
	void recordStarted();
	void recordStopped();
	void recordCancelled();

private:
	std::vector<ssc::VideoSourcePtr> getRecordingVideoSources(ssc::ToolPtr tool);
	bool getWriteColor() const;
	void sendAcquisitionDataToReconstructer();

	AcquisitionPtr mBase;
	USSavingRecorderPtr mCore;
};
typedef boost::shared_ptr<USAcquisition> USAcquisitionPtr;

/**
* @}
*/
}

#endif /* CXUSACQUISITION_H_ */
