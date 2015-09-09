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

#ifndef CXREQUESTENTERSTATETRANSITION_H_
#define CXREQUESTENTERSTATETRANSITION_H_

#include "cxResourceExport.h"

#include <QEvent>
#include <QAbstractTransition>

namespace cx
{
/**
 * \file
 * \ingroup org_custusx_core_state
 * @{
 */

/** \brief Utility class for StateService states.
 *
 */
struct cxResource_EXPORT  RequestEnterStateEvent: public QEvent
{
	RequestEnterStateEvent(const QString &stateUid) :
					QEvent(QEvent::Type(QEvent::User + 1)), mStateUid(stateUid)
	{}

	QString mStateUid;
};

/** \brief Utility class for StateService states.
 *
 * \date 5. aug. 2010
 * \\author jbake
 */
class cxResource_EXPORT RequestEnterStateTransition: public QAbstractTransition
{
public:
	RequestEnterStateTransition(const QString &stateUid) :
					mStateUid(stateUid)
	{}

protected:
	virtual bool eventTest(QEvent *e)
	{
		if (e->type() != QEvent::Type(QEvent::User + 1)) // StringEvent
			return false;
		RequestEnterStateEvent *se = static_cast<RequestEnterStateEvent*>(e);
		return (mStateUid == se->mStateUid);
	}

	virtual void onTransition(QEvent *)
	{}

private:
	QString mStateUid;
};

/**
 * @}
 */
} //namespace cx
#endif /* CXREQUESTENTERSTATETRANSITION_H_ */
