/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
