/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxDefinitionStrings.h"

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, ORIENTATION_TYPE, otCOUNT)
{
	"Oblique",
	"Orthogonal"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, ORIENTATION_TYPE, otCOUNT);

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, PLANE_TYPE, ptCOUNT)
{
	"NoPlane",
	"Sagittal",
	"Coronal",
	"Axial",
	"Any",
	"Dual",
    "Radial",
    "ToolSide"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, PLANE_TYPE, ptCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, FOLLOW_TYPE, ftCOUNT)
{
	"Follow tool",
	"Fixed center"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, FOLLOW_TYPE, ftCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, CLINICAL_VIEW, mdCOUNT)
{
	"Neurological",
	"Radiological",
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, CLINICAL_VIEW, mdCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, MESSAGE_LEVEL, mlCOUNT)
{
	"INFO",
	"WARNING",
	"ERROR",
	"DEBUG",
	"COUT",
	"CERR",
	"SUCCESS",
	"STATUS",
	"RAW"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, MESSAGE_LEVEL, mlCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, LOG_SEVERITY, msCOUNT)
{
	"ERROR",
	"WARNING",
	"INFO",
	"DEBUG"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, LOG_SEVERITY, msCOUNT)


SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, PATIENT_COORDINATE_SYSTEM, pcsCOUNT)
{
	"LPS",
	"RAS"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, PATIENT_COORDINATE_SYSTEM, pcsCOUNT)


SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, COORDINATE_SYSTEM, csCOUNT)
{
	"reference",
	"data",
	"patient reference",
	"tool",
	"sensor",
	"tool offset",
	"voxel"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, COORDINATE_SYSTEM, csCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, TRACKING_SYSTEM, tsCOUNT)
{
	"none",
	"polaris",
	"polaris spectra",
	"polaris vicra",
	"polaris classic",
	"aurora",
	"micron"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, TRACKING_SYSTEM, tsCOUNT)

/*SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(ssc, TOOL_TYPE, ttCOUNT)
{
	"none",
	"reference",
	"manual",
	"pointer",
	"usprobe"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(ssc, TOOL_TYPE, ttCOUNT)*/
