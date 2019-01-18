/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <QTimer>
#include <QApplication>

#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"

#include "catch.hpp"

#include "cxDicomConverter.h"
#include "cxReporter.h"
#include "cxImage.h"
#include "cxTypeConversions.h"
#include "vtkImageMathematics.h"
#include "vtkImageAccumulate.h"
#include "vtkImageData.h"
#include "cxDataLocations.h"
#include "cxVolumeHelpers.h"
#include "org_custusx_dicom_Export.h"
#include "cxDicomWidget.h"
#include "cxLogicManager.h"
#include "cxFileManagerServiceProxy.h"

typedef vtkSmartPointer<vtkImageAccumulate> vtkImageAccumulatePtr;
typedef vtkSmartPointer<vtkImageMathematics> vtkImageMathematicsPtr;
typedef vtkSmartPointer<vtkMetaImageWriter> vtkMetaImageWriterPtr;
typedef QSharedPointer<ctkDICOMDatabase> ctkDICOMDatabasePtr;

namespace cxtest
{

class DicomConverterTestFixture
{
public:
	DicomConverterTestFixture()
	{
		cx::DataLocations::setTestMode();
	}

	void checkImagesEqual(vtkImageDataPtr input1, vtkImageDataPtr input2)
	{
		REQUIRE(input1.Get()!=(vtkImageData*)NULL);
		REQUIRE(input2.Get()!=(vtkImageData*)NULL);
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

	cx::ImagePtr loadImageFromFile(cx::FileManagerServicePtr filemanager, QString filename, QString uid)
	{
		cx::ImagePtr image = cx::Image::create(uid,uid);
		filemanager->readInto(image, filename);
		//cx::DataReaderWriter().readInto(image, filename);
		return image;
	}

	QString getDatabaseFileName() const
	{
		return cx::DataLocations::getTestDataPath()+"/temp/testDatabase";
	}

	void eraseDatabase()
	{
		QString databaseFileName = this->getDatabaseFileName();
		QFile(databaseFileName).remove();
	}

	ctkDICOMDatabasePtr openDatabase()
	{
		QString databaseFileName = this->getDatabaseFileName();

		QString dbPath = QFileInfo(databaseFileName).absolutePath();
		QDir(dbPath).mkpath(".");

		ctkDICOMDatabasePtr DICOMDatabase;
		DICOMDatabase = ctkDICOMDatabasePtr(new ctkDICOMDatabase);
		DICOMDatabase->openDatabase( databaseFileName );

		return DICOMDatabase;
	}

	ctkDICOMDatabasePtr loadDirectory(QString folder)
	{
		this->eraseDatabase();
		ctkDICOMDatabasePtr DICOMDatabase = this->openDatabase();
		QSharedPointer<ctkDICOMIndexer> DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
		DICOMIndexer->addDirectory(*DICOMDatabase,folder,"");

		return DICOMDatabase;
	}

	QString getOneFromList(QStringList strings)
	{
		REQUIRE(strings.size()==1);
		return strings.front();
	}
};

typedef boost::shared_ptr<class DicomWidgetFixture> DicomWidgetFixturePtr;
class DicomWidgetFixture : public cx::DicomWidget
{
public:
	DicomWidgetFixture(ctkPluginContext* context) :
		cx::DicomWidget(context, NULL)
	{
		this->init();
	}

	void init()
	{
		this->prePaintEvent();
	}

	ctkDICOMDatabase* getDb() const
	{
		return this->getDatabase();
	}

};

TEST_CASE("DicomConverter: Fixture test", "[unit][plugins][org.custusx.dicom]")
{
	DicomConverterTestFixture fixture;

	cx::ImagePtr flatImage = cx::Image::create("uid1", "name1");
	vtkImageDataPtr flatVtkImage = cx::generateVtkImageDataSignedShort(Eigen::Array3i(30,30,20), cx::Vector3D(1,1,1), 1);
	flatImage->setVtkImageData(flatVtkImage);

	cx::ImagePtr zeroImage = cx::Image::create("uid1", "name1");
	vtkImageDataPtr zeroVtkImage = cx::generateVtkImageDataSignedShort(Eigen::Array3i(30,30,20), cx::Vector3D(1,1,1), 0);
	zeroImage->setVtkImageData(zeroVtkImage);

	fixture.checkImagesEqual(flatImage, flatImage);
//	fixture.checkImagesEqual(flatImage, flatImage);
//	fixture.checkImagesEqual(referenceImage, referenceImage);
	CHECK(true);
}

TEST_CASE("DicomConverter: Open database", "[unit][plugins][org.custusx.dicom]")
{
	cx::Reporter::initialize();

	DicomConverterTestFixture fixture;
	fixture.eraseDatabase();
	ctkDICOMDatabasePtr db = fixture.openDatabase();

	CHECK(db->isOpen());

	cx::Reporter::shutdown();
}

TEST_CASE("DicomConverter: Convert Kaisa", "[integration][plugins][org.custusx.dicom]")
{
	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(context);

	cx::Reporter::initialize();
	bool verbose = true;
	DicomConverterTestFixture fixture;
	// input  I: kaisa dicom data -> pass dicom data through converter
	// input II: kaisa mhd file   -> load
	//
	// verify converter output equals loaded mhd file output

	QString inputDicomDataDirectory = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/DICOM/";
	QString referenceImageFilename = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/MetaImage/Kaisa.mhd";

	ctkDICOMDatabasePtr db = fixture.loadDirectory(inputDicomDataDirectory);

	QString patient = fixture.getOneFromList(db->patients());
	QString study = fixture.getOneFromList(db->studiesForPatient(patient));
	QString series = fixture.getOneFromList(db->seriesForStudy(study));
	QStringList files = db->filesForSeries(series);

	if (verbose)
	{
		std::cout << "patient " << patient << std::endl;
		std::cout << "study " << study << std::endl;
		std::cout << "series " << series << std::endl;
		std::cout << "files " << files.join("\n") << std::endl;
	}

	cx::DicomConverter converter;
	converter.setDicomDatabase(db.data());
	cx::ImagePtr convertedImage = converter.convertToImage(series);

	cx::ImagePtr referenceImage = fixture.loadImageFromFile(filemanager, referenceImageFilename, "reference");
	referenceImage->setModality(cx::imSC); // hack: "SC" is not supported by mhd, it is instead set to "OTHER"

	if (verbose)
	{
		std::cout << "image: " << streamXml2String(*referenceImage) << std::endl;
		referenceImage->getBaseVtkImageData()->Print(std::cout);
		if (convertedImage)
		{
			std::cout << "converted: " << streamXml2String(*convertedImage) << std::endl;
			convertedImage->getBaseVtkImageData()->Print(std::cout);
			filemanager->save(convertedImage, cx::DataLocations::getTestDataPath()+"/temp/kaisa_series5353_out.mhd");
		}
	}

	fixture.checkImagesEqual(referenceImage, referenceImage); //
	fixture.checkImagesEqual(convertedImage, referenceImage);

	cx::LogicManager::shutdown();
}

TEST_CASE("DicomConverter: Convert DICOM dataset from Radiology department - verify .mhd file is written",
          "[integration][plugins][org.custusx.dicom]")
{
    cx::Reporter::initialize();

	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(context);

    bool verbose = true;
    DicomConverterTestFixture fixture;

    QString inputDicomDataDirectory = cx::DataLocations::getTestDataPath()+"/testing/Dicom_series_import";

    ctkDICOMDatabasePtr db = fixture.loadDirectory(inputDicomDataDirectory);

    QString patient = fixture.getOneFromList(db->patients());
    QString study = fixture.getOneFromList(db->studiesForPatient(patient));
    QString series = fixture.getOneFromList(db->seriesForStudy(study));
    QStringList files = db->filesForSeries(series);


    cx::DicomConverter converter;
    converter.setDicomDatabase(db.data());
    cx::ImagePtr convertedImage = converter.convertToImage(series);
    QString fileName = cx::DataLocations::getTestDataPath()+"/temp/"+
            convertedImage->getName() + ".mhd";
    if(verbose)
    {
        std::cout << "Filename : " << fileName << std::endl;
    }
    // If file already exists - delete it
    QFile imageFile(fileName);
    if(imageFile.exists())
        imageFile.remove();
    REQUIRE(!QFileInfo::exists(fileName));

	//cx::MetaImageReader().saveImage(convertedImage, fileName);
	filemanager->save(convertedImage, fileName);
    // Verify that image file has been written
    REQUIRE(QFileInfo::exists(fileName));

		cx::LogicManager::shutdown();
}

#ifdef CX_CUSTUS_SINTEF
TEST_CASE("DicomConverter: Convert P5 and get correct z spacing", "[integration][plugins][org.custusx.dicom]")
{
	cx::Reporter::initialize();

	bool verbose = true;
	DicomConverterTestFixture fixture;

	QString inputDicomDataDirectory = cx::DataLocations::getLargeTestDataPath()+"/testing/Person5/";

	{
		INFO(inputDicomDataDirectory);
		REQUIRE(QDir(inputDicomDataDirectory).exists());
	}

	ctkDICOMDatabasePtr db = fixture.loadDirectory(inputDicomDataDirectory);

	QString patient = fixture.getOneFromList(db->patients());
	QString study = fixture.getOneFromList(db->studiesForPatient(patient));
	QString series = "1.3.46.670589.11.5710.5.0.8052.2008052116071954458"; //s3DI/MC6 (210 images)
	QStringList files = db->filesForSeries(series);

	if (verbose)
	{
		std::cout << "patient " << patient << std::endl;
		std::cout << "study " << study << std::endl;
		std::cout << "series " << series << std::endl;
		std::cout << "files " << files.join("\n") << std::endl;
	}

	cx::DicomConverter converter;
	converter.setDicomDatabase(db.data());
	cx::ImagePtr convertedImage = converter.convertToImage(series);

	REQUIRE(convertedImage);
	{
		INFO(convertedImage->getSpacing()[2]);
		INFO(" == 0.5");
		REQUIRE(cx::similar(convertedImage->getSpacing()[2], 0.5, 0.001));
	}
}

TEST_CASE("DicomConverter: US data from SW, missing position data", "[integration][plugins][org.custusx.dicom]")
{
	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(context);

    //Transform matrix should be identity and not zero
    DicomConverterTestFixture fixture;

    QString inputDicomDataDirectory = cx::DataLocations::getLargeTestDataPath()+"/testing/SW_US_dicom_import/";
    QString referenceImageFilename = cx::DataLocations::getLargeTestDataPath()+"/testing/SW_US_dicom_import/US__Tissue__2__10_06_1_20151013T095633.mhd";

    ctkDICOMDatabasePtr db = fixture.loadDirectory(inputDicomDataDirectory);
    QString patient = fixture.getOneFromList(db->patients());
    QString study = fixture.getOneFromList(db->studiesForPatient(patient));
    QString series = fixture.getOneFromList(db->seriesForStudy(study));
    QStringList files = db->filesForSeries(series);
    cx::DicomConverter converter;
    converter.setDicomDatabase(db.data());

    cx::ImagePtr convertedImage = converter.convertToImage(series);
		cx::ImagePtr referenceImage = fixture.loadImageFromFile(filemanager, referenceImageFilename, "reference");

    fixture.checkImagesEqual(referenceImage, referenceImage); //
    fixture.checkImagesEqual(convertedImage, referenceImage);

		cx::LogicManager::shutdown();
}
#endif

TEST_CASE("DicomConverter: Auto delete database", "[integration][plugins][org.custusx.dicom]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();

	DicomWidgetFixture* widgetFixture = new DicomWidgetFixture(context);

	//load DICOM
	QString inputDicomDataDirectory = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/DICOM/";
	ctkDICOMDatabase* database = widgetFixture->getDb();
	QSharedPointer<ctkDICOMIndexer> DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
	DICOMIndexer->addDirectory(*database,inputDicomDataDirectory,"");

	QStringList patients = database->patients();
	REQUIRE(patients.size() == 1);

	//delete widget and create new
	delete widgetFixture;
	widgetFixture = new DicomWidgetFixture(context);
	widgetFixture->init();

	database = widgetFixture->getDb();
	patients = database->patients();
	REQUIRE(patients.empty());

	cx::LogicManager::shutdown();
	delete widgetFixture;
}

}//cxtest

