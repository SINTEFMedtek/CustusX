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
  mMessageManager(MessageManager::getInstance()),
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
int LandmarkRep::getNumberOfLandmarks() const
{
  return mImage->getLandmarks()->GetNumberOfTuples();
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

  std::map<int, vtkVectorTextFollowerPair>::iterator it1 = mTextFollowerActors.begin();
  while(it1 != mTextFollowerActors.end())
  {
    (it1->second).second->SetVisibility(on);
    it1++;
  }
  std::map<int, vtkActorPtr>::iterator it2 = mSkinPointActors.begin();
  while(it2 != mSkinPointActors.end())
  {
    it2->second->SetVisibility(on);
    it2++;
  }
    mShowLandmarks = on;
}
void LandmarkRep::setImage(ssc::ImagePtr image)
{
  if(image == mImage || image.get() == NULL)
    return;

  if(mImage)
  {
    mImage->disconnectRep(mSelf);
    disconnect(mImage.get(), SIGNAL(landmarkAdded(double, double, double,unsigned int)),
            this, SLOT(addPermanentPointSlot(double, double, double,unsigned int)));
    disconnect(this, SIGNAL(removePermanentPoint(double, double, double,unsigned int)),
            mImage.get(), SLOT(removeLandmarkSlot(double, double, double,unsigned int)));
    for(std::set<ssc::View *>::iterator it = mViews.begin();it != mViews.end();it++)
    {
      ssc::View* view = *it;
      this->removeRepActorsFromViewRenderer(view);
    }
  }

  mImage = image;

  mSkinPointActors.clear();
  mTextFollowerActors.clear();

  mImage->connectRep(mSelf);
  connect(mImage.get(), SIGNAL(landmarkAdded(double, double, double,unsigned int)),
          this, SLOT(addPermanentPointSlot(double, double, double,unsigned int)));
  connect(this, SIGNAL(removePermanentPoint(double, double, double,unsigned int)),
          mImage.get(), SLOT(removeLandmarkSlot(double, double, double,unsigned   int)));
  vtkDoubleArrayPtr landmarks = mImage->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();
  for(int i=0; i<numberOfLandmarks; i++)
  {
    double* landmark;
    landmark = landmarks->GetTuple(i);
    this->addPoint(landmark[0], landmark[1], landmark[2], landmark[3]);
  }
}
ssc::ImagePtr LandmarkRep::getImage() const
{
  return mImage;
}
void LandmarkRep::removePermanentPoint(unsigned int index)
{
  unsigned int numberOfLandmarksInImage = this->getNumberOfLandmarks();

  vtkDoubleArrayPtr landmarks = mImage->getLandmarks();
  for(unsigned int i=0; i<numberOfLandmarksInImage; i++)
  {
    //Do NOT use landmarks->GetTupleValue(idNumber-1, point);
    double* landmark = landmarks->GetTuple(i);
    if(landmark[3] == index)
    {
      emit removePermanentPoint(landmark[0], landmark[1], landmark[2], landmark[3]);
      break;
    }
  }

  std::map<int, vtkActorPtr>::iterator skinPointActorToRemove = mSkinPointActors.find(index);
  std::map<int, vtkVectorTextFollowerPair>::iterator textFollowerActorToRemove = mTextFollowerActors.find(index);

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
    view->GetInteractor()->Render();
    it++;
  }
}
void LandmarkRep::addPermanentPointSlot(double x, double y, double z, unsigned int index)
{
  this->addPoint(x, y, z, index);
}
void LandmarkRep::addRepActorsToViewRenderer(ssc::View* view)
{
  if(view == NULL)
  {
    mMessageManager->sendWarning("Trying to add rep actors to view renderer, but view is null.");
    return;
  }
  vtkRendererPtr renderer = view->getRenderer();
  if(renderer.GetPointer() == NULL)
  {
    mMessageManager->sendWarning("Trying to add rep actors to view renderer, but renderer is null.");
    return;
  }

  std::map<int, vtkActorPtr>::iterator it1 = mSkinPointActors.begin();
  while(it1 != mSkinPointActors.end())
  {
    if(!renderer->HasViewProp(it1->second))
    {
      it1->second->SetVisibility(mShowLandmarks);
      renderer->AddActor(it1->second);
    }
    it1++;
  }
  std::map<int, vtkVectorTextFollowerPair>::iterator it2 = mTextFollowerActors.begin();
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
    mMessageManager->sendWarning("Trying to remove rep actors to view renderer, but view is null.");
    return;
  }
  vtkRendererPtr renderer = view->getRenderer();
  if(renderer.GetPointer() == NULL)
  {
    mMessageManager->sendWarning("Trying to remove rep actors to view renderer, but renderer is null.");
    return;
  }

  std::map<int, vtkActorPtr>::iterator it1 = mSkinPointActors.begin();
  while(it1 != mSkinPointActors.end())
  {
    if(renderer->HasViewProp(it1->second))
      renderer->RemoveActor(it1->second);
    it1++;
  }
  std::map<int, vtkVectorTextFollowerPair>::iterator it2 = mTextFollowerActors.begin();
  while(it2 != mTextFollowerActors.end())
  {
    if(renderer->HasViewProp(it2->second.second))
      renderer->RemoveActor(it2->second.second);
    it2++;
  }
}
void LandmarkRep::addPoint(double& x, double& y, double& z, int index)
{
  vtkImageDataPtr imageData = mImage->getRefVtkImageData();
  double imageCenter[3];
  imageData->GetCenter(imageCenter);

  double centerToSkinVector[3];
  centerToSkinVector[0] = x - imageCenter[0];
  centerToSkinVector[1] = y - imageCenter[1];
  centerToSkinVector[2] = z - imageCenter[2];

  vtkMath::Normalize(centerToSkinVector);

  double numberPosition[3];
  numberPosition[0] = x + 10*centerToSkinVector[0];
  numberPosition[1] = y + 10*centerToSkinVector[1];
  numberPosition[2] = z + 10*centerToSkinVector[2];

  vtkVectorTextPtr text;
  vtkFollowerPtr followerActor;
  vtkPolyDataMapperPtr textMapper;
  std::map<int, vtkVectorTextFollowerPair>::iterator textFollowerIt = mTextFollowerActors.find(index);
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
  std::stringstream numberstream;
  numberstream << index;
  text->SetText(numberstream.str().c_str());

  textMapper->SetInput(text->GetOutput());

  followerActor->SetMapper(textMapper);
  followerActor->SetPosition(numberPosition);
  followerActor->SetScale(mTextScale[0], mTextScale[1], mTextScale[2]);
  followerActor->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);

  mTextFollowerActors.insert(std::pair<int, vtkVectorTextFollowerPair>
                            (index, vtkVectorTextFollowerPair(text, followerActor)));

  vtkSphereSourcePtr sphere = vtkSphereSourcePtr::New();
  vtkPolyDataMapperPtr sphereMapper;
  vtkActorPtr skinPointActor;
  std::map<int, vtkActorPtr>::iterator actorIt = mSkinPointActors.find(index);
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
  skinPointActor->SetPosition(x, y, z);

  mSkinPointActors.insert(std::pair<int, vtkActorPtr>(index, skinPointActor));

  std::stringstream info;
  info << "Added the permanent point, (";
  info << x;
  info << ",";
  info << y;
  info << ",";
  info << z;
  info << ") to the landmark.";
  mMessageManager->sendInfo(info.str());

  for(std::set<ssc::View *>::iterator it = mViews.begin();it != mViews.end();it++)
  {
    ssc::View* view = *it;
    this->addRepActorsToViewRenderer(view);
    view->GetInteractor()->Render();
  }
}
void LandmarkRep::internalUpdate()
{
  if(mTextFollowerActors.size() != mSkinPointActors.size())
  {
    std::stringstream errormessage;
    errormessage << "There is an inconsistency in landmarkrep, (mTextFollowerActors ==";
    errormessage << mTextFollowerActors.size();
    errormessage << ") != (mSkinPointActors ==";
    errormessage << mSkinPointActors.size();
    errormessage << "), trying to fix the error...";
    mMessageManager->sendError(errormessage.str());

    //trying to set repopulate the map
    this->setImage(mImage);
  }

  std::map<int, vtkVectorTextFollowerPair>::iterator it = mTextFollowerActors.begin();
  while(it != mTextFollowerActors.end())
  {
    int index = it->first;
    std::stringstream numberstream;
    numberstream << index;
    it->second.first->SetText(numberstream.str().c_str());
    it->second.first->Modified();
    it->second.second->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);
    it->second.second->SetScale(mTextScale[0], mTextScale[1], mTextScale[2]);
    it->second.second->Modified();
    it++;
  }
}
}//namespace cx
