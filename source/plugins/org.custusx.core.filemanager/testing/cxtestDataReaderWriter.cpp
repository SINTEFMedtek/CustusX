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

}
