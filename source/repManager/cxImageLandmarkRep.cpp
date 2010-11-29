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
  mType = "cxImageLandmarkRep";
}

ImageLandmarkRep::~ImageLandmarkRep()
{}

QString ImageLandmarkRep::getType() const
{
  return mType;
}

void ImageLandmarkRep::setImage(ssc::ImagePtr image)
{
  if(image == mImage)
    return;

  if(mImage)
  {
    //std::cout << "Landmark disconnect: (" << mImage->getName() << ") - " << this->getUid() << std::endl;
    mImage->disconnectFromRep(mSelf);
    disconnect(mImage.get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkAddedSlot(QString)));
    disconnect(mImage.get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkRemovedSlot(QString)));
    disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    this->clearAll();
  }

  mImage = image;

  if(mImage)
  {
    mImage->connectToRep(mSelf);
    connect(mImage.get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkAddedSlot(QString)));
    connect(mImage.get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkRemovedSlot(QString)));
    connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    this->addAll();
  }
}

ssc::ImagePtr ImageLandmarkRep::getImage() const
{
  return mImage;
}

void ImageLandmarkRep::landmarkAddedSlot(QString uid)
{
  ssc::Landmark landmark = mImage->getLandmarks()[uid];
  ssc::Vector3D p_r = mImage->get_rMd().coord(landmark.getCoord()); // p_r = point in ref space
  this->addPoint(p_r, uid);
  //std::cout << "LandmarkRep::landmarkAddedSlot(" << uid << ") " << this->getUid() << std::endl;
}

void ImageLandmarkRep::transformChangedSlot()
{
  //std::cout << "LandmarkRep::transformChangedSlot()" << std::endl;

  ssc::LandmarkMap landmarksMap = mImage->getLandmarks();
  ssc::LandmarkMap::iterator it = landmarksMap.begin();
  for(;it != landmarksMap.end();++it)
  {
    //this->landmarkAddedSlot(it->first);
    ssc::Landmark landmark = it->second;
    ssc::Vector3D p_r = mImage->get_rMd().coord(landmark.getCoord()); // p_r = point in ref space
    this->addPoint(p_r, it->first);
//    std::cout << "LandmarkRep::landmarkAddedSlot(" << uid << ") " << this->getUid() << std::endl;
  }
}

void ImageLandmarkRep::clearAll()
{
  ssc::LandmarkMap landmarksMap = mImage->getLandmarks();
  ssc::LandmarkMap::iterator it = landmarksMap.begin();
  for(;it != landmarksMap.end();++it)
  {
    this->landmarkRemovedSlot(it->first);
  }

  mSkinPointActors.clear();
  mTextFollowerActors.clear();
}

void ImageLandmarkRep::addAll()
{
  ssc::LandmarkMap landmarksMap = mImage->getLandmarks();
  ssc::LandmarkMap::iterator it = landmarksMap.begin();
  for(;it != landmarksMap.end();++it)
  {
    this->landmarkAddedSlot(it->first);
  }
}

void ImageLandmarkRep::setPosition(ssc::Vector3D coord, QString uid)
{
  ssc::Vector3D imageCenter = mImage->get_rMd().coord(mImage->boundingBox().center());
//  vtkImageDataPtr imageData = mImage->getRefVtkImageData();
//  ssc::Vector3D imageCenter(imageData->GetCenter());
  ssc::Vector3D centerToSkinVector = (coord - imageCenter).normal();

  ssc::Vector3D numberPosition = coord + 10.0*centerToSkinVector;

  mTextFollowerActors[uid].second->SetPosition(numberPosition.begin());
  mSkinPointActors[uid]->SetPosition(coord.begin());
}

}//namespace cx
