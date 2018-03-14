/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDicomImageReader.h"

#include "cxVolumeHelpers.h"
#include "dcvrpn.h"
#include "cxLogger.h"

namespace cx
{

DicomImageReaderPtr DicomImageReader::createFromFile(QString filename)
{
	DicomImageReaderPtr retval(new DicomImageReader);
	if (retval->loadFile(filename))
		return retval;
	else
		return DicomImageReaderPtr();
}

DicomImageReader::DicomImageReader() :
	mDataset(NULL)
{
}

bool DicomImageReader::loadFile(QString filename)
{
	mFilename = filename;
	OFCondition status = mFileFormat.loadFile(filename.toLatin1().data());
	if( !status.good() )
	{
		return false;
	}

	mDataset = mFileFormat.getDataset();
	return true;
}

ctkDICOMItemPtr DicomImageReader::item() const
{
	return this->wrapInCTK(mDataset);
}

double DicomImageReader::getDouble(const DcmTagKey& tag, const unsigned long pos, const OFBool searchIntoSub) const
{
//	return this->getDouble(mDataset, tag, pos, searchIntoSub);
	double retval = 0;
	OFCondition condition;
	condition = mDataset->findAndGetFloat64(tag, retval, pos, searchIntoSub);
	if (!condition.good())
	{
		QString tagName = this->item()->TagDescription(tag);
		this->error(QString("Failed to get tag %1/%2").arg(tagName).arg(pos));
	}
	return retval;
}
//double DicomImageReader::getDouble(DcmObject* dcmObject, const DcmTagKey& tag, const unsigned long pos, const OFBool searchIntoSub) const
//{
//	DcmStack stack;
//	dcmObject->search(tag, stack);

//	DcmElement* element = dynamic_cast<DcmElement*>(stack.top());

//		if(!element)
//		{
//			QString tagName = this->item()->TagDescription(tag);
//			this->error(QString("Failed to get DcmAttributeTag with tag %1/%2").arg(tagName).arg(pos));
//			return 0;
//		}
//		else
//		{
//			double val;
//			element->getFloat64(val);
//		}

//	double retval = 0;
//	OFCondition condition;
//	condition = element->getFloat64(retval, pos);
//	if (!condition.good())
//	{
//		QString tagName = this->item()->TagDescription(tag);
//		this->error(QString("Failed to get tag %1/%2").arg(tagName).arg(pos));
//	}
//	return retval;
//}

DicomImageReader::WindowLevel DicomImageReader::getWindowLevel() const
{
	WindowLevel retval;
	retval.center = this->getDouble(DCM_WindowCenter, 0, OFTrue);
	retval.width = this->getDouble(DCM_WindowWidth, 0, OFTrue);
	return retval;
}

bool DicomImageReader::isLocalizerImage() const
{
	//DICOM standard PS3.3 section C.7.6.1.1.2 Image Type
	//http://dicom.nema.org/medical/dicom/current/output/html/part03.html#sect_C.7.6.1.1.2
	bool retval = false;

	OFCondition condition;
	OFString value;
	condition = mDataset->findAndGetOFString(DCM_ImageType, value, 2, OFTrue);
	if (condition.good())
	{
		QString imageSpesialization(value.c_str());
		if (imageSpesialization.compare("LOCALIZER", Qt::CaseSensitive) == 0)
			retval = true;
	}
	return retval;
}

int DicomImageReader::getNumberOfFrames() const
{
	int numberOfFrames = this->item()->GetElementAsInteger(DCM_NumberOfFrames);
	if (numberOfFrames==0)
	{
		unsigned short rows = 0;
		unsigned short columns = 0;
		mDataset->findAndGetUint16(DCM_Rows, rows, 0, OFTrue);
		mDataset->findAndGetUint16(DCM_Columns, columns, 0, OFTrue);
		if (rows*columns > 0)
			numberOfFrames = 1; // seems like we have a 2D image
	}
	return numberOfFrames;
}

Transform3D DicomImageReader::getImageTransformPatient() const
{
	Vector3D pos;
	Vector3D e_x;
	Vector3D e_y;

	for (int i=0; i<3; ++i)
	{
		e_x[i] = this->getDouble(DCM_ImageOrientationPatient, i, OFTrue);
		e_y[i] = this->getDouble(DCM_ImageOrientationPatient, i+3, OFTrue);
		pos[i] = this->getDouble(DCM_ImagePositionPatient, i, OFTrue);
	}

	Vector3D zero_vec(0,0,0);
	if( similar(e_x,zero_vec) && similar(e_y,zero_vec)) // Zero matrix
	{
		report("Set transform matrix to identity");
		e_x[0]=1;
		e_y[1]=1;
	}

	Transform3D retval = cx::createTransformIJC(e_x, e_y, pos);
	return retval;
}
ctkDICOMItemPtr DicomImageReader::wrapInCTK(DcmItem* item) const
{
	if (!item)
		return ctkDICOMItemPtr();
	ctkDICOMItemPtr retval(new ctkDICOMItem);
	retval->InitializeFromItem(item);
	return retval;
}

void DicomImageReader::error(QString message) const
{
	reportError(QString("Dicom convert: [%1] in %2").arg(message).arg(mFilename));
}

vtkImageDataPtr DicomImageReader::createVtkImageData()
{
	//TODO: Use DicomImage::createMonochromeImage() to get a monochrome copy for convenience

	DicomImage dicomImage(mFilename.toLatin1().data()); //, CIF_MayDetachPixelData );
	const DiPixel *pixels = dicomImage.getInterData();
	if (!pixels)
	{
		this->error("Found no pixel data");
		return vtkImageDataPtr();
	}

	vtkImageDataPtr data = vtkImageDataPtr::New();

	data->SetSpacing(this->getSpacing().data());

	Eigen::Array3i dim = this->getDim(dicomImage);
	data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);

	int samplesPerPixel = pixels->getPlanes();
	int scalarSize = dim.prod() * samplesPerPixel;
	int pixelDepth = dicomImage.getDepth();

	switch (pixels->getRepresentation())
	{
	case EPR_Uint8:
//		std::cout << "  VTK_UNSIGNED_CHAR" << std::endl;
		data->AllocateScalars(VTK_UNSIGNED_CHAR, samplesPerPixel);
		break;
	case EPR_Uint16:
//		std::cout << "  VTK_UNSIGNED_SHORT" << std::endl;
		data->AllocateScalars(VTK_UNSIGNED_SHORT, samplesPerPixel);
		break;
	case EPR_Uint32:
//		std::cout << "  VTK_UNSIGNED_INT" << std::endl;
		data->AllocateScalars(VTK_UNSIGNED_INT, samplesPerPixel);
		break;
	case EPR_Sint8:
//		std::cout << "  VTK_CHAR" << std::endl;
		data->AllocateScalars(VTK_CHAR, samplesPerPixel);
		break;
	case EPR_Sint16:
//		std::cout << "  VTK_SHORT" << std::endl;
		data->AllocateScalars(VTK_SHORT, samplesPerPixel);
		break;
	case EPR_Sint32:
//		std::cout << "  VTK_INT" << std::endl;
		data->AllocateScalars(VTK_INT, samplesPerPixel);
		break;
	}

	int bytesPerPixel = data->GetScalarSize() * samplesPerPixel;

	memcpy(data->GetScalarPointer(), pixels->getData(), pixels->getCount()*bytesPerPixel);
	if (pixels->getCount()!=scalarSize)
		this->error("Mismatch in pixel counts");
	setDeepModified(data);
	return data;
}

Eigen::Array3d DicomImageReader::getSpacing() const
{
	Eigen::Array3d spacing;
	spacing[0] = this->getDouble(DCM_PixelSpacing, 0, OFTrue);
	spacing[1] = this->getDouble(DCM_PixelSpacing, 1, OFTrue);

	double sliceThickness = this->getDouble(DCM_SliceThickness, 0, OFTrue);
	spacing[2] = sliceThickness;

	if(this->isMultiFrameImage())
	{
		double sliceSpacing = this->getSliceSpacing();
		if(similar(sliceSpacing, 0))
			CX_LOG_WARNING() << "Cannot get slice spacing. Using slice thickness instead: " << sliceThickness;
		else
			spacing[2] = sliceSpacing;
	}

//	double spacingBetweenSlices = this->getDouble(DCM_SpacingBetweenSlices, 0, OFTrue);//Usually only for MR
//	std::cout << "DCM_SpacingBetweenSlices: " << spacingBetweenSlices << std::endl;

//	std::cout << "  spacing: " << spacing << std::endl;
	return spacing;
}

bool DicomImageReader::isMultiFrameImage() const
{
	//For now, just use number of z positions as indicator
	QVector<double> zPos = this->getZPositions();
	if(zPos.size() < 2)
		return false;
	return true;
}

double DicomImageReader::getSliceSpacing() const
{
	double retval;

	QVector<double> zPos = this->getZPositions();
	if(zPos.size() < 2)
		return 0;
	retval = zPos[1] - zPos[0];

	for(int i = 2; i < zPos.size(); ++i)
	{
		double dist = zPos[i] - zPos[i-1];
		if(!similar(dist, retval))
			CX_LOG_WARNING() << "Distance between frame: " << i << " and " << i+1 << " is: " << dist << " != " << "dist between frame 0 and 1: " << retval;
	}
	if(retval < 0)
		retval = zPos[0] - zPos[1];
	return retval;
}

QVector<double> DicomImageReader::getZPositions() const
{
	QVector<double> retval;
	DcmStack cleanStack;
	DcmElement* stackElement;
	OFCondition condition;
	int i = 0;
	do
	{
		do
			condition = mDataset->nextObject(cleanStack, OFTrue);
		while(condition.good() && cleanStack.top()->getTag() != DCM_ImagePositionPatient);

		++i;
		if(condition.good())
		{
			stackElement = dynamic_cast<DcmElement*>(cleanStack.top());
			double val;
			condition = stackElement->getFloat64(val, 2);
			if(condition.bad())
			{
				CX_LOG_WARNING() << "Cannot get z pos for frame " << i;
				return retval;
			}
			retval << val;
//			CX_LOG_DEBUG() << "frame " << i << " z pos: " << val;
		}
	}
	while(condition.good());
	return retval;
}

//Transform3D DicomImageReader::getTransform(DcmItem* dcmItem)
//{

//		e_x[i] = this->getDouble(DCM_ImageOrientationPatient, i, OFTrue);
//		e_y[i] = this->getDouble(DCM_ImageOrientationPatient, i+3, OFTrue);
//		pos[i] = this->getDouble(DCM_ImagePositionPatient, i, OFTrue);


//	condition = stackElement->findAndGetOFString(DCM_ImagePositionPatient, value, 2, OFTrue);
//}

Eigen::Array3i DicomImageReader::getDim(const DicomImage& dicomImage) const
{
	Eigen::Array3i dim;
	dim[0] = dicomImage.getWidth();
	dim[1] = dicomImage.getHeight();
	dim[2] = dicomImage.getFrameCount();
	return dim;
}

QString DicomImageReader::getPatientName() const
{
	QString rawName = this->item()->GetElementAsString(DCM_PatientName);
	return this->formatPatientName(rawName);
}

QString DicomImageReader::formatPatientName(QString rawName) const
{
	// ripped from ctkDICOMModel

	OFString dicomName = rawName.toStdString().c_str();
	OFString formattedName;
	OFString lastName, firstName, middleName, namePrefix, nameSuffix;
	OFCondition l_error = DcmPersonName::getNameComponentsFromString(dicomName,
																	 lastName, firstName, middleName, namePrefix, nameSuffix);
	if (l_error.good())
	{
		formattedName.clear();
		/* concatenate name components per this convention
   * Last, First Middle, Suffix (Prefix)
   * */
		if (!lastName.empty())
		{
			formattedName += lastName;
			if ( !(firstName.empty() && middleName.empty()) )
			{
				formattedName += ",";
			}
		}
		if (!firstName.empty())
		{
			formattedName += " ";
			formattedName += firstName;
		}
		if (!middleName.empty())
		{
			formattedName += " ";
			formattedName += middleName;
		}
		if (!nameSuffix.empty())
		{
			formattedName += ", ";
			formattedName += nameSuffix;
		}
		if (!namePrefix.empty())
		{
			formattedName += " (";
			formattedName += namePrefix;
			formattedName += ")";
		}
	}
	return QString(formattedName.c_str());
}


} // namespace cx

