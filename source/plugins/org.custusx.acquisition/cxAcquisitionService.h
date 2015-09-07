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
	virtual void toggleRecord(TYPES context) = 0;
	virtual void startRecord(TYPES context) = 0;
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
