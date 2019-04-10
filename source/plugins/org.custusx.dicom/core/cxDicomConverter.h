/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
