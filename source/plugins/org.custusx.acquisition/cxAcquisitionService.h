/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXACQUSITIONSERVICE_H
#define CXACQUSITIONSERVICE_H

#include "org_custusx_acquisition_Export.h"

#include <QObject>
#include <vector>
#include <boost/shared_ptr.hpp>
class QDomNode;

#define AcquisitionService_iid "cx::AcquisitionService"

namespace cx
{

typedef boost::shared_ptr<class AcquisitionService> AcquisitionServicePtr;
typedef boost::shared_ptr<class RecordSession> RecordSessionPtr;

/** \brief Acqusition services abstract interface
 *
 *  \ingroup org_custusx_acqiusition
 *  \date 2014-11-26
 *  \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_acquisition_EXPORT AcquisitionService : public QObject
{
	Q_OBJECT
public:
	virtual ~AcquisitionService() {}

	enum TYPE
	{
		tTRACKING,
		tUS,
		tCOUNT
	};
	typedef QFlags<TYPE> TYPES;
//	Q_DECLARE_FLAGS( TYPES, TYPE);

	enum STATE
	{
		sRUNNING = 0,
		sPOST_PROCESSING = 1,
		sNOT_RUNNING = 2
	};

	// Core interface

	virtual bool isNull() = 0;
	static AcquisitionServicePtr getNullObject();

	virtual RecordSessionPtr getLatestSession() = 0;
	virtual std::vector<RecordSessionPtr> getSessions() = 0;

	virtual bool isReady(TYPES context) const = 0;
	virtual QString getInfoText(TYPES context) const = 0;
	virtual STATE getState() const = 0;
//	virtual void toggleRecord(TYPES context, QString category);
	virtual void startRecord(TYPES context, QString category, RecordSessionPtr session = RecordSessionPtr()) = 0;
	virtual void stopRecord() = 0;
	virtual void cancelRecord() = 0;
	virtual void startPostProcessing() = 0;
	virtual void stopPostProcessing() = 0;

	virtual int getNumberOfSavingThreads() const = 0;

	// Extented interface

	RecordSessionPtr getSession(QString uid);

signals:
	void started();
	void cancelled();
	void stateChanged();
	void usReadinessChanged();
	void acquisitionStopped();

	void recordedSessionsChanged();

	// From USAcquisition
	void acquisitionDataReady(); ///< emitted when data is acquired and sent to the reconstruction module
	void saveDataCompleted(QString mhdFilename);///< emitted when data has been saved to file
};

} // cx
Q_DECLARE_INTERFACE(cx::AcquisitionService, AcquisitionService_iid)

#endif // CXACQUSITIONSERVICE_H
