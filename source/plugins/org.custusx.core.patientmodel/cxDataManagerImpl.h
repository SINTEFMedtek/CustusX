/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXDATAMANAGERIMPL_H_
#define CXDATAMANAGERIMPL_H_

#include "org_custusx_core_patientmodel_Export.h"
#include "cxPrecompiledHeader.h"

#include <map>
#include <set>
#include <string>
#include <QMutex>
#include <vector>
#include "cxImage.h"
#include "cxMesh.h"
#include "cxDataManager.h"
#include <QFileInfo>
#include "boost/scoped_ptr.hpp"
#include "cxPatientModelService.h"

class QDomElement;

namespace cx
{

typedef boost::shared_ptr<class DataManager> DataServicePtr;
typedef boost::shared_ptr<class DataManagerImpl> DataManagerImplPtr;

/** Default implementation of DataManager.
 *
 * Used by CustusX.
 *
 * \ingroup org_custusx_core_patientmodel
 * \date Jan 6, 2009
 * \author christiana
 */
class org_custusx_core_patientmodel_EXPORT DataManagerImpl: public DataManager
{
Q_OBJECT
public:
	static DataManagerImplPtr create(ActiveDataPtr activeData);
	virtual ~DataManagerImpl();
	void setSpaceProvider(SpaceProviderPtr spaceProvider);
	void setDataFactory(DataFactoryPtr dataFactory);

	// streams
	virtual VideoSourcePtr getStream(const QString& uid) const;
	virtual StreamMap getStreams() const;
	virtual void loadStream(VideoSourcePtr stream);

	// images
	virtual std::map<QString, ImagePtr> getImages() const;

	void loadData(DataPtr data);
	DataPtr loadData(const QString& uid, const QString& path);
    std::map<QString, DataPtr> getData() const;
	DataPtr getData(const QString& uid) const;
	virtual SpaceProviderPtr getSpaceProvider();
	virtual DataFactoryPtr getDataFactory();

	// meshes
	virtual std::map<QString, MeshPtr> getMeshes() const;

	// global data (move to separate class if list grows)
	virtual Vector3D getCenter() const;
	virtual void setCenter(const Vector3D& center);
    void setOperatingTable(const OperatingTable &ot);
    OperatingTable getOperatingTable() const;

	virtual QString addLandmark();
	virtual void deleteLandmarks();
	virtual void setLandmarkNames(std::vector<QString> names);
	virtual void setLandmarkName(QString uid, QString name);
	virtual void setLandmarkActive(QString uid, bool active);
	virtual std::map<QString, LandmarkProperty> getLandmarkProperties() const;
	virtual void clear(); ///< remove all stuff from manager
	virtual void removeData(const QString& uid, QString basePath);

	//Interface for saving/loading
	virtual void addXml(QDomNode& parentNode); ///< adds xml information about the datamanger and its variabels
	virtual void parseXml(QDomNode& datamangerNode, QString absolutePath = QString());///< Use a XML node to load data. \param datamangerNode A XML data representation of the DataManager. \param absolutePath Absolute path to the data elements. Used together with the relative paths stored in the filePath elements.

	virtual CLINICAL_VIEW getClinicalApplication() const;
	virtual void setClinicalApplication(CLINICAL_VIEW application);

	virtual Transform3D get_rMpr() const; ///< get the patient registration transform
	virtual void set_rMpr(const Transform3D& val); ///<  set the transform from patient to reference space
	virtual RegistrationHistoryPtr get_rMpr_History() const;

	virtual LandmarksPtr getPatientLandmarks();
	virtual PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const;

	virtual void generateUidAndName(QString* _uid, QString* _name);
protected:
	DataManagerImpl(ActiveDataPtr activeData);

protected:
	std::map<QString, VideoSourcePtr> mStreams;
	DataMap mData;
	Vector3D mCenter;
    OperatingTable mOperatingTable;
	CLINICAL_VIEW mClinicalApplication;
	void deleteFiles(DataPtr data, QString basePath);

	DataPtr loadData(QDomElement node, QString rootPath);
	int findUniqueUidNumber(QString uidBase) const;

	void readClinicalView();

	LandmarkPropertyMap mLandmarkProperties; ///< uid and name
	RegistrationHistoryPtr m_rMpr_History; ///< transform from the patient reference to the reference, along with historical data.
	LandmarksPtr mPatientLandmarks; ///< in space patient reference.
	mutable PresetTransferFunctions3DPtr mPresetTransferFunctions3D;

	SpaceProviderPtr mSpaceProvider;
	DataFactoryPtr mDataFactory;
	ActiveDataPtr mActiveData;

private:
	QDir findRelativePath(QDomElement node, QString rootPath);
	QString findPath(QDomElement node);
	QString findAbsolutePath(QDir relativePath, QString rootPath);
private slots:
	void settingsChangedSlot(QString key);
};

} // namespace cx

#endif /* CXDATAMANAGERIMPL_H_ */
