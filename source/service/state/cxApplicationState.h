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

#ifndef CXAPPLICATIONSTATE_H_
#define CXAPPLICATIONSTATE_H_

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "sscEnumConverter.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxRequestEnterStateTransition.h"
//#include "cxStateService.h"
#include "cxPatientData.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceState
 * @{
 */

/**\brief State in a ApplicationStateMachine
 *
 *  \date Aug 17, 2010
 *  \author christiana
 */
class ApplicationState: public QState
{
Q_OBJECT
public:
	ApplicationState(QState* parent, QString uid, QString name);
	virtual ~ApplicationState();
	virtual void onEntry(QEvent * event);
	virtual void onExit(QEvent * event);
	virtual QString getUid() const;
	virtual QString getName() const;
	std::vector<ApplicationState*> getChildStates();
	QAction* createAction(QActionGroup* group);
	virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const = 0;
protected slots:
	void setActionSlot();
protected:
	QString mUid;
	QString mName;
	QAction* mAction;
	bool mActive;
};

class ParentApplicationState: public ApplicationState
{
Q_OBJECT
public:
	ParentApplicationState(QState* parent) :
					ApplicationState(parent, "ParentUid", "Parent")
	{
	}
	virtual void onEntry(QEvent * event) {}
	virtual void onExit(QEvent * event) {}
	virtual ~ParentApplicationState() {}
	virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdCOUNT; }
};

class LaboratoryApplicationState: public ApplicationState
{
Q_OBJECT
public:
	LaboratoryApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(ssc::mdLABORATORY), enum2string(ssc::mdLABORATORY))
	{}
	virtual ~LaboratoryApplicationState() {}
	virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdLABORATORY; }
};

class BronchoscopyApplicationState: public ApplicationState
{
Q_OBJECT
public:
	BronchoscopyApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(ssc::mdBRONCHOSCOPY), enum2string(ssc::mdBRONCHOSCOPY))
	{}
	virtual ~BronchoscopyApplicationState() {}
	virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdBRONCHOSCOPY; }
};

class NeurologyApplicationState: public ApplicationState
{
Q_OBJECT
public:
	NeurologyApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(ssc::mdNEUROLOGY), enum2string(ssc::mdNEUROLOGY))
	{}
	virtual ~NeurologyApplicationState() {}
	virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdNEUROLOGY; }
};

class LaparoscopyApplicationState: public ApplicationState
{
Q_OBJECT
public:
	LaparoscopyApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(ssc::mdLAPAROSCOPY), enum2string(ssc::mdLAPAROSCOPY))
	{}
	virtual ~LaparoscopyApplicationState() {}
	virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdLAPAROSCOPY; }
};

class EndovascularApplicationState: public ApplicationState
{
Q_OBJECT
public:
	EndovascularApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(ssc::mdENDOVASCULAR), enum2string(ssc::mdENDOVASCULAR))
	{}
	virtual ~EndovascularApplicationState() {}
	virtual ssc::CLINICAL_APPLICATION getClinicalApplication() const { return ssc::mdENDOVASCULAR; }
};

/**
 * @}
 */
} // namespace cx

#endif /* CXAPPLICATIONSTATE_H_ */
