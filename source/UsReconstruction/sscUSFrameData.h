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
  explicit USFrameData(ImagePtr inputFrameData);
  void removeFrame(unsigned int index);
  unsigned char* getFrame(unsigned int index);
  int* getDimensions();
  Vector3D getSpacing();
  std::string getName();
  std::string getUid();
  std::string getFilePath();
private:
  ImagePtr mImage;
  std::vector<unsigned char*> mFrames;
  int* mDimensions;
  Vector3D mSpacing;
};
  
  typedef boost::shared_ptr<USFrameData> USFrameDataPtr;
}//namespace ssc

#endif SSCUSFRAMEDATA_H_