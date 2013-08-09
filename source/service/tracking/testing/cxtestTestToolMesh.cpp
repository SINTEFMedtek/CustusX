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
	mMesh = ssc::DataManager::getInstance()->loadData(mMeshFileName, mMeshFileName, ssc::rtSTL);
	return (mMesh != NULL);
}


} /* namespace cxtest */
