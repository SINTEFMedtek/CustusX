// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCDATAMANAGERIMPL_H_
#define SSCDATAMANAGERIMPL_H_

#include <map>
#include <set>
#include <string>
#include <QMutex>
#include <vector>
#include "sscImage.h"
#include "sscMesh.h"
#include "sscDataManager.h"
#include <QFileInfo>
#include "boost/scoped_ptr.hpp"


class QDomElement;

namespace cx
{


/**
 * \date Jan 6, 2009
 * \author christiana
 */

/**\brief Default implementation of DataManager.
 *
 * Used by CustusX.
 *
 * \ingroup sscData
 */
class DataManagerImpl: public DataManager
{
Q_OBJECT
public:
	static void initialize();

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
	virtual ImagePtr createImage(vtkImageDataPtr data, QString uidBase, QString nameBase, QString filePath);
	virtual ImagePtr createDerivedImage(vtkImageDataPtr data, QString uid, QString name, ImagePtr parentImage, QString filePath);
	virtual MeshPtr createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath);

	virtual Transform3D get_rMpr() const; ///< get the patient registration transform
	virtual void set_rMpr(const Transform3D& val); ///<  set the transform from patient to reference space
	virtual RegistrationHistoryPtr get_rMpr_History();

	virtual LandmarksPtr getPatientLandmarks();

protected:
	DataManagerImpl();
	virtual ~DataManagerImpl();
	virtual void verifyParentFrame(DataPtr data); ///< checks if data has a valid frameOfReferenceUid, generates and adds it if not

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
	DataPtr readData(const QString& uid, const QString& path, const QString& type);
	int findUniqueUidNumber(QString uidBase) const;
	void generateUidAndName(QString* _uid, QString* _name);

	LandmarkPropertyMap mLandmarkProperties; ///< uid and name
	RegistrationHistoryPtr m_rMpr_History; ///< transform from the patient reference to the reference, along with historical data.
	LandmarksPtr mPatientLandmarks; ///< in space patient reference.

public slots:
	void vtkImageDataChangedSlot();
};

} // namespace cx

#endif /* SSCDATAMANAGERIMPL_H_ */
