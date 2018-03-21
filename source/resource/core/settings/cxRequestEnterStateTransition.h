/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
