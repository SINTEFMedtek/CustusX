/*
 * cxDicomConverter.h
 *
 *  Created on: May 4, 2014
 *      Author: christiana
 */

#ifndef CXDICOMCONVERTER_H_
#define CXDICOMCONVERTER_H_

#include "cxImage.h"
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
class DicomConverter
{
public:
	DicomConverter();
	virtual ~DicomConverter();

	void setDicomDatabase(ctkDICOMDatabase* database);
	ImagePtr convertToImage(QString seriesUid);

private:
	ctkDICOMDatabase* mDatabase;
};

} /* namespace cx */
#endif /* CXDICOMCONVERTER_H_ */
