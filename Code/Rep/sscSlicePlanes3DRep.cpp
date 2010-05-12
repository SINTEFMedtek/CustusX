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
	mColors.push_back(Vector3D(0,   1,   1));
	mColors.push_back(Vector3D(0,   0.6, 1));
	mColors.push_back(Vector3D(0.5, 0.5, 1));
}

void SlicePlanesProxy::setViewportData(ssc::VIEW_AREA_TYPE type, ssc::SliceProxyPtr slice, const DoubleBoundingBox3D& vp_s)
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
		
		connect(data.mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceProxyChanged()));				
		mData[type] = data;		
	}
	
	mData[type].vp_s = vp_s;		

	emit changed();
}

SlicePlanesProxy::DataMap SlicePlanesProxy::getData()
{
	return mData;
}

void SlicePlanesProxy::sliceProxyChanged()
{
	emit changed();
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
	RepImpl(uid, name)
{
}

SlicePlanes3DRep::~SlicePlanes3DRep()
{
}

void SlicePlanes3DRep::addRepActorsToViewRenderer(ssc::View* view)
{
	for (DataMap::iterator i=mData.begin(); i!=mData.end(); ++i)
	{
		SlicePlanesProxy::DataType baseData = mProxy->getData()[i->first];
		
		i->second.mText = vtkTextActor3DPtr::New();
		i->second.mText->SetInput("O");		
		i->second.mText->GetTextProperty()->SetColor(baseData.mColor.begin());
		i->second.mText->GetTextProperty()->SetFontSize(20);
		i->second.mText->GetTextProperty()->BoldOn();
		view->getRenderer()->AddActor(i->second.mText);
		i->second.mRect.reset(new Rect3D(view->getRenderer(), baseData.mColor));
	}
}

void SlicePlanes3DRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	for (DataMap::iterator i=mData.begin(); i!=mData.end(); ++i)
	{
		view->getRenderer()->RemoveActor(i->second.mText);
		i->second.mPoint.reset();
		i->second.mRect.reset();
		i->second.mAxes.reset();
	}
}

void SlicePlanes3DRep::changedSlot()
{
	SlicePlanesProxy::DataMap baseData = mProxy->getData();
	
	for (SlicePlanesProxy::DataMap::iterator i=baseData.begin(); i!=baseData.end(); ++i)
	{
		SlicePlanesProxy::DataType& base = i->second;
		DataType& data = mData[i->first];
		
		Transform3D rMs = base.mSliceProxy->get_sMr().inv();
		Transform3D vpMnvp = createTransformNormalize(DoubleBoundingBox3D(0,1,0,1,0,1), base.vp_s);
		Vector3D pos_s = vpMnvp.coord(base.mPointPos_normvp);
	 	Transform3D T = createTransformTranslate(pos_s);
	 	
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
	}		
}

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
}

void SlicePlanes3DMarkerIn2DRep::setProxy(ssc::VIEW_AREA_TYPE type, SlicePlanesProxyPtr proxy)
{
	//Logger::log("vm.log", "SlicePlanes3DMarkerIn2DRep::setProxy");
	mType = type;
	mProxy = proxy;
	connect(mProxy.get(), SIGNAL(changed()), this, SLOT(changedSlot()));	
	changedSlot();
}


}
