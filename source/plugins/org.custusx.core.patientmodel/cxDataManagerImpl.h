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

/**\brief Default implementation of DataManager.
 *
 * Used by CustusX.
 *
 * \ingroup cx_resource_core_data
 * \date Jan 6, 2009
 * \author christiana
 */
class org_custusx_core_patientmodel_EXPORT DataManagerImpl: public DataManager
{
Q_OBJECT
public:
	static DataManagerImplPtr create();
	virtual ~DataManagerImpl();
//	static void initialize();
	void setSpaceProvider(SpaceProviderPtr spaceProvider);
	void setDataFactory(DataFactoryPtr dataFactory);

	// streams
	virtual VideoSourcePtr getStream(const QString& uid) const;
	virtual StreamMap getStreams() const;
	virtual void loadStream(VideoSourcePtr stream);

	// images
	virtual void saveImage(ImagePtr image, const QString& basePath);///< Save image to file \param image Image to save \param basePath Absolute path to patient data folder
	virtual ImagePtr getImage(const QString& uid) const;
	virtual std::map<QString, ImagePtr> getImages() const;

	void loadData(DataPtr data);
	DataPtr loadData(const QString& uid, const QString& path);
    virtual void saveData(DataPtr data, const QString& basePath); ///< Save data to file
    std::map<QString, DataPtr> getData() const;
	DataPtr getData(const QString& uid) const;
	virtual SpaceProviderPtr getSpaceProvider();
	virtual DataFactoryPtr getDataFactory();

	// meshes
	virtual void saveMesh(MeshPtr mesh, const QString& basePath);///< Save mesh to file \param mesh to save \param basePath Absolute path to patient data folder
	virtual MeshPtr getMesh(const QString& uid) const;
	virtual std::map<QString, MeshPtr> getMeshes() const;

	// global data (move to separate class if list grows)
	virtual Vector3D getCenter() const;
	virtual void setCenter(const Vector3D& center);

	// state information
	virtual ImagePtr getActiveImage() const; ///< used for system state
	virtual void setActiveImage(ImagePtr activeImage); ///< used for system state

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

	virtual CLINICAL_APPLICATION getClinicalApplication() const;
	virtual void setClinicalApplication(CLINICAL_APPLICATION application);

	virtual Transform3D get_rMpr() const; ///< get the patient registration transform
	virtual void set_rMpr(const Transform3D& val); ///<  set the transform from patient to reference space
	virtual RegistrationHistoryPtr get_rMpr_History() const;

	virtual LandmarksPtr getPatientLandmarks();
	virtual PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const;

	virtual bool getDebugMode() const;
	virtual void setDebugMode(bool on);

	virtual void generateUidAndName(QString* _uid, QString* _name);

protected:
	DataManagerImpl();
//	virtual void verifyParentFrame(DataPtr data); ///< checks if data has a valid frameOfReferenceUid, generates and adds it if not

protected:
	std::map<QString, VideoSourcePtr> mStreams;
	DataMap mData;
	Vector3D mCenter;
	CLINICAL_APPLICATION mClinicalApplication;
	void deleteFiles(DataPtr data, QString basePath);

	//state
	ImagePtr mActiveImage;
	virtual ImagePtr loadImage(const QString& uid, const QString& filename);
	virtual MeshPtr loadMesh(const QString& uid, const QString& fileName);
	DataPtr loadData(QDomElement node, QString rootPath);
//	DataPtr readData(const QString& uid, const QString& path, const QString& type);
	int findUniqueUidNumber(QString uidBase) const;

	LandmarkPropertyMap mLandmarkProperties; ///< uid and name
	RegistrationHistoryPtr m_rMpr_History; ///< transform from the patient reference to the reference, along with historical data.
	LandmarksPtr mPatientLandmarks; ///< in space patient reference.
	bool mDebugMode; ///< if set: allow lots of weird debug stuff.
	mutable PresetTransferFunctions3DPtr mPresetTransferFunctions3D;

	SpaceProviderPtr mSpaceProvider;
	DataFactoryPtr mDataFactory;

public slots:
	void vtkImageDataChangedSlot();
};

} // namespace cx

#endif /* CXDATAMANAGERIMPL_H_ */
