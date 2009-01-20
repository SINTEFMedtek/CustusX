#ifndef SSCDATAMANAGER_H_
#define SSCDATAMANAGER_H_

#include <map>
#include <string>
#include <vector>
#include "sscImage.h"
#include "sscMesh.h"
#include "sscVector3D.h"


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
//-----
enum MESH_READER_TYPE
{
	mrtPOLYDATA,
	mrtSTL,
};

/**Manager for images, meshes, and associated data.
 *
 * Simply calling instance() will instantiate the default manager DataManagerImpl.
 * It is also possible to subclass and use setInstance() to set another type.
 */
class DataManager
{
public:
	static DataManager* instance();

	// images
	virtual ImagePtr loadImage(const std::string& filename, READER_TYPE type) = 0;
	virtual ImagePtr getImage(const std::string& uid) = 0;
	virtual std::map<std::string, ImagePtr> getImages() = 0;

	virtual std::map<std::string, std::string> getImageUidsAndNames() const = 0;
	virtual std::vector<std::string> getImageNames() const = 0;
	virtual std::vector<std::string> getImageUids() const = 0;

	// meshes
	virtual MeshPtr loadMesh(const std::string& fileName, MESH_READER_TYPE meshType) = 0;
	virtual MeshPtr getMesh(const std::string& uid) = 0;
	virtual std::map<std::string, MeshPtr> getMeshes() = 0;

	virtual std::map<std::string, std::string> getMeshUIDsWithNames() const = 0;
	virtual std::vector<std::string> getMeshUIDs() const = 0;
	virtual std::vector<std::string> getMeshNames() const = 0;
	
	// global data (move to separate class if list grows)
	virtual Vector3D getCenter() const = 0; ///< current common center point for user viewing. 
	virtual void setCenter(const Vector3D& center) = 0;

protected:
	static void setInstance(DataManager* instance);
	DataManager();
	virtual ~DataManager();

private:
	static DataManager* mInstance;
};

} // namespace ssc

#endif /*SSCDATAMANAGER_H_*/
