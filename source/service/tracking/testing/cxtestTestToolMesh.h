#ifndef CXTESTTESTTOOLMESH_H_
#define CXTESTTESTTOOLMESH_H_

#include "cxMesh.h"
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
};

} /* namespace cxtest */
#endif /* CXTESTTESTTOOLMESH_H_ */
