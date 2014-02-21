#include "cxtestTestToolMesh.h"

#include "cxDataLocations.h"
#include "sscDataManager.h"

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
	mMeshFileName = mCurrentToolPath + filename;
	mMesh = cx::DataManager::getInstance()->loadData(mMeshFileName, mMeshFileName);
	return (mMesh != NULL);
}


} /* namespace cxtest */
