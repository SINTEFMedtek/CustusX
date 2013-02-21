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

#include "cxIGTLinkConversion.h"

#include "sscProbeData.h"
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include "sscTypeConversions.h"
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>

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
//	std::cout << "img dim " << size[0] << size[1] << size[2] << std::endl;
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

//	imageImport->GetOutput()->Update();
	vtkImageDataPtr imageRGB = this->createFilterFormat2RGB("RGBA", imageImport->GetOutput());
	imageRGB->Update();

	ssc::ImagePtr retval(new ssc::Image(deviceName, imageRGB));
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



//void OpenIGTLinkRTSource::updateImage(IGTLinkImageMessage::Pointer message)
//{
//	static CyclicActionTimer timer("Update Video Image");
//	timer.begin();
//#if 1 // remove to use test image
//	if (!message)
//	{
//		std::cout << "got empty image !!!" << std::endl;
//		this->setEmptyImage();
//		return;
//	}

//	this->updateImageImportFromIGTMessage(message);
//	mImageImport->GetOutput()->Update();
//#endif

//	mTimeout = false;
//	mTimeoutTimer->start();

//	// this seems to add 3ms per update()
//	// insert a ARGB->RBGA filter. TODO: need to check the input more thoroughly here, this applies only to the internal CustusX US pipeline.
//	if (mImageImport->GetOutput()->GetNumberOfScalarComponents() == 4 && !mFilter_IGTLink_to_RGB)
//	{
//		// the cx sonix server sends BGRX
//		if (QString(message->GetDeviceName()) == "ImageSenderSonix")
//		{
//			mFilter_IGTLink_to_RGB = this->createFilterBGR2RGB(mImageImport->GetOutput());
//		}
//		// the cx mac QT grabber server sends ARGB,
//		// the cx opencv server also sends ARGB, in order to mimic the mac server.
//		else if (QString(message->GetDeviceName()) == "cxOpenCVGrabber" || QString(message->GetDeviceName()) == "GrabberServer")
//		{
//			mFilter_IGTLink_to_RGB = this->createFilterARGB2RGB(mImageImport->GetOutput());
//		}
//		else // default: strip alpha channel (should not happen, but cx expects RGB or Gray, not alpha)
//		{
//			mFilter_IGTLink_to_RGB = this->createFilterRGBA2RGB(mImageImport->GetOutput());
//		}

//		if (mFilter_IGTLink_to_RGB)
//			mRedirecter->SetInput(mFilter_IGTLink_to_RGB);
//	}
//	timer.time("convert");

//	//	std::cout << "emit newframe:\t" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toStdString() << std::endl;
//	emit newFrame();
//	timer.time("emit");

//	if (timer.intervalPassed())
//	{
//		static int counter=0;
////		if (++counter%10==0)
////			ssc::messageManager()->sendDebug(timer.dumpStatisticsSmall());
//		timer.reset();
//	}

//}



vtkImageDataPtr IGTLinkConversion::createFilterFormat2RGB(QString format, vtkImageDataPtr input)
{
	int R = format.indexOf('R', 0, Qt::CaseInsensitive);
	int G = format.indexOf('G', 0, Qt::CaseInsensitive);
	int B = format.indexOf('B', 0, Qt::CaseInsensitive);
	if (R<0 || G<0 || B<0 || format.size()>4)
	{
		R = 0;
		G = 1;
		B = 2;
	}

	return this->createFilterAny2RGB(R, G, B, input);
}

vtkImageDataPtr IGTLinkConversion::createFilterAny2RGB(int R, int G, int B, vtkImageDataPtr input)
{
	input->Update();
	if (input->GetNumberOfScalarComponents() == 1)
		return input;

	vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();
	vtkImageExtractComponentsPtr splitterRGB = vtkImageExtractComponentsPtr::New();
	splitterRGB->SetInput(input);
	splitterRGB->SetComponents(R, G, B);
	merger->SetInput(0, splitterRGB->GetOutput());
	return merger->GetOutput();
}

} // namespace cx
