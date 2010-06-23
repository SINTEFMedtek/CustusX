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

class QDomDocument;
//class QSettings;
typedef boost::shared_ptr<class QSettings> QSettingsPtr;

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
  PatientData(QObject* parent);
  virtual ~PatientData() {}

  QString getActivePatientFullPath() const;
  QString getActivePatientFolder() const;

public slots:
  void newPatient(QString choosenDir);
  void loadPatient(QString chosenDir);
  void importData(QString fileName);
  void savePatient();///< Save all application data to XML file

signals:
  void patientChanged();

private:
  //patient
  void setActivePatient(const QString& activePatientFolder); ///< set the activepatientfolder (relative to the globalPatientDataFolder)
  void createPatientFolders(QString choosenDir); ///< Create patient folders and save xml for new patient and for load patient for a directory whitout xml file.

  //saving/loading
  void generateSaveDoc(QDomDocument& doc);
  void readLoadDoc(QDomDocument& loadDoc);

  //Patient
  QString mActivePatientFolder; ///< Folder for storing the files for the active patient. Path relative to globalPatientDataFolder.
  //Preferences
  QSettingsPtr mSettings; ///< Object for storing all program/user specific settings

};

typedef boost::shared_ptr<PatientData> PatientDataPtr;

} // namespace cx

#endif /* CXPATIENTDATA_H_ */
