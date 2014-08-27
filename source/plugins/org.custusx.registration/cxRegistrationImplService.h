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

#ifndef CXREGISTRATIONIMPLSERVICE_H_
#define CXREGISTRATIONIMPLSERVICE_H_

#include "cxRegistrationService.h"
#include "org_custusx_registration_Export.h"
#include "qdatetime.h"

namespace cx
{
class RegistrationTransform;

/**
 * Implementation of Registration service.
 *
 * \ingroup org_custusx_registration
 *
 *  \date 2014-08-26
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Geir Arne Tangen, SINTEF
 */
class org_custusx_registration_EXPORT RegistrationImplService : public RegistrationService
{
	Q_INTERFACES(cx::RegistrationService)
public:
	RegistrationImplService();
	virtual ~RegistrationImplService();

	virtual void setMovingData(DataPtr data);
	virtual void setFixedData(DataPtr data);
	virtual DataPtr getMovingData();
	virtual DataPtr getFixedData();

	virtual void applyImage2ImageRegistration(Transform3D delta_pre_rMd, QString description);
	virtual void applyPatientRegistration(Transform3D rMpr_new, QString description);

	virtual QDateTime getLastRegistrationTime();
	virtual void setLastRegistrationTime(QDateTime time);

	//TODO: Make private
	virtual void updateRegistration(QDateTime oldTime, RegistrationTransform deltaTransform, DataPtr data, QString masterFrame);

private:
	DataPtr mFixedData; ///< the data that shouldn't update its matrices during a registrations
	DataPtr mMovingData; ///< the data that should update its matrices during a registration

	QDateTime mLastRegistrationTime; ///< last timestamp for registration during this session. All registrations in one session results in only one reg transform.
};

typedef boost::shared_ptr<RegistrationImplService> RegistrationImplServicePtr;

} /* namespace cx */

#endif /* CXREGISTRATIONIMPLSERVICE_H_ */

