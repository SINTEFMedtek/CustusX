// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscSNW2Volume.h"

#include <boost/cstdint.hpp>
#include <QStringList>
#include <QFile>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkUnsignedShortArray.h>
#include <vtkLookupTable.h>
#include <vtkImageAccumulate.h>

#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscImage.h"
#include "sscSnwInifile.h"
#include "sscMD5Check.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{

/**helper conversion function 
 */
Vector3D stringList2Vector3D(QStringList raw)
{
	Vector3D retval(0, 0, 0);
	if (raw.size() != 3)
		return retval;
	for (int i = 0; i < raw.size(); ++i)
	{
		retval[i] = raw[i].toDouble();
	}
	return retval;
}

SNW2VolumeMetaData::DateTime SNW2VolumeMetaData::DateTime::fromDateAndTime(QString date, QString time)
{
	DateTime retval;
	retval.mDate = date;
	retval.mTime = time;
	return retval;
}

SNW2VolumeMetaData::DateTime SNW2VolumeMetaData::DateTime::fromTimestamp(QString ts)
{
	DateTime retval;
	QStringList val = ts.split("T");
	if (val.size() == 2)
	{
		retval.mDate = val[0];
		retval.mTime = val[1];
	}
	return retval;
}

QString SNW2VolumeMetaData::DateTime::timestamp() const
{
	if (mDate.isEmpty())
		return "";
	return createTimestamp(robustReadTime());
}

QString SNW2VolumeMetaData::DateTime::time() const
{
	return mTime;
}
QString SNW2VolumeMetaData::DateTime::date() const
{
	return mDate;
}

bool SNW2VolumeMetaData::DateTime::isValid() const
{
	return robustReadTime().isValid();
}

QDateTime SNW2VolumeMetaData::DateTime::toQDateTime() const
{
	return this->convertTimestamp2QDateTime(this->timestamp());
}

/** defines the format Sonowand uses as its timestamp format  
 *  - in conjunction with QString.
 */
QString SNW2VolumeMetaData::DateTime::timestampFormat() const
{
	return QString(timestampSecondsFormat());
}

QDateTime SNW2VolumeMetaData::DateTime::convertTimestamp2QDateTime(const QString& timestamp) const ///< converter for the custom sonowand timestamp format
{
	return QDateTime::fromString(timestamp, timestampFormat());
}

QString SNW2VolumeMetaData::DateTime::createTimestamp(const QDateTime& datetime) const ///< converter for the custom sonowand timestamp format
{
	return datetime.toString(timestampFormat());
}

QDateTime SNW2VolumeMetaData::DateTime::robustReadTime() const
{
	return robustReadTime(mDate + "T" + mTime);
}

/**read a date time on the format yyyyMMdd'T'hhmm.zzzzzz,
 * but with 0-6 z's.
 */
QDateTime SNW2VolumeMetaData::DateTime::robustReadTime(QString rawDateTime) const
{
	QStringList val = rawDateTime.split(".");
	//QDateTime retval = QDateTime::fromString(val[0], "yyyyMMdd'T'hhmmss");
	QDateTime retval = convertTimestamp2QDateTime(val[0]);
	if (val.size() == 2)
	{
		double frac = ("0." + val[1]).toDouble();
		retval.addMSecs((int) frac * 1000);
	}
	return retval;
}

SNW2VolumeMetaData::SNW2VolumeMetaData()
{
	Volume.mWindowCenter = -1.0;
	Volume.mWindowWidth = -1.0;
	Volume.mLLR = 0.0;
	Volume.mFirstPixel = -1;
	Volume.mLastPixel = -1;
	mIntraoperative = false;
	Lut.mBitsPerSample = 0;
	Lut.mLength = 0;
	Lut.mStart = -1;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

/**Factory function
 */
SNW2VolumePtr SNW2Volume::create(const QString& filePath)
{
	SNW2VolumePtr retval(new SNW2Volume(filePath));

	if (!retval->loadAll() || !retval->ensureCenterWindowValid(&retval->mMetaData.Volume.mWindowWidth,
	                                                           &retval->mMetaData.Volume.mWindowCenter,
	                                                           &retval->mMetaData.Volume.mLLR))
	{
		return SNW2VolumePtr();
	}
	return retval;
}

SNW2VolumePtr SNW2Volume::create(const QString& filePath, const SNW2VolumeMetaData& metaData,
	vtkImageDataPtr imageData, vtkLookupTablePtr lut)
{
	SNW2VolumePtr retval(new SNW2Volume(filePath));

	retval->mMetaData = metaData;
	retval->mImageData = imageData;
	retval->mLut = lut;
	retval->mImage = ssc::ImagePtr(new ssc::Image("series_" + retval->uid(), retval->mImageData));

	return retval;
}

void SNW2Volume::writeStatus(const QString& text) const
{
	qWarning("%s", text.toAscii().constData());
}

bool SNW2Volume::loadAll()
{
	if (!loadMetaData())
	{
		writeStatus("volume[" + uid() + "], failed - metadata");
		return false;
	}
	//writeStatus("volume["+uid()+"], loaded metadata");
	return true;
}

SNW2Volume::SNW2Volume(const QString& filePath)
{
	mFilePath = filePath;
	mUid = filePath.split("/").back();
}

SNW2Volume::~SNW2Volume()
{
}

QString SNW2Volume::rawDataFileName() const
{
	return filePath() + ".raw";
}

QString SNW2Volume::rawLutFileName() const
{
	return filePath() + ".lut";
}

QString SNW2Volume::iniFileName() const
{
	return filePath() + ".ini";
}

void SNW2Volume::save() const
{
	rawSaveMetaData(iniFileName(), mMetaData);
	rawSaveVolumeData(rawDataFileName(), mImageData);
	rawSaveLutData(rawLutFileName(), mLut);
}

template<class T>
QStringList SNW2Volume::streamable2QStringList(const T& val) const
{
	return qstring_cast(val).split(' ', QString::SkipEmptyParts);
}

boost::array<int, 3> SNW2Volume::stringList2IntArray3(QStringList raw) const
{
	boost::array<int, 3> retval;
	std::fill(retval.begin(), retval.end(), 0);
	if (raw.size() != 3)
		return retval;
	for (int i = 0; i < raw.size(); ++i)
	{
		retval[i] = raw[i].toInt();
	}
	return retval;
}

///////////////////////////////////////////////////////////
//////////     load methods     ///////////////////////////
///////////////////////////////////////////////////////////


bool SNW2Volume::loadMetaData()
{
	if (mMetaData.mConversionTime.isValid()) // already loaded?
	{
		return true;
	}

	if (!rawLoadMetaData(mMetaData))
	{
		return false; // load or validation failed
	}

	if (!mMetaData.mConversionTime.isValid()) // load failed?
	{
		return false;
	}

	return true;
}

bool SNW2Volume::rawLoadMetaData(SNW2VolumeMetaData &data) const
{
	SonowandInifile file(iniFileName());

	if (!file.checkOK())
	{
		writeStatus("metadata bad ini checksum for " + file.fileName());
		return false;
	}

	data.mConversionTime = SNW2VolumeMetaData::DateTime::fromDateAndTime(file.value("Info/ConversionDate").toString(),
		file.value("Info/ConversionTime").toString());
	data.mModality = file.value("Info/Modality").toString();
	data.mModalityType = file.value("Info/ModalityType").toString();
	data.mIntraoperative = file.value("Info/Intraoperative").toBool();
	data.mName = file.value("Info/Name").toString();

	if (data.mModality == "US")
	{
		data.mModalityType = file.value("Ultrasound/DataType").toString();
		if (data.mModalityType.toUpper() == "FLOW")
			data.mModalityType = "Angio";
		if (data.mModalityType.toUpper() == "TISSUE")
			data.mModalityType = "Tissue";
	}

	data.mAcquisitionTime = SNW2VolumeMetaData::DateTime::fromDateAndTime(file.value("Acquisition/Date").toString(),
		file.value("Acquisition/Time").toString());
	data.DICOM.mFrameOfReferenceUID = file.value("DICOM/FrameOfReferenceUid").toString();

	QStringList orientation = file.value("DICOM/ImageOrientationPatient").toStringList();
	QStringList position = file.value("DICOM/ImagePositionPatient").toStringList();
	Vector3D iVec(1, 0, 0), jVec(0, 1, 0);
	if (orientation.size() == 6)
	{
		iVec = stringList2Vector3D(orientation.mid(0, 3));
		jVec = stringList2Vector3D(orientation.mid(3, 3));
	}
	Vector3D origin = stringList2Vector3D(position);
	data.DICOM.m_imgMraw = createTransformIJC(iVec, jVec, origin);

	data.DICOM.mSeriesID = file.value("DICOM/SeriesID").toString();
	data.DICOM.mSeriesDescription = file.value("DICOM/SeriesDescription").toString();

	QVariant pixelspacing(file.value("Data/PixelSpacing").toStringList().join(","));
	QVariant picdimension(file.value("Data/Dimension").toStringList().join(","));
	if (pixelspacing.type() == QVariant::StringList)
	{
		data.Volume.mSpacing = stringList2Vector3D(pixelspacing.toStringList());
	}
	else
	{
		data.Volume.mSpacing = stringList2Vector3D(pixelspacing.toString().split(","));
	}
	if (picdimension.type() == QVariant::StringList)
	{
		data.Volume.mDim = stringList2IntArray3(picdimension.toStringList());
	}
	else
	{
		data.Volume.mDim = stringList2IntArray3(picdimension.toString().split(","));
	}
	if (data.Volume.mDim[0] <= 0 || data.Volume.mDim[1] <= 0 || data.Volume.mDim[2] <= 0)
	{
		writeStatus("metadata bad size for " + file.fileName());
		return false;
	}
	data.Volume.mWindowCenter = file.value("Data/WindowCenter", mMetaData.Volume.mWindowCenter).toDouble();
	data.Volume.mWindowWidth = file.value("Data/WindowWidth", mMetaData.Volume.mWindowWidth).toDouble();
	data.Volume.mLLR = file.value("Data/LLR", mMetaData.Volume.mLLR).toDouble();
	data.Volume.mFirstPixel = file.value("Data/FirstPixel", mMetaData.Volume.mFirstPixel).toDouble();
	data.Volume.mLastPixel = file.value("Data/LastPixel", mMetaData.Volume.mLastPixel).toDouble();
	data.Volume.mBitsPerSample = file.value("Data/BitsPerSample").toInt();
	data.Volume.mSamplesPerPixel = file.value("Data/SamplesPerPixel").toInt();

	data.Lut.mBitsPerSample = file.value("Lut/BitsPerSample").toInt();
	data.Lut.mLength = file.value("Lut/Length").toInt();
	data.Lut.mStart = file.value("Lut/Start").toInt();
	data.Lut.mType = file.value("Lut/Type").toString();

	return true;
}

bool SNW2Volume::loadVolumeData()
{
	if (mImage) // already loaded?
	{
		return true;
	}

	bool success = true;
	if (mMetaData.Lut.mType != "None")
	{
		success = success && CheckMD5(cstring_cast(rawLutFileName()));
		if (!success)
		{
			writeStatus("md5 checksum failed for lut [" + uid() + "]");
			return false;
		}
	}
	mImageData = vtkImageDataPtr::New();
	mImage = ssc::ImagePtr(new ssc::Image("series_" + uid(), mImageData));
	mLut = vtkLookupTablePtr::New();

	success = success && rawLoadVtkImageData();
	success = success && rawLoadLut(rawLutFileName(), mLut);

	if (!success)
	{
		mImageData = NULL;
		mImage.reset();
		mLut = NULL;
	}

	return success;
}

bool SNW2Volume::rawLoadVtkImageData()
{
	mImageData->Initialize();
	mImageData->SetSpacing(mMetaData.Volume.mSpacing.begin());
	mImageData->SetExtent(0, mMetaData.Volume.mDim[0] - 1, 0, mMetaData.Volume.mDim[1] - 1, 0, mMetaData.Volume.mDim[2] - 1);
	int scalarSize = mMetaData.Volume.mDim[0] * mMetaData.Volume.mDim[1] * mMetaData.Volume.mDim[2];

	QTime pre = QTime::currentTime();
	QFile file(rawDataFileName());
	file.open(QIODevice::ReadOnly);
	QDataStream stream(&file);
	char *rawchars = (char*) malloc(file.size());

	stream.readRawData(rawchars, file.size());
	//writeStatus("loaded raw volume["+uid()+"], time="+qstring_cast(pre.msecsTo(QTime::currentTime()))+"ms");
	pre = QTime::currentTime();
	bool success = checksumData(rawDataFileName(), (unsigned char*) rawchars, file.size());
	//writeStatus("checksummed volume["+uid()+"], time="+qstring_cast(pre.msecsTo(QTime::currentTime()))+"ms");
	if (!success)
	{
		writeStatus("md5 checksum failed for volume [" + uid() + "]");
		return false;
	}

	// Initialize VTK image
	if (mMetaData.Volume.mSamplesPerPixel == 1 && mMetaData.Volume.mBitsPerSample == 8)
	{
		boost::uint8_t *image = (boost::uint8_t*) (rawchars);
		if (scalarSize != file.size())
		{
			qWarning("unexpected file size for %s should be %d was %lld", file.fileName().toAscii().constData(),
				scalarSize, file.size());
		}

		vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
		array->SetNumberOfComponents(1);
		array->SetArray(image, scalarSize, 0); // take ownership
		mImageData->SetScalarTypeToUnsignedChar();
		mImageData->GetPointData()->SetScalars(array);
	}
	else if (mMetaData.Volume.mSamplesPerPixel == 1 && mMetaData.Volume.mBitsPerSample == 16)
	{
		boost::uint16_t *image = (boost::uint16_t*) (rawchars);
		if (scalarSize * 2 != file.size())
		{
			writeStatus("unexpected file size for " + file.fileName());
		}

		vtkUnsignedShortArrayPtr array = vtkUnsignedShortArrayPtr::New();
		array->SetNumberOfComponents(1);
		array->SetArray(image, scalarSize, 0); // take ownership
		mImageData->SetScalarTypeToUnsignedShort();
		mImageData->GetPointData()->SetScalars(array);
	}
	else if (mMetaData.Volume.mSamplesPerPixel == 3 && mMetaData.Volume.mBitsPerSample == 8)
	{
		boost::uint8_t *image = (boost::uint8_t*) (rawchars);
		if (scalarSize * 3 != file.size())
		{
			writeStatus("unexpected file size for " + file.fileName());
		}

		vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
		array->SetNumberOfComponents(3);

		array->SetArray(image, scalarSize * 3, 0); // take ownership
		mImageData->SetScalarTypeToUnsignedChar();
		mImageData->SetNumberOfScalarComponents(3);
		mImageData->GetPointData()->SetScalars(array);
	}
	else
	{
		writeStatus("failed to load volume with samplesPerPixel=" + qstring_cast(mMetaData.Volume.mSamplesPerPixel)
			+ " and bitsPerSample=" + qstring_cast(mMetaData.Volume.mBitsPerSample));
		return false;
	}
	mImageData->UpdateInformation();
	return true;
}

bool SNW2Volume::checksumData(QString filename, const unsigned char* const data, int size)
{
	return CheckMD5InMemory(cstring_cast(filename), data, size);
}

bool SNW2Volume::rawLoadLut(const QString& filename, vtkLookupTablePtr lut) const
{
	lut->SetNumberOfTableValues(mMetaData.Lut.mLength);

	if (mMetaData.Lut.mType == "None")
	{
		//// sscImageLUT2D requires a detailed lut in order to work. Thats why we cant have 2 values here.
		int range = 256;

		lut->Build();
		lut->SetNumberOfTableValues(range);
		lut->SetTableRange(0, range);
		for (int i = 0; i < range; ++i)
		{
			double v = double(i) / double(range - 1);
			lut->SetTableValue(i, v, v, v, 1);
		}
		lut->Modified();
	}
	else if (mMetaData.Lut.mBitsPerSample == 8 && mMetaData.Lut.mType == "RGBA")
	{
		int lutSize = mMetaData.Lut.mLength;
		if (lutSize == 0) // An old way to indicate no lut
		{
			return true;
		}
		QFile file(filename);
		file.open(QIODevice::ReadOnly);
		QDataStream stream(&file);
		if (!file.isOpen())
		{
			writeStatus("volume" + uid() + "] " + "could not open LUT file");
			return false;
		}
		if (file.size() != lutSize * 4)
		{
			writeStatus("volume" + uid() + "] " + "wrong size RGBA lut");
			return false;
		}
		vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
		array->SetNumberOfComponents(4);
		boost::uint8_t *raw = (boost::uint8_t*) malloc(lutSize * 4);
		char* charArray = reinterpret_cast<char*> (raw);
		stream.readRawData(charArray, lutSize * 4);
		array->SetArray(raw, lutSize * 4, 0); // take ownership
		lut->SetTable(array);
	}
	else
	{
		writeStatus("volume" + uid() + "]" + "failed to load lut");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////
//////////     save methods     ///////////////////////////
///////////////////////////////////////////////////////////


void SNW2Volume::rawSaveMetaData(const QString& filename, const SNW2VolumeMetaData& data) const
{
	//writeStatus("writing ini "+filename);
	SonowandInifile file(filename);

	file.setValue("Format/Type", "SNW2");
	file.setValue("Format/Version", 1);

	file.setValue("Info/ConversionDate", data.mConversionTime.date());
	file.setValue("Info/ConversionTime", data.mConversionTime.time());

	file.setValue("Info/Modality", data.mModality);
	file.setValue("Info/ModalityType", data.mModalityType);
	file.setValue("Info/Intraoperative", data.mIntraoperative);
	file.setValue("Info/Name", data.mName);

	file.setValue("Acquisition/Date", data.mAcquisitionTime.date());
	file.setValue("Acquisition/Time", data.mAcquisitionTime.time());

	file.setValue("DICOM/FrameOfReferenceUid", data.DICOM.mFrameOfReferenceUID);

	Vector3D iVec = data.DICOM.m_imgMraw.vector(Vector3D(1, 0, 0));
	Vector3D jVec = data.DICOM.m_imgMraw.vector(Vector3D(0, 1, 0));
	Vector3D pos = data.DICOM.m_imgMraw.coord(Vector3D(0, 0, 0));
	QStringList position = streamable2QStringList(pos);
	QStringList orientation = streamable2QStringList(iVec) + streamable2QStringList(jVec);
	file.setValue("DICOM/ImageOrientationPatient", orientation);
	file.setValue("DICOM/ImagePositionPatient", position);

	file.setValue("DICOM/SeriesID", data.DICOM.mSeriesID);
	file.setValue("DICOM/SeriesDescription", data.DICOM.mSeriesDescription);

	file.setValue("Data/PixelSpacing", streamable2QStringList(data.Volume.mSpacing));

	QStringList dimensionString;
	dimensionString << QString::number(data.Volume.mDim[0]);
	dimensionString << QString::number(data.Volume.mDim[1]);
	dimensionString << QString::number(data.Volume.mDim[2]);
	file.setValue("Data/Dimension", dimensionString);

	file.setValue("Data/WindowCenter", data.Volume.mWindowCenter);
	file.setValue("Data/WindowWidth", data.Volume.mWindowWidth);
	file.setValue("Data/LLR", data.Volume.mLLR);
	file.setValue("Data/FirstPixel", data.Volume.mFirstPixel);
	file.setValue("Data/LastPixel", data.Volume.mLastPixel);
	file.setValue("Data/BitsPerSample", data.Volume.mBitsPerSample);
	file.setValue("Data/SamplesPerPixel", data.Volume.mSamplesPerPixel);

	file.setValue("Lut/BitsPerSample", data.Lut.mBitsPerSample);
	file.setValue("Lut/Length", data.Lut.mLength);
	file.setValue("Lut/Start", data.Lut.mStart);
	file.setValue("Lut/Type", data.Lut.mType);
}

void SNW2Volume::rawSaveVolumeData(const QString& filename, vtkImageDataPtr imageData) const
{
	//writeStatus("writing volume "+filename);

	int scalarSize = mMetaData.Volume.mDim[0] * mMetaData.Volume.mDim[1] * mMetaData.Volume.mDim[2];

	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);

	if (mMetaData.Volume.mSamplesPerPixel == 1 && mMetaData.Volume.mBitsPerSample == 8)
	{
		vtkUnsignedCharArrayPtr array = vtkUnsignedCharArray::SafeDownCast(mImageData->GetPointData()->GetScalars());
		char* charArray = reinterpret_cast<char*> (array->GetPointer(0));
		stream.writeRawData(charArray, scalarSize);
	}
	else if (mMetaData.Volume.mSamplesPerPixel == 1 && mMetaData.Volume.mBitsPerSample == 16)
	{
		vtkUnsignedShortArrayPtr array = vtkUnsignedShortArray::SafeDownCast(mImageData->GetPointData()->GetScalars());
		char* charArray = reinterpret_cast<char*> (array->GetPointer(0));
		stream.writeRawData(charArray, scalarSize * 2);
	}
	else if (mMetaData.Volume.mSamplesPerPixel == 3 && mMetaData.Volume.mBitsPerSample == 8)
	{
		vtkUnsignedCharArrayPtr array = vtkUnsignedCharArray::SafeDownCast(mImageData->GetPointData()->GetScalars());
		char* charArray = reinterpret_cast<char*> (array->GetPointer(0));
		stream.writeRawData(charArray, scalarSize * 3);
	}
	else
	{
		writeStatus(uid() + "failed to save volume with samplesPerPixel=" + qstring_cast(
			mMetaData.Volume.mSamplesPerPixel) + " and bitsPerSample=" + qstring_cast(mMetaData.Volume.mBitsPerSample));
	}

	file.close();
	GenerateMD5(rawDataFileName().toAscii().constData());
}

void SNW2Volume::rawSaveLutData(const QString& filename, vtkLookupTablePtr lut) const
{
	if (mMetaData.Lut.mType == "None")
	{
		return;
	}

	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);
	int lutSize = lut->GetNumberOfTableValues();

	if ((mMetaData.Lut.mType == "RGBA") && (mMetaData.Lut.mBitsPerSample == 8) && (mMetaData.Lut.mStart == 0))
	{
		vtkUnsignedCharArrayPtr array = lut->GetTable();
		char* charArray = reinterpret_cast<char*> (array->GetPointer(0));
		stream.writeRawData(charArray, lutSize * 4);
	}
	else
	{
		writeStatus(uid() + ": failed to save lut with type=" + qstring_cast(mMetaData.Lut.mType)
			+ " and bitsPerSample=" + qstring_cast(mMetaData.Lut.mBitsPerSample));
	}

	file.close();
	GenerateMD5(rawLutFileName().toAscii().constData());
}

/** Insert default values for Center/Window if not set by load
 */
bool SNW2Volume::ensureCenterWindowValid(double* windowPtr, double* levelPtr, double* llrPtr)
{
	double& window = *windowPtr;
	double& level = *levelPtr;
	double& llr = *llrPtr;

	boost::array<double, 2> range;
	if (mMetaData.Volume.mFirstPixel < 0 || mMetaData.Volume.mLastPixel < 0)
	{
		if (!loadVolumeData())
		{
			return false;
		}
		// If we lack this data, we are forced to load the image already. Breaks lazy loading.
		vtkImageAccumulatePtr histogram = getImage()->getHistogram();
		range[0] = histogram->GetMin()[0];
		range[1] = histogram->GetMax()[0];
	}
	else
	{
		range[0] = mMetaData.Volume.mFirstPixel;
		range[1] = mMetaData.Volume.mLastPixel;
	}

	if (window <= 0)
	{
		window = (range[1] - range[0]);
	}
	if (level < 0)
	{
		level = range[0] + (range[1] - range[0]) / 2.0;
	}

	// non-us volumes that have a preset get a llr equal to the lower end of the window.
	if (!mMetaData.mIntraoperative && window > 0 && level > 0)
	{
		llr = 0;
	}

	if (mMetaData.mModalityType.toUpper().contains("ANGIO")) // set a default LLR for angio. //TODO move it to a better place. Tried to move it to vmLayer::createFromSeries
	{
		llr = range[0] + 0.2 * (range[1] - range[0]);
	}
	else if (mMetaData.mModality == "US") // set a default LLR for US (0 is defined to be transparent by scanconversion). //TODO move it to a better place.
	{
		llr = 1;
	}
	return true;
}

vtkImageDataPtr SNW2Volume::getVtkImageData()
{
	if (!mImageData)
	{
		loadVolumeData();
	}
	return mImageData;
}

ssc::ImagePtr SNW2Volume::getImage()
{
	if (!mImageData)
	{
		loadVolumeData();
	}
	return mImage;
}

vtkLookupTablePtr SNW2Volume::getLut()
{
	if (!mImageData)
	{
		loadVolumeData();
	}
	return mLut;
}

} // namespace ssc
