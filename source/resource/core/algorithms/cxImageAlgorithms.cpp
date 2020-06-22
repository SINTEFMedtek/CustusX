/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImageAlgorithms.h"

#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>

#include <vtkImageResample.h>
#include <vtkImageClip.h>
#include <vtkImageChangeInformation.h>

#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxUtilHelpers.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxRegistrationTransform.h"

#include "cxTime.h"
#include "cxVolumeHelpers.h"

#include "cxSlicedImageProxy.h"
#include "cxSliceProxy.h"
#include "cxLogger.h"
#include "cxEnumConversion.h"


namespace cx
{

/** Return an image that is resampled into space q.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImage(PatientModelServicePtr dataManager, ImagePtr image, Transform3D qMd)
{
	//TODO: fix error:
	// There is an error in the transfer functions of the returned image from this function

	// provide a resampled volume for algorithms requiring that (such as PickerRep)
	vtkMatrix4x4Ptr orientatorMatrix = vtkMatrix4x4Ptr::New();
	vtkImageReslicePtr orientator = vtkImageReslicePtr::New();
	orientator->SetInputData(image->getBaseVtkImageData());
	orientator->SetInterpolationModeToLinear();
	orientator->SetOutputDimensionality(3);
	orientator->SetResliceAxes(qMd.inv().getVtkMatrix());
	orientator->AutoCropOutputOn();
	orientator->Update();
	vtkImageDataPtr rawResult = orientator->GetOutput();

//  rawResult->Update();

	QString uid = image->getUid() + "_or%1";
	QString name = image->getName()+" or%1";
//  ImagePtr oriented = dataManager->createDerivedImage(rawResult, uid, name, image);

	ImagePtr oriented = createDerivedImage(dataManager,
										 uid, name,
										 rawResult, image);

	oriented->get_rMd_History()->setRegistration(image->get_rMd() * qMd.inv());
	oriented->mergevtkSettingsIntosscTransform();

	return oriented;
}

/** Return an image that is resampled with a new output spacing.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImage(PatientModelServicePtr dataManager, ImagePtr image, const Vector3D spacing, QString uid, QString name)
{
	vtkImageResamplePtr resampler = vtkImageResamplePtr::New();
	resampler->SetInputData(image->getBaseVtkImageData());
	resampler->SetAxisOutputSpacing(0, spacing[0]);
	resampler->SetAxisOutputSpacing(1, spacing[1]);
	resampler->SetAxisOutputSpacing(2, spacing[2]);
	resampler->Update();
	vtkImageDataPtr rawResult = resampler->GetOutput();

	if (uid.isEmpty())
	{
		uid = image->getUid() + "_res%1";
		name = image->getName()+" res%1";
	}

	ImagePtr retval = createDerivedImage(dataManager,
										 uid, name,
										 rawResult, image);
	return retval;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr duplicateImage(PatientModelServicePtr dataManager, ImagePtr image)
{
	Vector3D spacing(image->getBaseVtkImageData()->GetSpacing());
	return resampleImage(dataManager, image, spacing, image->getUid()+"_copy%1", image->getName()+" copy%1");
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
vtkImageDataPtr cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox)
{
	vtkImageClipPtr clip = vtkImageClipPtr::New();
	clip->SetInputData(input);
	clip->SetOutputWholeExtent(cropbox.begin());
	clip->ClipDataOn();
	clip->Update();
	vtkImageDataPtr rawResult = clip->GetOutput();

//  rawResult->Update();
//  rawResult->UpdateInformation();
	rawResult->ComputeBounds();
	return rawResult;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr cropImage(PatientModelServicePtr dataManager, ImagePtr image)
{
	DoubleBoundingBox3D bb = image->getCroppingBox();
	double* sp = image->getBaseVtkImageData()->GetSpacing();
	IntBoundingBox3D cropbox(
				static_cast<int>(bb[0]/sp[0]+0.5), static_cast<int>(bb[1]/sp[0]+0.5),
				static_cast<int>(bb[2]/sp[1]+0.5), static_cast<int>(bb[3]/sp[1]+0.5),
				static_cast<int>(bb[4]/sp[2]+0.5), static_cast<int>(bb[5]/sp[2]+0.5));
	vtkImageDataPtr rawResult = cropImage(image->getBaseVtkImageData(), cropbox);

	QString uid = image->getUid() + "_crop%1";
	QString name = image->getName()+" crop%1";
	ImagePtr result = createDerivedImage(dataManager,
										 uid, name,
										 rawResult, image);
	result->mergevtkSettingsIntosscTransform();

	return result;
}

/**
 */
QDateTime extractTimestamp(QString text)
{
	// retrieve timestamp as
	QRegExp tsReg("[0-9]{8}T[0-9]{6}");
	if (tsReg.indexIn(text)>0)
	{
		QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
		return datetime;
	}
	return QDateTime();
}

//From https://www.vtk.org/Wiki/VTK/Examples/Cxx/Qt/ImageDataToQImage
QImage vtkImageDataToQImage(vtkImageDataPtr imageData, bool overlay, QColor overlayColor)
{
	if ( !imageData ) { return QImage(); }

	int width = imageData->GetDimensions()[0];
	int height = imageData->GetDimensions()[1];

	QImage image( width, height, QImage::Format_ARGB32 ); //32 bit
	QRgb *rgbPtr = reinterpret_cast<QRgb *>( image.bits() ) + width * ( height - 1 );
	unsigned char *colorsPtr = reinterpret_cast<unsigned char *>( imageData->GetScalarPointer() );

	// Loop over the vtkImageData contents.
	for ( int row = 0; row < height; row++ )
	{
		for ( int col = 0; col < width; col++ )
		{
			// Swap the vtkImageData RGB values with an equivalent QColor
			*( rgbPtr++ ) = convertToQColor(colorsPtr, overlay, overlayColor);

			colorsPtr += imageData->GetNumberOfScalarComponents();
		}

		rgbPtr -= width * 2;
	}

	return image;
}

QRgb convertToQColor(unsigned char *colorsPtr, bool overlay, QColor overlayColor)
{
	if(overlay)
		return modifyOverlayColor(colorsPtr, overlayColor);

	//32 bit
	QRgb rgb;
	rgb = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2], colorsPtr[3] ).rgba();
	//	rgb = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2], opacity ).rgba(); //Don't use alpha for now

	return rgb;
}

bool isDark(unsigned char *colorsPtr)
{
	unsigned char threshold = 10;
	if (colorsPtr[0] < threshold &&
			colorsPtr[1] < threshold &&
			colorsPtr[2] < threshold)
		return true;
	return false;
}

QRgb modifyOverlayColor(unsigned char *colorsPtr, QColor overlayColor)
{
	if(isDark(colorsPtr))
		overlayColor.setAlpha(0);

	QRgb retval = overlayColor.rgba();
	return retval;
}

vtkImageDataPtr createSlice(ImagePtr image, PLANE_TYPE planeType, Vector3D outputSpacing, Eigen::Array3i outputDimensions, ToolPtr sliceTool, PatientModelServicePtr patientModel, bool applyLUT)
{
	vtkImageDataPtr retval = vtkImageDataPtr::New();

	if(!image || !patientModel)
	{
		return retval;
	}

	cx::SliceProxyPtr proxy = cx::SliceProxy::create(patientModel);
	SlicedImageProxyPtr imageSlicer(new SlicedImageProxy);

	proxy->setTool(sliceTool);

	imageSlicer->setSliceProxy(proxy);
	imageSlicer->setImage(image);

	proxy->initializeFromPlane(planeType, false, false, 1, 0);
	proxy->setClinicalApplicationToFixedValue(mdRADIOLOGICAL);//Always create slices in radiological view

	// Using these values centers image in view, but seems to lock the manual image movement in some directions.
	double screenX = outputDimensions[0]*outputSpacing[0] / 2;
	double screenY = outputDimensions[1]*outputSpacing[1] / 2;

	imageSlicer->setOutputFormat(Vector3D(-screenX,-screenY,0), outputDimensions, outputSpacing);

	imageSlicer->update();
	if (applyLUT)
	{
		imageSlicer->getOutputPort()->Update();
		retval->DeepCopy(imageSlicer->getOutput());
	}
	else //Don't use LUT
	{
		imageSlicer->getOutputPortWithoutLUT()->Update();
		retval->DeepCopy(imageSlicer->getOutputWithoutLUT());

	}

	return retval;
}

vtkImageDataPtr createSlice(ImagePtr image, PLANE_TYPE planeType, Vector3D position_r, Vector3D target_r, double offset, bool applyLUT)
{
	vtkImageDataPtr slicedImage = vtkImageDataPtr::New();

	vtkImageReslicePtr imageReslicer = vtkImageReslicePtr::New();

	imageReslicer->SetInputData(image->getBaseVtkImageData());
	imageReslicer->SetBackgroundLevel(image->getMin());

	imageReslicer->SetInterpolationModeToLinear();
	imageReslicer->SetOutputDimensionality(2);


	Eigen::Array3d inputSpacing = image->getSpacing();

	//imageReslicer->SetOutputOrigin(image->getBaseVtkImageData()->GetOrigin()); // set to [0, 0, 0] ??
//	double origin[3];
//	image->getBaseVtkImageData()->GetOrigin(origin);

//	Eigen::Array3d spacing = image->getSpacing();
	int extent[6];
	image->getBaseVtkImageData()->GetExtent(extent);

	Transform3D rMd = image->get_rMd();

	//Eigen::Array3i dim(image->getBaseVtkImageData()->GetDimensions());

	Transform3D positionTransform_d = rMd.inv() * createTransformTranslate(position_r);
	vtkSmartPointer<vtkMatrix4x4> resliceAxes = vtkSmartPointer<vtkMatrix4x4>::New();

	Transform3D targetTransform_d;
	Vector3D direction;
	Vector3D up(0, -1, 0);
	Vector3D xAxis;
	Vector3D yAxis;


	switch (planeType)
	{
	case ptAXIAL:
		resliceAxes->SetElement(0, 0, 1);
		resliceAxes->SetElement(0, 1, 0);
		resliceAxes->SetElement(0, 2, 0);
		resliceAxes->SetElement(0, 3, 0);
		resliceAxes->SetElement(1, 0, 0);
		resliceAxes->SetElement(1, 1, -1);
		resliceAxes->SetElement(1, 2, 0);
		resliceAxes->SetElement(1, 3, 0);
		resliceAxes->SetElement(2, 0, 0);
		resliceAxes->SetElement(2, 1, 0);
		resliceAxes->SetElement(2, 2, 1);
		resliceAxes->SetElement(2, 3, positionTransform_d(2,3) + offset);
		resliceAxes->SetElement(3, 0, 0);
		resliceAxes->SetElement(3, 1, 0);
		resliceAxes->SetElement(3, 2, 0);
		resliceAxes->SetElement(3, 3, 1);
		imageReslicer->SetResliceAxes(resliceAxes);
		imageReslicer->SetOutputExtent(extent[0], extent[1], extent[2], extent[3], 0, 0);
		imageReslicer->SetOutputSpacing(inputSpacing[0], inputSpacing[1], 0);
		break;
	case ptCORONAL:
		resliceAxes->SetElement(0, 0, 1);
		resliceAxes->SetElement(0, 1, 0);
		resliceAxes->SetElement(0, 2, 0);
		resliceAxes->SetElement(0, 3, 0);
		resliceAxes->SetElement(1, 0, 0);
		resliceAxes->SetElement(1, 1, 0);
		resliceAxes->SetElement(1, 2, 1);
		resliceAxes->SetElement(1, 3, positionTransform_d(1,3) + offset);
		resliceAxes->SetElement(2, 0, 0);
		resliceAxes->SetElement(2, 1, 1);
		resliceAxes->SetElement(2, 2, 0);
		resliceAxes->SetElement(2, 3, 0);
		resliceAxes->SetElement(3, 0, 0);
		resliceAxes->SetElement(3, 1, 0);
		resliceAxes->SetElement(3, 2, 0);
		resliceAxes->SetElement(3, 3, 1);
		imageReslicer->SetResliceAxes(resliceAxes);
		imageReslicer->SetOutputExtent(extent[0], extent[1], extent[4], extent[5], 0, 0);
		imageReslicer->SetOutputSpacing(inputSpacing[0], inputSpacing[2], 0);
		break;
	case ptSAGITTAL:
		resliceAxes->SetElement(0, 0, 0);
		resliceAxes->SetElement(0, 1, 0);
		resliceAxes->SetElement(0, 2, -1);
		resliceAxes->SetElement(0, 3, positionTransform_d(0,3));
		resliceAxes->SetElement(1, 0, 1);
		resliceAxes->SetElement(1, 1, 0);
		resliceAxes->SetElement(1, 2, 0);
		resliceAxes->SetElement(1, 3, 0);
		resliceAxes->SetElement(2, 0, 0);
		resliceAxes->SetElement(2, 1, 1);
		resliceAxes->SetElement(2, 2, 0);
		resliceAxes->SetElement(2, 3, 0);
		resliceAxes->SetElement(3, 0, 0);
		resliceAxes->SetElement(3, 1, 0);
		resliceAxes->SetElement(3, 2, 0);
		resliceAxes->SetElement(3, 3, 1);
		imageReslicer->SetResliceAxes(resliceAxes);
		imageReslicer->SetOutputExtent(extent[2], extent[3], extent[4], extent[5], 0, 0);
		imageReslicer->SetOutputSpacing(inputSpacing[1], inputSpacing[2], 0);
		break;
	case ptRADIALPLANE:
		targetTransform_d = rMd.inv() * createTransformTranslate(target_r);
		direction(0) = targetTransform_d(0,3) - positionTransform_d(0,3);
		direction(1) = targetTransform_d(1,3) - positionTransform_d(1,3);
		direction(2) = targetTransform_d(2,3) - positionTransform_d(2,3);
		direction = direction.normalized();
		xAxis = up.cross(direction).normalized();
		yAxis = direction.cross(xAxis).normalized();

		resliceAxes->SetElement(0, 0, xAxis(0));
		resliceAxes->SetElement(0, 1, yAxis(0));
		resliceAxes->SetElement(0, 2, direction(0));
		resliceAxes->SetElement(0, 3, positionTransform_d(0,3) + offset*direction(0));
		resliceAxes->SetElement(1, 0, xAxis(1));
		resliceAxes->SetElement(1, 1, yAxis(1));
		resliceAxes->SetElement(1, 2, direction(1));
		resliceAxes->SetElement(1, 3, positionTransform_d(1,3) + offset*direction(1));
		resliceAxes->SetElement(2, 0, xAxis(2));
		resliceAxes->SetElement(2, 1, yAxis(2));
		resliceAxes->SetElement(2, 2, direction(2));
		resliceAxes->SetElement(2, 3, positionTransform_d(2,3) + offset*direction(2));
		resliceAxes->SetElement(3, 0, 0);
		resliceAxes->SetElement(3, 1, 0);
		resliceAxes->SetElement(3, 2, 0);
		resliceAxes->SetElement(3, 3, 1);
		imageReslicer->SetResliceAxes(resliceAxes);
		imageReslicer->SetOutputExtent(extent[0], extent[1], extent[2], extent[3], 0, 0);
		imageReslicer->SetOutputSpacing(inputSpacing[0], inputSpacing[1], 0);
		break;
	default:
		CX_LOG_WARNING() << "Not a valid plane type." << enum2string(planeType);
	}

	imageReslicer->Update();

	if (applyLUT)
	{
		ApplyLUTToImage2DProxyPtr imageWithLUTProxyPtr = ApplyLUTToImage2DProxyPtr(new ApplyLUTToImage2DProxy());
		vtkImageChangeInformationPtr redirecterPtr = vtkImageChangeInformationPtr::New();
		redirecterPtr->SetInputConnection(imageReslicer->GetOutputPort());

		imageWithLUTProxyPtr->setInput(redirecterPtr, image->getLookupTable2D()->getOutputLookupTable());

		imageWithLUTProxyPtr->getOutputPort()->Update();
		slicedImage->DeepCopy(imageWithLUTProxyPtr->getOutput());
	}
	else
			slicedImage = imageReslicer->GetOutput();

	return slicedImage;
}

std::vector<int> getSliceVoxelFrom3Dposition(ImagePtr image, PLANE_TYPE planeType, Vector3D position_r)
{
	std::vector<int> voxel(2,0);

	Transform3D rMd = image->get_rMd();
	Eigen::Array3d spacing = image->getSpacing();
	Transform3D positionTransform_d = rMd.inv() * createTransformTranslate(position_r);
	int xVoxel = std::round( positionTransform_d(0,3) / spacing(0) );
	int yVoxel = std::round( positionTransform_d(1,3) / spacing(1) );
	int zVoxel = std::round( positionTransform_d(2,3) / spacing(2) );

	switch (planeType)
	{
	case ptAXIAL:
		voxel[0] = xVoxel;
		voxel[1] = yVoxel;
		break;
	case ptCORONAL:
		voxel[0] = xVoxel;
		voxel[1] = zVoxel;
		break;
	case ptSAGITTAL:
		voxel[0] = yVoxel;
		voxel[1] = zVoxel;
		break;
	default:
		CX_LOG_WARNING() << "Not a valid plane type." << enum2string(planeType);
	}
	return voxel;
}

int getSliceNumberFrom3Dposition(ImagePtr image, PLANE_TYPE planeType, Vector3D position_r)
{
	int sliceNumber;

	Transform3D rMd = image->get_rMd();
	Eigen::Array3d spacing = image->getSpacing();
	Transform3D positionTransform_d = rMd.inv() * createTransformTranslate(position_r);

	switch (planeType)
	{
	case ptAXIAL:
		sliceNumber = std::round( positionTransform_d(2,3) / spacing(2) );
		break;
	case ptCORONAL:
		sliceNumber = std::round( positionTransform_d(1,3) / spacing(1) );
		break;
	case ptSAGITTAL:
		sliceNumber = std::round( positionTransform_d(0,3) / spacing(0) );
		break;
	default:
		CX_LOG_WARNING() << "Not a valid plane type." << enum2string(planeType);
	}
	return sliceNumber;
}

Vector3D get3DpositionFromSliceVoxel(ImagePtr image, PLANE_TYPE planeType, std::vector<int> voxel, int sliceNumber)
{
	Vector3D position_r;

	Transform3D rMd = image->get_rMd();
	Eigen::Array3d spacing = image->getSpacing();
	Vector3D position_d;

	switch (planeType)
	{
	case ptAXIAL:
		position_d(0) = voxel[0]*spacing(0);
		position_d(1) = voxel[1]*spacing(1);
		position_d(2) = sliceNumber*spacing(2);
		break;
	case ptCORONAL:
		position_d(0) = voxel[0]*spacing(0);
		position_d(1) = sliceNumber*spacing(1);
		position_d(2) = voxel[1]*spacing(2);
		break;
	case ptSAGITTAL:
		position_d(0) = sliceNumber*spacing(0);
		position_d(1) = voxel[0]*spacing(1);
		position_d(2) = voxel[1]*spacing(2);
		break;
	default:
		CX_LOG_WARNING() << "Not a valid plane type." << enum2string(planeType);
		return position_r;
	}

	Transform3D positionTransform_r = rMd * createTransformTranslate(position_d);
	position_r(0) = positionTransform_r(0,3);
	position_r(1) = positionTransform_r(1,3);
	position_r(2) = positionTransform_r(2,3);

	return position_r;
}

} // namespace cx
