/*
 * DICOMLibAPI.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: christiana
 */

#include "sscDICOMLibAPI.h"
#include "DICOMLib.h"
#include <iostream>
#include "sscTypeConversions.h"
#include "sscImage.h"
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkUnsignedShortArray.h>
#include <vtkUnsignedCharArray.h>
#include <boost/cstdint.hpp>
#include "sscTime.h"
#include "sscRegistrationTransform.h"
#include "sscImageLUT2D.h"
#include "sscImageTF3D.h"

Series::Series(struct series_t* base) : mBase(base)
{

}

ssc::ImagePtr Series::getImage()
{
//	ssc::ImagePtr retval;
	ssc::ImagePtr mImage;

	if (!mBase)
		return mImage;

	DICOMLib_UseAutoVOI(mBase);
	volume_t *data = DICOMLib_GetVolume(mBase, NULL);
	std::cout << "data: " << data << std::endl;
	//	assert(data);
	const char *ptr = (const char *) data->volume;

	int lutsize = mBase->VOI.lut.length;
	std::cout << "lutsize " << lutsize << std::endl;

	vtkImageDataPtr mImageData;
//	if (mImage) // already loaded?
//	{
//		return true;
//	}

	bool success = true;
	mImageData = vtkImageDataPtr::New();
	mImage = ssc::ImagePtr(new ssc::Image("series_", mImageData));
//	mLut = vtkLookupTablePtr::New();

//	TODO:
//
//	- fyll snw metadata fra dicom.
//	- bruk snwformat-imagelesefunksjonen her også - lag generell.

	ssc::SNW2VolumeMetaData metaData;
	success = success && this->rawLoadMetaData(metaData);
	success = success && this->rawLoadVtkImageData(mImageData, metaData, ptr);
//	success = success && rawLoadLut(rawLutFileName(), mLut);

	mImage->setFilePath(metaData.mName);
	mImage->setFilePath("Images/"+metaData.mName+".mhd");
	mImage->get_rMd_History()->setParentSpace(metaData.DICOM.mFrameOfReferenceUID);
	mImage->get_rMd_History()->setRegistration(metaData.DICOM.m_imgMraw);
	mImage->setModality(metaData.mModality);
	mImage->setImageType(metaData.mModalityType);
	mImage->setName(metaData.mName);
	mImage->setAcquisitionTime(metaData.mAcquisitionTime.toQDateTime());
	mImage->getLookupTable2D()->setWindow(metaData.Volume.mWindowWidth);
	mImage->getLookupTable2D()->setLevel(metaData.Volume.mWindowCenter);
	mImage->getTransferFunctions3D()->setWindow(metaData.Volume.mWindowWidth);
	mImage->getTransferFunctions3D()->setLevel(metaData.Volume.mWindowCenter);

//	if (!success)
//	{
//		mImageData = NULL;
//		mImage.reset();
//		mLut = NULL;
//	}

	// test save
//	ssc::SNW2VolumePtr vol = ssc::SNW2Volume::create("/home/christiana/temp/outvol", metaData, mImageData,
//		vtkLookupTablePtr());
//	vol->save();
//	std::cout << "saved vol" << std::endl;

	return mImage;
}

bool Series::rawLoadMetaData(ssc::SNW2VolumeMetaData &data) const
{
	data.mConversionTime = ssc::SNW2VolumeMetaData::DateTime::fromTimestamp(QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat()));
	data.mModality = mBase->modality;
//	data.mModalityType = file.value("Info/ModalityType").toString();
//	data.mIntraoperative = file.value("Info/Intraoperative").toBool();
//	data.mName = file.value("Info/Name").toString();
	data.mName = mBase->seriesDescription; //TODO finn rett verdi
	if (data.mName.isEmpty())
		data.mName = mBase->seriesID;
//
//	if (data.mModality == "US")
//	{
//		data.mModalityType = file.value("Ultrasound/DataType").toString();
//		if (data.mModalityType.toUpper() == "FLOW")
//			data.mModalityType = "Angio";
//		if (data.mModalityType.toUpper() == "TISSUE")
//			data.mModalityType = "Tissue";
//	}
//
	data.mAcquisitionTime = ssc::SNW2VolumeMetaData::DateTime::fromDateAndTime(mBase->seriesDate, mBase->seriesTime);
	data.DICOM.mFrameOfReferenceUID = mBase->frameOfReferenceUID;

	ssc::Vector3D iVec(mBase->image_orientation);
	ssc::Vector3D jVec(mBase->image_orientation+3);
	ssc::Vector3D origin(mBase->first_instance->image_position); // instances are sorted along z-axis
	data.DICOM.m_imgMraw = ssc::createTransformIJC(iVec, jVec, origin);

	data.DICOM.mSeriesID = mBase->seriesID;
	data.DICOM.mSeriesDescription = mBase->seriesDescription;

	data.Volume.mSpacing[0] = mBase->pixel_spacing[0];
	data.Volume.mSpacing[1] = mBase->pixel_spacing[1];
	data.Volume.mSpacing[2] = mBase->slice_thickness;

	data.Volume.mDim[0] = mBase->columns;
	data.Volume.mDim[1] = mBase->rows;
	data.Volume.mDim[2] = mBase->frames;

	if (mBase->VOI.numPresets>0)
	{
		data.Volume.mWindowCenter = mBase->VOI.preset[0].window.center;
		data.Volume.mWindowWidth = mBase->VOI.preset[0].window.width;
	}
	else
	{
		data.Volume.mWindowCenter = mBase->VOI.minmax.center;
		data.Volume.mWindowWidth = mBase->VOI.minmax.width;
	}
//	data.Volume.mLLR = file.value("Data/LLR", mMetaData.Volume.mLLR).toDouble();
	data.Volume.mFirstPixel = mBase->firstpixel;
	data.Volume.mLastPixel = mBase->lastpixel;
	data.Volume.mBitsPerSample = mBase->bits_per_sample;
	data.Volume.mSamplesPerPixel = mBase->samples_per_pixel;

	data.Lut.mBitsPerSample = 0;
	data.Lut.mLength = 0;
	data.Lut.mStart = 0;
	data.Lut.mType = "None";

	return true;
}

bool Series::rawLoadVtkImageData(vtkImageDataPtr mImageData, ssc::SNW2VolumeMetaData mMetaData, const char* rawchars)
{
	mImageData->Initialize();
	mImageData->SetDimensions(mMetaData.Volume.mDim.begin());
	mImageData->SetSpacing(mMetaData.Volume.mSpacing.begin());
	mImageData->SetExtent(0, mMetaData.Volume.mDim[0] - 1, 0, mMetaData.Volume.mDim[1] - 1, 0, mMetaData.Volume.mDim[2] - 1);
	int scalarSize = mMetaData.Volume.mDim[0] * mMetaData.Volume.mDim[1] * mMetaData.Volume.mDim[2];

//	QTime pre = QTime::currentTime();
//	QFile file(rawDataFileName());
//	file.open(QIODevice::ReadOnly);
//	QDataStream stream(&file);
//	char *rawchars = (char*) malloc(file.size());
//
//	stream.readRawData(rawchars, file.size());
//	//writeStatus("loaded raw volume["+uid()+"], time="+qstring_cast(pre.msecsTo(QTime::currentTime()))+"ms");
//	pre = QTime::currentTime();
//	bool success = checksumData(rawDataFileName(), (unsigned char*) rawchars, file.size());
//	//writeStatus("checksummed volume["+uid()+"], time="+qstring_cast(pre.msecsTo(QTime::currentTime()))+"ms");
//	if (!success)
//	{
//		writeStatus("md5 checksum failed for volume [" + uid() + "]");
//		return false;
//	}

	// Initialize VTK image
	if (mMetaData.Volume.mSamplesPerPixel == 1 && mMetaData.Volume.mBitsPerSample == 8)
	{
		boost::uint8_t *image = (boost::uint8_t*) (rawchars);
//		if (scalarSize != file.size())
//		{
//			qWarning("unexpected file size for %s should be %d was %lld", file.fileName().toAscii().constData(),
//				scalarSize, file.size());
//		}

		vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
		array->SetNumberOfComponents(1);
		array->SetArray(image, scalarSize, 0); // take ownership
		mImageData->SetScalarTypeToUnsignedChar();
		mImageData->GetPointData()->SetScalars(array);
	}
	else if (mMetaData.Volume.mSamplesPerPixel == 1 && mMetaData.Volume.mBitsPerSample == 16)
	{
		boost::uint16_t *image = (boost::uint16_t*) (rawchars);
//		if (scalarSize * 2 != file.size())
//		{
//			qWarning("%s", "unexpected file size for " + file.fileName());
//		}

		vtkUnsignedShortArrayPtr array = vtkUnsignedShortArrayPtr::New();
		array->SetNumberOfComponents(1);
		array->SetArray(image, scalarSize, 0); // take ownership
		mImageData->SetScalarTypeToUnsignedShort();
		mImageData->GetPointData()->SetScalars(array);
	}
	else if (mMetaData.Volume.mSamplesPerPixel == 3 && mMetaData.Volume.mBitsPerSample == 8)
	{
		boost::uint8_t *image = (boost::uint8_t*) (rawchars);
//		if (scalarSize * 3 != file.size())
//		{
//			qWarning("%s", "unexpected file size for " + file.fileName());
//		}

		vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
		array->SetNumberOfComponents(3);

		array->SetArray(image, scalarSize * 3, 0); // take ownership
		mImageData->SetScalarTypeToUnsignedChar();
		mImageData->SetNumberOfScalarComponents(3);
		mImageData->GetPointData()->SetScalars(array);
	}
	else
	{
		qWarning("%s", QString("failed to load volume with samplesPerPixel=" + qstring_cast(mMetaData.Volume.mSamplesPerPixel)
			+ " and bitsPerSample=" + qstring_cast(mMetaData.Volume.mBitsPerSample)).toAscii().constData());
		return false;
	}
	return true;
}


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


Study::Study(struct study_t *base) : mBase(base)
{

}

std::vector<SeriesPtr> Study::getSeries()
{
	std::vector<SeriesPtr> retval;

	series_t* iter = DICOMLib_GetSeries(mBase, NULL);

	for ( ; iter!=NULL; iter = iter->next_series )
	{
		SeriesPtr current(new Series(iter));
		retval.push_back(current);
	}

	return retval;
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

DICOMLibAPIPtr DICOMLibAPI::New()
{
	return DICOMLibAPIPtr(new DICOMLibAPI());
}

DICOMLibAPI::DICOMLibAPI() : mData(NULL)
{
	DICOMLib_Init();
}

DICOMLibAPI::~DICOMLibAPI()
{
	DICOMLib_Done();
}

void DICOMLibAPI::setRootFolder(QString root)
{
	mRootFolder = root;
	this->refresh();
}

void DICOMLibAPI::refresh()
{
	struct series_t *series;
	mData = DICOMLib_StudiesFromPath(cstring_cast(mRootFolder), NULL, DICOMLIB_NO_CACHE);
//	std::cout << "study: " << study << std::endl;
}

std::vector<StudyPtr> DICOMLibAPI::getStudies()
{
	std::vector<StudyPtr> retval;
	study_t *iter = mData;

	for ( ; iter!=NULL; iter = iter->next_study )
	{
		StudyPtr current(new Study(iter));
		retval.push_back(current);
	}

	return retval;
}
