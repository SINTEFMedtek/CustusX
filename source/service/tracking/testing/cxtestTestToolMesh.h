#ifndef CXTESTTESTTOOLMESH_H_
#define CXTESTTESTTOOLMESH_H_

#include "sscMesh.h"
#include <QString>

namespace cxtest {

/*
 * TestToolMesh
 * \brief Helper class for testing tool mesh file
 * \date Aug 8, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class TestToolMesh {
public:
	TestToolMesh();
	void setToolPath(QString path);
	bool canLoadMesh(QString filename);
private:
	QString mToolToolPath;
	QString mCurrentToolPath;
	QString mMeshFileName;
	ssc::DataPtr mMesh;
};

} /* namespace cxtest */
#endif /* CXTESTTESTTOOLMESH_H_ */
