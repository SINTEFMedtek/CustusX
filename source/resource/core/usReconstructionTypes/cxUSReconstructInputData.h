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
#ifndef CXUSRECONSTRUCTINPUTDATA_H
#define CXUSRECONSTRUCTINPUTDATA_H

#include "cxResourceExport.h"

#include <vector>
#include "cxProbeSector.h"

namespace cx
{

typedef boost::shared_ptr<class USFrameData> USFrameDataPtr;
typedef boost::shared_ptr<class Image> ImagePtr;

/**
 * \addtogroup cx_resource_usreconstructiontypes
 * \{
 */

typedef boost::shared_ptr<class TimedPosition> TimedPositionPtr;

/** \brief One position with timestamp
 */
class cxResource_EXPORT TimedPosition
{
public:
	double mTime;// Should always be in ms
	Transform3D mPos;
};
cxResource_EXPORT inline bool operator<(const TimedPosition& lhs, const TimedPosition& rhs)
{
	return lhs.mTime < rhs.mTime;
}

struct cxResource_EXPORT USReconstructInputData
{
	QString mFilename; ///< filename used for current data read

	USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
	std::vector<TimedPosition> mFrames;
	std::vector<TimedPosition> mPositions;
	ProbeSector mProbeDefinition;
	QString mProbeUid;
	Transform3D rMpr; ///< patient registration

	vtkImageDataPtr getMask();
	bool isValid() const;
	bool is8bit() const;
};

/**
 * \}
 */

}//namespace cx


#endif // CXUSRECONSTRUCTINPUTDATA_H
