#ifndef SSCDATAMANAGER_H_
#define SSCDATAMANAGER_H_

#include <map>
#include <string>
#include <vector>
#include "sscImage.h"
#include "sscMesh.h"



namespace ssc
{

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

class DataManager
{
public:
	DataManager();
	virtual ~DataManager();
	
	static DataManager* instance();
	
	// core interface
	ImagePtr loadImage(const std::string& filename, READER_TYPE type);
	std::map<std::string, ImagePtr> getImages();
	// convenience interface
	ImagePtr getImage(const std::string& uid);
	std::map<std::string, std::string> getImageUidsAndNames() const; 
	std::vector<std::string> getImageNames() const;
	std::vector<std::string> getImageUids() const; 
		
	// ditto for meshes
	
private:
	static DataManager* mInstance;
	std::map<READER_TYPE, ImageReaderPtr> mImageReaders;
	std::map<std::string, ImagePtr> mImages;
	std::map<std::string, MeshPtr> mMeshes;
	
};

} // namespace ssc

#endif /*SSCDATAMANAGER_H_*/
