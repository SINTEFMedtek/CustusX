// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCUSFRAMEDATA_H_
#define SSCUSFRAMEDATA_H_

#include <vector>
#include "sscImage.h"
#include "sscProbeSector.h"

namespace ssc
{

/**
 * \addtogroup sscUSReconstruction
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

/**\brief Helper class encapsulating a 2S US data set.
 *
 * The class is a thin wrapper around a Image , with the
 * added possibility to remove single frames. (The Image is defined
 * as consisting of a sequence of frames).
 *
 *  \date August 17, 2010
 *  \author Ole Vegard Solberg
 */
class USFrameData
{
public:
	explicit USFrameData(ImagePtr inputFrameData, bool angio = false);
	void reinitialize();
	void removeFrame(unsigned int index);
	unsigned char* getFrame(unsigned int index);
	int* getDimensions();
	Vector3D getSpacing();
	QString getName();
	QString getUid();
	QString getFilePath();
	void setAngio(bool angio);///< Use only angio data as input. reinitialize() must be called afterwards

private:
//	ImagePtr getBase();
	vtkImageDataPtr useAngio(ImagePtr inputFrameData);/// Use only US angio data as input. Removes grayscale from the US data and converts the remaining color to grayscale
	ImagePtr mBaseImage;
	vtkImageDataPtr mProcessedImage; // baseimage converted to grayscale using angio or luminance algorithm
	std::vector<unsigned char*> mFrames;
	int* mDimensions;
	Vector3D mSpacing;
	bool mUseAngio;
};

typedef boost::shared_ptr<USFrameData> USFrameDataPtr;

struct USReconstructInputData
{
	QString mFilename; ///< filename used for current data read

	ssc::USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
	std::vector<ssc::TimedPosition> mFrames;
	std::vector<ssc::TimedPosition> mPositions;
	ssc::ImagePtr mMask;///< Clipping mask for the input data
	ssc::ProbeSector mProbeData;
};

/**
 * \}
 */

}//namespace ssc

#endif // SSCUSFRAMEDATA_H_
