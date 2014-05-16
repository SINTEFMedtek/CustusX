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
#ifndef CXDICOMIMAGEREADER_H
#define CXDICOMIMAGEREADER_H

#include "ctkDICOMDatabase.h"
#include "cxTypeConversions.h"
#include "cxTime.h"
#include "vtkImageData.h"
#include "cxRegistrationTransform.h"
#include <vtkImageAppend.h>
#include "cxReporter.h"

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
class DicomImageReader
{
public:
	static DicomImageReaderPtr createFromFile(QString filename);
	Transform3D getImageTransformPatient() const;
	vtkImageDataPtr createVtkImageData();
	ctkDICOMItemPtr item();

private:
	DcmFileFormat mFileFormat;
	DcmDataset *mDataset;
	ctkDICOMItemPtr mDicomItem;
	QString mFilename;

	DicomImageReader();
	bool loadFile(QString filename);
	Eigen::Array3d getSpacing() const;
	Eigen::Array3i getDim(const DicomImage& dicomImage) const;

	Transform3D getImageTransformPatient_multifile() const;
	Transform3D getImageTransformPatient_singlefile() const;
	bool isSingleFile() const;
	DcmItem* findAndGetSequenceItem(DcmItem* parent, DcmTagKey tagKey, int number=0) const;
	ctkDICOMItemPtr wrapInCTK(DcmItem* item) const;
	Eigen::Array3d getSpacing_multifile() const;
	Eigen::Array3d getSpacing_singlefile() const;
	Eigen::Array3d getSpacing(ctkDICOMItemPtr item) const;
	Transform3D getImageTransformPatient(ctkDICOMItemPtr planePosition, ctkDICOMItemPtr planeOrientation) const;
};


} // namespace cx


#endif // CXDICOMIMAGEREADER_H
