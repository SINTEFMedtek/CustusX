/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "vtkForwardDeclarations.h"
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include "cxMesh.h"
#include "cxXMLPolyDataMeshReader.h"
#include "cxLogicManager.h"
#include "cxMetricManager.h"
#include "cxMNIReaderWriter.h"
#include "cxDataLocations.h"
#include "cxFileManagerServiceProxy.h"
#include "cxImage.h"

namespace cxtest
{

TEST_CASE("Can read out.vtp file into a cx::mesh", "[manual]")
{
   //read the file
   QString filename = "out.vtp";
   cx::LogicManager::initialize();
   cx::XMLPolyDataMeshReader *reader =  new cx::XMLPolyDataMeshReader(cx::LogicManager::getInstance()->getPluginContext());
   REQUIRE(reader->canRead("mesh", filename));


   // ------------- VTK -------------
   //polydata
   int expected_number_of_cells_in_polydata = 96;
   int expected_max_cell_size = 3;
   vtkPolyDataPtr polydata = reader->loadVtkPolyData(filename);
   REQUIRE(polydata->GetNumberOfCells() == expected_number_of_cells_in_polydata);
   REQUIRE(polydata->GetMaxCellSize() == expected_max_cell_size);

   //scalars
   //looking for a scalar array containing the color for each cell in the polydata
   int number_of_color_components_in_rgba = 4;
   QString name_of_scalar_array_containing_cell_colors = "Scalars_";
   vtkDataArray *scalars = polydata->GetCellData()->GetScalars(); //get the array containing colors per cell
   REQUIRE(scalars->GetNumberOfTuples() == expected_number_of_cells_in_polydata); //there are as many color values as there are cells, 1 color per cell
   REQUIRE(scalars->GetNumberOfComponents() == number_of_color_components_in_rgba); //the colors are rgba format
   REQUIRE(QString(scalars->GetName()).compare(name_of_scalar_array_containing_cell_colors) == 0); //the name is what we expect

   //to get a popup with the rendererd polygon set visualize=true;
   bool visualize = true;
   if(visualize)
   {
     vtkPolyDataMapperPtr mapper = vtkPolyDataMapperPtr::New();
     mapper->SetInputData(polydata);

     vtkActorPtr actor = vtkActorPtr::New();
     actor->SetMapper(mapper);

     vtkRendererPtr renderer = vtkRendererPtr::New();
     vtkRenderWindowPtr render_window = vtkRenderWindowPtr::New();
     render_window->AddRenderer(renderer);
     vtkRenderWindowInteractorPtr interactor = vtkRenderWindowInteractorPtr::New();
     interactor->SetRenderWindow(render_window);

     renderer->AddActor(actor);

     render_window->Render();
     interactor->Start();
   }

   // ------------- CX -------------

   //mesh
   QString name = "test";
   QString expected_color_of_mesh = "#ff0000"; //red
   cx::DataPtr data_mesh = reader->read(name, filename);
   REQUIRE(data_mesh->getName() == name);
   cx::MeshPtr mesh = boost::dynamic_pointer_cast<cx::Mesh>(data_mesh);
   REQUIRE(mesh);
   REQUIRE(expected_color_of_mesh.compare(mesh->getColor().name()) == 0);

   //REQUIRE(std::string(mesh->getColorArray()) != "");
   //std::cout << "color array: " << mesh->getColorArray() << std::endl;

   delete reader;
}

TEST_CASE("Import point metrics from MNI Tag Point file", "[integration][metrics]")
{
	cx::LogicManager::initialize();
	cx::DataLocations::setTestMode();

	vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(2, 1);
	QString volumeUid = "DummyImage1";
	cx::ImagePtr dummyImage(new cx::Image(volumeUid, dummyImageData));
	cx::logicManager()->getPatientModelService()->insertData(dummyImage);

	QString dataPath = cx::DataLocations::getTestDataPath();
	QString tagFile = dataPath + "/testing/metrics_export_import/Case1-MRI-beforeUS.tag";
	std::vector<QString> uids;
	uids.push_back(volumeUid);
	uids.push_back(volumeUid);

	//to avoid the popup we need to set the volume uids before the import
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());
	std::vector<cx::FileReaderWriterServicePtr> importers = filemanager->getImportersForDataType("pointMetric");
	boost::shared_ptr<cx::MNIReaderWriter> mniImporter;
	for(int i=0; i<importers.size(); ++i)
	{
		if(importers[i]->getName() == "MNIReaderWriter")
			mniImporter = boost::dynamic_pointer_cast<cx::MNIReaderWriter>(importers[i]);
	}
	mniImporter->setVolumeUidsRelatedToPointsInMNIPointFile(uids);

	//scope here to delete the metric manager before shutting down the logic manager.
	{
		cx::MetricManager manager(cx::logicManager()->getViewService(),
								  cx::logicManager()->getPatientModelService(),
								  cx::logicManager()->getTrackingService(),
								  cx::logicManager()->getSpaceProvider(),
								  cx::logicManager()->getFileManagerService());

		//MetricFixture fixture;

		int number_of_metrics_before_import = manager.getNumberOfMetrics();
		manager.importMetricsFromMNITagFile(tagFile);

		int number_of_metrics_in_file = 4;
		int number_of_metrics_after_import = manager.getNumberOfMetrics();
		CHECK(number_of_metrics_after_import == (number_of_metrics_before_import+number_of_metrics_in_file));
	}

	cx::LogicManager::shutdown();
}

}
