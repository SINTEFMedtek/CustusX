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
#ifndef CXDICOMIMAGEREADER_H
#define CXDICOMIMAGEREADER_H

#include "org_custusx_dicom_Export.h"

#include "ctkDICOMDatabase.h"
#include "cxTypeConversions.h"
#include "cxTime.h"
#include "vtkImageData.h"
#include "cxRegistrationTransform.h"
#include <vtkImageAppend.h>


#include "ctkDICOMItem.h"
#include "dcfilefo.h" // DcmFileFormat
#include "dcdeftag.h" // defines all dcm tags
#include "dcmimage.h"
#include <string.h>
#include "cxForwardDeclarations.h"

typedef boost::shared_ptr<class ctkDICOMItem> ctkDICOMItemPtr;

namespace cx
{
typedef boost::shared_ptr<class DicomImageReader> DicomImageReaderPtr;

/** Utility for reading dicom images
 *
 *
 * \ingroup org.custusx.dicom
 * \date 2014-05-13
 * \author christiana
 */
class org_custusx_dicom_EXPORT DicomImageReader
{
public:
	struct WindowLevel
	{
		double center;
		double width;
	};

public:
	static DicomImageReaderPtr createFromFile(QString filename);
	Transform3D getImageTransformPatient() const;
	vtkImageDataPtr createVtkImageData();
	ctkDICOMItemPtr item() const;
	WindowLevel getWindowLevel() const;
	int getNumberOfFrames() const;
	QString getPatientName() const;
	bool isLocalizerImage() const;

private:
	DcmFileFormat mFileFormat;
	DcmDataset *mDataset;
	QString mFilename;

	DicomImageReader();
	bool loadFile(QString filename);
	Eigen::Array3d getSpacing() const;
	Eigen::Array3i getDim(const DicomImage& dicomImage) const;
	void error(QString message) const;
	double getDouble(const DcmTagKey& tag, const unsigned long pos=0, const OFBool searchIntoSub = OFFalse) const;
//	double getDouble(DcmObject *dcmObject, const DcmTagKey &tag, const unsigned long pos, const bool searchIntoSub) const;
	QString formatPatientName(QString rawName) const;

	ctkDICOMItemPtr wrapInCTK(DcmItem* item) const;
	double getSliceSpacing() const;
	QVector<double> getZPositions() const;
	bool isMultiFrameImage() const;
};


} // namespace cx


#endif // CXDICOMIMAGEREADER_H
