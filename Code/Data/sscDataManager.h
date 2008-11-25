#ifndef SSCDATAMANAGER_H_
#define SSCDATAMANAGER_H_

#include <map>
#include <string>
#include <vector>
#include "sscImage.h"
#include "sscMesh.h"



namespace ssc
{

//-----
enum READER_TYPE
{
	rtDICOM,
	rtSONOWAND_M3D,
	rtMETAIMAGE,
	rtAUTO
};

class ImageReader
{
public:
	virtual bool canLoad(const std::string& filename) = 0;
	virtual ImagePtr load(const std::string& filename) = 0;
};
typedef boost::shared_ptr<ImageReader> ImageReaderPtr;

class MetaImageReader : public ImageReader
{
public:
	virtual bool canLoad(const std::string& filename) { return true; }
	virtual ImagePtr load(const std::string& filename);
};

//-----
enum MESH_READER_TYPE
{
	mrtPOLYDATA,
	mrtSTL,
};

class MeshReader
{
public:
	virtual bool canLoad(const std::string& filename) = 0;
	virtual MeshPtr load(const std::string& filename) = 0;
};
typedef boost::shared_ptr<MeshReader> MeshReaderPtr;


class PolyDataMeshReader : public MeshReader
{
public:
	virtual bool canLoad(const std::string& filename) { return true; }
	virtual MeshPtr load(const std::string& filename);
};


class StlMeshReader : public MeshReader
{
public:
	virtual bool canLoad(const std::string& filename) { return true; }
	virtual MeshPtr load(const std::string& filename);
};


//-----
class DataManager
{
public:
	static DataManager* instance();

	// images
	ImagePtr loadImage(const std::string& filename, READER_TYPE type);
	ImagePtr getImage(const std::string& uid);
	std::map<std::string, ImagePtr> getImages();

	std::map<std::string, std::string> getImageUidsAndNames() const;
	std::vector<std::string> getImageNames() const;
	std::vector<std::string> getImageUids() const;

	// meshes
	MeshPtr loadMesh(const std::string& fileName, MESH_READER_TYPE meshType);
	MeshPtr getMesh(const std::string& uid);
	std::map<std::string, MeshPtr> getMeshes();
	
	std::map<std::string, std::string> getMeshUIDsWithNames() const;
	std::vector<std::string> getMeshUIDs() const;
	std::vector<std::string> getMeshNames() const;



private:
	DataManager();
	virtual ~DataManager();

	static DataManager* mInstance;

	std::map<std::string, ImagePtr> mImages;
	std::map<READER_TYPE, ImageReaderPtr> mImageReaders;

	std::map<std::string, MeshPtr> mMeshes;
	std::map<MESH_READER_TYPE, MeshReaderPtr> mMeshReaders;

};

} // namespace ssc

#endif /*SSCDATAMANAGER_H_*/
