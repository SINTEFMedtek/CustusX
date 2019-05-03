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

TEST_CASE("Ceetron: Check nothing", "[unit][plugins][hide]")
{
	CHECK(true);
}


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

	qint32 cgeoFirstValue, cgeo2nd, cgeo3rd;
	QDataStream stream(&byteArray, QIODevice::ReadOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
//	stream.setVersion(QDataStream::Qt_5_9);
	stream >> cgeoFirstValue;
	stream >> cgeo2nd;
	stream >> cgeo3rd;

	//.cgeo file should start with 12072001, then 0 and 1. All 32 bit int.
	CHECK(cgeoFirstValue == 12072001);
	CHECK(cgeo2nd == 0);
	CHECK(cgeo3rd == 1);
}

TEST_CASE("CgeoReaderWriter: Convert to QByteArray", "[unit][plugins][org.custusx.ceetron]")
{
	cx::VisServicesPtr mocServices = cx::VisServices::getNullObjects();
	cx::CgeoReaderWriter readerWriter(mocServices->patient());

	cx::MeshPtr mesh = cx::Mesh::create("meshUid1","meshName1");
	QByteArray byteArray = readerWriter.convertToQByteArray(mesh);

	qint32 cgeoFirstValue, cgeo2nd, cgeo3rd;
	QDataStream stream(&byteArray, QIODevice::ReadOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream >> cgeoFirstValue;
	stream >> cgeo2nd;
	stream >> cgeo3rd;

	//.cgeo file should start with 12072001, then 0 and 1. All 32 bit int.
//	qint32 cgeoFirstValue = byteArray[0];
	CHECK(cgeoFirstValue == 12072001);
	CHECK(cgeo2nd == 0);
	CHECK(cgeo3rd == 1);
}
