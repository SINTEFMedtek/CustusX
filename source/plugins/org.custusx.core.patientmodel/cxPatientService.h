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

#ifndef CXPATIENTSERVICE_H_
#define CXPATIENTSERVICE_H_

#include "org_custusx_core_patientmodel_Export.h"

#include <QObject>
#include "boost/shared_ptr.hpp"
#include "cxForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_service_patient
 * @{
 */

typedef boost::shared_ptr<class PatientData> PatientDataPtr;
typedef boost::shared_ptr<class PatientService> PatientServicePtr;
typedef boost::shared_ptr<class DataManager> DataServicePtr;

/**\brief The virtual patient
 * \ingroup cx_service_patient
 *
 * PatientService provides access to the Patient Specific Model (PaSM).
 *   - data entities
 *   - relations between entities in space and time and structure
 *   - load/save + simple operations
 *
 *
 *   At least, this is the goal. Just now, it holds the old
 *   PatientData object, which manages save/load session/patient.
 *
 *  \date Jun 14, 2011
 *  \author christiana
 *
 */
class org_custusx_core_patientmodel_EXPORT PatientService: public QObject
{
Q_OBJECT
public:
	static PatientServicePtr create(DataServicePtr dataService);
	virtual ~PatientService();

//	static PatientService* getInstance();

//	static void initialize();
//	static void shutdown();

	PatientDataPtr getPatientData();
	DataServicePtr getDataService();

private:
//	static PatientService* mInstance;
//	static void setInstance(PatientService* instance);

	PatientService(DataServicePtr dataService);

	PatientService(PatientService const&); // not implemented
	PatientService& operator=(PatientService const&); // not implemented

	PatientDataPtr mPatientData;
	DataServicePtr mDataService;

	/**
	  * Clear the global cache used by the entire application (cx::DataLocations::getCachePath()).
	  */
	void clearCache();
};

//PatientService* patientService();

/**
 * @}
 */
}

#endif /* CXPATIENTSERVICE_H_ */
