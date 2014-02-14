/*
 * sscDefinitions.cpp
 *
 *  Created on: May 11, 2010
 *      Author: christiana
 */

#include "sscDefinitionStrings.h"

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
	"Radial"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, PLANE_TYPE, ptCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, FOLLOW_TYPE, ftCOUNT)
{
	"Follow tool",
	"Fixed center"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, FOLLOW_TYPE, ftCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, CLINICAL_APPLICATION, mdCOUNT)
{
	"Laboratory",
	"Neurology",
	"Laparoscopy",
	"Endovascular",
	"Bronchoscopy"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, CLINICAL_APPLICATION, mdCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, MESSAGE_LEVEL, mlCOUNT)
{
	"INFO",
	"WARNING",
	"ERROR",
	"DEBUG",
	"COUT",
	"CERR",
	"SUCCESS",
	"STATUS"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, MESSAGE_LEVEL, mlCOUNT)

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
