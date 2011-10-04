/*
 *  sscUSFrameData.h
 *  CustusX3
 *
 *  Created by Ole Vegard Solberg on 8/17/10.
 *  Copyright 2010 SINTEF. All rights reserved.
 *
 */

#ifndef SSCUSFRAMEDATA_H_
#define SSCUSFRAMEDATA_H_

#include <vector>
#include "sscImage.h"
#include "sscProbeSector.h"

namespace ssc
{
typedef boost::shared_ptr<class TimedPosition> TimedPositionPtr;
/** Represents one position with timestamp 
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

private:
  ImagePtr getBase();
  vtkImageDataPtr useAngio(ImagePtr inputFrameData);/// Use only US angio data as input. Removes grayscale from the US data and converts the remaining color to grayscale
  ImagePtr mImage;
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

}//namespace ssc

#endif // SSCUSFRAMEDATA_H_
