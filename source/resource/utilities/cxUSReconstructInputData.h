// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXUSRECONSTRUCTINPUTDATA_H
#define CXUSRECONSTRUCTINPUTDATA_H

#include <vector>
#include "sscProbeSector.h"

namespace ssc
{

typedef boost::shared_ptr<class USFrameData> USFrameDataPtr;
typedef boost::shared_ptr<class Image> ImagePtr;

/**
 * \addtogroup cxResourceUtilities
 * \{
 */

typedef boost::shared_ptr<class TimedPosition> TimedPositionPtr;

/** \brief One position with timestamp
 */
class TimedPosition
{
public:
	double mTime;// Should always be in ms
	Transform3D mPos;
};
inline bool operator<(const TimedPosition& lhs, const TimedPosition& rhs)
{
	return lhs.mTime < rhs.mTime;
}

struct USReconstructInputData
{
	QString mFilename; ///< filename used for current data read

	ssc::USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
	std::vector<ssc::TimedPosition> mFrames;
	std::vector<ssc::TimedPosition> mPositions;
	ssc::ImagePtr mMask;///< Clipping mask for the input data
	ssc::ProbeSector mProbeData;
	QString mProbeUid;
};

/**
 * \}
 */

}//namespace ssc


#endif // CXUSRECONSTRUCTINPUTDATA_H
