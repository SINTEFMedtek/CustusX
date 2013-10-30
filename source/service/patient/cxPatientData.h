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

#ifndef CXPATIENTDATA_H_
#define CXPATIENTDATA_H_

#include "boost/shared_ptr.hpp"
#include <QString>
#include <QObject>
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"
#include <QDomDocument>

class QDomDocument;

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

/**given a root node, use the /-separated path to descend
 * into the root children recursively. Create elements if
 * necessary.
 *
 */
QDomElement getElementForced(QDomNode root, QString path);

/**
 * \brief Functionality for storing patient data in a folder on the disk
 * and access to these data.
 * \ingroup cxServicePatient
 *
 * Keeps track of a single active patient (patient folder).
 * Only one such patient can be active at a time, since the
 * global managers are updated by this class.
 *
 *   \date May 18, 2010
 *   \author christiana
 *
 */
class PatientData: public QObject
{
Q_OBJECT
public:
	PatientData();
	virtual ~PatientData();

	QString getActivePatientFolder() const;
	bool isPatientValid() const;

	QDomElement getCurrentWorkingElement(QString path);
	QDomDocument getCurrentWorkingDocument();  ///< use only during save/load.

	QString generateFilePath(QString folderName, QString ending); ///< Creates the folder in the active patient and returns the path to a (not existing) filename generated using the current timestamp

public slots:
	void newPatient(QString choosenDir);
	void loadPatient(QString chosenDir);
	/** \brief Import data into CustusX
	 * \param fileName The file name of the imported file(s)
	 * \param[out] infoText Information about any errors/warnings that occurred during import
	 */
	DataPtr importData(QString fileName, QString &infoText);
	void removeData(QString uid);
	void savePatient(); ///< Save all application data to XML file
	void clearPatient();
	void exportPatient(bool niftiFormat);
	void autoSave();
	void startupLoadPatient();
	void writeRecentPatientData();

signals:
	void patientChanged();
	void cleared();

	void isSaving();
	void isLoading();

private:
	//patient
	QString getNullFolder() const;
	void setActivePatient(const QString& activePatientFolder); ///< set the activepatientfolder (absolute path)
	void createPatientFolders(QString choosenDir); ///< Create patient folders and save xml for new patient and for load patient for a directory whitout xml file.
	QString getCommandLineStartupPatient();

	//saving/loading
	void generateSaveDoc(QDomDocument& doc);
	void readLoadDoc(QDomDocument& loadDoc, QString patientFolder);

	/** \brief Copy file (=source) dest folder.
	 * \param source Name of file to be copied.
	 * \param dest Destination of copy operation
	 * \param[out] infoText Information about any errors/warnings that occurred during copy
	 */
	bool copyFile(QString source, QString dest, QString &infoText);

	/** \brief Copy filename and all files with the same name (and different extension)
	 * to destFolder.
	 * \param fileName Name of file to be copied. All files with the same first name will be copied (all different extensions in the same folder as fineName)
	 * \param destFolder Destination of copy operation
	 * \param[out] infoText Information about any errors/warnings that occurred during copy
	 */
//	bool copyAllSimilarFiles(QString fileName, QString destFolder, QString &infoText);
	static QString getVersionName();

	//Patient
	QString mActivePatientFolder; ///< Folder for storing the files for the active patient. Path relative to globalPatientDataFolder.
	QDomDocument mWorkingDocument; ///< available during load and save, used to add/extract extra info from the file.
};

typedef boost::shared_ptr<PatientData> PatientDataPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXPATIENTDATA_H_ */
