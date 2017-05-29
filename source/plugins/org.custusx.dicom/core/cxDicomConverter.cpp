/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	uid = this->convertToValidFilename(uid);
	return uid;
}

QString DicomConverter::convertToValidFilename(QString text) const
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
    QStringList illegal;
    illegal << "\\s" << "\\." << ":" << ";" << "\\<" << "\\>" << "\\*"
            << "\\^" << "," << "\\%";
    QRegExp regexp(QString("(%1)").arg(illegal.join("|")));
    seriesDescription = seriesDescription.replace(regexp,"_");
    return seriesDescription;
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

	QString modality = reader->item()->GetElementAsString(DCM_Modality);
	image->setModality(modality);

	DicomImageReader::WindowLevel windowLevel = reader->getWindowLevel();
	image->setInitialWindowLevel(windowLevel.width, windowLevel.center);

	Transform3D M = reader->getImageTransformPatient();
	image->get_rMd_History()->setRegistration(M);

	reportDebug(QString("Image created from %1").arg(filename));
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
	for (int i=0; i<images.size(); ++i)
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
	double p1 = sorted.rbegin()->first;
	double p0 = sorted.begin()->first;
	return (p1-p0)/sorted.size();
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
