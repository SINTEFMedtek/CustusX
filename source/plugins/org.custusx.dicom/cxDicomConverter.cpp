/*
 * cxDicomConverter.cpp
 *
 *  Created on: May 4, 2014
 *      Author: christiana
 */

#include "cxDicomConverter.h"

namespace cx
{

DicomConverter::DicomConverter()
{
}

DicomConverter::~DicomConverter()
{
}

void DicomConverter::setDicomDatabase(ctkDICOMDatabase* database)
{
	mDatabase = database;
}

ImagePtr DicomConverter::convertToImage(QString seriesUid)
{
	return ImagePtr();
}

} /* namespace cx */
