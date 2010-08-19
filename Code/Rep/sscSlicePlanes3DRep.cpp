#include "sscSlicePlanes3DRep.h"

#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextActor3D.h>

#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscVtkHelperClasses.h"

namespace ssc
{

SlicePlanesProxy::SlicePlanesProxy()
{
  mVisible = false;
	mColors.push_back(Vector3D(0,   1,   1));
	mColors.push_back(Vector3D(0,   0.6, 1));
	mColors.push_back(Vector3D(0.5, 0.5, 1));
}

void SlicePlanesProxy::setVisible(bool visible)
{
  mVisible = visible;
  emit changed();
}

bool SlicePlanesProxy::getVisible() const
{
  return mVisible;
}

void SlicePlanesProxy::setViewportData(PLANE_TYPE type, ssc::SliceProxyPtr slice, const DoubleBoundingBox3D& vp_s)
{
	if (!slice)
		return;

	if (!mData.count(type))
	{
		DataType data;
		data.mPointPos_normvp = Vector3D(0.1,0.8,0.0);
		data.vp_s = vp_s;
		data.mSliceProxy = slice;
		data.mColor = mColors[mData.size() % mColors.size()];
		
		connect(data.mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SIGNAL(changed()));
		mData[type] = data;		
	}
	
	mData[type].vp_s = vp_s;		

	emit changed();
}

SlicePlanesProxy::DataMap SlicePlanesProxy::getData()
{
	return mData;
}



///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


SlicePlanes3DRepPtr SlicePlanes3DRep::New(const std::string& uid, const std::string& name)
{
	SlicePlanes3DRepPtr retval(new SlicePlanes3DRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

SlicePlanes3DRep::SlicePlanes3DRep(const std::string& uid, const std::string& name) :
	RepImpl(uid, name), mView(NULL)
{
}

SlicePlanes3DRep::~SlicePlanes3DRep()
{
}

void SlicePlanes3DRep::addRepActorsToViewRenderer(ssc::View* view)
{
//  std::cout << "SlicePlanes3DRep::addRepActorsToViewRenderer() " << mData.size() << std::endl;

//	for (DataMap::iterator i=mData.begin(); i!=mData.end(); ++i)
//	{
//		SlicePlanesProxy::DataType baseData = mProxy->getData()[i->first];
//
//		i->second.mText = vtkTextActor3DPtr::New();
//		i->second.mText->SetInput("O");
//		i->second.mText->GetTextProperty()->SetColor(baseData.mColor.begin());
//		i->second.mText->GetTextProperty()->SetFontSize(20);
//		i->second.mText->GetTextProperty()->BoldOn();
//		view->getRenderer()->AddActor(i->second.mText);
//		i->second.mRect.reset(new Rect3D(view->getRenderer(), baseData.mColor));
//	}
	mView = view;
  this->changedSlot();
}

void SlicePlanes3DRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  this->clearActors();
  mView = NULL;
}

void SlicePlanes3DRep::clearActors()
{
  if (!mView)
    return;

  for (DataMap::iterator i=mData.begin(); i!=mData.end(); ++i)
  {
    mView->getRenderer()->RemoveActor(i->second.mText);
    i->second.mPoint.reset();
    i->second.mRect.reset();
    i->second.mAxes.reset();
  }
  mData.clear();
}

void SlicePlanes3DRep::changedSlot()
{
  if (!mView)
    return;

  if (!mProxy->getVisible())
  {
    this->clearActors();
    return;
  }

  SlicePlanesProxy::DataMap baseData = mProxy->getData();

	for (SlicePlanesProxy::DataMap::iterator i=baseData.begin(); i!=baseData.end(); ++i)
	{
    SlicePlanesProxy::DataType& base = i->second;
    DataType& data = mData[i->first];

    if (!data.mText)
	  {
      data.mText = vtkTextActor3DPtr::New();
      data.mText->SetInput("O");
      data.mText->GetTextProperty()->SetColor(base.mColor.begin());
      data.mText->GetTextProperty()->SetFontSize(20);
      data.mText->GetTextProperty()->BoldOn();
	    mView->getRenderer()->AddActor(data.mText);
	  }
	  if (!data.mRect)
	  {
	    data.mRect.reset(new Rect3D(mView->getRenderer(), base.mColor));
	  }
		
		Transform3D rMs = base.mSliceProxy->get_sMr().inv();
		Transform3D vpMnvp = createTransformNormalize(DoubleBoundingBox3D(0,1,0,1,0,1), base.vp_s);
		Vector3D pos_s = vpMnvp.coord(base.mPointPos_normvp);
	 	Transform3D T = createTransformTranslate(pos_s);
	 	
//	 	std::cout << "-----------------------------------------------------" << std::endl;
//    std::cout << "vp_s" << i->first << " -- " << base.vp_s << std::endl;
//	 	std::cout << "rMs" << i->first << " -- " << rMs << std::endl;
//    std::cout << "vp_r" << i->first << " -- " << transform(rMs.inv(), base.vp_s) << std::endl;
//    std::cout << "-----------------------------------------------------" << std::endl;

	 	if (data.mText)
	 	{
	 		data.mText->SetUserMatrix((rMs*T).matrix());
	 	}
		
		if (data.mPoint)
		{
			data.mPoint->setColor(base.mColor);
			data.mPoint->setValue(rMs.coord(pos_s));
		}
		if (data.mRect)
		{
			data.mRect->updatePosition(base.vp_s, rMs);
		}
		if (data.mAxes)
		{
			data.mAxes->setPosition(rMs);
		}

//		this->setVisibility(data);
	}		
}

//void SlicePlanes3DRep::setVisibility(DataType data)
//{
//  bool visible = mProxy->getVisible();
//
//  if (data.mText)
//    data.mText->setVisible(visible);
//  if (data.mPoint)
//    data.mPoint->setVisible(visible);
//  if (data.mRect)
//    data.mRect->setVisible(visible);
//  if (data.mAxes)
//    data.mAxes->setVisible(visible);
//}

void SlicePlanes3DRep::setProxy(SlicePlanesProxyPtr proxy)
{
	mProxy = proxy;
	connect(mProxy.get(), SIGNAL(changed()), this, SLOT(changedSlot()));	
	changedSlot();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

SlicePlanes3DMarkerIn2DRepPtr SlicePlanes3DMarkerIn2DRep::New(const std::string& uid, const std::string& name)
{
	SlicePlanes3DMarkerIn2DRepPtr retval(new SlicePlanes3DMarkerIn2DRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

SlicePlanes3DMarkerIn2DRep::SlicePlanes3DMarkerIn2DRep(const std::string& uid, const std::string& name) :
	RepImpl(uid, name)
{
}

SlicePlanes3DMarkerIn2DRep::~SlicePlanes3DMarkerIn2DRep()
{
}

void SlicePlanes3DMarkerIn2DRep::addRepActorsToViewRenderer(ssc::View* view)
{
	//Logger::log("vm.log", "SlicePlanes3DMarkerIn2DRep::addRepActorsToViewRenderer");
	SlicePlanesProxy::DataType baseData = mProxy->getData()[mType];
	
	mText.reset(new ssc::TextDisplay("O", baseData.mColor, 28));
	mText->textProperty()->BoldOn();
	mText->setPosition(baseData.mPointPos_normvp);
	mText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	view->getRenderer()->AddActor2D(mText->getActor());
}

void SlicePlanes3DMarkerIn2DRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mText.reset();
}

void SlicePlanes3DMarkerIn2DRep::changedSlot()
{
  if (mText)
    mText->getActor()->SetVisibility(mProxy->getVisible());
}

void SlicePlanes3DMarkerIn2DRep::setProxy(PLANE_TYPE type, SlicePlanesProxyPtr proxy)
{
	//Logger::log("vm.log", "SlicePlanes3DMarkerIn2DRep::setProxy");
	mType = type;
	mProxy = proxy;
	connect(mProxy.get(), SIGNAL(changed()), this, SLOT(changedSlot()));	
	changedSlot();
}


}
