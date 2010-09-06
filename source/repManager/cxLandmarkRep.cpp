#include "cxLandmarkRep.h"

#include <sstream>
#include <vtkMath.h>
#include <vtkImageData.h>
#include <vtkDoubleArray.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscLandmark.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"

namespace cx
{

LandmarkRepPtr LandmarkRep::New(const std::string& uid, const std::string& name)
{
  LandmarkRepPtr retval(new LandmarkRep(uid, name));
  retval->mSelf = retval;
  return retval;
}

LandmarkRep::LandmarkRep(const std::string& uid, const std::string& name) :
  RepImpl(uid, name),
  mType("cxLandmarkRep"),
  mShowLandmarks(true)
{
  mTextScale[0] = mTextScale[1] = mTextScale[2] = 20;

  connect(ssc::dataManager(), SIGNAL(landmarkPropertiesChanged()),
          this, SLOT(internalUpdate()));
}

LandmarkRep::~LandmarkRep()
{}

std::string LandmarkRep::getType() const
{
  return mType;
}

void LandmarkRep::setColor(RGB_ color)
{
  mColor = color;
}

void LandmarkRep::setTextScale(int& x, int& y,int& z)
{
  mTextScale[0] = x;
  mTextScale[1] = y;
  mTextScale[2] = z;

  this->internalUpdate();
}

void LandmarkRep::showLandmarks(bool on)
{
  if(on == mShowLandmarks)
    return;

  std::map<std::string, vtkVectorTextFollowerPair>::iterator it1 = mTextFollowerActors.begin();
  while(it1 != mTextFollowerActors.end())
  {
    (it1->second).second->SetVisibility(on);
    it1++;
  }
  std::map<std::string, vtkActorPtr>::iterator it2 = mSkinPointActors.begin();
  while(it2 != mSkinPointActors.end())
  {
    it2->second->SetVisibility(on);
    it2++;
  }
    mShowLandmarks = on;
}

void LandmarkRep::setImage(ssc::ImagePtr image)
{
  if(image == mImage)
    return;

  if(mImage)
  {
    //std::cout << "Landmark disconnect: (" << mImage->getName() << ") - " << this->getUid() << std::endl;
    mImage->disconnectFromRep(mSelf);
    disconnect(mImage.get(), SIGNAL(landmarkAdded(std::string)), this, SLOT(landmarkAddedSlot(std::string)));
    disconnect(mImage.get(), SIGNAL(landmarkRemoved(std::string)), this, SLOT(landmarkRemovedSlot(std::string)));
    disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    this->clearAll();
//    ssc::LandmarkMap landmarksMap = mImage->getLandmarks();
//    ssc::LandmarkMap::iterator it = landmarksMap.begin();
//    for(;it != landmarksMap.end();++it)
//    {
//      this->landmarkRemovedSlot(it->first);
//    }
  }

  mImage = image;

//  mSkinPointActors.clear();
//  mTextFollowerActors.clear();
  
  if(mImage)
  {
    //std::cout << "Landmark connect (" << mImage->getName() << ") - "  << this->getUid() << std::endl;
    mImage->connectToRep(mSelf);
    connect(mImage.get(), SIGNAL(landmarkAdded(std::string)), this, SLOT(landmarkAddedSlot(std::string)));
    connect(mImage.get(), SIGNAL(landmarkRemoved(std::string)), this, SLOT(landmarkRemovedSlot(std::string)));
    connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    this->addAll();
//    ssc::LandmarkMap landmarksMap = mImage->getLandmarks();
//    ssc::LandmarkMap::iterator it = landmarksMap.begin();
//    for(;it != landmarksMap.end();++it)
//    {
//      this->landmarkAddedSlot(it->first);
//    }
  }
}

void LandmarkRep::clearAll()
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

void LandmarkRep::addAll()
{
  ssc::LandmarkMap landmarksMap = mImage->getLandmarks();
  ssc::LandmarkMap::iterator it = landmarksMap.begin();
  for(;it != landmarksMap.end();++it)
  {
    this->landmarkAddedSlot(it->first);
  }
}

void LandmarkRep::transformChangedSlot()
{
  //std::cout << "LandmarkRep::transformChangedSlot()" << std::endl;
//  this->clearAll();
//  this->addAll();

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

ssc::ImagePtr LandmarkRep::getImage() const
{
  return mImage;
}

void LandmarkRep::landmarkAddedSlot(std::string uid)
{
  ssc::Landmark landmark = mImage->getLandmarks()[uid];
  ssc::Vector3D p_r = mImage->get_rMd().coord(landmark.getCoord()); // p_r = point in ref space
  this->addPoint(p_r, uid);
  //std::cout << "LandmarkRep::landmarkAddedSlot(" << uid << ") " << this->getUid() << std::endl;
}

void LandmarkRep::landmarkRemovedSlot(std::string uid)
{
  std::map<std::string, vtkActorPtr>::iterator skinPointActorToRemove = mSkinPointActors.find(uid);
  std::map<std::string, vtkVectorTextFollowerPair>::iterator textFollowerActorToRemove = mTextFollowerActors.find(uid);

  std::set<ssc::View *>::iterator it = mViews.begin();
  while(it != mViews.end())
  {
    ssc::View* view = (*it);
    if(view == NULL)
    {
      continue;
    }
    vtkRendererPtr renderer = view->getRenderer();
    if(renderer != NULL)
    {
      if(skinPointActorToRemove != mSkinPointActors.end() &&
         renderer->HasViewProp(skinPointActorToRemove->second.GetPointer()))
      {
        renderer->RemoveActor(skinPointActorToRemove->second.GetPointer());
      }
      if(textFollowerActorToRemove != mTextFollowerActors.end() &&
         renderer->HasViewProp(textFollowerActorToRemove->second.second.GetPointer()))
      {
        renderer->RemoveActor(textFollowerActorToRemove->second.second.GetPointer());
      }

      if(skinPointActorToRemove != mSkinPointActors.end())
        mSkinPointActors.erase(skinPointActorToRemove);

      if(textFollowerActorToRemove != mTextFollowerActors.end())
        mTextFollowerActors.erase(textFollowerActorToRemove);
    }
    this->internalUpdate();
    it++;
  }

  //std::cout << "LandmarkRep::landmarkRemovedSlot(" << uid << ") " << this->getUid() << std::endl;
}

void LandmarkRep::addRepActorsToViewRenderer(ssc::View* view)
{
  if(view == NULL)
  {
    ssc::messageManager()->sendWarning("Trying to add rep actors to view renderer, but view is null.");
    return;
  }
  vtkRendererPtr renderer = view->getRenderer();
  if(renderer.GetPointer() == NULL)
  {
    ssc::messageManager()->sendWarning("Trying to add rep actors to view renderer, but renderer is null.");
    return;
  }

  std::map<std::string, vtkActorPtr>::iterator it1 = mSkinPointActors.begin();
  while(it1 != mSkinPointActors.end())
  {
    if(!renderer->HasViewProp(it1->second))
    {
      it1->second->SetVisibility(mShowLandmarks);
      renderer->AddActor(it1->second);
    }
    it1++;
  }
  std::map<std::string, vtkVectorTextFollowerPair>::iterator it2 = mTextFollowerActors.begin();
  while(it2 != mTextFollowerActors.end())
  {
    if(!renderer->HasViewProp(it2->second.second))
    {
      it2->second.second->SetCamera(renderer->GetActiveCamera());
      it2->second.second->SetVisibility(mShowLandmarks);
      renderer->AddActor(it2->second.second);
    }
    it2++;
  }
}

void LandmarkRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  if(view == NULL)
  {
    ssc::messageManager()->sendWarning("Trying to remove rep actors to view renderer, but view is null.");
    return;
  }
  vtkRendererPtr renderer = view->getRenderer();
  if(renderer.GetPointer() == NULL)
  {
    ssc::messageManager()->sendWarning("Trying to remove rep actors to view renderer, but renderer is null.");
    return;
  }

  std::map<std::string, vtkActorPtr>::iterator it1 = mSkinPointActors.begin();
  while(it1 != mSkinPointActors.end())
  {
    if(renderer->HasViewProp(it1->second))
      renderer->RemoveActor(it1->second);
    it1++;
  }
  std::map<std::string, vtkVectorTextFollowerPair>::iterator it2 = mTextFollowerActors.begin();
  while(it2 != mTextFollowerActors.end())
  {
    if(renderer->HasViewProp(it2->second.second))
      renderer->RemoveActor(it2->second.second);
    it2++;
  }
}

/**
 * Designed to take landmarks from the image.
 * @param x world coordinat, ref space
 * @param y world coordinat, ref space
 * @param z world coordinat, ref space
 * @param index the landmarks index
 */
void LandmarkRep::addPoint(ssc::Vector3D coord, std::string uid)
{
  //vtkImageDataPtr imageData = mImage->getRefVtkImageData();
  //ssc::Vector3D imageCenter(imageData->GetCenter());
  //ssc::Vector3D centerToSkinVector = (coord - imageCenter).normal();

  //ssc::Vector3D numberPosition = coord + 10.0*centerToSkinVector;

  vtkVectorTextPtr text;
  vtkFollowerPtr followerActor;
  vtkPolyDataMapperPtr textMapper;
  std::map<std::string, vtkVectorTextFollowerPair>::iterator textFollowerIt = mTextFollowerActors.find(uid);
  if(textFollowerIt == mTextFollowerActors.end())
  {
    text = vtkVectorTextPtr::New();
    followerActor = vtkFollowerPtr::New();
    textMapper = vtkPolyDataMapperPtr::New();
  }
  else
  {
    text = textFollowerIt->second.first;
    followerActor = textFollowerIt->second.second;
    textMapper = dynamic_cast<vtkPolyDataMapper*>(followerActor->GetMapper());
  }

  std::map<std::string, ssc::LandmarkProperty> props = ssc::dataManager()->getLandmarkProperties();
  std::string name = props[uid].getName();
  text->SetText(name.c_str());

  textMapper->SetInput(text->GetOutput());

  followerActor->SetMapper(textMapper);
  //followerActor->SetPosition(numberPosition.begin());
  followerActor->SetScale(mTextScale[0], mTextScale[1], mTextScale[2]);
  followerActor->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);

  mTextFollowerActors[uid] = vtkVectorTextFollowerPair(text, followerActor);

  vtkSphereSourcePtr sphere = vtkSphereSourcePtr::New();
  vtkPolyDataMapperPtr sphereMapper;
  vtkActorPtr skinPointActor;
  std::map<std::string, vtkActorPtr>::iterator actorIt = mSkinPointActors.find(uid);
  if(actorIt == mSkinPointActors.end())
  {
    sphereMapper = vtkPolyDataMapperPtr::New();
    skinPointActor = vtkActorPtr::New();
  }
  else
  {
    skinPointActor = actorIt->second;
    sphereMapper = dynamic_cast<vtkPolyDataMapper*>(skinPointActor->GetMapper());
  }
  sphere->SetRadius(2);
  sphereMapper->SetInputConnection(sphere->GetOutputPort());
  skinPointActor->SetMapper(sphereMapper);
  skinPointActor->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);
  //skinPointActor->SetPosition(coord.begin());

  mSkinPointActors[uid] = skinPointActor;

  //ssc::messageManager()->sendInfo("Added permanent point to landmark("+uid+"): "+string_cast(coord));

  for(std::set<ssc::View *>::iterator it = mViews.begin();it != mViews.end();it++)
  {
    ssc::View* view = *it;
    this->addRepActorsToViewRenderer(view);
  }

  this->setPosition(coord, uid);
}

void LandmarkRep::setPosition(ssc::Vector3D coord, std::string uid)
{
  ssc::Vector3D imageCenter = mImage->get_rMd().coord(mImage->boundingBox().center());
//  vtkImageDataPtr imageData = mImage->getRefVtkImageData();
//  ssc::Vector3D imageCenter(imageData->GetCenter());
  ssc::Vector3D centerToSkinVector = (coord - imageCenter).normal();

  ssc::Vector3D numberPosition = coord + 10.0*centerToSkinVector;

  mTextFollowerActors[uid].second->SetPosition(numberPosition.begin());
  mSkinPointActors[uid]->SetPosition(coord.begin());
}

void LandmarkRep::internalUpdate()
{
  std::map<std::string, ssc::LandmarkProperty> props = ssc::dataManager()->getLandmarkProperties();
  std::map<std::string, vtkVectorTextFollowerPair>::iterator it = mTextFollowerActors.begin();
  while(it != mTextFollowerActors.end())
  {
    std::string uid = it->first;
    std::string name = props[uid].getName();
    it->second.first->SetText(name.c_str());
    it->second.first->Modified();
    it->second.second->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);
    it->second.second->SetScale(mTextScale[0], mTextScale[1], mTextScale[2]);
    it->second.second->Modified();
    it++;
  }
}
}//namespace cx
