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

#ifndef CXRecordSession_H_
#define CXRecordSession_H_

#include "org_custusx_acquisition_Export.h"

#include <QString>
#include <map>
#include "boost/shared_ptr.hpp"
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"

class QDomNode;

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_acquisition
 * @{
 */

typedef boost::shared_ptr<class RecordSession> RecordSessionPtr;
typedef std::map<double, Transform3D> TimedTransformMap;

/**
 * RecordSession
 *
 * \brief
 *
 * \date Dec 8, 2010
 * \author Janne Beate Bakeng
 */
class org_custusx_acquisition_EXPORT RecordSession
{
public:
	RecordSession(QString uid, double startTime, double stopTime, QString description);
	RecordSession(int id, double startTime, double stopTime, QString description);
	virtual ~RecordSession();

	QString getUid();
	QString getDescription();
	QString getHumanDescription() const;
	double getStartTime();
	double getStopTime();

	void setStopTime(double val) { mStopTime = val; }

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);

	static TimedTransformMap getToolHistory_prMt(ToolPtr tool, RecordSessionPtr session);

protected:
	int getID() const;
	QString getCategory() const;
	QDateTime getTimestamp() const;

	QString mUid;
	double mStartTime;
	double mStopTime;
	QString mDescription;
};

/**
 * @}
 */
}//namespace cx

#endif /* CXRecordSession_H_ */
