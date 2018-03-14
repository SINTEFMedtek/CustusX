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
#include "cxDataReaderWriter.h"

namespace cxtest
{

TEST_CASE("Can read out.vtp file into a cx::mesh", "[manual]")
{
   //read the file
   QString filename = "out.vtp";
   cx::XMLPolyDataMeshReader *reader =  new cx::XMLPolyDataMeshReader();
   REQUIRE(reader->canLoad("mesh", filename));


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
   cx::DataPtr data_mesh = reader->load(name, filename);
   REQUIRE(data_mesh->getName() == name);
   cx::MeshPtr mesh = boost::dynamic_pointer_cast<cx::Mesh>(data_mesh);
   REQUIRE(mesh);
   REQUIRE(expected_color_of_mesh.compare(mesh->getColor().name()) == 0);

   //REQUIRE(std::string(mesh->getColorArray()) != "");
   std::cout << "color array: " << mesh->getColorArray() << std::endl;

   delete reader;
}

}
