/*
 * DICOMLibAPI.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: christiana
 */

#include "sscDICOMLibAPI.h"
#include <QThread>
#include <QMutexLocker>
#include "DICOMLib.h"
#include <iostream>
#include "sscTypeConversions.h"
#include "sscImage.h"
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkUnsignedShortArray.h>
#include <vtkShortArray.h>
#include <vtkUnsignedCharArray.h>
#include <boost/cstdint.hpp>
#include "sscTime.h"
#include "sscRegistrationTransform.h"
#include "sscImageLUT2D.h"
#include "sscImageTF3D.h"
#include "sscLogger.h"
#include "boost/function.hpp"
#include "boost/bind.hpp"

typedef vtkSmartPointer<vtkShortArray> vtkShortArrayPtr;

namespace ssc
{


Series::Series(struct series_t* base, StudyWeakPtr parent) :
	mBase(base),
	mParent(parent)
{
	this->getMetaData();
	mUid = mMetaData->mConversionTime.timestamp()
		+ "_"
		+ mMetaData->DICOM.mSeriesID;
}

ssc::SNW2VolumeMetaDataPtr Series::getMetaData()
{
	if (mMetaData)
		return mMetaData;

	mMetaData.reset(new ssc::SNW2VolumeMetaData());
	this->rawLoadMetaData(*mMetaData);
	return mMetaData;
}

StudyPtr Series::getParentStudy()
{
	return mParent.lock();
}

ssc::ImagePtr Series::getImage()
{
//	ssc::ImagePtr retval;
//	ssc::ImagePtr mImage;

	if (!mBase)
		return mImage;

	if (mImage)
		return mImage;

	DICOMLib_UseAutoVOI(mBase);
	QTime t_start = QTime::currentTime();
	volume_t *data = DICOMLib_GetVolume(mBase, NULL);
	std::cout << "DICOMLib_GetVolume():  " << t_start.msecsTo(QTime::currentTime()) << std::endl; t_start = QTime::currentTime();
	SSC_LOG("RescaleIntercept: %i", data->rescaleIntercept);
//	std::cout << "data: " << data << std::endl;
	//	assert(data);
	const char *ptr = (const char *) data->volume;

//	int lutsize = mBase->VOI.lut.length;
//	std::cout << "lutsize " << lutsize << std::endl;

	bool success = true;
	vtkImageDataPtr imageData = vtkImageDataPtr::New();
	mImage = ssc::ImagePtr(new ssc::Image(this->getUid(), imageData));
//	mLut = vtkLookupTablePtr::New();

//	TODO:
//
//	- fyll snw metadata fra dicom.
//	- bruk snwformat-imagelesefunksjonen her også - lag generell.

	// force load metadata once more : this is IMPORTANT because some metainfo is read using DICOMLib_GetVolume()
	this->rawLoadMetaData(*mMetaData);
	ssc::SNW2VolumeMetaData metaData = *mMetaData;
//	ssc::SNW2VolumeMetaData metaData;
//	success = success && this->rawLoadMetaData(metaData);
	success = success && this->rawLoadVtkImageData(imageData, metaData, ptr, data->rescaleIntercept);
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
	SSC_LOG("slice_thickness: %f, %s", mBase->slice_thickness, qstring_cast(data.Volume.mSpacing).toAscii().constData());

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

bool Series::rawLoadVtkImageData(vtkImageDataPtr mImageData, ssc::SNW2VolumeMetaData mMetaData, const char* rawchars, int rescaleIntercept)
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
	else if (mMetaData.Volume.mSamplesPerPixel == 1 && mMetaData.Volume.mBitsPerSample == 16 && rescaleIntercept!=0)
	{
		// this is an originally signed volume that is converted by the DICOMLib.
		// revert to original state.

		boost::int16_t *image = (boost::int16_t*) (rawchars);
		boost::uint16_t *uimage = (boost::uint16_t*) (rawchars);

//		SSC_LOG("starting %i", scalarSize);
		for (int i =0; i<scalarSize; ++i)
		{
//			SSC_LOG("  %i, uval=%i", i, uimage[i]);
			image[i] = boost::int16_t(uimage[i]) - rescaleIntercept;
//			SSC_LOG("  val=%i", image[i]);
		}

		vtkShortArrayPtr array = vtkShortArrayPtr::New();
		array->SetNumberOfComponents(1);
		array->SetArray(image, scalarSize, 0); // take ownership
		mImageData->SetScalarTypeToShort();
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

void StudyData::put(std::ostream& s) const
{
	s << std::left;
	int hw = 20;
	s << setw(hw) << "PatientName" << mPatientName << std::endl;
	s << setw(hw) << "PatientID" << mPatientID << std::endl;
	s << setw(hw) << "PatientBirthDate" << mPatientBirthDate.toString("yyyy-MM-dd") << std::endl;
	s << setw(hw) << "PatientSex" << mPatientSex << std::endl;
	s << setw(hw) << "StudyDate" << mStudyDate.toString("yyyy-MM-dd hh:mm") << std::endl;
	s << setw(hw) << "StudyID" << mStudyID << std::endl;
	s << setw(hw) << "StudyDescription" << mStudyDescription << std::endl;
}

StudyPtr Study::New(study_t *base)
{
	StudyPtr retval(new Study(base));
	retval->mSelf = retval;
	return retval;
}

Study::Study(struct study_t *base) : mBase(base)
{

}

std::vector<SeriesPtr> Study::getSeries()
{
//	mSeries.clear();

	if (mSeries.empty())
		this->buildSeries();
	return mSeries;
}

void Study::buildSeries()
{
	for (series_t* iter = DICOMLib_GetSeries(mBase, NULL); iter!=NULL; iter = iter->next_series )
	{
		SeriesPtr current(new Series(iter, mSelf));
 		mSeries.push_back(current);
	}
}

StudyData Study::getData()
{
	StudyData retval;
	retval.mPatientName = QString::fromUtf8(mBase->patientName);
	retval.mPatientID = mBase->patientID;
	retval.mPatientBirthDate = QDate::fromString(QString(mBase->patientBirthDate), "yyyyMMdd");
	retval.mPatientSex = mBase->patientSex;
	retval.mStudyDate = SNW2VolumeMetaData::DateTime::fromDateAndTime(mBase->studyDate, mBase->studyTime).toQDateTime();
	retval.mStudyID = mBase->studyID;
	retval.mStudyDescription = QString::fromUtf8(mBase->studyDescription);

	return retval;
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

static RefreshThread* staticRefreshThreadInstance = NULL;

RefreshThread::RefreshThread(QObject* parent, DICOMLibAPI* base) :
	QThread(parent), mBase(base)
{
	staticRefreshThreadInstance = this;
}

RefreshThread::~RefreshThread()
{
	staticRefreshThreadInstance = NULL;
}

int freeProgressFunc(int value)
{
//	SSC_LOG("progress %i", value);
	staticRefreshThreadInstance->progressFunc(value);
	return 0;
}

void RefreshThread::progressFunc(int value)
{
	emit progress(value);
}

void RefreshThread::run()
{
//	boost::function<int(int)> func = boost::bind(&RefreshThread::progressFunc, this, _1);
	emit progress(0);
	QTime t_start = QTime::currentTime();
	QMutexLocker sentry(&mBase->mMutex);

	mBase->mData = DICOMLib_StudiesFromPath(cstring_cast(mBase->mRootFolder), freeProgressFunc, DICOMLIB_NO_CACHE);
	std::cout << "DICOMLib_StudiesFromPath():  " << t_start.msecsTo(QTime::currentTime()) << std::endl; t_start = QTime::currentTime();
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

DICOMLibAPIPtr DICOMLibAPI::New()
{
	return DICOMLibAPIPtr(new DICOMLibAPI());
}

DICOMLibAPI::DICOMLibAPI() : mMutex(QMutex::Recursive), mData(NULL), mRefreshThread(NULL)
{
	DICOMLib_Init();
}

DICOMLibAPI::~DICOMLibAPI()
{
	DICOMLib_Done();
}

void DICOMLibAPI::setRootFolder(QString root)
{
	QMutexLocker sentry(&mMutex);
	if (mRootFolder == root)
		return;

	SSC_LOG("%s", root.toAscii().constData());

	mRootFolder = root;
	sentry.unlock();

	this->refresh();
}

QString DICOMLibAPI::getRootFolder() const
{
	QMutexLocker sentry(&mMutex);
	return mRootFolder;
}


void DICOMLibAPI::refresh()
{
	mRefreshThread = new RefreshThread(this, this);
	connect(mRefreshThread, SIGNAL(started()),     this, SIGNAL(refreshStarted()));
	connect(mRefreshThread, SIGNAL(progress(int)), this, SIGNAL(refreshProgress(int)));
	connect(mRefreshThread, SIGNAL(finished()),    this, SLOT(refreshFinishedSlot()));
	mRefreshThread->start();

//	QTime t_start = QTime::currentTime();
////	struct series_t *series;
//	mData = DICOMLib_StudiesFromPath(cstring_cast(mRootFolder), NULL, DICOMLIB_NO_CACHE);
////	std::cout << "study: " << study << std::endl;
//	std::cout << "DICOMLib_StudiesFromPath():  " << t_start.msecsTo(QTime::currentTime()) << std::endl; t_start = QTime::currentTime();
//	SSC_LOG("%s", "hei");
//	emit changed();
}

void DICOMLibAPI::refreshFinishedSlot()
{
	delete mRefreshThread;
	mRefreshThread = NULL;

	emit refreshFinished();
	emit changed();
}

std::vector<StudyPtr> DICOMLibAPI::getStudies()
{
//	mStudies.clear();
	if (mStudies.empty())
		this->buildStudies();
	return mStudies;
}

void DICOMLibAPI::buildStudies()
{
	QMutexLocker sentry(&mMutex);

	for (study_t *iter = mData; iter!=NULL; iter = iter->next_study )
	{
		StudyPtr current = Study::New(iter);
		mStudies.push_back(current);
	}
}

} // namespace ssc
