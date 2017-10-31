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

#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "cxTransform3D.h"
#include "cxBranchList.h"

namespace cxtest
{

/**
 * This function makes a vtkPolyDataPtr to a dummy centerline looking like a fork: -<
 *
 */
vtkPolyDataPtr makeDummyCenterLine()
{
    vtkSmartPointer<vtkPoints> points =
        vtkSmartPointer<vtkPoints>::New();

    //branch 1
    int n1 = 100;
    int n2 = 100;
    int n3 = 100;
    int i = 0;
    double stepsize = 0.05;
    double jaggedsize = 0.001;
    while (i < n1)
    {
        double x = i * stepsize;
        double y = jaggedsize*std::pow(-1, i+1);
        double z = 0;
        points->InsertPoint(i, x, y, z);
        ++i;
    }

    //branch2
    while (i < n1 + n2)
    {
        double x = i * stepsize;
        double y = (i - n1 + 1)*stepsize + jaggedsize*std::pow(-1, i+1);
        double z = 0;
        points->InsertPoint(i, x, y, z);
        ++i;
    }

    //branch3
    while (i < n1 + n2 + n3)
    {
        double x = (i-n2)* stepsize;
        double y = (i - (n1+n2) + 1)*(-stepsize) + jaggedsize*std::pow(-1, i+1);
        double z = 0;
        points->InsertPoint(i, x, y, z);
        ++i;
    }


    // Create a cell array to store the lines in and add the lines to it
    vtkSmartPointer<vtkCellArray> lines =
        vtkSmartPointer<vtkCellArray>::New();

    //Create lines
    for (unsigned int i = 0; i < points->GetNumberOfPoints()-n3-1; ++i)
    {
        vtkSmartPointer<vtkLine> line =
            vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, i);
        line->GetPointIds()->SetId(1, i + 1);
        lines->InsertNextCell(line);
    }

    i = n1-1;

    //Create first forked line
    vtkSmartPointer<vtkLine> line =
        vtkSmartPointer<vtkLine>::New();
    line->GetPointIds()->SetId(0, i);
    line->GetPointIds()->SetId(1, i + n2 +1);
    lines->InsertNextCell(line);

    //create rest of fork
    for (unsigned int i = points->GetNumberOfPoints() - n3; i < points->GetNumberOfPoints()-1; ++i)
    {
        vtkSmartPointer<vtkLine> line =
            vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, i);
        line->GetPointIds()->SetId(1, i + 1);
        lines->InsertNextCell(line);
    }

    CHECK(lines->GetNumberOfCells() == n1+n2+n3 -1);

    // Create a polydata to store everything in
    vtkSmartPointer<vtkPolyData> linesPolyData =
        vtkSmartPointer<vtkPolyData>::New();

    // Add the points to the dataset
    linesPolyData->SetPoints(points);

    // Add the lines to the dataset
    linesPolyData->SetLines(lines);

    return linesPolyData;
}



TEST_CASE("Test the number of branches in the dummy centerline", "[unit][bronchoscopy]")
{
    vtkPolyDataPtr linesPolyData = makeDummyCenterLine();

    cx::Transform3D rMd;
    rMd.setIdentity();
    int N = linesPolyData->GetNumberOfPoints();
    Eigen::MatrixXd CLpoints(3,N);
    for(vtkIdType i = 0; i < N; i++)
        {
        double p[3];
        linesPolyData->GetPoint(i,p);
        Eigen::Vector3d position;
        position(0) = p[0]; position(1) = p[1]; position(2) = p[2];
        CLpoints.block(0 , i , 3 , 1) = rMd.coord(position);
        //std::cout << "************ P: " << p[0] << " " << p[1] << " " << p[2] << " ************* " << std::endl;
        }
    //std::cout << "***** CLpoints: " << CLpoints << " *******" << std::endl;
    cx::BranchListPtr bl = cx::BranchListPtr(new cx::BranchList());
    bl->findBranchesInCenterline(CLpoints);

    CHECK(bl->getBranches().size() == 3);
}


} //namespace cxtest
