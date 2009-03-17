#include "cxInriaRep3D.h"

#include <vtkRenderer.h>
#include "sscView.h"

namespace cx
{
InriaRep3D::InriaRep3D(const std::string& uid, const std::string& name) :
  ssc::RepImpl(uid, name),
  mType("cxInriaRep3D"),
  mInria(vtkViewImage3D::New())
{
  mInria->SetLinkZoom(true);
}
InriaRep3D::~InriaRep3D()
{}
std::string InriaRep3D::getType() const
{
  return mType;
}
void InriaRep3D::connectToView(ssc::View *theView)
{
  if(isConnectedToView(theView))
    return;

  mViews.insert(theView);
  this->addRepActorsToViewRenderer(theView);
}
void InriaRep3D::disconnectFromView(ssc::View *theView)
{
  mViews.erase(theView);
  this->removeRepActorsFromViewRenderer(theView);
}
InriaRep3D::vtkViewImage3DPtr InriaRep3D::getVtkViewImage3D()
{
  return mInria;
}
void InriaRep3D::addRepActorsToViewRenderer(ssc::View* view)
{
  mInria->SetRenderWindow(view->GetRenderWindow());
  mInria->SetRenderer(view->getRenderer());
  mInria->SetBackgroundColor(0.0, 0.0, 0.0);
}
void InriaRep3D::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mInria->SetRenderWindow(NULL);
  mInria->SetRenderer(NULL);
}
}//namespace cx
