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

#include "cxPluginAcquisitionExport.h"

#include <QObject>
#include <vector>
#include <boost/shared_ptr.hpp>

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
class cxPluginAcquisition_EXPORT AcquisitionService : public QObject
{
	Q_OBJECT
public:
	virtual ~AcquisitionService() {}

	// Core interface

	virtual bool isNull() = 0;
	static AcquisitionServicePtr getNullObject();

	virtual RecordSessionPtr getLatestSession() = 0;
	virtual std::vector<RecordSessionPtr> getSessions() = 0;

	// Extented interface

	RecordSessionPtr getSession(QString uid);
};

} // cx
Q_DECLARE_INTERFACE(cx::AcquisitionService, AcquisitionService_iid)

#endif // CXACQUSITIONSERVICE_H
