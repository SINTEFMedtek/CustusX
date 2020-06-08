/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPATIENTMODELSERVICE_H_
#define CXPATIENTMODELSERVICE_H_

#include "cxResourceExport.h"

#include <QObject>
#include <map>
#include "boost/shared_ptr.hpp"
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxDefinitions.h"

class QDateTime;
class QDomElement;

namespace cx
{
typedef boost::shared_ptr<class Data> DataPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
}

#define PatientModelService_iid "cx::PatientModelService"

namespace cx
{

class RegistrationTransform;
class LandmarkProperty;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class Landmarks> LandmarksPtr;
typedef boost::shared_ptr<class TransferFunctions3DPresets> PresetTransferFunctions3DPtr;
typedef boost::shared_ptr<class RegistrationHistory> RegistrationHistoryPtr;

/**
 * @brief The OperatingTable class
 *
 * The OperatingTable holds a transform and is a coordinate system fixed relative to the reference, R.
 * The definition is like Dicom, given that the patient lies on the back on the table, Left - Posterior - Superior.
 * This is like the default R.
 * The OT up direction can be changed in the preferences, and other objects can use it to find up and down vectors as needed.
 */
struct cxResource_EXPORT OperatingTable
{
    explicit OperatingTable()
		: rMot(Transform3D::Identity())
    {}
    explicit OperatingTable(Transform3D tr)
		: rMot(tr)
    {}
	Transform3D rMot; ///< Transform from OT to reference space.

	Vector3D getVectorUp() const {return rMot.vector(Vector3D(0,-1,0));} ///< Returns the table's up vector in R space.
};


/** \brief The virtual patient
 *
 * PatientModelService provides access to the Patient Specific Model (PaSM).
 *   - data entities
 *   	- volumes
 *   	- surfaces
 *   	- metrics
 *   	- etc
 *   - relations between entities in space, time and structure
 *   - load/save
 *
 *
 *  \ingroup cx_resource_core_data
 *  \date 2014-05-15
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT PatientModelService : public QObject
{
	Q_OBJECT
public:
	virtual ~PatientModelService() {}

	enum DataFilter
	{
		HideUnavailable,
		AllData
	};

	// core Data interface
	virtual void insertData(DataPtr data, bool overWrite = false) = 0;
	virtual std::map<QString, DataPtr> getDatas(DataFilter filter = HideUnavailable) const = 0;
	virtual std::map<QString, DataPtr> getChildren(QString parent_uid, QString of_type="") const = 0;
	/** Create Data object of given type.
	 *
	 *  uid must be unique, or contain the string %1 that will be replaced with a running
	 *  index that makes the uid unique. The same applies to name. An empty name will set
	 *  name = uid.
	 */
	virtual DataPtr createData(QString type, QString uid, QString name="") = 0;
	virtual void removeData(QString uid) = 0;

	// extended Data interface
	template <class DATA>
	std::map<QString, boost::shared_ptr<DATA> > getDataOfType() const;
	DataPtr getData(const QString& uid) const;
	template <class DATA>
	boost::shared_ptr<DATA> getData(const QString& uid) const;
	template<class DATA>
	boost::shared_ptr<DATA> createSpecificData(QString uid, QString name="");

	// streams
	virtual std::map<QString, VideoSourcePtr> getStreams() const = 0;
	VideoSourcePtr getStream(const QString &uid) const; ///< Convenience function getting a specified stream
	// patient registration
	virtual Transform3D get_rMpr() const;
	virtual RegistrationHistoryPtr get_rMpr_History() const = 0;

	// active data
	virtual ActiveDataPtr getActiveData() const = 0;

	// landmarks
	virtual LandmarksPtr getPatientLandmarks() const = 0; ///< landmark defined in patient space
	/** Get all defined landmarks.
	 *  These landmarks are additionally defined in specific coordinate spaces,
	 *  such as patient and for each Data.
	 */
	virtual std::map<QString, LandmarkProperty> getLandmarkProperties() const = 0;
	virtual void setLandmarkName(QString uid, QString name) = 0;
	virtual void setLandmarkActive(QString uid, bool active) = 0;
	virtual QString addLandmark() = 0;
	virtual void deleteLandmarks() = 0;
	// utility
	virtual void updateRegistration_rMpr(const QDateTime& oldTime, const RegistrationTransform& newTransform);

	virtual QString getActivePatientFolder() const = 0;
	QString generateFilePath(QString folderName, QString ending);

	virtual bool isPatientValid() const = 0;
	virtual DataPtr importData(QString fileName, QString &infoText) = 0;
	virtual void exportPatient(PATIENT_COORDINATE_SYSTEM externalSpace) = 0;

	virtual PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const = 0;

	virtual void setCenter(const Vector3D& center) = 0;
	virtual Vector3D getCenter() const = 0; ///< current common center point for user viewing.

    virtual void setOperatingTable(const OperatingTable& ot) = 0;
    virtual OperatingTable getOperatingTable() const = 0;

	virtual CLINICAL_VIEW getClinicalApplication() const = 0;
	virtual void setClinicalApplication(CLINICAL_VIEW application) = 0;

	virtual void autoSave() = 0;//TODO remove, and integrate into other functions
	virtual bool isNull() = 0;

	static PatientModelServicePtr getNullObject();

	virtual void makeAvailable(const QString& uid, bool available) = 0;///<Exclude this data from getDatas()

signals:
    void operatingTableChanged();
	void centerChanged(); ///< emitted when center is changed.
	void dataAddedOrRemoved();
	void landmarkPropertiesChanged(); ///< emitted when global info about a landmark changed
	void clinicalApplicationChanged();
	void rMprChanged();
	void streamLoaded();
	void patientChanged();
	void videoAddedToTrackedStream();
};


template <class DATA>
std::map<QString, boost::shared_ptr<DATA> > PatientModelService::getDataOfType() const
{
	std::map<QString, DataPtr> data = this->getDatas();
	std::map<QString, boost::shared_ptr<DATA> > retval;
	for (std::map<QString, DataPtr>::const_iterator i=data.begin(); i!=data.end(); ++i)
	{
		boost::shared_ptr<DATA> val = boost::dynamic_pointer_cast<DATA>(i->second);
		if (val)
			retval[val->getUid()] = val;
	}
	return retval;
}

template <class DATA>
boost::shared_ptr<DATA> PatientModelService::getData(const QString& uid) const
{
	return boost::dynamic_pointer_cast<DATA>(this->getData(uid));
}

template<class DATA>
boost::shared_ptr<DATA> PatientModelService::createSpecificData(QString uid, QString name)
{
	DataPtr retval = this->createData(DATA::getTypeName(), uid, name);
	return boost::dynamic_pointer_cast<DATA>(retval);
}

} // namespace cx
Q_DECLARE_INTERFACE(cx::PatientModelService, PatientModelService_iid)


#endif /* CXPATIENTMODELSERVICE_H_ */
