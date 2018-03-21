/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXUSRECONSTRUCTINPUTDATA_H
#define CXUSRECONSTRUCTINPUTDATA_H

#include "cxResourceExport.h"

#include <vector>
#include "cxProbeSector.h"
#include "cxData.h"
#include "cxTool.h"

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
	TimeInfo mTimeInfo;
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
	std::map<double, ToolPositionMetadata> mTrackerRecordedMetadata;
	std::map<double, ToolPositionMetadata> mReferenceRecordedMetadata;
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
