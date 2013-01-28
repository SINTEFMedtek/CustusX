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

#ifndef CXPATIENTSERVICE_H_
#define CXPATIENTSERVICE_H_

#include <QObject>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

typedef boost::shared_ptr<class PatientData> PatientDataPtr;

/**\brief The virtual patient
 * \ingroup cxServicePatient
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
class PatientService: public QObject
{
Q_OBJECT
public:
	static PatientService* getInstance();

	static void initialize();
	static void shutdown();

	PatientDataPtr getPatientData();

private:
	static PatientService* mInstance;
	static void setInstance(PatientService* instance);

	PatientService();
	virtual ~PatientService();

	PatientService(PatientService const&); // not implemented
	PatientService& operator=(PatientService const&); // not implemented

	PatientDataPtr mPatientData;

	/**
	  * Clear the global cache used by the entire application (cx::DataLocations::getCachePath()).
	  */
	void clearCache();
};

PatientService* patientService();

/**
 * @}
 */
}

#endif /* CXPATIENTSERVICE_H_ */
