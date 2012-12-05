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
#include <vector>
#include "sscImage.h"
#include "sscMesh.h"
#include "sscDataManager.h"
#include <QFileInfo>

class QDomElement;

namespace ssc
{

/**\brief Interface for Data file readers.
 *
 * \ingroup sscData
 */
class DataReader
{
public:
	virtual ~DataReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename) = 0;
	virtual DataPtr load(const QString& uid, const QString& filename) = 0;
};
typedef boost::shared_ptr<DataReader> DataReaderPtr;

/**\brief Reader for metaheader .mhd files.
 *
 * \ingroup sscData
 */
class MetaImageReader: public DataReader
{
public:
	virtual ~MetaImageReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("mhd", Qt::CaseInsensitive) == 0 || fileType.compare("mha", Qt::CaseInsensitive) == 0);
	}
	virtual DataPtr load(const QString& uid, const QString& filename);

	vtkImageDataPtr load(const QString& filename);
};

/**\brief Reader for minc files.
 *
 * \ingroup sscData
 */
class MincImageReader: public DataReader
{
public:
	virtual ~MincImageReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("mnc", Qt::CaseInsensitive) == 0);
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
};

//class MeshReader :
//{
//public:
//	virtual ~MeshReader() {}
//	virtual bool canLoad(const QString& filename) = 0;
//	virtual MeshPtr load(const QString& uid, const QString& filename) = 0;
//};
//typedef boost::shared_ptr<MeshReader> MeshReaderPtr;


/**\brief Reader for .vtk files.
 *
 * \ingroup sscData
 */
class PolyDataMeshReader: public DataReader
{
public:
	virtual ~PolyDataMeshReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("vtk", Qt::CaseInsensitive) == 0);
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Reader for STL files.
 *
 * \ingroup sscData
 */
class StlMeshReader: public DataReader
{
public:
	virtual ~StlMeshReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("stl", Qt::CaseInsensitive) == 0);
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
};

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

	virtual std::map<QString, QString> getImageUidsAndNames() const;
	virtual std::vector<QString> getImageNames() const;
	virtual std::vector<QString> getImageUids() const;

	void loadData(DataPtr data);
	DataPtr loadData(const QString& uid, const QString& path, READER_TYPE type);
    virtual void saveData(DataPtr data, const QString& basePath); ///< Save data to file
    std::map<QString, DataPtr> getData() const;
	DataPtr getData(const QString& uid) const;
	//  virtual void saveData(DataPtr image, const QString& basePath, bool headerOnly=false);///< Save image to file \param basePath Absolute path to patient data folder

	// meshes
	virtual void saveMesh(MeshPtr mesh, const QString& basePath);///< Save mesh to file \param mesh to save \param basePath Absolute path to patient data folder
	virtual MeshPtr getMesh(const QString& uid) const;
	virtual std::map<QString, MeshPtr> getMeshes() const;

	virtual std::map<QString, QString> getMeshUidsWithNames() const;
	virtual std::vector<QString> getMeshUids() const;
	virtual std::vector<QString> getMeshNames() const;

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
	virtual void removeData(const QString& uid);

	//virtual MeshPtr getActiveMesh() const; ///< used for system state
	//virtual void setActiveMesh(MeshPtr activeMesh); ///< used for system state

	//Interface for saving/loading
	virtual void addXml(QDomNode& parentNode); ///< adds xml information about the datamanger and its variabels
	virtual void parseXml(QDomNode& datamangerNode, QString absolutePath = QString());///< Use a XML node to load data. \param datamangerNode A XML data representation of the DataManager. \param absolutePath Absolute path to the data elements. Used together with the relative paths stored in the filePath elements.

	virtual CLINICAL_APPLICATION getClinicalApplication() const;
	virtual void setClinicalApplication(CLINICAL_APPLICATION application);
	virtual ImagePtr createImage(vtkImageDataPtr data, QString uidBase, QString nameBase, QString filePath);
	virtual ImagePtr createDerivedImage(vtkImageDataPtr data, QString uid, QString name, ImagePtr parentImage, QString filePath);
	virtual MeshPtr createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath);

protected:
	DataManagerImpl();
	virtual ~DataManagerImpl();
	virtual void verifyParentFrame(DataPtr data); ///< checks if data has a valid frameOfReferenceUid, generates and adds it if not

protected:
	std::map<QString, VideoSourcePtr> mStreams;
	DataMap mData;
	Vector3D mCenter;
	CLINICAL_APPLICATION mClinicalApplication;
	typedef std::set<DataReaderPtr> DataReadersType;
	DataReadersType mDataReaders;

	//state
	ImagePtr mActiveImage;
	//MeshPtr mActiveMesh;
	virtual ImagePtr loadImage(const QString& uid, const QString& filename, READER_TYPE notused);
	virtual MeshPtr loadMesh(const QString& uid, const QString& fileName, READER_TYPE notused);
	//  READER_TYPE getReaderType(QString fileType);
	DataPtr loadData(QDomElement node, QString rootPath);
	DataPtr readData(const QString& uid, const QString& path, const QString& type);
	int findUniqueUidNumber(QString uidBase) const;
	void generateUidAndName(QString* _uid, QString* _name);

	LandmarkPropertyMap mLandmarkProperties; ///< uid and name

public slots:
	void vtkImageDataChangedSlot();
	void transferFunctionsChangedSlot();
};

} // namespace ssc

#endif /* SSCDATAMANAGERIMPL_H_ */
