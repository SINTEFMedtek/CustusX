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

#ifndef SSCDATAMANAGER_H_
#define SSCDATAMANAGER_H_

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QObject>

#include "vtkForwardDeclarations.h"
#include "sscVector3D.h"
#include "sscLandmark.h"
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"

namespace cx
{
// forward declarations
typedef boost::shared_ptr<class TransferFunctions3DPresets> PresetTransferFunctions3DPtr;
typedef boost::shared_ptr<class RegistrationHistory> RegistrationHistoryPtr;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class DataFactory> DataFactoryPtr;

/**\brief Interface for a manager of data objects.
 *
 * Simply calling instance() will instantiate the default manager DataManagerImpl.
 * It is also possible to subclass and use setInstance() to set another type.
 *
 * \ingroup sscData
 */
class DataManager: public QObject
{
Q_OBJECT
public:
	typedef std::map<QString, DataPtr> DataMap;
	typedef std::map<QString, ImagePtr> ImagesMap;
	typedef std::map<QString, MeshPtr> MeshMap;
	typedef std::map<QString, VideoSourcePtr> StreamMap;

	static DataManager* getInstance();
	static void shutdown();

	// streams
	virtual VideoSourcePtr getStream(const QString& uid) const = 0;
	virtual StreamMap getStreams() const = 0;
	virtual void loadStream(VideoSourcePtr stream) = 0;

	// images
	virtual ImagePtr loadImage(const QString& uid, const QString& filename) = 0;
	//virtual void loadImage(ImagePtr image) = 0; ///< load an image generated outside the manager.
	virtual void saveImage(ImagePtr image, const QString& basePath) = 0; ///< Save image to file
	virtual ImagePtr getImage(const QString& uid) const = 0;
	virtual std::map<QString, ImagePtr> getImages() const = 0;

	// meshes
	virtual void saveMesh(MeshPtr mesh, const QString& basePath) = 0; ///< Save mesh to file
	virtual MeshPtr loadMesh(const QString& uid, const QString& fileName) = 0;
	virtual MeshPtr getMesh(const QString& uid) const = 0;
	virtual std::map<QString, MeshPtr> getMeshes() const = 0;

	// data
	virtual void loadData(DataPtr data) = 0;
	virtual DataPtr loadData(const QString& uid, const QString& path) = 0;
    virtual void saveData(DataPtr data, const QString& basePath) = 0; ///< Save data to file
    virtual std::map<QString, DataPtr> getData() const = 0;
	virtual DataPtr getData(const QString& uid) const = 0;
	virtual SpaceProviderPtr getSpaceProvider() = 0;
	virtual DataFactoryPtr getDataFactory() = 0;


	// global data (move to separate class if list grows)
	virtual Vector3D getCenter() const = 0; ///< current common center point for user viewing.
	virtual void setCenter(const Vector3D& center) = 0;

	// state information
	virtual ImagePtr getActiveImage() const; ///< used for system state
	virtual void setActiveImage(ImagePtr activeImage); ///< used for system state
	virtual PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const;

	virtual QString addLandmark() = 0;
	virtual void setLandmarkNames(std::vector<QString> names) = 0;
	virtual void setLandmarkName(QString uid, QString name) = 0;
	virtual void setLandmarkActive(QString uid, bool active) = 0;
	virtual LandmarkPropertyMap getLandmarkProperties() const = 0;
	virtual CLINICAL_APPLICATION getClinicalApplication() const = 0;
	virtual void setClinicalApplication(CLINICAL_APPLICATION application) = 0;
	virtual void clear() = 0; ///< remove all stuff from manager
	virtual ImagePtr createImage(vtkImageDataPtr data, QString uidBase, QString nameBase, QString filePath = "Images") = 0;
	virtual ImagePtr createDerivedImage(vtkImageDataPtr data, QString uid, QString name, ImagePtr parentImage, QString filePath = "Images") = 0;
	virtual MeshPtr createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath) = 0;
	virtual void removeData(const QString& uid, QString basePath) = 0; ///< remove data from datamanger, emit signal

	virtual void addXml(QDomNode& parentNode) = 0; ///< adds xml information about the datamanger and its variabels
	virtual void parseXml(QDomNode& datamangerNode, QString absolutePath = QString()) = 0; ///< Use a XML node to load data. \param datamangerNode A XML data representation of the DataManager. \param absolutePath Absolute path to the data elements. Used together with the relative paths stored in the filePath elements.

	virtual Transform3D get_rMpr() const = 0; ///< get the patient registration transform
	virtual void set_rMpr(const Transform3D& val) = 0; ///<  set the transform from patient to reference space
	virtual RegistrationHistoryPtr get_rMpr_History() = 0;
	virtual LandmarksPtr getPatientLandmarks() = 0;

signals:
	void centerChanged(); ///< emitted when center is changed.
	void dataAddedOrRemoved();
	void activeImageChanged(const QString& uId); ///< emitted when the active image is changed
	void landmarkPropertiesChanged(); ///< emitted when global info about a landmark changed
	void clinicalApplicationChanged();
	void streamLoaded();
	void rMprChanged(); ///< emitted when the transformation between patient reference and (data) reference is set

protected:
	static void setInstance(DataManager* instance);
	DataManager();
	virtual ~DataManager();

private:
	static DataManager* mInstance;
};

DataManager* dataManager();

} // namespace cx

#endif /*SSCDATAMANAGER_H_*/
