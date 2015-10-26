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

#ifndef CXCOORDINATESYSTEMHELPERS_H_
#define CXCOORDINATESYSTEMHELPERS_H_

#include "cxResourceExport.h"

#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include <vector>
#include <QString>

namespace cx
{

/**\brief Identification of a Coordinate system.
 *
 * \sa CoordinateSystemProvider
 *
 * \ingroup cx_resource_core_utilities
 */
struct cxResource_EXPORT CoordinateSystem
{
    CoordinateSystem() : mId(csCOUNT) {}
	explicit CoordinateSystem(COORDINATE_SYSTEM id, QString ref="") : mId(id), mRefObject(ref) {}
	COORDINATE_SYSTEM mId; ///< the type of coordinate system
	QString           mRefObject; ///< for tool, sensor and data we need a object uid to define the coordinate system

	static CoordinateSystem reference() { return CoordinateSystem(csREF); }
	static CoordinateSystem patientReference() { return CoordinateSystem(csPATIENTREF); }
	QString toString() const;
	static CoordinateSystem fromString(QString text);
	bool isValid() const;

};
cxResource_EXPORT bool operator==(const CoordinateSystem& lhs, const CoordinateSystem& rhs);
typedef CoordinateSystem Space;



}//namespace cx
#endif /* CXCOORDINATESYSTEMHELPERS_H_ */
