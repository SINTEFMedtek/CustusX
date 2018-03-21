/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTUSRECONSTRUCTIONFILEFIXTURE_H
#define CXTESTUSRECONSTRUCTIONFILEFIXTURE_H

#include "cxForwardDeclarations.h"
#include "cxUSReconstructInputData.h"
#include "cxImageDataContainer.h"

#include "cxResourceExport.h"

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
class cxResource_EXPORT USReconstructionFileFixture
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
		QString streamUid;
		std::vector<cx::TimeInfo> imageTimestamps;
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
