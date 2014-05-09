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

#include "cxDicomConverter.h"
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

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

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


typedef boost::shared_ptr<class DicomItem> DicomItemPtr;
//TODO inherit from ctkDicomItem instead.
class DicomItem
{
public:
	static DicomItemPtr fromFile(QString filename)
	{
		DicomItemPtr retval(new DicomItem);
		if (retval->loadFile(filename))
			return retval;
		else
			return DicomItemPtr();
	}

	Transform3D getImageTransformPatient() const
	{
		Vector3D imagePositionPatient;
		Vector3D imageOrientationPatientX;
		Vector3D imageOrientationPatientY;
		for(int i=0; i<3; ++i)
		{
			imagePositionPatient[i] = mDicomItem.GetElementAsDouble(DCM_ImagePositionPatient, i);
			imageOrientationPatientX[i] = mDicomItem.GetElementAsDouble(DCM_ImageOrientationPatient, i);
			imageOrientationPatientY[i] = mDicomItem.GetElementAsDouble(DCM_ImageOrientationPatient, i+3);
		}

		Transform3D retval = cx::createTransformIJC(imageOrientationPatientX,
													imageOrientationPatientY,
													imagePositionPatient);
		return retval;
	}

	vtkImageDataPtr createVtkImageData()
	{
		DicomImage dicomImage(mFilename.toLatin1().data()); //, CIF_MayDetachPixelData );
		const DiPixel *pixels = dicomImage.getInterData();

		vtkImageDataPtr data = vtkImageDataPtr::New();

		Eigen::Array3d spacing;
		spacing[0] = mDicomItem.GetElementAsDouble(DCM_PixelSpacing, 0);
		spacing[1] = mDicomItem.GetElementAsDouble(DCM_PixelSpacing, 1);
		spacing[2] = mDicomItem.GetElementAsDouble(DCM_PixelSpacing, 2);
		data->SetSpacing(spacing.data());

		Eigen::Array3d dim;
		dim[0] = dicomImage.getWidth();
		dim[1] = dicomImage.getHeight();
		dim[2] = dicomImage.getFrameCount();
		data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);

		int samplesPerPixel = mDicomItem.GetElementAsUnsignedShort(DCM_SamplesPerPixel);
		int scalarSize = dim.prod() * samplesPerPixel;
		int pixelDepth = dicomImage.getDepth();

		switch (pixels->getRepresentation())
		{
		case EPR_Uint8:
			data->AllocateScalars(VTK_UNSIGNED_CHAR, samplesPerPixel);
			break;
		case EPR_Uint16:
			data->AllocateScalars(VTK_UNSIGNED_SHORT, samplesPerPixel);
			break;
		case EPR_Uint32:
			std::cout << "DICOM EPR_Uint32 not supported" << std::endl;
			return vtkImageDataPtr();
			break;
		case EPR_Sint8:
			data->AllocateScalars(VTK_CHAR, samplesPerPixel);
			break;
		case EPR_Sint16:
			std::cout << "converting signed short" << std::endl;
			data->AllocateScalars(VTK_SHORT, samplesPerPixel);
			break;
		case EPR_Sint32:
			std::cout << "DICOM EPR_Sint32 not supported" << std::endl;
			return vtkImageDataPtr();
			break;
		}

		int bytesPerPixel = pixelDepth/8 * samplesPerPixel;
		std::cout << "bytesPerPixel: " << bytesPerPixel << std::endl;
		memcpy(data->GetScalarPointer(), pixels->getData(), pixels->getCount()*bytesPerPixel);
		if (pixels->getCount()!=scalarSize)
		{
			std::cout << "error: mismatch in pixel counts" << std::endl;
		}

		return data;
	}


	DcmFileFormat mFileFormat;
	DcmDataset *mDataset;
	ctkDICOMItem mDicomItem;
	QString mFilename;

private:
	DicomItem() : mDataset(NULL) {}
	bool loadFile(QString filename)
	{
		mFilename = filename;
		OFCondition status = mFileFormat.loadFile(filename.toLatin1().data());
		if( !status.good() )
		  {
			return false;
		  }

		mDataset = mFileFormat.getDataset();
		mDicomItem.InitializeFromItem(mDataset);
		return true;
	}
};

ImagePtr DicomConverter::createCxImageFromDicomFile(QString filename)
{

	ctkDICOMDatabase* db = mDatabase;
//	QStringList files = db->filesForSeries(series);
//ctkDICOMObjectModel
//	QString file = files.front();
//	std::cout << "file: " << file << std::endl;

	DicomItemPtr file0 = DicomItem::fromFile(filename);
	QString seriesDescription = file0->mDicomItem.GetElementAsString(DCM_SeriesDescription);
	QString seriesNumber = file0->mDicomItem.GetElementAsString(DCM_SeriesNumber);
//	std::cout << "DCM_SeriesDescription: " <<  file0->mDicomItem.GetElementAsString(DCM_SeriesDescription) << std::endl;

	// uid: uid _ <timestamp>
	// name: find something from series
	QString currentTimestamp = timestampSecondsFormat();
	QString uid = QString("%1_%2_%3").arg(seriesDescription).arg(seriesNumber).arg(currentTimestamp);
	QString name = QString("%1").arg(seriesDescription);
	cx::ImagePtr image = cx::Image::create(uid, name);

	QString modality = file0->mDicomItem.GetElementAsString(DCM_Modality);
	image->setModality(modality);
//	std::cout << "Mod: " << modality << std::endl;

	double windowCenter = file0->mDicomItem.GetElementAsDouble(DCM_WindowCenter);
	double windowWidth = file0->mDicomItem.GetElementAsDouble(DCM_WindowWidth);
//	std::cout << "Win: " << windowCenter << ", " << windowWidth << std::endl;
	image->setInitialWindowLevel(windowWidth, windowCenter);

	Transform3D M = file0->getImageTransformPatient();
//	std::cout << "M\n" << M << std::endl;
	image->get_rMd_History()->setRegistration(M);

	vtkImageDataPtr imageData = file0->createVtkImageData();
	image->setVtkImageData(imageData);

//imageData->Print(std::cout);
	return image;
}


ImagePtr DicomConverter::convertToImage(QString series)
{

	ctkDICOMDatabase* db = mDatabase;
	QStringList files = db->filesForSeries(series);
//ctkDICOMObjectModel
//	QString file = files.front();
//	std::cout << "file: " << file << std::endl;
	ImagePtr startImage = this->createCxImageFromDicomFile(files.front());

	Vector3D e_sort = startImage->get_rMd().vector(Vector3D(0,0,1));

	std::map<double, ImagePtr> sorted;
	for (int i=0; i<files.size(); ++i)
	{
		ImagePtr image = this->createCxImageFromDicomFile(files[i]);
		Vector3D pos = image->get_rMd().coord(Vector3D(0,0,0));
		double dist = dot(pos, e_sort);

		sorted[dist] = image;
	}

	vtkImageAppendPtr appender = vtkImageAppendPtr::New();
	appender->SetAppendAxis(2);

	std::vector<Vector3D> positions;
	std::vector<double> distances;
	for (std::map<double, ImagePtr>::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
	{
		ImagePtr current = iter->second;

		Vector3D pos = current->get_rMd().coord(Vector3D(0,0,0));
		positions.push_back(pos);

		if (positions.size()>=2)
		{
			Vector3D p0 = positions[positions.size()-2];
			Vector3D p1 = positions[positions.size()-1];
			double dist = dot(p1-p0, e_sort);
			Vector3D tilt = cross(p1-p0, e_sort);
			if (!similar(tilt.length(), 0.0))
			{
				std::cout << "Dicom convert: found gantry tilt, rejecting series.";
				return ImagePtr();
			}
//			std::cout << "distance: " << dist << std::endl;
			distances.push_back(dist);
		}

		if (distances.size()>=2)
		{
			double d0 = distances[distances.size()-2];
			double d1 = distances[distances.size()-1];
			if (!similar(d0, d1))
			{
				std::cout << "Dicom convert: found uneven slice spacing, rejecting series.";
				return ImagePtr();
			}
		}

		appender->AddInputData(current->getBaseVtkImageData());
	}
	appender->Update();

	vtkImageDataPtr wholeImage = appender->GetOutput();
	Eigen::Array3d spacing(wholeImage->GetSpacing());
	spacing[2] = distances.front();
	wholeImage->SetSpacing(spacing.data());

	ImagePtr retval = sorted.begin()->second;
	retval->setVtkImageData(wholeImage);

	wholeImage->Print(std::cout);
	return retval;


//	DicomItemPtr file0 = DicomItem::fromFile(file);
//	QString seriesDescription = file0->mDicomItem.GetElementAsString(DCM_SeriesDescription);
//	QString seriesNumber = file0->mDicomItem.GetElementAsString(DCM_SeriesNumber);
//	std::cout << "DCM_SeriesDescription: " <<  file0->mDicomItem.GetElementAsString(DCM_SeriesDescription) << std::endl;

//	// uid: uid _ <timestamp>
//	// name: find something from series
//	QString currentTimestamp = timestampSecondsFormat();
//	QString uid = QString("%1_%2_%3").arg(seriesDescription).arg(seriesNumber).arg(currentTimestamp);
//	QString name = QString("%1").arg(seriesDescription);
//	cx::ImagePtr image = cx::Image::create(uid, name);

//	QString modality = file0->mDicomItem.GetElementAsString(DCM_Modality);
//	image->setModality(modality);
//	std::cout << "Mod: " << modality << std::endl;

//	double windowCenter = file0->mDicomItem.GetElementAsDouble(DCM_WindowCenter);
//	double windowWidth = file0->mDicomItem.GetElementAsDouble(DCM_WindowWidth);
//	std::cout << "Win: " << windowCenter << ", " << windowWidth << std::endl;

//	Transform3D M = file0->getImageTransformPatient();
//	std::cout << "M\n" << M << std::endl;
//	image->get_rMd_History()->setRegistration(M);

////	int rows = file0->mDicomItem.GetElementAsUnsignedShort(DCM_Rows);
////	int columns = file0->mDicomItem.GetElementAsUnsignedShort(DCM_Columns);
////	Eigen::Array2d spacing;
////	spacing[0] = file0->mDicomItem.GetElementAsDouble(DCM_PixelSpacing, 0);
////	spacing[1] = file0->mDicomItem.GetElementAsDouble(DCM_PixelSpacing, 1);
////	int samplesPerPixel = file0->mDicomItem.GetElementAsUnsignedShort(DCM_SamplesPerPixel);
////	int bitsAllocated = file0->mDicomItem.GetElementAsUnsignedShort(DCM_BitsAllocated);
////	int pixelRepresentation = file0->mDicomItem.GetElementAsUnsignedShort(DCM_PixelRepresentation);

//	vtkImageDataPtr imageData = file0->createVtkImageData();
//	image->setVtkImageData(imageData);

//imageData->Print(std::cout);


//	//	DcmElement* element(NULL);
////	file0->mDataset->findAndGetElement(DCM_PixelData, element);
////    DcmDecimalString* ds = dynamic_cast<DcmDecimalString*>(element);


////	Vector3D imagePositionPatient;
////	for(int i=0; i<3; ++i)
////		imagePositionPatient[i] = file0->mDicomItem.GetElementAsDouble(DCM_ImagePositionPatient, i);

////	QString seriesDescription = db->fileValue(file, "SeriesDescription");
////	std::cout << "seriesDescription: " << seriesDescription << std::endl;
////	QString seriesDescription2 = db->fileValue(file, 0x0008, 0x103e);
////	std::cout << "seriesDescription2: " << seriesDescription2 << std::endl;
////	std::cout << "tag2: [" << db->groupElementToTag(0x0008, 0x103e) << "]" << std::endl;

////	QString pixd = db->fileValue(file, 0x7fe0, 0x0010);
////	std::cout << "pixd: " << pixd.size() << ": " << pixd << std::endl;
////		"SeriesDescription"

////	DcmFileFormat fileFormat;

////	OFCondition status = fileFormat.loadFile( file.toLatin1().data());
////	if( !status.good() )
////	  {
////		return ImagePtr();
////	  // TODO: Through an error message.
////	  }

////	DcmDataset *dataset = fileFormat.getDataset();
////	ctkDICOMItem dicomItem;
////	dicomItem.InitializeFromItem(dataset);


//	return image;
}

} /* namespace cx */
