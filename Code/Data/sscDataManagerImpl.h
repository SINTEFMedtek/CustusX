/*
 * sscDataManagerImpl.h
 *
 *  Created on: Jan 6, 2009
 *      Author: christiana
 */

#ifndef SSCDATAMANAGERIMPL_H_
#define SSCDATAMANAGERIMPL_H_

#include <map>
#include <string>
#include <vector>
#include "sscImage.h"
#include "sscMesh.h"
#include "sscDataManager.h"

class QDomElement;

namespace ssc
{

class DataReader
{
public:
  virtual ~DataReader()
  {
  }
  virtual bool canLoad(const std::string& filename) = 0;
  virtual DataPtr load(const std::string& uid, const std::string& filename) = 0;
};
typedef boost::shared_ptr<DataReader> DataReaderPtr;

class MetaImageReader: public DataReader
{
public:
  virtual ~MetaImageReader()
  {
  }
  virtual bool canLoad(const std::string& filename)
  {
    return true;
  }
  virtual DataPtr load(const std::string& uid, const std::string& filename);
};

//class MeshReader :
//{
//public:
//	virtual ~MeshReader() {}
//	virtual bool canLoad(const std::string& filename) = 0;
//	virtual MeshPtr load(const std::string& uid, const std::string& filename) = 0;
//};
//typedef boost::shared_ptr<MeshReader> MeshReaderPtr;


class PolyDataMeshReader: public DataReader
{
public:
  virtual ~PolyDataMeshReader()
  {
  }
  virtual bool canLoad(const std::string& filename)
  {
    return true;
  }
  virtual DataPtr load(const std::string& uid, const std::string& filename);
};

class StlMeshReader: public DataReader
{
public:
  virtual ~StlMeshReader()
  {
  }
  virtual bool canLoad(const std::string& filename)
  {
    return true;
  }
  virtual DataPtr load(const std::string& uid, const std::string& filename);
};

//-----
class DataManagerImpl: public DataManager
{
Q_OBJECT
public:
  static void initialize();

  // images
  virtual void saveImage(ImagePtr image, const std::string& basePath);///< Save image to file \param basePath Absolute path to patient data folder
  virtual ImagePtr getImage(const std::string& uid) const;
  virtual std::map<std::string, ImagePtr> getImages() const;

  virtual std::map<std::string, std::string> getImageUidsAndNames() const;
  virtual std::vector<std::string> getImageNames() const;
  virtual std::vector<std::string> getImageUids() const;

  void loadData(DataPtr data);
  DataPtr loadData(const std::string& uid, const std::string& path, READER_TYPE type);
  std::map<std::string, DataPtr> getData() const;
  DataPtr getData(const std::string& uid) const;

  // meshes
  virtual MeshPtr getMesh(const std::string& uid) const;
  virtual std::map<std::string, MeshPtr> getMeshes() const;

  virtual std::map<std::string, std::string> getMeshUidsWithNames() const;
  virtual std::vector<std::string> getMeshUids() const;
  virtual std::vector<std::string> getMeshNames() const;

  // global data (move to separate class if list grows)
  virtual Vector3D getCenter() const;
  virtual void setCenter(const Vector3D& center);

  // state information
  virtual ImagePtr getActiveImage() const; ///< used for system state
  virtual void setActiveImage(ImagePtr activeImage); ///< used for system state

  virtual std::string addLandmark();
  virtual void setLandmarkNames(std::vector<std::string> names);
  virtual void setLandmarkName(std::string uid, std::string name);
  virtual void setLandmarkActive(std::string uid, bool active);
  virtual std::map<std::string, LandmarkProperty> getLandmarkProperties() const;
  virtual void clear(); ///< remove all stuff from manager

  //virtual MeshPtr getActiveMesh() const; ///< used for system state
  //virtual void setActiveMesh(MeshPtr activeMesh); ///< used for system state

  //Interface for saving/loading
  virtual void addXml(QDomNode& parentNode); ///< adds xml information about the datamanger and its variabels
  virtual void parseXml(QDomNode& datamangerNode, QString absolutePath = QString());///< Use a XML node to load data. \param datamangerNode A XML data representation of the DataManager. \param absolutePath Absolute path to the data elements. Used together with the relative paths stored in the filePath elements.

  virtual MEDICAL_DOMAIN getMedicalDomain() const;
  virtual void setMedicalDomain(MEDICAL_DOMAIN domain);
  virtual ImagePtr createImage(vtkImageDataPtr data, std::string uidBase, std::string nameBase, std::string filePath);
  virtual MeshPtr createMesh(vtkPolyDataPtr data, std::string uidBase, std::string nameBase, std::string filePath);

protected:
  DataManagerImpl();
  virtual ~DataManagerImpl();
  virtual void verifyParentFrame(DataPtr data); ///< checks if data has a valid frameOfReferenceUid, generates and adds it if not

protected:
  DataMap mData;
  Vector3D mCenter;
  MEDICAL_DOMAIN mMedicalDomain;
  std::map<READER_TYPE, DataReaderPtr> mDataReaders;

  //state
  ImagePtr mActiveImage;
  //MeshPtr mActiveMesh;
  virtual ImagePtr loadImage(const std::string& uid, const std::string& filename, READER_TYPE type);
  virtual MeshPtr loadMesh(const std::string& uid, const std::string& fileName, READER_TYPE meshType);
  READER_TYPE getReaderType(QString fileType);
  void loadData(QDomElement node, QString rootPath);
  DataPtr readData(const std::string& uid, const std::string& path, READER_TYPE type);
  int findUniqueUidNumber(std::string uidBase) const;

  LandmarkPropertyMap mLandmarkProperties; ///< uid and name

public slots:
  void vtkImageDataChangedSlot();
  void transferFunctionsChangedSlot();
};

} // namespace ssc

#endif /* SSCDATAMANAGERIMPL_H_ */
