/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxDefinitionStrings.h"

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, ORIENTATION_TYPE, otCOUNT)
{
	"Oblique",
	"Orthogonal"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, ORIENTATION_TYPE, otCOUNT);

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, PLANE_TYPE, ptCOUNT)
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
DEFINE_ENUM_STRING_CONVERTERS_END(cx, PLANE_TYPE, ptCOUNT)

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, FOLLOW_TYPE, ftCOUNT)
{
	"Follow tool",
	"Fixed center"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, FOLLOW_TYPE, ftCOUNT)

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, CLINICAL_VIEW, mdCOUNT)
{
	"Neurological",
	"Radiological",
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, CLINICAL_VIEW, mdCOUNT)

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, MESSAGE_LEVEL, mlCOUNT)
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
DEFINE_ENUM_STRING_CONVERTERS_END(cx, MESSAGE_LEVEL, mlCOUNT)

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, LOG_SEVERITY, msCOUNT)
{
	"ERROR",
	"WARNING",
	"INFO",
	"DEBUG"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, LOG_SEVERITY, msCOUNT)


DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, PATIENT_COORDINATE_SYSTEM, pcsCOUNT)
{
	"LPS",
	"RAS"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, PATIENT_COORDINATE_SYSTEM, pcsCOUNT)


DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, COORDINATE_SYSTEM, csCOUNT)
{
	"reference",
	"data",
	"patient reference",
	"tool",
	"sensor",
	"tool offset",
	"voxel"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, COORDINATE_SYSTEM, csCOUNT)

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, TRACKING_SYSTEM, tsCOUNT)
{
	"none",
	"polaris",
	"polaris spectra",
	"polaris vicra",
	"polaris classic",
	"aurora",
	"micron"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, TRACKING_SYSTEM, tsCOUNT)

/*DEFINE_ENUM_STRING_CONVERTERS_BEGIN(ssc, TOOL_TYPE, ttCOUNT)
{
	"none",
	"reference",
	"manual",
	"pointer",
	"usprobe"
}
DEFINE_ENUM_STRING_CONVERTERS_END(ssc, TOOL_TYPE, ttCOUNT)*/


DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, IMAGE_MODALITY, imCOUNT)
{
	"UNKNOWN",
	"CT",
	"MR",
	"US",
	"PET",
	"SC"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, IMAGE_MODALITY, imCOUNT)

DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, IMAGE_SUBTYPE, istCOUNT)
{
	"UNKNOWN",
	"",
	"T1",
	"T2",
	"FLAIR",
	"B-Mode",
	"Angio",
	"Segmentation"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, IMAGE_SUBTYPE, istCOUNT)


DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, LUNG_STRUCTURES, lsCOUNT)
{
	"UNKNOWN",
	"Lungs",
	"Tumors",
	"Lesions",
	"Lymph Nodes",
	"Vena Azygos",
	"Vena Cava",
	"Aorta",
	"Subclavian Artery",
	"Lung Vessels",
	"Heart",
	"Esophagus",
	"Spine",
	"Airways",
	"Centerlines",
	"Pulmonary Veins",
	"Pulmonary Trunk",
	"Medium Organs",
	"Small Organs",
	"Nodules"
}
DEFINE_ENUM_STRING_CONVERTERS_END(cx, LUNG_STRUCTURES, lsCOUNT)
