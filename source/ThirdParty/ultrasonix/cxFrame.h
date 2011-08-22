#ifndef CXFRAME_H_
#define CXFRAME_H_

namespace cx
{
class Frame
{
public:
  bool mNewStatus;
  double mTimestamp; ///< Timestamp in seconds since 1/1/1970 (epoch)
  int mWidth; ///< Width in pixels
  int mHeight; ///< Height in pixels
  int mPixelFormat; ///< Pixel format in OSType (FourCC)
  unsigned char* mFirstPixel; ///< Pointer to first pixel in frame
  double mSpacing[2];
  double mOrigin[2];

  Frame():
    mNewStatus(false),
    mTimestamp(0.0),
    mWidth(0),
    mHeight(0),
    mPixelFormat(0) {};
};
}//namespace cx

#endif //CXFRAME_H_
