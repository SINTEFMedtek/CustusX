#ifndef CXIMAGELANDMARKREP_H_
#define CXIMAGELANDMARKREP_H_

#include "cxLandmarkRep.h"

namespace cx
{
typedef boost::shared_ptr<class ImageLandmarkRep> ImageLandmarkRepPtr;

/**
 * \class ImageLandmarkRep
 *
 * \brief
 *
 * \date 23. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class ImageLandmarkRep : public LandmarkRep
{
public:
  static ImageLandmarkRepPtr New(const QString& uid, const QString& name=""); ///constructor
  virtual ~ImageLandmarkRep();

  void setImage(ssc::ImagePtr image); ///< sets the image data should be retrieved from
  virtual QString getType() const { return "ImageLandmarkRep"; }

protected:
  ImageLandmarkRep(const QString& uid, const QString& name=""); ///< sets default text scaling to 20
  ImageLandmarksSourcePtr mImage;
};

}//namespace cx
#endif /* CXIMAGELANDMARKREP_H_ */
