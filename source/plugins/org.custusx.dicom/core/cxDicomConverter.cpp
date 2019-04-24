/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDicomConverter.h"
#include "ctkDICOMDatabase.h"
#include "cxTypeConversions.h"
#include "cxTime.h"
#include "vtkImageData.h"
#include "cxRegistrationTransform.h"
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include "cxReporter.h"

#include "cxLogger.h"
#include "ctkDICOMItem.h"
#include "dcfilefo.h" // DcmFileFormat
#include "dcdeftag.h" // defines all dcm tags
#include "dcmimage.h"
#include <string.h>

#include "cxDicomImageReader.h"
#include "cxCustomMetaImage.h"

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

QString DicomConverter::generateUid(DicomImageReaderPtr reader)
{
	QString seriesDescription = reader->item()->GetElementAsString(DCM_SeriesDescription);
	QString seriesNumber = reader->item()->GetElementAsString(DCM_SeriesNumber);

	// uid: uid _ <timestamp>
	// name: find something from series
	QString currentTimestamp = QDateTime::currentDateTime().toString(timestampSecondsFormat());
	QString uid = QString("%1_%2_%3").arg(seriesDescription).arg(seriesNumber).arg(currentTimestamp);
	uid = this->convertToValidName(uid);
	return uid;
}

QString DicomConverter::convertToValidName(QString text) const
{
	QStringList illegal;
	illegal << "\\s" << "\\." << ":" << ";" << "\\<" << "\\>" << "\\*"
			<< "\\^" << "," << "\\%";
	QRegExp regexp(QString("(%1)").arg(illegal.join("|")));
	text = text.replace(regexp, "_");
	return text	;
}


QString DicomConverter::generateName(DicomImageReaderPtr reader)
{
	QString seriesDescription = reader->item()->GetElementAsString(DCM_SeriesDescription);
	QString name = convertToValidName(seriesDescription);
	return name;
}

ImagePtr DicomConverter::createCxImageFromDicomFile(QString filename, bool ignoreLocalizerImages)
{
	DicomImageReaderPtr reader = DicomImageReader::createFromFile(filename);
	if (!reader)
	{
		reportWarning(QString("File not found: %1").arg(filename));
		return ImagePtr();
	}

	if(ignoreLocalizerImages && reader->isLocalizerImage())
	{
		reportWarning(QString("Localizer image removed from series: %1").arg(filename));
		return ImagePtr();
	}

	if (reader->getNumberOfFrames()==0)
	{
		reportWarning(QString("Found no images in %1, skipping.").arg(filename));
		return ImagePtr();
	}

	QString uid = this->generateUid(reader);
	QString name = this->generateName(reader);
	cx::ImagePtr image = cx::Image::create(uid, name);

	vtkImageDataPtr imageData = reader->createVtkImageData();
	if (!imageData)
	{
		reportWarning(QString("Failed to create image for %1.").arg(filename));
		return ImagePtr();
	}
	image->setVtkImageData(imageData);

	QString modalityString = reader->item()->GetElementAsString(DCM_Modality);
	image->setModality(convertToModality(modalityString));

	image->setImageType(istEMPTY);//Setting image subtype to empty for now. DCM_ImageType (value 3, and 4) may possibly be used. Also series name often got this kind of information.

	DicomImageReader::WindowLevel windowLevel = reader->getWindowLevel();
	image->setInitialWindowLevel(windowLevel.width, windowLevel.center);

	Transform3D M = reader->getImageTransformPatient();
	image->get_rMd_History()->setRegistration(M);

//	reportDebug(QString("Image created from %1").arg(filename));
	return image;
}

std::vector<ImagePtr> DicomConverter::createImages(QStringList files)
{
	std::vector<ImagePtr> retval;
	for (int i=0; i<files.size(); ++i)
	{
		bool ignoreSpesialImages = true;
		ImagePtr image = this->createCxImageFromDicomFile(files[i], ignoreSpesialImages);
		if (image)
			retval.push_back(image);
	}
	return retval;
}

std::map<double, ImagePtr> DicomConverter::sortImagesAlongDirection(std::vector<ImagePtr> images, Vector3D  e_sort)
{
	std::map<double, ImagePtr> sorted;
	for (unsigned i=0; i<images.size(); ++i)
	{
		Vector3D pos = images[i]->get_rMd().coord(Vector3D(0,0,0));
		double dist = dot(pos, e_sort);

		sorted[dist] = images[i];
	}
	return sorted;
}

bool DicomConverter::slicesFormRegularGrid(std::map<double, ImagePtr> sorted, Vector3D e_sort) const
{
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
			distances.push_back(dist);

			Vector3D tilt = cross(p1-p0, e_sort);
			double sliceGantryTiltTolerance = 0.001;
			if (!similar(tilt.length(), 0.0, sliceGantryTiltTolerance))
			{
				reportError(QString("Dicom convert: found gantry tilt: %1, cannot create image.").arg(tilt.length()));
				return false;
			}
		}

		if (distances.size()>=2)
		{
			double d0 = distances[distances.size()-2];
			double d1 = distances[distances.size()-1];
			double sliceSpacingTolerance = 0.01;
			if (!similar(d0, d1, sliceSpacingTolerance))
			{
				reportError(QString("Dicom convert: found uneven slice spacing: %1 != %2, cannot create image.").arg(d0).arg(d1));
				return false;
			}
		}
	}

	return true;
}

double DicomConverter::getMeanSliceDistance(std::map<double, ImagePtr> sorted) const
{
	if (sorted.size()==0)
		return 0;

	// check for multislice image
	vtkImageDataPtr first = sorted.begin()->second->getBaseVtkImageData();
	if (first->GetDimensions()[2]>1)
		return first->GetSpacing()[2];

	if (sorted.size()<2)
		return 0;

	// use average of all slices
	double zValueLastImage = sorted.rbegin()->first;
	double zValueFirstImage = sorted.begin()->first;
	unsigned long numHolesBetweenImages = sorted.size() - 1;
	return (zValueLastImage-zValueFirstImage)/numHolesBetweenImages;
}

ImagePtr DicomConverter::mergeSlices(std::map<double, ImagePtr> sorted) const
{
	vtkImageAppendPtr appender = vtkImageAppendPtr::New();
	appender->SetAppendAxis(2);

	ImagePtr retval = sorted.begin()->second;

	int i = 0;

	for (std::map<double, ImagePtr>::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
	{
		ImagePtr current = iter->second;

		// Set window width and level to the values of the middle frame
		if (i == sorted.size() / 2)
			retval->setInitialWindowLevel(current->getInitialWindowWidth(), current->getInitialWindowLevel());
		++i;

		//Convert all slices to same format
		vtkImageCastPtr imageCast = vtkImageCastPtr::New();
		imageCast->SetInputData(current->getBaseVtkImageData());
		imageCast->SetOutputScalarTypeToShort();
		imageCast->Update();

		appender->AddInputData(imageCast->GetOutput());
	}
	appender->Update();

	vtkImageDataPtr wholeImage = appender->GetOutput();
	Eigen::Array3d spacing(wholeImage->GetSpacing());
	spacing[2] = this->getMeanSliceDistance(sorted);
	wholeImage->SetSpacing(spacing.data());

	retval->setVtkImageData(wholeImage);

	return retval;
}

ImagePtr DicomConverter::convertToImage(QString series)
{
	QStringList files = mDatabase->filesForSeries(series);

	std::vector<ImagePtr> images = this->createImages(files);

	if (images.empty())
		return ImagePtr();

	if (images.size()==1)
	{
		return images.front();
	}

	Vector3D e_sort = images.front()->get_rMd().vector(Vector3D(0,0,1));

	std::map<double, ImagePtr> sorted = this->sortImagesAlongDirection(images, e_sort);

	if (!this->slicesFormRegularGrid(sorted, e_sort))
		return ImagePtr();

	ImagePtr retval = this->mergeSlices(sorted);
	return retval;
}

} /* namespace cx */
