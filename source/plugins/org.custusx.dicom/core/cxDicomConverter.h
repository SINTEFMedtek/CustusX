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

#ifndef CXDICOMCONVERTER_H_
#define CXDICOMCONVERTER_H_

#include "cxImage.h"
#include "org_custusx_dicom_Export.h"
class ctkDICOMDatabase;

namespace cx
{
typedef boost::shared_ptr<class DicomImageReader> DicomImageReaderPtr;

/**
 * Import dicom series into cx Image.
 *
 * \ingroup org_custusx_dicom
 *
 * \date 2014-04-04
 * \author Christian Askeland
 */
class org_custusx_dicom_EXPORT DicomConverter
{
public:
	DicomConverter();
	virtual ~DicomConverter();

	void setDicomDatabase(ctkDICOMDatabase* database);
	ImagePtr convertToImage(QString seriesUid);

private:
	QString generateUid(DicomImageReaderPtr reader);
	QString generateName(DicomImageReaderPtr reader);
	std::map<double, ImagePtr> sortImagesAlongDirection(std::vector<ImagePtr> images, Vector3D  e_sort);
	ImagePtr mergeSlices(std::map<double, ImagePtr> sorted) const;
	double getMeanSliceDistance(std::map<double, ImagePtr> sorted) const;
	bool slicesFormRegularGrid(std::map<double, ImagePtr> sorted, Vector3D e_sort) const;
	// ignoreLocalizerImages is a tag to ignore special images. For now only localizer images are ignored
	ImagePtr createCxImageFromDicomFile(QString filename, bool ignoreLocalizerImages);
	std::vector<ImagePtr> createImages(QStringList files);
    QString convertToValidName(QString text) const;

	ctkDICOMDatabase* mDatabase;
};

} /* namespace cx */
#endif /* CXDICOMCONVERTER_H_ */
