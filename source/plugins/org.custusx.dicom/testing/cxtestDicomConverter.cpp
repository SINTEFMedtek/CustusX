#include <QTimer>
#include <QApplication>

#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"

#include "catch.hpp"

#include "cxDicomConverter.h"

#include "cxDataReaderWriter.h"
#include "cxImage.h"
#include "cxTypeConversions.h"
#include "vtkImageMathematics.h"
#include "vtkImageAccumulate.h"
#include "vtkImageData.h"

typedef vtkSmartPointer<vtkImageAccumulate> vtkImageAccumulatePtr;
typedef vtkSmartPointer<vtkImageMathematics> vtkImageMathematicsPtr;

void checkImagesEqual(vtkImageDataPtr input1, vtkImageDataPtr input2)
{
	REQUIRE(input1->GetDataDimension() == input2->GetDataDimension());
	REQUIRE(input1->GetScalarType() == input2->GetScalarType());
	REQUIRE(input1->GetNumberOfScalarComponents() == input2->GetNumberOfScalarComponents());
	REQUIRE(Eigen::Array3i(input1->GetDimensions()).isApprox(Eigen::Array3i(input2->GetDimensions())));
	CHECK(Eigen::Array3d(input1->GetSpacing()).isApprox(Eigen::Array3d(input2->GetSpacing())));
	CHECK(Eigen::Array3d(input1->GetOrigin()).isApprox(Eigen::Array3d(input2->GetOrigin())));
	// check spacing, dim, type, origin

	vtkImageMathematicsPtr diff = vtkImageMathematicsPtr::New();
	diff->SetOperationToSubtract();
	diff->SetInput1Data(input1);
	diff->SetInput2Data(input2);
	diff->Update();

    vtkImageAccumulatePtr histogram = vtkImageAccumulatePtr::New();
    histogram->SetInputData(0, diff->GetOutput());
    histogram->Update();

    Eigen::Array3d histogramRange = Eigen::Array3d(histogram->GetMax()) - Eigen::Array3d(histogram->GetMin());

    for (int i=0; i<input1->GetNumberOfScalarComponents(); ++i)
    {
        CHECK(histogramRange[i] <  0.01);
        CHECK(histogramRange[i] > -0.01);
    }
}

//bool imagesIsEqual(vtkImageDataPtr input1, vtkImageDataPtr input2)
//{
//	if (input1->GetDataDimension() != input2->GetDataDimension())
//		return false;
//	if (input1->GetScalarType() != input2->GetScalarType())
//		return false;
//	if (input1->GetNumberOfScalarComponents() != input2->GetNumberOfScalarComponents())
//		return false;
//	if (!Eigen::Array3i(input1->GetDimensions()).isApprox(Eigen::Array3i(input2->GetDimensions())))
//		return false;
//	if (!Eigen::Array3d(input1->GetSpacing()).isApprox(Eigen::Array3d(input2->GetSpacing())))
//		return false;
//	if (!Eigen::Array3d(input1->GetOrigin()).isApprox(Eigen::Array3d(input2->GetOrigin())))
//		return false;
//	// check spacing, dim, type, origin
//
//	vtkImageMathematicsPtr diff = vtkImageMathematicsPtr::New();
//	diff->SetOperationToSubtract();
//	diff->SetInput1Data(input1);
//	diff->SetInput2Data(input2);
//	diff->Update();
//
//    vtkImageAccumulatePtr histogram = vtkImageAccumulatePtr::New();
//    histogram->SetInputData(0, diff->GetOutput());
//    histogram->Update();
//
//    Eigen::Array3d histogramRange = Eigen::Array3d(histogram->GetMax()) - Eigen::Array3d(histogram->GetMin());
//
//    if (histogramRange.maxCoeff()>0.01)
//    	return false;
//    if (histogramRange.minCoeff()<0.01)
//    	return false;
//
//	return true;
//}

void checkImagesEqual(cx::ImagePtr input1, cx::ImagePtr input2)
{
	CHECK(input1->getModality() == input2->getModality());
	CHECK(cx::similar(input1->getInitialWindowLevel(), input2->getInitialWindowLevel()));
	CHECK(cx::similar(input1->getInitialWindowWidth(), input2->getInitialWindowWidth()));
	CHECK(cx::similar(input1->get_rMd(), input2->get_rMd()));
	checkImagesEqual(input1->getBaseVtkImageData(), input2->getBaseVtkImageData());
}
//bool imagesIsEqual(cx::ImagePtr input1, cx::ImagePtr input2)
//{
//	if (input1->getModality() != input2->getModality())
//		return false;
//	if (!cx::similar(input1->getInitialWindowLevel(), input2->getInitialWindowLevel()))
//		return false;
//	if (!cx::similar(input1->getInitialWindowWidth(), input2->getInitialWindowWidth()))
//		return false;
//	if (!cx::similar(input1->get_rMd(), input2->get_rMd()))
//		return false;
//	if (!imagesIsEqual(input1->getBaseVtkImageData(), input2->getBaseVtkImageData()))
//		return false;
//	return true;
//}

TEST_CASE("DicomConverter: Convert Kaisa", "[unit][plugins][org.custusx.dicom][hide]")
{
	// input  I: kaisa dicom data -> pass dicom data through converter
	// input II: kaisa mhd file   -> load
	//
	// verify converter output equals loaded mhd file output

	QString databaseFileName = "/Users/christiana/dev/temp/testDatabase";
	QString inputDicomDataDirectory = "/Users/christiana/dev/dicom/kaisa_korrigert/";
	QString referenceImageFilename = "/Users/christiana/Patients/data/kaisa_series5353.mhd";

	cx::DicomConverter converter;

	QSharedPointer<ctkDICOMDatabase> DICOMDatabase = QSharedPointer<ctkDICOMDatabase> (new ctkDICOMDatabase);
    DICOMDatabase->openDatabase( databaseFileName );

    QSharedPointer<ctkDICOMIndexer> DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
    DICOMIndexer->addDirectory(*DICOMDatabase,inputDicomDataDirectory,"");

    cx::ImagePtr referenceImage = cx::Image::create("reference","reference");
    cx::MetaImageReader().readInto(referenceImage, referenceImageFilename);

    std::cout << "image: " << streamXml2String(*referenceImage) << std::endl;

    checkImagesEqual(referenceImage, referenceImage);

	CHECK(true);
}

/** Run a full acquisition from MHD source local server using OpenIGTLink.
 * Save data and evaluate results.
 */
//TEST_CASE_METHOD(cxtest::AcquisitionFixture, "Acquisition: Stream one MDHSource from LocalServer and save to disk", "[integration][modules][Acquisition][not_win32]")
//{
//	this->mConnectionMethod = "Local Server";
//	this->mNumberOfExpectedStreams = 1;
//	this->initialize();
//	QTimer::singleShot(20*1000,   qApp, SLOT(quit()) );
//	qApp->exec();
//	this->verify();
//}
