/*
 * cxIGTLinkConversion.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: olevs
 */

#include "cxIGTLinkConversion.h"

#include "sscProbeData.h"
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include "sscTypeConversions.h"

typedef vtkSmartPointer<vtkImageImport> vtkImageImportPtr;

namespace cx
{

namespace
{
//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  //float position[3];
  //float orientation[4];

  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
}
}

IGTLinkConversion::IGTLinkConversion()
{
	// TODO Auto-generated constructor stub

}

IGTLinkConversion::~IGTLinkConversion()
{
	// TODO Auto-generated destructor stub
}


IGTLinkImageMessage::Pointer IGTLinkConversion::encode(ssc::ImagePtr image)
{
	//	IGTLinkImageMessage::Pointer retval;
	vtkImageDataPtr rawImage = image->getBaseVtkImageData();

	static int staticCounter = 0;
	//------------------------------------------------------------
	// size parameters
	int   size[]     = {256, 256, 1};       // image dimension
	rawImage->GetDimensions(size);
	//size[2] = 1; // grab only one frame

	double spacingD[3];
	float spacingF[3];
	rawImage->GetSpacing(spacingD);
	spacingF[0] = spacingD[0];
	spacingF[1] = spacingD[1];
	spacingF[2] = spacingD[2];
	int*   svsize   = size;
	int   svoffset[] = {0, 0, 0};           // sub-volume offset
	int   scalarType = -1;

	if (rawImage->GetNumberOfScalarComponents()==4)
	{
		if (rawImage->GetScalarType()==VTK_UNSIGNED_CHAR)
		{
			scalarType = igtl::ImageMessage::TYPE_UINT32;// scalar type
		}
	}

	if (rawImage->GetNumberOfScalarComponents()==1)
	{
		if (rawImage->GetScalarType()==VTK_UNSIGNED_SHORT)
		{
			scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type
		}
		else if (rawImage->GetScalarType()==VTK_UNSIGNED_CHAR)
		{
			scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
		}
	}

	if (scalarType==-1)
	{
		std::cerr << "unknown image type" << std::endl;
		return IGTLinkImageMessage::Pointer();
	}

	//------------------------------------------------------------
	// Create a new IMAGE type message
	IGTLinkImageMessage::Pointer imgMsg = IGTLinkImageMessage::New();
	imgMsg->SetDimensions(size);
	imgMsg->SetSpacing(spacingF);
	imgMsg->SetScalarType(scalarType);
	imgMsg->SetDeviceName(cstring_cast(image->getUid()));
	imgMsg->SetSubVolume(svsize, svoffset);
	imgMsg->AllocateScalars();

	QDateTime lastGrabTime = image->getAcquisitionTime();
	igtl::TimeStamp::Pointer timestamp;
	timestamp = igtl::TimeStamp::New();
	//  double now = 1.0/1000*(double)QDateTime::currentDateTime().toMSecsSinceEpoch();
	double grabTime = 1.0 / 1000 * (double) lastGrabTime.toMSecsSinceEpoch();
	timestamp->SetTime(grabTime);
	imgMsg->SetTimeStamp(timestamp);

	//------------------------------------------------------------
	// Set image data (See GetTestImage() bellow for the details)
	//  GetTestImage(imgMsg, filedir, index);

	int fsize = imgMsg->GetImageSize();
	//    int frame = (staticCounter++) % image->GetDimensions()[2];
	//  std::cout << "emitting frame " << frame << ", image size=" << fsize << ", comp="<< image->GetNumberOfScalarComponents() << ", scalarType="<< scalarType << ", dim=("<< image->GetDimensions()[0] << ", "<< image->GetDimensions()[1] << ")" << std::endl;
	memcpy(imgMsg->GetScalarPointer(), rawImage->GetScalarPointer(0,0,0), fsize); // not sure if we need to copy

	//------------------------------------------------------------
	// Get randome orientation matrix and set it.
	igtl::Matrix4x4 matrix;
	GetRandomTestMatrix(matrix);
	imgMsg->SetMatrix(matrix);

	return imgMsg;


	//	return retval;
}

ssc::ImagePtr IGTLinkConversion::decode(IGTLinkImageMessage::Pointer message)
{
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	// Retrive the image data
	float spacing[3]; // spacing (mm/pixel)
	int svsize[3]; // sub-volume size
	int svoffset[3]; // sub-volume offset
	int scalarType; // scalar type
	int size[3]; // image dimension

	// Note: subvolumes is not supported. Implement when needed.

	scalarType = message->GetScalarType();
	message->GetDimensions(size);
	message->GetSpacing(spacing);
	message->GetSubVolume(svsize, svoffset);
//	message->GetOrigin(origin);
	QString deviceName = message->GetDeviceName();
//  std::cout << "size : " << ssc::Vector3D(size[0], size[1], size[2]) << std::endl;

	//for linear probes used in other substance than the scanner is calibrated for we want to compensate
	//for the change in sound of speed in that substance, do this by changing spacing in the images y-direction,
	//this is only valid for linear probes
//	spacing[1] *= mLinearSoundSpeedCompesation;


	imageImport->SetNumberOfScalarComponents(1);

	switch (scalarType)
	{
	case IGTLinkImageMessage::TYPE_INT8:
		std::cout << "signed char is not supported. Falling back to unsigned char." << std::endl;
		imageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case IGTLinkImageMessage::TYPE_UINT8:
		imageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case IGTLinkImageMessage::TYPE_INT16:
		imageImport->SetDataScalarTypeToShort();
		break;
	case IGTLinkImageMessage::TYPE_UINT16:
//    std::cout << "SetDataScalarTypeToUnsignedShort." << std::endl;
//		mImageImport->SetDataScalarTypeToUnsignedShort();
	imageImport->SetNumberOfScalarComponents(2);
	imageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case IGTLinkImageMessage::TYPE_INT32:
	case IGTLinkImageMessage::TYPE_UINT32:
//    std::cout << "SetDataScalarTypeTo4channel." << std::endl;
		// assume RGBA unsigned colors
		imageImport->SetNumberOfScalarComponents(4);
//    mImageImport->SetDataScalarTypeToInt();
		imageImport->SetDataScalarTypeToUnsignedChar();
//    std::cout << "32bit received" << std::endl;
		break;
	case IGTLinkImageMessage::TYPE_FLOAT32:
		imageImport->SetDataScalarTypeToFloat();
		break;
	case IGTLinkImageMessage::TYPE_FLOAT64:
		imageImport->SetDataScalarTypeToDouble();
		break;
	default:
		std::cout << "unknown type. Falling back to unsigned char." << std::endl;
		imageImport->SetDataScalarTypeToUnsignedChar();
	}

	// get timestamp from igtl second-format:
	igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
	message->GetTimeStamp(timestamp);
//  static double last = 0;
//  if (last==0)
//    last = timestamp->GetTimeStamp();
//  std::cout << "raw time" << timestamp->GetTimeStamp() << ", " << timestamp->GetTimeStamp() - last << std::endl;

	double timestampMS = timestamp->GetTimeStamp() * 1000;
//	mLastTimestamp += mTimestampCalibration;

//	mDebug_orgTime = timestamp->GetTimeStamp() * 1000; // ms
	imageImport->SetDataOrigin(0, 0, 0);
	imageImport->SetDataSpacing(spacing[0], spacing[1], spacing[2]);
	imageImport->SetWholeExtent(0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetImportVoidPointer(message->GetScalarPointer());

	imageImport->Modified();

//	// Update the parts of the probe data that must be read from the image.
//	ssc::ProbeData::ProbeImageData imageData = mSonixProbeData.getImage();
//	imageData.mSpacing = ssc::Vector3D(spacing[0], spacing[1], spacing[2]);
//	imageData.mSize = QSize(size[0], size[1]);
//	imageData.mClipRect_p = ssc::DoubleBoundingBox3D(0, imageData.mSize.width(), 0, imageData.mSize.height(), 0, 0);
//	mSonixProbeData.setImage(imageData);
////	std::cout << "Received Sonix message:\n" << streamXml2String(mSonixProbeData) << std::cout;

//	//Only do the following for the digital Ultrasonix interface
//	if (updateSonixParameters)
//	{
//		//TODO: Send all necessary parameters (origin + size) in IGTLinkUSStatusMessage and only update from there.
//		this->updateSonix();
//	}

	imageImport->GetOutput()->Update();
	ssc::ImagePtr retval(new ssc::Image(deviceName, imageImport->GetOutput()));
	retval->setAcquisitionTime(QDateTime::fromMSecsSinceEpoch(timestampMS));

	return retval;
}

IGTLinkUSStatusMessage::Pointer IGTLinkConversion::encode(ssc::ProbeData input)
{
	IGTLinkUSStatusMessage::Pointer retval = IGTLinkUSStatusMessage::New();

	retval->SetOrigin(input.getImage().mOrigin_p.data());
	// 1 = sector, 2 = linear
	retval->SetProbeType(input.getType());

	retval->SetDepthStart(input.getDepthStart());// Start of sector in mm from origin
	retval->SetDepthEnd(input.getDepthEnd());	// End of sector in mm from origin
	retval->SetWidth(input.getWidth());// Width of sector in mm for LINEAR, Width of sector in radians for SECTOR.

	//  std::cout << "origin: " << mFrameGeometry.origin[0] << " " << mFrameGeometry.origin[1] << " " << mFrameGeometry.origin[2] << std::endl;
	//  std::cout << "imageType: " << mFrameGeometry.imageType << std::endl;
	//  std::cout << "depthStart: " << mFrameGeometry.depthStart << " end: " << mFrameGeometry.depthEnd << std::endl;
	//  std::cout << "width: " << mFrameGeometry.width << std::endl;
	//  std::cout << "tilt: " << mFrameGeometry.tilt << std::endl;


	return retval;
}

//'copied' from OpenIGTLinkRTSource::updateSonixStatus()
ssc::ProbeData IGTLinkConversion::decode(IGTLinkUSStatusMessage::Pointer probeMessage, IGTLinkImageMessage::Pointer imageMessage, ssc::ProbeData base)
{
	ssc::ProbeData retval = base;

	if (probeMessage)
	{
		// Update the parts of the probe data that is read from the probe message.
		retval.setType(ssc::ProbeData::TYPE(probeMessage->GetProbeType()));
		retval.setSector(
				probeMessage->GetDepthStart(),
				probeMessage->GetDepthEnd(),
				probeMessage->GetWidth(),
				0);
		ssc::ProbeData::ProbeImageData imageData = retval.getImage();
		imageData.mOrigin_p = ssc::Vector3D(probeMessage->GetOrigin());
		retval.setImage(imageData);
	}

	if (imageMessage)
	{
		// Update the parts of the probe data that must be read from the image.

		// Retrive the image data
		float spacing[3]; // spacing (mm/pixel)
		int size[3]; // image dimension
		imageMessage->GetDimensions(size);
		imageMessage->GetSpacing(spacing);

		ssc::ProbeData::ProbeImageData imageData = retval.getImage();
		imageData.mSpacing = ssc::Vector3D(spacing[0], spacing[1], spacing[2]);
		imageData.mSize = QSize(size[0], size[1]);
		imageData.mClipRect_p = ssc::DoubleBoundingBox3D(0, imageData.mSize.width(), 0, imageData.mSize.height(), 0, 0);
		retval.setImage(imageData);
	}

	return retval;
}

////Copied from OpenIGTLinkRTSource::getValidProbe()
//ProbePtr IGTLinkConversion::getValidProbe()
//{
//	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
//	if (!tool)
//		return ProbePtr();
//	ProbePtr probe = boost::shared_dynamic_cast<Probe>(tool->getProbe());
//	if (!probe)
//	{
//		ssc::messageManager()->sendWarning("OpenIGTLinkRTSource::updateSonixStatus: Found no Probe");
//		return ProbePtr();
//	}

//	return probe;
//}

} // namespace cx
