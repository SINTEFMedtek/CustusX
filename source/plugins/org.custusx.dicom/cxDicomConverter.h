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

#ifndef CXDICOMCONVERTER_H_
#define CXDICOMCONVERTER_H_

#include "cxImage.h"
#include "org_custusx_dicom_Export.h"
class ctkDICOMDatabase;

namespace cx
{

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
	ImagePtr createCxImageFromDicomFile(QString filename);
	ctkDICOMDatabase* mDatabase;
};

} /* namespace cx */
#endif /* CXDICOMCONVERTER_H_ */
