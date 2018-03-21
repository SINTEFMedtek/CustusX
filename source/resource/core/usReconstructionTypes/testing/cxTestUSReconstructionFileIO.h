/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
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
		std::map<double, cx::Transform3D> trackerData;
		bool writeColor;
		cx::ToolPtr tool;
		cx::ImageDataContainerPtr imageData;
		std::vector<double> imageTimestamps;
		cx::Transform3D rMpr;
	};

	QString getDataPath();
	void assertValidFolderForSession(QString path, QString sessionName);
	ReconstructionData createSampleReconstructData();
	ReconstructionData createEmptyReconstructData();

	cx::USReconstructInputData createUSReconstructData(ReconstructionData input);

	QString write(ReconstructionData input);
	cx::USReconstructInputData read(QString filename);
	void assertCorrespondence(ReconstructionData input, cx::USReconstructInputData output);

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestUSReconstructionFileIO );

#endif // CXTESTUSRECONSTRUCTIONFILEIO_H
