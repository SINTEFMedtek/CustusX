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
