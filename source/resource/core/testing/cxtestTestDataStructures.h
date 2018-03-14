/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTDATASTRUCTURES_H
#define CXTESTDATASTRUCTURES_H

#include "cxtestresource_export.h"

#include "cxImage.h"
#include "cxMesh.h"
#include "cxTrackedStream.h"

namespace cxtest
{
class CXTESTRESOURCE_EXPORT TestDataStructures
{
public:public:
	cx::ImagePtr image1;
	cx::ImagePtr image2;
	cx::MeshPtr mesh1;
	cx::TrackedStreamPtr trackedStream1;
	TestDataStructures();
};
}//cxtest
#endif // CXTESTDATASTRUCTURES_H
