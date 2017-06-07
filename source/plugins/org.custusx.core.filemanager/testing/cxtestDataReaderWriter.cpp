/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

namespace cxtest
{

TEST_CASE("Can read out.vtp file into a cx::mesh", "[manual]")
{
   //read the file
   QString filename = "out.vtp";
   cx::XMLPolyDataMeshReader *reader =  new cx::XMLPolyDataMeshReader();
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

}
