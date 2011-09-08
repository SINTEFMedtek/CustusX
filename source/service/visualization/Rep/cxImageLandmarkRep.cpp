#include "cxImageLandmarkRep.h"

#include <vtkFollower.h>

namespace cx
{

ImageLandmarkRepPtr ImageLandmarkRep::New(const QString& uid, const QString& name)
{
  ImageLandmarkRepPtr retval(new ImageLandmarkRep(uid, name));
  retval->mSelf = retval;
  return retval;
}

ImageLandmarkRep::ImageLandmarkRep(const QString& uid, const QString& name) :
  LandmarkRep(uid, name)
{
//  mType = "cxImageLandmarkRep";

  mImage = ImageLandmarksSourcePtr(new ImageLandmarksSource());
  this->setPrimarySource(mImage);
//  this->setSecondarySource(PatientLandmarksSourcePtr(new PatientLandmarksSource()));

}

ImageLandmarkRep::~ImageLandmarkRep()
{}


void ImageLandmarkRep::setImage(ssc::ImagePtr image)
{
	mImage->setImage(image);
}
//void ImageLandmarkRep::setPosition(QString uid)
//{
//  ssc::Landmark landmark = mImage->getLandmarks()[uid];
//  ssc::Vector3D p_r = mImage->get_rMd().coord(landmark.getCoord()); // p_r = point in ref space
//
//  ssc::Vector3D imageCenter = mImage->get_rMd().coord(mImage->boundingBox().center());
//  ssc::Vector3D centerToSkinVector = (p_r - imageCenter).normal();
//  ssc::Vector3D numberPosition = p_r + 10.0*centerToSkinVector;
//
//  mGraphics[uid].mPoint->setValue(p_r);
//  mGraphics[uid].mText->setPosition(numberPosition);
//}

}//namespace cx
