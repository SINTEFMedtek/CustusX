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
#ifndef CXTESTUSRECONSTRUCTIONFILEFIXTURE_H
#define CXTESTUSRECONSTRUCTIONFILEFIXTURE_H

#include "cxForwardDeclarations.h"
#include "cxUSReconstructInputData.h"
#include "cxImageDataContainer.h"

namespace cxtest
{

/**
 * Unit tests for classes
 * cx::USRecontructionFileMaker and cx::USReconstructionFileReader.
 *
 * \ingroup cxTest
 * \date apr 16, 2013
 * \author christiana
 */
class USReconstructionFileFixture
{
public:
	USReconstructionFileFixture();
	~USReconstructionFileFixture();

protected:
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

} // namespace cxtest

#endif // CXTESTUSRECONSTRUCTIONFILEFIXTURE_H
