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

namespace ssc
{

class ImageReader
{
public:
	virtual ~ImageReader() {}
	virtual bool canLoad(const std::string& filename) = 0;
	virtual ImagePtr load(const std::string& filename) = 0;
};
typedef boost::shared_ptr<ImageReader> ImageReaderPtr;

class MetaImageReader : public ImageReader
{
public:
	virtual ~MetaImageReader() {}
	virtual bool canLoad(const std::string& filename) { return true; }
	virtual ImagePtr load(const std::string& filename);
};


class MeshReader
{
public:
	virtual ~MeshReader() {}
	virtual bool canLoad(const std::string& filename) = 0;
	virtual MeshPtr load(const std::string& filename) = 0;
};
typedef boost::shared_ptr<MeshReader> MeshReaderPtr;


class PolyDataMeshReader : public MeshReader
{
public:
	virtual ~PolyDataMeshReader() {}
	virtual bool canLoad(const std::string& filename) { return true; }
	virtual MeshPtr load(const std::string& filename);
};


class StlMeshReader : public MeshReader
{
public:
	virtual ~StlMeshReader() {}
	virtual bool canLoad(const std::string& filename) { return true; }
	virtual MeshPtr load(const std::string& filename);
};


//-----
class DataManagerImpl : public DataManager
{
public:
	static void initialize();

	// images
	virtual ImagePtr loadImage(const std::string& filename, READER_TYPE type);
	virtual ImagePtr getImage(const std::string& uid);
	virtual std::map<std::string, ImagePtr> getImages();

	virtual std::map<std::string, std::string> getImageUidsAndNames() const;
	virtual std::vector<std::string> getImageNames() const;
	virtual std::vector<std::string> getImageUids() const;

	// meshes
	virtual MeshPtr loadMesh(const std::string& fileName, MESH_READER_TYPE meshType);
	virtual MeshPtr getMesh(const std::string& uid);
	virtual std::map<std::string, MeshPtr> getMeshes();

	virtual std::map<std::string, std::string> getMeshUidsWithNames() const;
	virtual std::vector<std::string> getMeshUids() const;
	virtual std::vector<std::string> getMeshNames() const;

	// global data (move to separate class if list grows)
	virtual Vector3D getCenter() const;
	virtual void setCenter(const Vector3D& center);

	void addXml(QDomNode& parentNode); ///< adds xml information about the datamanger and its variabels
	void parseXml(QDomNode& datamangerNode);///< Use a XML node to load data. \param datamangerNode A XML data representation of the DataManager.

protected:
	DataManagerImpl();
	virtual ~DataManagerImpl();
protected:
	typedef std::map<std::string, ImagePtr> ImagesMap;
	ImagesMap mImages;
	Vector3D mCenter;
	std::map<READER_TYPE, ImageReaderPtr> mImageReaders;

	std::map<std::string, MeshPtr> mMeshes;
	std::map<MESH_READER_TYPE, MeshReaderPtr> mMeshReaders;
};

} // namespace ssc

#endif /* SSCDATAMANAGERIMPL_H_ */
