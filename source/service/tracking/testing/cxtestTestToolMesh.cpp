#include "cxtestTestToolMesh.h"

#include "cxDataLocations.h"
//#include "cxDataManager.h"
#include "cxDataReaderWriter.h"

namespace cxtest {

TestToolMesh::TestToolMesh()
{
}

void TestToolMesh::setToolPath(QString path)
{
	mCurrentToolPath = cx::DataLocations::getExistingConfigPath("/tool/Tools/"+path, "unknown");
//	mCurrentToolPath = mToolToolPath + path;
}

bool TestToolMesh::canLoadMesh(QString filename)
{
	bool success = true;

	QString fullFileName = mCurrentToolPath + filename;

	QString type = cx::DataReaderWriter().findDataTypeFromFile(fullFileName);
	success = success && (type==cx::Mesh::getTypeName());

	cx::MeshPtr mesh = cx::Mesh::create("test");
	success = success && mesh->load(fullFileName);

	return success;
}


} /* namespace cxtest */
