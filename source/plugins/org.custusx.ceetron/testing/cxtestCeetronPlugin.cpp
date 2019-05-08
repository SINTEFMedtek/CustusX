/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QDir>
#include "cxCgeoReaderWriter.h"
#include "cxMesh.h"
#include "cxVisServices.h"
#include "cxDataLocations.h"
#include "cxLogger.h"
#include "cxtestCgeoTestFunctions.h"


namespace cxtest
{
TEST_CASE("CgeoReaderWriter: Write file", "[unit][plugins][org.custusx.ceetron]")
{
	cx::VisServicesPtr mocServices = cx::VisServices::getNullObjects();
	cx::CgeoReaderWriter readerWriter(mocServices->patient());

	cx::MeshPtr mesh = cx::Mesh::create("meshUid1","meshName1");

	//Save file, then load file into QByteArray
	QDir().mkpath(cx::DataLocations::getCachePath() + "/Images");//Save fails if folder don't exist
	QString tempFilename = cx::DataLocations::getCachePath() + "/" + mesh->getFilename() + ".cgeo";

	readerWriter.write(mesh, tempFilename);

	QFile modelFile(tempFilename);
	if(!modelFile.open(QIODevice::ReadOnly))
	{
		CX_LOG_WARNING() << "Cannot read temporary file: " << tempFilename;
	}

	QByteArray byteArray = modelFile.readAll();
	checkCgeoByteArray(byteArray);

}

TEST_CASE("CgeoReaderWriter: Convert to QByteArray", "[unit][plugins][org.custusx.ceetron]")
{
	cx::VisServicesPtr mocServices = cx::VisServices::getNullObjects();
	cx::CgeoReaderWriter readerWriter(mocServices->patient());

	cx::MeshPtr mesh = cx::Mesh::create("meshUid1","meshName1");
	QByteArray byteArray = readerWriter.convertToQByteArray(mesh);
	checkCgeoByteArray(byteArray);
}

}//cxtest
