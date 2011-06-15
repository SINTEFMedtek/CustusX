/*
 * cxPatientService.h
 *
 *  Created on: Jun 14, 2011
 *      Author: christiana
 */

#ifndef CXPATIENTSERVICE_H_
#define CXPATIENTSERVICE_H_

#include <QObject>
#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class PatientData> PatientDataPtr;

/**
 * PatientService provides access to the Patient Specific Model (PaSM).
 *   - data entities
 *   - relations between entities in space and time and structure
 *   - load/save + simple operations
 *
 *
 *   At least, this is the goal. Just now, it holds the old
 *   PatientData object, which manages save/load session/patient.
 */
class PatientService : public QObject
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
};

PatientService* patientService();

}

#endif /* CXPATIENTSERVICE_H_ */
