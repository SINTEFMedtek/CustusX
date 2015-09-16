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
	static DataManagerImplPtr create();
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

	// state information
	virtual ImagePtr getActiveImage() const; ///< used for system state
	virtual DataPtr getActiveData() const;
	virtual void setActiveImage(ImagePtr activeImage); ///< used for system state
	virtual void setActiveData(DataPtr activeData);

	virtual QString addLandmark();
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
	DataManagerImpl();

protected:
	std::map<QString, VideoSourcePtr> mStreams;
	DataMap mData;
	Vector3D mCenter;
	CLINICAL_VIEW mClinicalApplication;
	void deleteFiles(DataPtr data, QString basePath);

	//state
	QList<DataPtr> mActiveData;
	virtual QList<DataPtr> getActiveDataList() const;
	DataPtr loadData(QDomElement node, QString rootPath);
	int findUniqueUidNumber(QString uidBase) const;

	void readClinicalView();

	LandmarkPropertyMap mLandmarkProperties; ///< uid and name
	RegistrationHistoryPtr m_rMpr_History; ///< transform from the patient reference to the reference, along with historical data.
	LandmarksPtr mPatientLandmarks; ///< in space patient reference.
	mutable PresetTransferFunctions3DPtr mPresetTransferFunctions3D;

	SpaceProviderPtr mSpaceProvider;
	DataFactoryPtr mDataFactory;

private:
	QDir findRelativePath(QDomElement node, QString rootPath);
	QString findPath(QDomElement node);
	QString findAbsolutePath(QDir relativePath, QString rootPath);
	void emitSignals(DataPtr activeData);
	QStringList getActiveDataStringList() const;
	void loadActiveData(const QString activeDatas);
private slots:
	void settingsChangedSlot(QString key);
};

} // namespace cx

#endif /* CXDATAMANAGERIMPL_H_ */
