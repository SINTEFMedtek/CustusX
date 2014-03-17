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

#ifndef CXREQUESTENTERSTATETRANSITION_H_
#define CXREQUESTENTERSTATETRANSITION_H_

#include <QEvent>
#include <QAbstractTransition>

namespace cx
{
/**
 * \file
 * \addtogroup cx_service_state
 * @{
 */

/** \brief Utility class for StateService states.
 *
 */
struct RequestEnterStateEvent: public QEvent
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
class RequestEnterStateTransition: public QAbstractTransition
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
