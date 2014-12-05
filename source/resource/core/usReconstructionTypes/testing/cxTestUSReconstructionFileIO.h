/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
