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
#include "sscView.h"
#include "cxMessageManager.h"
/**
 * cxLandmarkRep.cpp
 *
 * \brief
 *
 * \date Dec 10, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
namespace cx
{
LandmarkRepPtr LandmarkRep::New(const std::string& uid, const std::string& name)
{
  //std::cout << "LandmarkRep::New" << std::endl;
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
  //std::cout << "LandmarkRep::LandmarkRep" << std::endl;
  mTextScale[0] = mTextScale[1] = mTextScale[2] = 20;
}
LandmarkRep::~LandmarkRep()
{
  //std::cout << "LandmarkRep::~LandmarkRep();" << std::endl;
}
std::string LandmarkRep::getType() const
{
  //std::cout << "LandmarkRep::getType()" << std::endl;
  return mType;
}
int LandmarkRep::getNumberOfLandmarks() const
{
  //std::cout << "LandmarkRep::getNumberOfLandmarks()" << std::endl;
  return mImage->getLandmarks()->GetNumberOfTuples();
}
void LandmarkRep::setColor(RGB color)
{
  //std::cout << "LandmarkRep::setColor" << std::endl;
  mColor = color;
}
void LandmarkRep::setTextScale(int& x, int& y,int& z)
{
  //std::cout << "LandmarkRep::setTextScale" << std::endl;
  mTextScale[0] = x;
  mTextScale[1] = y;
  mTextScale[2] = z;

  this->internalUpdate();
}
void LandmarkRep::showLandmarks(bool on)
{
  //std::cout << "LandmarkRep::showLandmarks" << std::endl;
  if(on == mShowLandmarks)
    return;

  std::vector<vtkVectorTextFollowerPair>::iterator it1 = mTextFollowerActors.begin();
  while(it1 != mTextFollowerActors.end())
  {
    it1->second->SetVisibility(on);
    it1++;
  }
  std::vector<vtkActorPtr>::iterator it2 = mSkinPointActors.begin();
  while(it2 != mSkinPointActors.end())
  {
    (*it2)->SetVisibility(on);
    it2++;
  }
    mShowLandmarks = on;
}
void LandmarkRep::setImage(ssc::ImagePtr image)
{
  if(image.get() == NULL || image == mImage)
    return;

  if(mImage.get() != NULL)
  {
    disconnect(mImage.get(), SIGNAL(landmarkAdded(double, double, double)),
            this, SLOT(addPermanentPointSlot(double, double, double)));
    disconnect(this, SIGNAL(removePermanentPoint(double, double, double)),
            mImage.get(), SLOT(removeLandmarkSlot(double, double, double)));
  }

  mImage = image;

  mSkinPointActors.clear();
  mTextFollowerActors.clear();

  vtkDoubleArrayPtr landmarks = mImage->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();
  for(int i=0; i<numberOfLandmarks; i++)
  {
    double* point;
    point = landmarks->GetTuple(i);
    this->addPoint(point[0], point[1], point[2], i);
  }

  connect(mImage.get(), SIGNAL(landmarkAdded(double, double, double)),
          this, SLOT(addPermanentPointSlot(double, double, double)));
  connect(this, SIGNAL(removePermanentPoint(double, double, double)),
          mImage.get(), SLOT(removeLandmarkSlot(double, double, double)));
}
ssc::ImagePtr LandmarkRep::getImage() const
{
  //std::cout << "LandmarkRep::getImage()" << std::endl;
  return mImage;
}
void LandmarkRep::removePermanentPoint(unsigned int idNumber)
{
  //std::cout << "LandmarkRep::removePermanentPoint" << std::endl;
  unsigned int numberOfLandmarksInImage = this->getNumberOfLandmarks();
  unsigned int numberOfSkinpointActors = mSkinPointActors.size();
  unsigned int numberOfTextFollowerActors = mTextFollowerActors.size();

  //std::cout << "Number of landmarks in mImage is: " << numberOfLandmarksInImage << std::endl;
  //std::cout << "Number of actor in mSkinPointActors is: " << numberOfSkinpointActors << std::endl;
  //std::cout << "Number of actor in mTextFollowerActors is: " << numberOfTextFollowerActors << std::endl;

  mImage->printLandmarks();
  if(idNumber == 0 ||
     numberOfLandmarksInImage < idNumber ||
     numberOfSkinpointActors < idNumber ||
     numberOfTextFollowerActors < idNumber)
  {
    std::stringstream warning;
    warning << "Cannot remove point ";
    warning << idNumber;
    warning << " because it dosn't exist in either the image or in the actor lists.";
    mMessageManager.sendWarning(warning.str());
    return;
  }

  vtkDoubleArrayPtr landmarks = mImage->getLandmarks();
  //Do NOT use landmarks->GetTupleValue(idNumber-1, point);
  double* point = landmarks->GetTuple(idNumber-1);
  std::stringstream debug;
  debug << "Found point: (";
  debug << point[0];
  debug << ",";
  debug << point[1];
  debug << ",";
  debug << point[2];
  debug << "), requesting to remove...";
  mMessageManager.sendInfo(debug.str());
  emit removePermanentPoint(point[0], point[1], point[2]);

  vtkActorPtr skinPointActorToRemove = mSkinPointActors.at(idNumber-1);
  vtkVectorTextFollowerPair textFollowerActorToRemove = mTextFollowerActors.at(idNumber-1);

  std::set<ssc::View *>::iterator it = mViews.begin();
  while(it != mViews.end())
  {
    ssc::View* view = (*it);
    if(view == NULL)
    {
      //std::cout << "view == NULL" << std::endl;
      continue;
    }
    //std::cout << "The views id is " << view->getUid() << std::endl;
    vtkRendererPtr renderer = view->getRenderer();
    if(renderer != NULL)
    {
      //std::cout << "renderer != NULL" << std::endl;
      if(renderer->HasViewProp(skinPointActorToRemove.GetPointer()))
      {
        //std::cout << "Renderer has the viewprop, removing it." << std::endl;
        renderer->RemoveActor(skinPointActorToRemove.GetPointer());
      }
      if(renderer->HasViewProp(textFollowerActorToRemove.second.GetPointer()))
      {
        //std::cout << "Renderer has the viewprop, removing it." << std::endl;
        renderer->RemoveActor(textFollowerActorToRemove.second.GetPointer());
      }

      std::vector<vtkActorPtr>::iterator it1 = mSkinPointActors.begin();
      std::vector<vtkVectorTextFollowerPair>::iterator it2 = mTextFollowerActors.begin();
      for(unsigned int i=1; i<=mSkinPointActors.size();i++)
      {
        if(i == idNumber)
        {
          //std::cout << "Erasing the actors from the actor lists..." << std::endl;
          mSkinPointActors.erase(it1);
          mTextFollowerActors.erase(it2);
        }
        if(it1 != mSkinPointActors.end())
          it1++;
        if(it2 != mTextFollowerActors.end())
          it2++;
      }
    }
    renderer->Render();
    it++;
  }
  this->internalUpdate();
  //std::cout << "After internalUpdate()" << std::endl;
  mImage->printLandmarks();
}
void LandmarkRep::addPermanentPointSlot(double x, double y, double z)
{
  //std::cout << "LandmarkRep::addPermanentPointSlot" << std::endl;
  this->addPoint(x, y, z);
}
void LandmarkRep::addRepActorsToViewRenderer(ssc::View* view)
{
  //std::cout << "LandmarkRep::addRepActorsToViewRenderer" << std::endl;
  if(view == NULL)
  {
    mMessageManager.sendWarning("Trying to add rep actors to view renderer, but view is null.");
    return;
  }
  vtkRendererPtr renderer = view->getRenderer();
  if(renderer.GetPointer() == NULL)
  {
    mMessageManager.sendWarning("Trying to add rep actors to view renderer, but renderer is null.");
    return;
  }

  std::vector<vtkActorPtr>::iterator it1 = mSkinPointActors.begin();
  while(it1 != mSkinPointActors.end())
  {
    if(!renderer->HasViewProp(*it1))
    {
      (*it1)->SetVisibility(mShowLandmarks);
      renderer->AddActor(*it1);
    }
    it1++;
  }
  std::vector<vtkVectorTextFollowerPair>::iterator it2 = mTextFollowerActors.begin();
  while(it2 != mTextFollowerActors.end())
  {
    if(!renderer->HasViewProp(it2->second))
    {
      it2->second->SetCamera(renderer->GetActiveCamera());
      it2->second->SetVisibility(mShowLandmarks);
      renderer->AddActor(it2->second);
    }
    it2++;
  }
}
void LandmarkRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  //std::cout << "LandmarkRep::removeRepActorsFromViewRenderer" << std::endl;
  if(view == NULL)
  {
    mMessageManager.sendWarning("Trying to remove rep actors to view renderer, but view is null.");
    return;
  }
  vtkRendererPtr renderer = view->getRenderer();
  if(renderer.GetPointer() == NULL)
  {
    mMessageManager.sendWarning("Trying to remove rep actors to view renderer, but renderer is null.");
    return;
  }

  std::vector<vtkActorPtr>::iterator it1 = mSkinPointActors.begin();
  while(it1 != mSkinPointActors.end())
  {
    if(renderer->HasViewProp(*it1))
      renderer->RemoveActor(*it1);
    it1++;
  }
  std::vector<vtkVectorTextFollowerPair>::iterator it2 = mTextFollowerActors.begin();
  while(it2 != mTextFollowerActors.end())
  {
    if(renderer->HasViewProp(it2->second))
      renderer->RemoveActor(it2->second);
    it2++;
  }
}
void LandmarkRep::addPoint(double& x, double& y, double& z, int numberInLine)
{
  //std::cout << "LandmarkRep::addPoint" << std::endl;
  vtkImageDataPtr imageData = mImage->getBaseVtkImageData();
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

  vtkVectorTextPtr text = vtkVectorTextPtr::New();
  std::stringstream numberstream;
  if(numberInLine == 0)
    numberInLine = mTextFollowerActors.size()+1;
  numberstream << numberInLine;
  text->SetText(numberstream.str().c_str());

  vtkPolyDataMapperPtr textMapper = vtkPolyDataMapperPtr::New();
  textMapper->SetInput(text->GetOutput());

  vtkFollowerPtr followerActor = vtkFollowerPtr::New();
  followerActor->SetMapper(textMapper);
  followerActor->AddPosition(numberPosition);
  followerActor->SetScale(mTextScale[0], mTextScale[1], mTextScale[2]);
  followerActor->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);

  mTextFollowerActors.push_back(vtkVectorTextFollowerPair(text, followerActor));

  vtkSphereSourcePtr sphere = vtkSphereSourcePtr::New();
  sphere->SetRadius(2);
  vtkPolyDataMapperPtr sphereMapper = vtkPolyDataMapperPtr::New();
  sphereMapper->SetInputConnection(sphere->GetOutputPort());
  vtkActorPtr skinPointActor = vtkActorPtr::New();
  skinPointActor->SetMapper(sphereMapper);
  skinPointActor->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);
  skinPointActor->SetPosition(x, y, z);

  mSkinPointActors.push_back(skinPointActor);

  std::stringstream info;
  info << "Added the permanent point, (";
  info << x;
  info << ",";
  info << y;
  info << ",";
  info << z;
  info << ") to the landmark.";
  mMessageManager.sendInfo(info.str());

  for(std::set<ssc::View *>::iterator it = mViews.begin();it != mViews.end();it++)
  {
    ssc::View* view = *it;
    this->addRepActorsToViewRenderer(view);
  }
}
void LandmarkRep::internalUpdate()
{
  //std::cout << "LandmarkRep::internalUpdate" << std::endl;
  if(mTextFollowerActors.size() != mSkinPointActors.size())
  {
    std::stringstream errormessage;
    errormessage << "There is an inconsistency in landmarkrep, (mTextFollowerActors ==";
    errormessage << mTextFollowerActors.size();
    errormessage << ") != (mSkinPointActors ==";
    errormessage << mSkinPointActors.size();
    errormessage << "), trying to fix the error...";
    mMessageManager.sendError(errormessage.str());

    //trying to set repopulate the vectors
    this->setImage(mImage);
  }

  std::vector<vtkVectorTextFollowerPair>::iterator it = mTextFollowerActors.begin();
  int i=1;
  while(it != mTextFollowerActors.end())
  {
    std::cout << "Setting VectorText to: " << i << std::endl;
    std::stringstream numberstream;
    numberstream << i;
    it->first->SetText(numberstream.str().c_str());
    it->first->Modified();
    it->second->GetProperty()->SetColor(mColor.R/255, mColor.G/255, mColor.B/255);
    it->second->SetScale(mTextScale[0], mTextScale[1], mTextScale[2]);
    it->second->Modified();
    it++;
    i++;
  }
}
}//namespace cx
