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
#ifndef CXELASTIXMANAGER_H_
#define CXELASTIXMANAGER_H_

#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxBoolProperty.h"
#include "cxStringProperty.h"
#include "cxElastixParameters.h"
#include "cxRegServices.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration_method_commandline
 * @{
 */

typedef boost::shared_ptr<class ElastixExecuter> ElastixExecuterPtr;

/**
 * \brief Manager for interfacing to the ElastiX registration package.
 *
 * \date Feb 4, 2012
 * \author Christian Askeland, SINTEF
 */
class ElastixManager : public QObject
{
	Q_OBJECT
public:
	ElastixManager(RegServicesPtr services);
	virtual ~ElastixManager();

	BoolPropertyPtr getDisplayProcessMessages() { return mDisplayProcessMessages; }
	BoolPropertyPtr getDisableRendering() { return mDisableRendering; }
	ElastixExecuterPtr getExecuter() { return mExecuter; }
	ElastixParametersPtr getParameters() { return mParameters; }

	void execute();
signals:
	void elastixChanged();

private slots:
	void executionFinishedSlot();
	void preprocessExecuter();

private:
	void addNonlinearData();

	ElastixParametersPtr mParameters;
	XmlOptionFile mOptions;
	BoolPropertyPtr mDisplayProcessMessages;
	BoolPropertyPtr mDisableRendering;
	ElastixExecuterPtr mExecuter;
	RegServicesPtr mServices;
};
typedef boost::shared_ptr<ElastixManager> ElastixManagerPtr;

/**
 * @}
 */
} /* namespace cx */
#endif /* CXELASTIXMANAGER_H_ */
