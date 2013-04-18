// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXTESTUSRECONSTRUCTIONFILEIO_H
#define CXTESTUSRECONSTRUCTIONFILEIO_H

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cxForwardDeclarations.h"
#include "cxUSReconstructInputData.h"
#include "cxImageDataContainer.h"

/**
 * Unit tests for classes
 * cx::USRecontructionFileMaker and cx::USReconstructionFileReader.
 *
 * \ingroup cxTest
 * \date apr 16, 2013
 * \author christiana
 */
class TestUSReconstructionFileIO : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testCreateUniqueFolders();
	void testCreateFolder();
	void testCreateEmptyUSReconstructInputData();
	void testCreateSampleUSReconstructInputData();
	void testSaveAndLoadUSReconstructInputData();

public:
	CPPUNIT_TEST_SUITE( TestUSReconstructionFileIO );
		CPPUNIT_TEST( testCreateUniqueFolders );
		CPPUNIT_TEST( testCreateFolder );
		CPPUNIT_TEST( testCreateEmptyUSReconstructInputData );
		CPPUNIT_TEST( testCreateSampleUSReconstructInputData );
		CPPUNIT_TEST( testSaveAndLoadUSReconstructInputData );
	CPPUNIT_TEST_SUITE_END();
private:
	struct ReconstructionData
	{
		QString sessionName;
		std::map<double, ssc::Transform3D> trackerData;
		bool writeColor;
		ssc::ToolPtr tool;
		cx::ImageDataContainerPtr imageData;
		std::vector<double> imageTimestamps;
		ssc::Transform3D rMpr;
	};

	QString getDataPath();
	void assertValidFolderForSession(QString path, QString sessionName);
	ReconstructionData createSampleReconstructData();
	ReconstructionData createEmptyReconstructData();

	ssc::USReconstructInputData createUSReconstructData(ReconstructionData input);

	QString write(ReconstructionData input);
	ssc::USReconstructInputData read(QString filename);
	void assertCorrespondence(ReconstructionData input, ssc::USReconstructInputData output);

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestUSReconstructionFileIO );

#endif // CXTESTUSRECONSTRUCTIONFILEIO_H
