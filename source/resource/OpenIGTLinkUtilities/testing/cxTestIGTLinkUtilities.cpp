#include "cxTestIGTLinkUtilities.h"
#include "sscImage.h"
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkConversion.h"
#include "sscVolumeHelpers.h"
#include "vtkImageData.h"

/** Create a 2D RGBA test image with some variation
  *
  */
vtkImageDataPtr createRGBATestImage()
{
	Eigen::Array3i dim(512,512,1);
	int components = 4;
	vtkImageDataPtr retval = ssc::generateVtkImageData(dim,
	                                                   ssc::Vector3D(0.5,0.6,0.7),
														255, components);
	int scalarSize = dim[0]*dim[1]*dim[2]*components;

	unsigned char* ptr = reinterpret_cast<unsigned char*>(retval->GetScalarPointer());

	for (unsigned z=0; z<dim[2]; ++z)
	{
		for (unsigned y=0; y<dim[1]; ++y)
		{
			for (unsigned x=0; x<dim[0]; ++x)
			{
				ptr[0] = 255;       // red
				ptr[1] = 0;         // green
				ptr[2] = x / 2;     // blue
				ptr[3] = 100;		// alpha
				ptr+=components;
			}
		}
	}

	return retval;
}


void TestIGTLinkUtilities::setUp()
{
//	cx::DataLocations::setTestMode();
//	cx::LogicManager::initialize();
}

void TestIGTLinkUtilities::tearDown()
{
//	cx::LogicManager::shutdown();
}

void TestIGTLinkUtilities::testConstructor()
{
}

int TestIGTLinkUtilities::getValue(ssc::ImagePtr data, int x, int y, int z)
{
	vtkImageDataPtr volume = data->getGrayScaleBaseVtkImageData();
	int val = (int)*reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
	return val;
}

Eigen::Array3i TestIGTLinkUtilities::getValue3i(ssc::ImagePtr data, int x, int y, int z)
{
	vtkImageDataPtr volume = data->getBaseVtkImageData();
	unsigned char* ptr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
	return Eigen::Array3i(ptr[0], ptr[1], ptr[2]);
}

void TestIGTLinkUtilities::setValue(vtkImageDataPtr data, int x, int y, int z, unsigned char val)
{
	*reinterpret_cast<unsigned char*>(data->GetScalarPointer(x,y,z)) = val;
}

void TestIGTLinkUtilities::testDecodeEncodeImage()
{
	typedef std::vector<std::pair<Eigen::Array3i, int> > ValVectorType;
	ValVectorType values;
	values.push_back(std::make_pair(Eigen::Array3i( 0,20,2), 0));
	values.push_back(std::make_pair(Eigen::Array3i(10,20,0), 4));
	values.push_back(std::make_pair(Eigen::Array3i(20,20,2), 6));

	QDateTime time = QDateTime::currentDateTime();
	vtkImageDataPtr rawImage = ssc::generateVtkImageData(Eigen::Array3i(100, 120, 10),
													ssc::Vector3D(0.5, 0.6, 0.7),
													0);
	for (ValVectorType::iterator iter=values.begin(); iter!=values.end(); ++iter)
	{
		Eigen::Array3i p = iter->first;
		int val = iter->second;
		this->setValue(rawImage, p[0], p[1], p[2], val);
	}

	ssc::ImagePtr input(new ssc::Image("my_uid", rawImage));
	input->setAcquisitionTime(time);

	cx::IGTLinkConversion converter;
	cx::IGTLinkImageMessage::Pointer msg = converter.encode(input);
	ssc::ImagePtr output = converter.decode(msg);

	CPPUNIT_ASSERT(output);
	CPPUNIT_ASSERT(time == output->getAcquisitionTime());
	CPPUNIT_ASSERT(input->getUid() == output->getUid());
	CPPUNIT_ASSERT(ssc::similar(Eigen::Array3i(input->getBaseVtkImageData()->GetDimensions()), Eigen::Array3i(output->getBaseVtkImageData()->GetDimensions())));
	CPPUNIT_ASSERT(ssc::similar(ssc::Vector3D(input->getBaseVtkImageData()->GetSpacing()), ssc::Vector3D(output->getBaseVtkImageData()->GetSpacing())));

	for (ValVectorType::iterator iter=values.begin(); iter!=values.end(); ++iter)
	{
		Eigen::Array3i p = iter->first;
		int val = iter->second;
		CPPUNIT_ASSERT(this->getValue(input, p[0], p[1], p[2]) == val);
		CPPUNIT_ASSERT(this->getValue(output, p[0], p[1], p[2]) == val);
	}
}

void TestIGTLinkUtilities::testDecodeEncodeColorImageRGBA()
{
	//testimage
	//	ptr[0] = 255;       // red
	//	ptr[1] = 0;         // green
	//	ptr[2] = x / 2;     // blue
	//	ptr[3] = 100;		// alpha

	Val3VectorType values;
	values.push_back(std::make_pair(Eigen::Array3i(  0,   0,  0), Eigen::Array3i(255,  0,  0)));
	values.push_back(std::make_pair(Eigen::Array3i(100,  50,  0), Eigen::Array3i(255,  0, 50)));
	values.push_back(std::make_pair(Eigen::Array3i(124,  20,  0), Eigen::Array3i(255,  0, 62)));

	this->testDecodeEncodeColorImage(values, "RGBA");
}

void TestIGTLinkUtilities::testDecodeEncodeColorImageBGR()
{
	//testimage
	//	ptr[0] = 255;       // red
	//	ptr[1] = 0;         // green
	//	ptr[2] = x / 2;     // blue
	//	ptr[3] = 100;		// alpha

	Val3VectorType values;
	values.push_back(std::make_pair(Eigen::Array3i(  0,   0,  0), Eigen::Array3i(  0,  0,255)));
	values.push_back(std::make_pair(Eigen::Array3i(100,  50,  0), Eigen::Array3i( 50,  0,255)));
	values.push_back(std::make_pair(Eigen::Array3i(124,  20,  0), Eigen::Array3i( 62,  0,255)));

	this->testDecodeEncodeColorImage(values, "BGR");
}

void TestIGTLinkUtilities::testDecodeEncodeColorImageARGB()
{
	//testimage
	//	ptr[0] = 255;       // red
	//	ptr[1] = 0;         // green
	//	ptr[2] = x / 2;     // blue
	//	ptr[3] = 100;		// alpha

	Val3VectorType values;
	values.push_back(std::make_pair(Eigen::Array3i(  0,   0,  0), Eigen::Array3i(0,  0, 100)));
	values.push_back(std::make_pair(Eigen::Array3i(100,  50,  0), Eigen::Array3i(0, 50, 100)));
	values.push_back(std::make_pair(Eigen::Array3i(124,  20,  0), Eigen::Array3i(0, 62, 100)));

	this->testDecodeEncodeColorImage(values, "ARGB");
}

void TestIGTLinkUtilities::testDecodeEncodeColorImage(Val3VectorType values, QString colorFormat)
{
	QDateTime time = QDateTime::currentDateTime();
	vtkImageDataPtr rawImage = createRGBATestImage();

	QString coreUid = "my_uid";

	ssc::ImagePtr input(new ssc::Image(QString("%1 [%2]").arg(coreUid).arg(colorFormat), rawImage));
	input->setAcquisitionTime(time);

	cx::IGTLinkConversion converter;
	cx::IGTLinkImageMessage::Pointer msg = converter.encode(input);
	ssc::ImagePtr output = converter.decode(msg);

	CPPUNIT_ASSERT(output);
	CPPUNIT_ASSERT(time == output->getAcquisitionTime());
	CPPUNIT_ASSERT(coreUid == output->getUid());
	CPPUNIT_ASSERT(ssc::similar(Eigen::Array3i(input->getBaseVtkImageData()->GetDimensions()), Eigen::Array3i(output->getBaseVtkImageData()->GetDimensions())));
	CPPUNIT_ASSERT(ssc::similar(ssc::Vector3D(input->getBaseVtkImageData()->GetSpacing()), ssc::Vector3D(output->getBaseVtkImageData()->GetSpacing())));

	for (Val3VectorType::iterator iter=values.begin(); iter!=values.end(); ++iter)
	{
		Eigen::Array3i p = iter->first;
		Eigen::Array3i val = iter->second;
//		CPPUNIT_ASSERT(ssc::similar(this->getValue3i(input, p[0], p[1], p[2]), val)); // cannot do this when changing color encoding
		CPPUNIT_ASSERT(ssc::similar(this->getValue3i(output, p[0], p[1], p[2]), val));
	}
}

void TestIGTLinkUtilities::testDecodeEncodeProbeData()
{
	// generate probe data input
	ssc::ProbeDataPtr input;
	input->setType(ssc::ProbeData::tSECTOR);
	ssc::ProbeData::ProbeImageData imageData = input->getImage();
	imageData.mOrigin_p = ssc::Vector3D(50,0,0); ///< probe origin in pixel space p. (upper-left corner origin)
	imageData.mSpacing = ssc::Vector3D(0.5, 0.6, 1.0);
	imageData.mSize = QSize(300, 200);
	imageData.mClipRect_p = ssc::DoubleBoundingBox3D (0, imageData.mSize.width(), 0, imageData.mSize.height(), 0, 0); ///< sector clipping rect, in addition to the standard sector definition. The probe sector is the intersection of the sector definition and the clip rect.
	input->setImage(imageData);
	input->setSector(10, 30, M_PI/2, 2);

	// generate an image based on the probe data. Part of the data is sent over this channel.
	vtkImageDataPtr rawImage = ssc::generateVtkImageData(Eigen::Array3i(imageData.mSize.width(), imageData.mSize.height(), 1),
													imageData.mSpacing,
													0);
	ssc::ImagePtr imageInput(new ssc::Image("my_uid", rawImage));

	// convert the data to igtlink and back
	cx::IGTLinkConversion converter;
	cx::IGTLinkUSStatusMessage::Pointer msg = converter.encode(input);
	cx::IGTLinkImageMessage::Pointer imageMessage = converter.encode(imageInput);
	ssc::ProbeDataPtr output = converter.decode(msg, imageMessage, ssc::ProbeDataPtr());

	// compare input<->output
	CPPUNIT_ASSERT(input->getType() == output->getType());
	CPPUNIT_ASSERT(input->getDepthStart() == output->getDepthStart());
	CPPUNIT_ASSERT(input->getDepthEnd() == output->getDepthEnd());
	//not supported CPPUNIT_ASSERT(input->getCenterOffset() == output->getCenterOffset());
	CPPUNIT_ASSERT(input->getWidth() == output->getWidth());
	CPPUNIT_ASSERT(ssc::similar(input->getImage().mClipRect_p, output->getImage().mClipRect_p)); // only supported for cliprect equal to entire image size.
	CPPUNIT_ASSERT(ssc::similar(input->getImage().mOrigin_p, output->getImage().mOrigin_p));
	CPPUNIT_ASSERT(ssc::similar(input->getImage().mSpacing, output->getImage().mSpacing));
	CPPUNIT_ASSERT(input->getImage().mSize.width() == output->getImage().mSize.width());
	CPPUNIT_ASSERT(input->getImage().mSize.height() == output->getImage().mSize.height());
	//not supported CPPUNIT_ASSERT(input->getTemporalCalibration() == output->getTemporalCalibration());
}
