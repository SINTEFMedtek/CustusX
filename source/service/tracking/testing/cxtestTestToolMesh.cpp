#include "cxtestTestToolMesh.h"

#include "cxDataLocations.h"
//#include "sscDataManager.h"
#include "sscDataReaderWriter.h"

namespace cxtest {

TestToolMesh::TestToolMesh()
{
	mToolToolPath = cx::DataLocations::getRootConfigPath()+QString("/tool/Tools/");
}

void TestToolMesh::setToolPath(QString path)
{
	mCurrentToolPath = mToolToolPath + path;
}

bool TestToolMesh::canLoadMesh(QString filename)
{
	bool success = true;

	QString fullFileName = mCurrentToolPath + filename;

	QString type = cx::DataReaderWriter().findDataTypeFromFile(fullFileName);
	success = success && (type==cx::Mesh::getTypeName());

	cx::MeshPtr mesh = cx::Mesh::create("test");
	success = success && mesh->load(fullFileName);

//	mMesh = mesh;
	return success;


//	mMesh = cx::dataManager()->loadData(mMeshFileName, mMeshFileName);
//	return (mesh!=NULL && mesh->);
}


} /* namespace cxtest */
