/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	QVector<double> getPositions(int cIndex) const;
	bool isMultiFrameImage() const;
	double calculateMultiFrameSpacing(int frameIndex) const;
};


} // namespace cx


#endif // CXDICOMIMAGEREADER_H
