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
#include "cxMessageManager.h"
#include "sscTypeConversions.h"

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
}
LandmarkRep::~LandmarkRep()
{}
std::string LandmarkRep::getType() const
{
  return mType;
}
//int LandmarkRep::getNumberOfLandmarks() const
//{
//  return mImage->getLandmarks().size();
//}
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
    mImage->disconnectFromRep(mSelf);
    disconnect(mImage.get(), SIGNAL(landmarkAdded(std::string)),
            this, SLOT(landmarkAddedSlot(std::string)));
    disconnect(mImage.get(), SIGNAL(landmarkRemoved(std::string)),
            this, SLOT(landmarkRemovedSlot(std::string)));

    ssc::LandmarkMap landmarksMap = mImage->getLandmarks();
    ssc::LandmarkMap::iterator it = landmarksMap.begin();
    for(;it != landmarksMap.end();++it)
    {
      this->landmarkRemovedSlot(it->first);
    }
  }

  mImage = image;

  mSkinPointActors.clear();
  mTextFollowerActors.clear();
  
  if(mImage)
  {
    mImage->connectToRep(mSelf);
    connect(mImage.get(), SIGNAL(landmarkAdded(std::string)),
            this, SLOT(landmarkAddedSlot(std::string)));
    connect(mImage.get(), SIGNAL(landmarkRemoved(std::string)),
            this, SLOT(landmarkRemovedSlot(std::string)));
    /*connect(mImage.get(), SIGNAL(landmarkAdded(double, double, double,unsigned int)),
            this, SLOT(addPermanentPointSlot(double, double, double,unsigned int)));
    connect(this, SIGNAL(removePermanentPoint(double, double, double,unsigned int)),
            mImage.get(), SLOT(removeLandmarkSlot(double, double, double,unsigned   int)));*/

    ssc::LandmarkMap landmarksMap = mImage->getLandmarks();
    ssc::LandmarkMap::iterator it = landmarksMap.begin();
    for(;it != landmarksMap.end();++it)
    {
      this->landmarkAddedSlot(it->first);
    }

    /*vtkDoubleArrayPtr landmarks = mImage->getLandmarks();
    int numberOfLandmarks = landmarks->GetNumberOfTuples();
    for(int i=0; i<numberOfLandmarks; i++)
    {
      double* landmark;
      landmark = landmarks->GetTuple(i);
      this->addPoint(landmark[0], landmark[1], landmark[2], landmark[3]);
    }*/
  }
}
ssc::ImagePtr LandmarkRep::getImage() const
{
  return mImage;
}

void LandmarkRep::landmarkAddedSlot(std::string uid)
{
  ssc::Landmark landmark = mImage->getLandmarks()[uid];
  ssc::Vector3D p_r = mImage->get_rMd().coord(landmark.getCoord());
  this->addPoint(p_r, uid);
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
    //view->GetInteractor()->Render();
    it++;
  }
}

/*void LandmarkRep::addPermanentPointSlot(double x, double y, double z, unsigned index)
{
  this->addPoint(x, y, z, index);
}*/
void LandmarkRep::addRepActorsToViewRenderer(ssc::View* view)
{
  if(view == NULL)
  {
    messageManager()->sendWarning("Trying to add rep actors to view renderer, but view is null.");
    return;
  }
  vtkRendererPtr renderer = view->getRenderer();
  if(renderer.GetPointer() == NULL)
  {
    messageManager()->sendWarning("Trying to add rep actors to view renderer, but renderer is null.");
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
    messageManager()->sendWarning("Trying to remove rep actors to view renderer, but view is null.");
    return;
  }
  vtkRendererPtr renderer = view->getRenderer();
  if(renderer.GetPointer() == NULL)
  {
    messageManager()->sendWarning("Trying to remove rep actors to view renderer, but renderer is null.");
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
//void LandmarkRep::addPoint(double& x, double& y, double& z, int index)
{
  vtkImageDataPtr imageData = mImage->getRefVtkImageData();
  ssc::Vector3D imageCenter(imageData->GetCenter());
  ssc::Vector3D centerToSkinVector = (coord - imageCenter).normal();

//  vtkMath::Normalize(centerToSkinVector);
  ssc::Vector3D numberPosition = coord + 10.0*centerToSkinVector;

/*  double numberPosition[3];
  numberPosition[0] = x + 10*centerToSkinVector[0];
  numberPosition[1] = y + 10*centerToSkinVector[1];
  numberPosition[2] = z + 10*centerToSkinVector[2];
*/
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
  //std::stringstream numberstream;
  //numberstream << index;
  text->SetText(uid.c_str());

  textMapper->SetInput(text->GetOutput());

  followerActor->SetMapper(textMapper);
  followerActor->SetPosition(numberPosition.begin());
  followerActor->SetScale(mTextScale[0], mTextScale[1], mTextScale[2]);
  followerActor->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);

  mTextFollowerActors[uid] = vtkVectorTextFollowerPair(text, followerActor);
  //mTextFollowerActors.insert(std::pair<int, vtkVectorTextFollowerPair>
  //                          (index, vtkVectorTextFollowerPair(text, followerActor)));

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
  skinPointActor->SetPosition(coord.begin());

  mSkinPointActors[uid] = skinPointActor;

  /*std::stringstream info;
  info << "Added the permanent point, (";
  info << x;
  info << ",";
  info << y;
  info << ",";
  info << z;
  info << ") to the landmark.";*/
  messageManager()->sendInfo("Added the permanent point to landmark: "+string_cast(coord));

  for(std::set<ssc::View *>::iterator it = mViews.begin();it != mViews.end();it++)
  {
    ssc::View* view = *it;
    this->addRepActorsToViewRenderer(view);
    //view->GetInteractor()->Render();
  }
}
void LandmarkRep::internalUpdate()
{
  /*if(mTextFollowerActors.size() != mSkinPointActors.size())
  {
    std::stringstream errormessage;
    errormessage << "There is an inconsistency in landmarkrep, (mTextFollowerActors ==";
    errormessage << mTextFollowerActors.size();
    errormessage << ") != (mSkinPointActors ==";
    errormessage << mSkinPointActors.size();
    errormessage << "), trying to fix the error...";
    messageManager()->sendError(errormessage.str());

    //trying to set repopulate the map
    this->setImage(mImage);
  }*/

  std::map<std::string, vtkVectorTextFollowerPair>::iterator it = mTextFollowerActors.begin();
  while(it != mTextFollowerActors.end())
  {
    it->second.first->SetText(it->first.c_str());
    it->second.first->Modified();
    it->second.second->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);
    it->second.second->SetScale(mTextScale[0], mTextScale[1], mTextScale[2]);
    it->second.second->Modified();
    it++;
  }
}
}//namespace cx
