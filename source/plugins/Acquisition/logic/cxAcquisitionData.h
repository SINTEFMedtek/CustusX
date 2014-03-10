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

#ifndef CXACQUISITIONDATA_H_
#define CXACQUISITIONDATA_H_

#include <QObject>
class QDomNode;
#include <vector>
#include "cxRecordSession.h"
#include "cxLegacySingletons.h"

namespace cx
{
typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
}

namespace cx
{
/**
* \file
* \addtogroup cxPluginAcquisition
* @{
*/


/** Shared Data and resources for the Acquisition Plugin
 *
 *  \date Jun 9, 2011
 *  \author christiana
 */
class AcquisitionData : public QObject
{
	Q_OBJECT

public:
	AcquisitionData();
	AcquisitionData(ReconstructManagerPtr reconstructer);
	virtual ~AcquisitionData();

	void addRecordSession(RecordSessionPtr session);
	void removeRecordSession(RecordSessionPtr session);
	std::vector<RecordSessionPtr> getRecordSessions();
	RecordSessionPtr getRecordSession(QString uid);
	QString getNewUid();

	ReconstructManagerPtr getReconstructer() { return mReconstructer; };

	//Interface for saving/loading
	void addXml(QDomNode& dataNode); ///< adds xml information about the StateService and its variabels
	void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the StateService.

signals:
	void  recordedSessionsChanged();

private:
	std::vector<RecordSessionPtr> mRecordSessions;

	// referenced plugins:
	ReconstructManagerPtr mReconstructer;
};
typedef boost::shared_ptr<AcquisitionData> AcquisitionDataPtr;



/**Shared Data and resources for the Acquisition Plugin
 *
 */
class Acquisition : public QObject
{
	Q_OBJECT

public:
	Acquisition(AcquisitionDataPtr pluginData, QObject* parent = 0);
	virtual ~Acquisition();

	enum STATE
	{
		sRUNNING = 0,
		sPOST_PROCESSING = 1,
		sNOT_RUNNING = 2
	};

	/** Set ready status of acq.
	  */
	void setReady(bool val, QString text);
	bool isReady() const { return mReady; }
	QString getInfoText() const { return mInfoText; }

	/** Start or stop recording, depending on current state.
	  */
	void toggleRecord();
	/** Start recording.
	  * Change state to running, create session object.
	  */
	void startRecord();
	/** Stop recording.
	  * Change state to not_running, finalize session object and keep it available.
	  */
	void stopRecord();
	/** Cancel recording.
	  * Change state to not_running, remove session object.
	  */
	void cancelRecord();
	/** Start post processing.
	  * Change state to post_processing.
	  */
	void startPostProcessing();
	/** Stop post processing.
	  * Change state to not_running.
	  */
	void stopPostProcessing();

	/** Get latest recording session.
	  *
	  * NULL if no last recording or the last one was cancelled.
	  * If called during a recording it will return the ongoing recording.
	  */
	RecordSessionPtr getLatestSession() { return mLatestSession; }
	/** Return the current state
	  */
	STATE getState() const { return mCurrentState; }
	AcquisitionDataPtr getPluginData() { return mPluginData; }

signals:
	/** Emitted each time start/stop/cancel/startpp/stoppp is called.
	  */
	void stateChanged();
	/** emitted when state changes to sRUNNING
	  */
	void started();
	/** emitted when state changes from sRUNNING
	  * with valid recording data.
	  */
	void acquisitionStopped();
	/** emitted when state changes from sRUNNING
	  * without valid recording data.
	  */
	void cancelled();
	/** Emitted if the readiness of the acq is changed.
	  * Use isReady() and getInfoText() get more info.
	  */
	void readinessChanged();

private:
	RecordSessionPtr mLatestSession;
	void setState(STATE newState);
	STATE mCurrentState;
	AcquisitionDataPtr mPluginData;

	bool mReady;
	QString mInfoText;
};
typedef boost::shared_ptr<Acquisition> AcquisitionPtr;


/**
* @}
*/
}

#endif /* CXACQUISITIONDATA_H_ */
