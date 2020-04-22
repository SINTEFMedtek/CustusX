/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

/*
#include "vtkForwardDeclarations.h"
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include "cxXMLPolyDataMeshReader.h"
*/

#include "cxMNIReaderWriter.h"
#include "cxDataLocations.h"
#include "cxFileManagerServiceProxy.h"
#include "cxImage.h"
#include "cxtestVisServices.h"
#include "cxLogger.h"
#include "cxPointMetric.h"

#include <ctkDICOMDatabase.h>
#include <ctkDICOMIndexer.h>
#include <QDir>
#include "cxDicomConverter.h"
#include "cxLogicManager.h"

typedef QSharedPointer<ctkDICOMDatabase> ctkDICOMDatabasePtr;

namespace cxtest
{

TEST_CASE("Import MNI Tag Point file", "[integration][metrics][mni]")
{
    QString dataPath = cx::DataLocations::getTestDataPath();
    QString tagFileWithoutLabels = dataPath + "/testing/metrics_export_import/metric_tags_without_labels.tag";
    QString tagFileWithLabels = dataPath + "/testing/metrics_export_import/metric_tags_with_labels.tag";

    REQUIRE(QFile::exists(tagFileWithoutLabels));
    REQUIRE(QFile::exists(tagFileWithLabels));

    TestVisServicesPtr services = TestVisServices::create();
    cx::MNIReaderWriter mni(services->patient(), services->view());

    std::vector<cx::DataPtr> data_with_labels = mni.read(tagFileWithLabels);
    REQUIRE(data_with_labels.size() == 6);
		for(unsigned i=0; i<data_with_labels.size(); ++i)
    {
				REQUIRE(data_with_labels[i]->getType() == cx::PointMetric::getTypeName());
        REQUIRE(data_with_labels[i]->getName().toInt() == 0);
    }

    std::vector<cx::DataPtr> data_without_labels = mni.read(tagFileWithoutLabels);
    REQUIRE(data_without_labels.size() == 6);
		for(unsigned i=0; i<data_without_labels.size(); ++i)
    {
				REQUIRE(data_without_labels[i]->getType() == cx::PointMetric::getTypeName());
        REQUIRE(data_without_labels[i]->getName().toInt() != 0);
    }
}

// Copy some code from test "DicomConverter: Convert Kaisa",
// and combine this with code from DICOMReader::importSeries()
TEST_CASE("Import Kaisa from DICOM", "[integration]")
{
	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(context);

	QString inputDicomDataDirectory = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/DICOM/";
	QString inputDicomDataFile = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/DICOM/SW7/1";

	cx::ImagePtr convertedImage;
	cx::DicomConverter converter;

	ctkDICOMDatabasePtr database = ctkDICOMDatabasePtr(new ctkDICOMDatabase);
	database->openDatabase(":memory:");
	converter.setDicomDatabase(database.data());

	QString dicomInput = inputDicomDataDirectory; // Need directory, not one of the files.
	CX_LOG_DEBUG() << "dicomInput: " << dicomInput;

	QSharedPointer<ctkDICOMIndexer> DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
	DICOMIndexer->addDirectory(*database, dicomInput,"");

	QStringList patients = database->patients();
	REQUIRE(patients.size() == 1);
	QString patient = patients[0];
	REQUIRE(!patient.isEmpty());
	CX_LOG_DEBUG() << "num patients: " << patients.size() << " patient: " << patient;

	QStringList studies = database->studiesForPatient(patient);
	REQUIRE(studies.size() == 1);
	QString study = studies[0];
	REQUIRE(!study.isEmpty());
	CX_LOG_DEBUG() << "num studies: " << studies.size() << " study: " << study;

	QStringList series = database->seriesForStudy(study);
	REQUIRE(series.size() == 1);
	QString serie = series[0];
	REQUIRE(!serie.isEmpty());
	CX_LOG_DEBUG() << "num series: " << series.size() << " serie: " << serie;

	QStringList files = database->filesForSeries(serie);
	CHECK(files.size() > 0);

	convertedImage = converter.convertToImage(serie);
	REQUIRE(convertedImage);

	cx::LogicManager::shutdown();
}
} //cxtest
