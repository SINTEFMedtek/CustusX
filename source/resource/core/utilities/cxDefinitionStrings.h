/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDEFINITIONSTRINGS_H_
#define CXDEFINITIONSTRINGS_H_

#include "cxDefinitions.h"
#include "cxEnumConverter.h"

// put compiler-intensive templates here instead of in the much used sscDefinitions.h

namespace cx
{
} //namespace end

SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, ORIENTATION_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, PLANE_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, FOLLOW_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, CLINICAL_VIEW);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, MESSAGE_LEVEL);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, LOG_SEVERITY);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, PATIENT_COORDINATE_SYSTEM);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, COORDINATE_SYSTEM);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, TRACKING_SYSTEM);

SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, IMAGE_MODALITY);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, IMAGE_SUBTYPE);

SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, LUNG_STRUCTURES);

#endif /*CXDEFINITIONSTRINGS_H_*/
