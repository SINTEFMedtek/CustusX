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

#ifndef CXDEFINITIONSTRINGS_H_
#define CXDEFINITIONSTRINGS_H_

#include "cxDefinitions.h"
#include "cxEnumConverter.h"

// put compiler-intensive templates here instead of in the much used sscDefinitions.h

namespace cx
{
} //namespace end

SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, ORIENTATION_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, PLANE_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, FOLLOW_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, CLINICAL_VIEW);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, MESSAGE_LEVEL);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, LOG_SEVERITY);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, PATIENT_COORDINATE_SYSTEM);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, COORDINATE_SYSTEM);
SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, TRACKING_SYSTEM);
//SNW_DECLARE_ENUM_STRING_CONVERTERS(ssc, TOOL_TYPE);

#endif /*CXDEFINITIONSTRINGS_H_*/
