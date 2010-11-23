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

  virtual QString getType() const;

  void setImage(ssc::ImagePtr image); ///< sets the image data should be retrieved from
  ssc::ImagePtr getImage() const; ///< returns a pointer to the image being used

public slots:
  virtual void landmarkAddedSlot(QString );
  virtual void transformChangedSlot();

protected:
  ImageLandmarkRep(const QString& uid, const QString& name=""); ///< sets default text scaling to 20
  virtual void clearAll();
  virtual void addAll();
  virtual void setPosition(ssc::Vector3D coord, QString uid);

  ssc::ImagePtr   mImage;         ///< the image which this rep is linked to
};

}//namespace cx
#endif /* CXIMAGELANDMARKREP_H_ */
