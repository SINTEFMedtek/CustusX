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
#include "cxDataLocations.h"
#include "org_custusx_dicom_Export.h"

typedef vtkSmartPointer<vtkImageAccumulate> vtkImageAccumulatePtr;
typedef vtkSmartPointer<vtkImageMathematics> vtkImageMathematicsPtr;
typedef vtkSmartPointer<vtkMetaImageWriter> vtkMetaImageWriterPtr;
typedef QSharedPointer<ctkDICOMDatabase> ctkDICOMDatabasePtr;
class DicomConverterTestFixture
{
public:
	void checkImagesEqual(vtkImageDataPtr input1, vtkImageDataPtr input2)
	{
		REQUIRE(input1.Get()!=NULL);
		REQUIRE(input2.Get()!=NULL);
		REQUIRE(input1->GetDataDimension() == input2->GetDataDimension());
		REQUIRE(input1->GetScalarType() == input2->GetScalarType());
		REQUIRE(input1->GetNumberOfScalarComponents() == input2->GetNumberOfScalarComponents());
		REQUIRE(Eigen::Array3i(input1->GetDimensions()).isApprox(Eigen::Array3i(input2->GetDimensions())));
		CHECK(Eigen::Array3d(input1->GetSpacing()).isApprox(Eigen::Array3d(input2->GetSpacing()), 1.0E-2));
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

	void checkImagesEqual(cx::ImagePtr input1, cx::ImagePtr input2)
	{
		REQUIRE(input1);
		REQUIRE(input2);
		CHECK(input1->getModality() == input2->getModality());
		CHECK(cx::similar(input1->getInitialWindowLevel(), input2->getInitialWindowLevel()));
		CHECK(cx::similar(input1->getInitialWindowWidth(), input2->getInitialWindowWidth()));
		CHECK(cx::similar(input1->get_rMd(), input2->get_rMd(), 1.0E-2));
		checkImagesEqual(input1->getBaseVtkImageData(), input2->getBaseVtkImageData());
	}

	cx::ImagePtr loadImageFromFile(QString filename, QString uid)
	{
		cx::ImagePtr image = cx::Image::create(uid,uid);
		cx::MetaImageReader().readInto(image, filename);
		return image;
	}

	ctkDICOMDatabasePtr loadDirectory(QString folder)
	{
		QString databaseFileName = cx::DataLocations::getTestDataPath()+"/temp/testDatabase";
//		QString databaseFileName = "/Users/christiana/dev/temp/testDatabase";
		QFile(databaseFileName).remove();

		ctkDICOMDatabasePtr DICOMDatabase;
		DICOMDatabase = ctkDICOMDatabasePtr(new ctkDICOMDatabase);
		DICOMDatabase->openDatabase( databaseFileName );

		QSharedPointer<ctkDICOMIndexer> DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
		DICOMIndexer->addDirectory(*DICOMDatabase,folder,"");

		return DICOMDatabase;
	}

	QString getOneFromList(QStringList strings)
	{
		REQUIRE(strings.size()==1);
		return strings.front();
	}

//	QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
//	QSharedPointer<ctkDICOMDatabase> database() { return DICOMDatabase; }
};

TEST_CASE("DicomConverter: Convert Kaisa", "[unit][plugins][org.custusx.dicom][hide]")
{
	DicomConverterTestFixture fixture;
	// input  I: kaisa dicom data -> pass dicom data through converter
	// input II: kaisa mhd file   -> load
	//
	// verify converter output equals loaded mhd file output

//	QString databaseFileName = "/Users/christiana/dev/temp/testDatabase";
//	QString inputDicomDataDirectory = "/Users/christiana/dev/dicom/kaisa_korrigert/";
//	QString referenceImageFilename = "/Users/christiana/Patients/data/kaisa_series5353.mhd";
	QString inputDicomDataDirectory = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/DICOM/";
	QString referenceImageFilename = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/MetaImage/Kaisa.mhd";

	ctkDICOMDatabasePtr db = fixture.loadDirectory(inputDicomDataDirectory);

	QString patient = fixture.getOneFromList(db->patients());
	QString study = fixture.getOneFromList(db->studiesForPatient(patient));
	QString series = fixture.getOneFromList(db->seriesForStudy(study));
	QStringList files = db->filesForSeries(series);

	std::cout << "patient " << patient << std::endl;
	std::cout << "study " << study << std::endl;
	std::cout << "series " << series << std::endl;
	std::cout << "files " << files.join("\n") << std::endl;

	cx::DicomConverter converter;
	converter.setDicomDatabase(db.data());
	cx::ImagePtr convertedImage = converter.convertToImage(series);

	cx::ImagePtr referenceImage = fixture.loadImageFromFile(referenceImageFilename, "reference");
	referenceImage->setModality("SC"); // hack: "SC" is not supported by mhd, it is instead set to "OTHER"

	std::cout << "image: " << streamXml2String(*referenceImage) << std::endl;
	std::cout << "converted: " << streamXml2String(*convertedImage) << std::endl;
	referenceImage->getBaseVtkImageData()->Print(std::cout);

	fixture.checkImagesEqual(referenceImage, referenceImage);
	fixture.checkImagesEqual(convertedImage, referenceImage);

//	// write to disk
	cx::MetaImageReader().saveImage(convertedImage, "/Users/christiana/Patients/data/kaisa_series5353_out.mhd");
//	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
//	writer->SetInputData(convertedImage->getBaseVtkImageData());
//	writer->SetFileName("/Users/christiana/Patients/data/kaisa_series5353_out.mhd");
//	writer->SetCompression(false);
//	writer->Write();

	CHECK(true);
}

