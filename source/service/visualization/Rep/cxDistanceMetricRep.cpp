/*
 * cxDistanceMetricRep.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
 */

#include <cxDistanceMetricRep.h>
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

class DistanceMetricCallback : public vtkCommand
{
public:
  DistanceMetricCallback() {}
  static DistanceMetricCallback* New() {return new DistanceMetricCallback;}
  void SetRep(DistanceMetricRep* rep) {mRep = rep;}
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    if (mRep)
      mRep->scaleText();
//    std::cout << "executing DistanceMetricCallback" << std::endl;
  }
  DistanceMetricRep* mRep;
};

DistanceMetricRepPtr DistanceMetricRep::New(const QString& uid, const QString& name)
{
	DistanceMetricRepPtr retval(new DistanceMetricRep(uid,name));
	return retval;
}

DistanceMetricRep::DistanceMetricRep(const QString& uid, const QString& name) :
		ssc::RepImpl(uid,name),
		mView(NULL),
		mColor(1,0,0)
{
}


void DistanceMetricRep::setDistanceMetric(DistanceMetricPtr point)
{
	if (mDistanceMetric)
		disconnect(mDistanceMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mDistanceMetric = point;

	if (mDistanceMetric)
		connect(mDistanceMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mGraphicalLine.reset();
	this->changedSlot();
}

void DistanceMetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
	mView = view;
	mGraphicalLine.reset();

  mDistanceMetricCallback = DistanceMetricCallbackPtr::New();
  mDistanceMetricCallback->SetRep(this);
//  mView->getRenderer()->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, DistanceMetricCallback::New());
  mView->getRenderer()->AddObserver(vtkCommand::ModifiedEvent, mDistanceMetricCallback);
  mView->getRenderer()->AddObserver(vtkCommand::ActiveCameraEvent, mDistanceMetricCallback);
  mView->getRenderer()->AddObserver(vtkCommand::ResetCameraEvent, mDistanceMetricCallback);

	this->changedSlot();
}

void DistanceMetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mView = NULL;
	mGraphicalLine.reset();

	mDistanceMetricCallback->SetRep(NULL);
//  mView->getRenderer()->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, DistanceMetricCallback::New());
  mView->getRenderer()->RemoveObserver(mDistanceMetricCallback);
}

void DistanceMetricRep::changedSlot()
{
  if (!mDistanceMetric->getPoint(0) || !mDistanceMetric->getPoint(1))
    return;

  if (!mGraphicalLine && mView && mDistanceMetric)
		mGraphicalLine.reset(new ssc::GraphicalLine3D(mView->getRenderer()));

	if (!mGraphicalLine)
		return;

	if (!mText)
	{
	  mText = vtkVectorText::New();
	  vtkPolyDataMapperPtr textMapper = vtkPolyDataMapperPtr::New();
    textMapper->SetInput(mText->GetOutput());
    mFollower = vtkFollower::New();
	  mFollower->SetMapper(textMapper);
	  mFollower->SetCamera(mView->getRenderer()->GetActiveCamera());
//	  mFollower->SetVisibility(true);
	  ssc::Vector3D mTextScale(2,2,2);
	  mFollower->SetScale(mTextScale.begin());
	  mFollower->GetProperty()->SetColor(mColor.begin());

	  mView->getRenderer()->AddActor(mFollower);
	}

	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mDistanceMetric->getPoint(0)->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mDistanceMetric->getPoint(0)->getCoordinate());

	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(mDistanceMetric->getPoint(1)->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p1_r = rM1.coord(mDistanceMetric->getPoint(1)->getCoordinate());

	mGraphicalLine->setColor(mColor);
	mGraphicalLine->setValue(p0_r, p1_r);
	mGraphicalLine->setStipple(0xF0FF);


	QString text = QString("%1 mm").arg(mDistanceMetric->getDistance(), 0, 'f', 1);
  mText->SetText(cstring_cast(text));
  ssc::Vector3D p_mean = (p0_r+p1_r)/2;
  mFollower->SetPosition(p_mean.begin());
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void DistanceMetricRep::scaleText()
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
  mFollower->SetScale(mTextScale.begin());
}


}
