/*
 * cxPatientData.h
 *
 *  Created on: May 18, 2010
 *      Author: christiana
 */

#ifndef CXPATIENTDATA_H_
#define CXPATIENTDATA_H_

#include "boost/shared_ptr.hpp"
#include <QString>
#include <QObject>
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"

class QDomDocument;

namespace cx
{

/**Functionality for storing patient data in a folder on the disk
 * and access to these data.
 * Keeps track of a single active patient (patient folder).
 * Only one such patient can be active at a time, since the
 * global managers are updated by this class.
 */
class PatientData : public QObject
{
  Q_OBJECT
public:
  PatientData();
  virtual ~PatientData() {}

  QString getActivePatientFolder() const;
  bool isPatientValid() const;

public slots:
  void newPatient(QString choosenDir);
  void loadPatient(QString chosenDir);
  ssc::DataPtr importData(QString fileName);
  void savePatient();///< Save all application data to XML file
  void clearPatient();
  void exportPatient();


signals:
  void patientChanged();

private:
  //patient
  QString getNullFolder() const;
  void setActivePatient(const QString& activePatientFolder); ///< set the activepatientfolder (absolute path)
  void createPatientFolders(QString choosenDir); ///< Create patient folders and save xml for new patient and for load patient for a directory whitout xml file.
  vtkPolyDataPtr mergeTransformIntoPolyData(vtkPolyDataPtr poly, ssc::Transform3D rMd);

  //saving/loading
  void generateSaveDoc(QDomDocument& doc);
  void readLoadDoc(QDomDocument& loadDoc, QString patientFolder);

  bool copyFile(QString source, QString dest);
  bool copyAllSimilarFiles(QString fileName, QString destFolder);

  //Patient
  QString mActivePatientFolder; ///< Folder for storing the files for the active patient. Path relative to globalPatientDataFolder.
};

typedef boost::shared_ptr<PatientData> PatientDataPtr;

} // namespace cx

#endif /* CXPATIENTDATA_H_ */
