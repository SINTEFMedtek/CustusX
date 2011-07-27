/*
 * cxAngleMetricRep.cpp
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#include <cxAngleMetricRep.h>

#include "sscView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "sscTypeConversions.h"

namespace cx
{

class AngleMetricCallback : public vtkCommand
{
public:
  AngleMetricCallback() {}
  static AngleMetricCallback* New() {return new AngleMetricCallback;}
  void SetRep(AngleMetricRep* rep) {mRep = rep;}
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    if (mRep)
      mRep->scaleText();
  }
  AngleMetricRep* mRep;
};

AngleMetricRepPtr AngleMetricRep::New(const QString& uid, const QString& name)
{
  AngleMetricRepPtr retval(new AngleMetricRep(uid,name));
  return retval;
}

AngleMetricRep::AngleMetricRep(const QString& uid, const QString& name) :
    ssc::RepImpl(uid,name),
    mView(NULL),
    mColor(1,0,0)
{
}


void AngleMetricRep::setMetric(AngleMetricPtr point)
{
  if (mMetric)
    disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

  mMetric = point;

  if (mMetric)
    connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

  mLine0.reset();
  mLine1.reset();
  this->changedSlot();
}

void AngleMetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
  mView = view;

  mLine0.reset();
  mLine1.reset();

  mMetricCallback = AngleMetricCallbackPtr::New();
  mMetricCallback->SetRep(this);
//  mView->getRenderer()->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, DistanceMetricCallback::New());
  mView->getRenderer()->AddObserver(vtkCommand::ModifiedEvent, mMetricCallback);
  mView->getRenderer()->AddObserver(vtkCommand::ActiveCameraEvent, mMetricCallback);
  mView->getRenderer()->AddObserver(vtkCommand::ResetCameraEvent, mMetricCallback);

  this->changedSlot();
}

void AngleMetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mView = NULL;

  mLine0.reset();
  mLine1.reset();

  mMetricCallback->SetRep(NULL);
  mView->getRenderer()->RemoveObserver(mMetricCallback);
}

void AngleMetricRep::changedSlot()
{
  if (!mMetric->getPoint(0) || !mMetric->getPoint(1))
    return;

  if (!mLine0 && !mLine1 && mView && mMetric)
  {
    mLine0.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
    mLine1.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
  }

  if (!mLine0 && !mLine1)
    return;

  if (!mText)
  {
    mText = vtkVectorText::New();
    vtkPolyDataMapperPtr textMapper = vtkPolyDataMapperPtr::New();
    textMapper->SetInput(mText->GetOutput());
    mFollower = vtkFollower::New();
    mFollower->SetMapper(textMapper);
    mFollower->SetCamera(mView->getRenderer()->GetActiveCamera());
//    mFollower->SetVisibility(true);
    ssc::Vector3D mTextScale(2,2,2);
    mFollower->SetScale(mTextScale.begin());
    mFollower->GetProperty()->SetColor(mColor.begin());

    mView->getRenderer()->AddActor(mFollower);
  }

  std::vector<ssc::Vector3D> p_r(4);
  for (unsigned i=0; i<4; ++i)
  {
    ssc::Transform3D rMi = ssc::SpaceHelpers::get_toMfrom(mMetric->getPoint(i)->getFrame(), ssc::CoordinateSystem(ssc::csREF));
    p_r[i] = rMi.coord(mMetric->getPoint(i)->getCoordinate());
  }

  mLine0->setColor(mColor);
  mLine1->setColor(mColor);
  mLine0->setStipple(0xF0FF);
  mLine1->setStipple(0xF0FF);
  mLine0->setValue(p_r[0], p_r[1]);
  mLine1->setValue(p_r[2], p_r[3]);

  QString text = QString("%1*").arg(mMetric->getAngle()/M_PI*180, 0, 'f', 1);
  mText->SetText(cstring_cast(text));
  ssc::Vector3D p_mean = (p_r[0]+p_r[1]+p_r[2]+p_r[3])/4;
  mFollower->SetPosition(p_mean.begin());
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void AngleMetricRep::scaleText()
{
  double targetSize = 0.025;// relative to vp height

  //  use the focal point and focal point + vup.
  //  Transform both to view space and remove z-coord.
  //  The distance between then in the view plane can
  //  be used to rescale the text.
  vtkCameraPtr camera = mView->getRenderer()->GetActiveCamera();
  ssc::Vector3D p_f(camera->GetFocalPoint());
  ssc::Vector3D vup(camera->GetViewUp());
  ssc::Vector3D p_fup = p_f+vup;
  mView->getRenderer()->WorldToView(p_f[0],p_f[1],p_f[2]);
  mView->getRenderer()->WorldToView(p_fup[0],p_fup[1],p_fup[2]);
  p_f[2] = 0;
  p_fup[2] = 0;
  double size = (p_f - p_fup).length()/2;
  double scale = targetSize/size;
//  std::cout << "s= " << size << "  ,scale= " << scale << std::endl;
  ssc::Vector3D mTextScale(scale,scale,scale);
  if (mFollower)
    mFollower->SetScale(mTextScale.begin());
}


}
