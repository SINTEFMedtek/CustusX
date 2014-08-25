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

#ifndef CXAPPLICATIONSTATE_H_
#define CXAPPLICATIONSTATE_H_

#include <iostream>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QAction>
#include "cxEnumConverter.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxRequestEnterStateTransition.h"
//#include "cxStateService.h"
#include "cxPatientData.h"

namespace cx
{
typedef boost::shared_ptr<class StateServiceBackend> StateServiceBackendPtr;

/**
 * \file
 * \addtogroup cx_service_state
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
	void setBackend(StateServiceBackendPtr backend);
	virtual void onEntry(QEvent * event);
	virtual void onExit(QEvent * event);
	virtual QString getUid() const;
	virtual QString getName() const;
	std::vector<ApplicationState*> getChildStates();
	QAction* createAction(QActionGroup* group);
	virtual CLINICAL_APPLICATION getClinicalApplication() const = 0;
protected slots:
	void setActionSlot();
protected:
	QString mUid;
	QString mName;
	QAction* mAction;
	bool mActive;
	StateServiceBackendPtr mBackend;
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
	virtual CLINICAL_APPLICATION getClinicalApplication() const { return mdCOUNT; }
};

class LaboratoryApplicationState: public ApplicationState
{
Q_OBJECT
public:
	LaboratoryApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(mdLABORATORY), enum2string(mdLABORATORY))
	{}
	virtual ~LaboratoryApplicationState() {}
	virtual CLINICAL_APPLICATION getClinicalApplication() const { return mdLABORATORY; }
};

class BronchoscopyApplicationState: public ApplicationState
{
Q_OBJECT
public:
	BronchoscopyApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(mdBRONCHOSCOPY), enum2string(mdBRONCHOSCOPY))
	{}
	virtual ~BronchoscopyApplicationState() {}
	virtual CLINICAL_APPLICATION getClinicalApplication() const { return mdBRONCHOSCOPY; }
};

class NeurologyApplicationState: public ApplicationState
{
Q_OBJECT
public:
	NeurologyApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(mdNEUROLOGY), enum2string(mdNEUROLOGY))
	{}
	virtual ~NeurologyApplicationState() {}
	virtual CLINICAL_APPLICATION getClinicalApplication() const { return mdNEUROLOGY; }
};

class LaparoscopyApplicationState: public ApplicationState
{
Q_OBJECT
public:
	LaparoscopyApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(mdLAPAROSCOPY), enum2string(mdLAPAROSCOPY))
	{}
	virtual ~LaparoscopyApplicationState() {}
	virtual CLINICAL_APPLICATION getClinicalApplication() const { return mdLAPAROSCOPY; }
};

class EndovascularApplicationState: public ApplicationState
{
Q_OBJECT
public:
	EndovascularApplicationState(QState* parent) :
					ApplicationState(parent, enum2string(mdENDOVASCULAR), enum2string(mdENDOVASCULAR))
	{}
	virtual ~EndovascularApplicationState() {}
	virtual CLINICAL_APPLICATION getClinicalApplication() const { return mdENDOVASCULAR; }
};

/**
 * @}
 */
} // namespace cx

#endif /* CXAPPLICATIONSTATE_H_ */
