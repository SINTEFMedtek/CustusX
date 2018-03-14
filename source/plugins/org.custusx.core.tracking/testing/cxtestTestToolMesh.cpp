/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestTestToolMesh.h"

#include "cxDataLocations.h"
#include "cxDataReaderWriter.h"

namespace cxtest {

TestToolMesh::TestToolMesh()
{
}

void TestToolMesh::setToolPath(QString path)
{
	mCurrentToolPath = cx::DataLocations::findConfigFolder("/tool/Tools/"+path);
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
