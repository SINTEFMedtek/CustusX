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

#ifndef SSCSNW2VOLUME_H_
#define SSCSNW2VOLUME_H_

#include <QDateTime>
#include "sscTransform3D.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

namespace ssc
{

/**\brief Metadata for the Sonowand 2 file format.
 *
 * \ingroup sscData
 */
class SNW2VolumeMetaData
{
public:
	class DateTime
	{
	public:
		static DateTime fromDateAndTime(QString date, QString time);
		static DateTime fromTimestamp(QString ts);
		QString timestamp() const;
		QDateTime toQDateTime() const;
		QString time() const;
		QString date() const;
		bool isValid() const;
	private:
		QString timestampFormat() const;
		QDateTime convertTimestamp2QDateTime(const QString& timestamp) const;
		QString createTimestamp(const QDateTime& datetime) const;
		QDateTime robustReadTime() const;
		QDateTime robustReadTime(QString rawDateTime) const;

		QString mDate;
		QString mTime;
	};

public:
	SNW2VolumeMetaData();

	DateTime mConversionTime;
	QString mModality;
	QString mModalityType;
	bool mIntraoperative;
	QString mName;
	DateTime mAcquisitionTime;

	struct VolumeDataType
	{
		Vector3D mSpacing;
		boost::array<int, 3> mDim;
		double mWindowCenter;
		double mWindowWidth;
		double mLLR;
		double mFirstPixel;
		double mLastPixel;

		int mBitsPerSample;
		int mSamplesPerPixel;
	};
	VolumeDataType Volume;

	struct LutDataType
	{
		int mBitsPerSample;
		int mLength;
		int mStart;
		QString mType;
	};
	LutDataType Lut;

	struct DICOMDataType
	{
		DICOMDataType() :
			m_imgMraw(Transform3D::Identity())
		{
		}
		QString mFrameOfReferenceUID;
		Transform3D m_imgMraw; // defines the transform from raw input data space to image space.
		QString mSeriesID;
		QString mSeriesDescription;
	};
	DICOMDataType DICOM;
};

typedef boost::shared_ptr<class SNW2Volume> SNW2VolumePtr;

/**\brief The Sonowand 2 file format.
 *
 * \ingroup sscData
 */
class SNW2Volume
{
public:
	static SNW2VolumePtr create(const QString& filePath);
	static SNW2VolumePtr create(const QString& filePath, const SNW2VolumeMetaData& metaData, vtkImageDataPtr imageData,
		vtkLookupTablePtr lut);
	virtual ~SNW2Volume();

	virtual vtkLookupTablePtr getLut();

	const SNW2VolumeMetaData& getMetaData() const
	{
		return mMetaData;
	}
	virtual Transform3D imgMraw() const
	{
		return mMetaData.DICOM.m_imgMraw;
	}

	virtual vtkImageDataPtr getVtkImageData();
	virtual ssc::ImagePtr getImage();

	QString uid() const
	{
		return mUid;
	}
	QString filePath() const
	{
		return mFilePath;
	}
	virtual void save() const;
	void setName(const QString& name)
	{
		mMetaData.mName = name;
	}
	virtual QString iniFileName() const;

private:
	vtkLookupTablePtr mLut;
	ssc::ImagePtr mImage;
	vtkImageDataPtr mImageData; ///< imagedata loaded directly from file/whatever, in image coordinate space
	SNW2VolumeMetaData mMetaData;
	QString mFilePath;
	QString mUid;

	SNW2Volume(const QString& filePath);
	QString rawDataFileName() const;
	QString rawLutFileName() const;

	void rawSaveMetaData(const QString& filename, const SNW2VolumeMetaData& data) const;
	void rawSaveVolumeData(const QString& filename, vtkImageDataPtr imageData) const;
	void rawSaveLutData(const QString& filename, vtkLookupTablePtr lut) const;

	bool loadAll();
	bool loadMetaData();
	bool loadVolumeData();
	boost::array<int, 3> stringList2IntArray3(QStringList raw) const;
	bool rawLoadMetaData(SNW2VolumeMetaData &data) const;
	bool rawLoadVtkImageData();
	bool rawLoadLut(const QString& filename, vtkLookupTablePtr lut) const;

	template<class T> QStringList streamable2QStringList(const T& val) const;
	bool checksumData(QString filename, const unsigned char* const data, int size);
	void writeStatus(const QString& text) const;
	bool ensureCenterWindowValid(double* windowPtr, double* levelPtr, double* llrPtr);
};

}

#endif /*SSCSNW2VOLUME_H_*/
