/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMESHHELPERS_H_
#define CXMESHHELPERS_H_

#include "cxResourceExport.h"

#include <vtkPolyData.h>

#include "cxTool.h"

/**
 * MeshHelpers
 *
 * \date Dec 16, 2010
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */

namespace cx
{

/**
 * \addtogroup cx_resource_core_utilities
 * \{
 */

cxResource_EXPORT vtkPolyDataPtr polydataFromTransforms(TimedTransformMap transformMap_prMt, Transform3D rMpr);
cxResource_EXPORT void loadMeshFromToolTransforms(PatientModelServicePtr dataManager, TimedTransformMap transforms_prMt);

/**
 * Get information about a ssc mesh.
 */
cxResource_EXPORT std::map<std::string, std::string> getDisplayFriendlyInfo(MeshPtr mesh);

/**
 * \}
 */

}//namespace cx

#endif //CXMESHHELPERS_H_

