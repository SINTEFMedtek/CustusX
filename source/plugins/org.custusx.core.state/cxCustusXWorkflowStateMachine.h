/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCUSTUSXWORKFLOWSTATEMACHINE_H_
#define CXCUSTUSXWORKFLOWSTATEMACHINE_H_

#include "org_custusx_core_state_Export.h"

#include <QObject>
#include "boost/shared_ptr.hpp"
#include "cxWorkflowStateMachine.h"

namespace cx
{

/** \brief State Machine for the Workflow Steps for CustusX
 *
 *  See StateService for a description.
 *
 * \ingroup org_custusx_core_state
 * \date 9. sept. 2015
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_core_state_EXPORT CustusXWorkflowStateMachine : public WorkflowStateMachine
{
Q_OBJECT
public:
		CustusXWorkflowStateMachine(VisServicesPtr services);
        virtual ~CustusXWorkflowStateMachine();

};

typedef boost::shared_ptr<CustusXWorkflowStateMachine> CustusXWorkflowStateMachinePtr;
}

#endif /* CXCUSTUSXWORKFLOWSTATEMACHINE_H_ */
