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


#include "vtkForwardDeclarations.h"

#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "cxTransform3D.h"
//#include "cxBranchList.h"

namespace cxtest
{

/**
 * @brief makeDummyCenterLine
 * This function makes a vtkPolyDataPtr to a dummy centerline looking like a fork: -<,
 * with three branches: - and / and \
 * The branches has a zig-zag pattern. The number of points in each branch is not including
 * a common branch point. - / \ with two points in each branch will thus be connected by
 * two additional lines, making it 5 cells in the vtkpolydata objec.
 * @param nfork1
 * Number of points in -
 * @param nfork2
 * Number of points in /
 * @param nfork3
 * Number of points in \
 * @param x0
 * X coordinate of start of -
 * @return
 */
vtkPolyDataPtr makeDummyCenterLine(int nfork1 = 100, int nfork2 = 100, int nfork3 = 100, int x0 = 0)
{
    vtkSmartPointer<vtkPoints> points =
        vtkSmartPointer<vtkPoints>::New();

    //branch 1
    int n1 = nfork1;
    int n2 = nfork2;
    int n3 = nfork3;
    int i = 0;
    double stepsize = 0.05;
    double jaggedsize = 0.001;
    bool jagged = true;
    while (i < n1)
    {
        double x = x0 + i * stepsize;
        double y = 0;
        if(jagged)
            y += jaggedsize*std::pow(-1, i+1);
        double z = 0;
        points->InsertPoint(i, x, y, z);
        ++i;
    }

    //branch2
    while (i < n1 + n2)
    {
        double x = x0 + i * stepsize;
        double y = (i - n1 + 1)*stepsize;
        if(jagged)
            y += jaggedsize*std::pow(-1, i+1);
        double z = 0;
        points->InsertPoint(i, x, y, z);
        ++i;
    }

    //branch3
    while (i < n1 + n2 + n3)
    {
        double x = x0 + (i-n2)* stepsize;
        double y = (i - (n1+n2) + 1)*(-stepsize);
        if(jagged)
            y += jaggedsize*std::pow(-1, i+1);
        double z = 0;
        points->InsertPoint(i, x, y, z);
        ++i;
    }


    // Create a cell array to store the lines in and add the lines to it
    vtkSmartPointer<vtkCellArray> lines =
        vtkSmartPointer<vtkCellArray>::New();

	//Create lines. For - and / it is easy
    for (unsigned int i = 0; i < points->GetNumberOfPoints()-n3-1; ++i)
    {
        vtkSmartPointer<vtkLine> line =
            vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, i);
        line->GetPointIds()->SetId(1, i + 1);
        lines->InsertNextCell(line);
    }

	//For \ you must attach it close to the fork point.
    if(n3 > 0)
    {
        i = n1-1;

		//Create first line of the fork between - and \.
        vtkSmartPointer<vtkLine> line =
                vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, i);
        line->GetPointIds()->SetId(1, i + n2 +1);
        lines->InsertNextCell(line);

		//create rest of \.
        for (unsigned int i = points->GetNumberOfPoints() - n3; i < points->GetNumberOfPoints()-1; ++i)
        {
            vtkSmartPointer<vtkLine> line =
                    vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, i);
            line->GetPointIds()->SetId(1, i + 1);
            lines->InsertNextCell(line);
        }
    }

    // Create a polydata to store everything in
    vtkSmartPointer<vtkPolyData> linesPolyData =
        vtkSmartPointer<vtkPolyData>::New();

    // Add the points to the dataset
    linesPolyData->SetPoints(points);

    // Add the lines to the dataset
    linesPolyData->SetLines(lines);

    return linesPolyData;
}
}
