// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscSlicePlanes3DRep.h"

#include <boost/bind.hpp>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextActor3D.h>

#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscVtkHelperClasses.h"
#include "sscTypeConversions.h"

namespace ssc
{

SlicePlanesProxy::SlicePlanesProxy()
{
	mConnectedTo3D = false;
	mVisible = true;
	mDrawPlane = false;

	Vector3D color1(0, 1, 1);
	Vector3D color2(0, 0.6, 1);
	Vector3D color3(0.5, 0.5, 1);

	mProperties.mColor[ptAXIAL] = color1;
	mProperties.mSymbol[ptAXIAL] = "A";

	mProperties.mColor[ptCORONAL] = color2;
	mProperties.mSymbol[ptCORONAL] = "C";

	mProperties.mColor[ptSAGITTAL] = color3;
	mProperties.mSymbol[ptSAGITTAL] = "S";

	mProperties.mColor[ptANYPLANE] = color1;
	mProperties.mSymbol[ptANYPLANE] = "O";

	mProperties.mColor[ptSIDEPLANE] = color2;
	mProperties.mSymbol[ptSIDEPLANE] = "|";

	mProperties.mColor[ptRADIALPLANE] = color3;
	mProperties.mSymbol[ptRADIALPLANE] = "X";

	mProperties.m2DFontSize = 20;
	mProperties.m3DFontSize = 28;
//	mProperties.mPointPos_normvp = Vector3D(0.1, 0.8, 0.0);
	mProperties.mPointPos_normvp = Vector3D(0.05, 0.95, 0.0);
	mProperties.mClipPlane = ptANYPLANE;
	mProperties.mLineWidth = 2;
	//  mProperties.mDrawPlane = false;
}

void SlicePlanesProxy::connectTo3D(bool on)
{
	mConnectedTo3D = on;
}

void SlicePlanesProxy::clearViewports()
{
	mData.clear();
}

void SlicePlanesProxy::setVisible(bool visible)
{
	mVisible = visible;
	emit changed();
}

bool SlicePlanesProxy::getVisible() const
{
	return mVisible && mConnectedTo3D;
}

void SlicePlanesProxy::setDrawPlanes(bool on)
{
	mDrawPlane = on;
	emit changed();
}

bool SlicePlanesProxy::getDrawPlanes() const
{
	return mDrawPlane;
}

void SlicePlanesProxy::setViewportData(PLANE_TYPE type, ssc::SliceProxyPtr slice, const DoubleBoundingBox3D& vp_s)
{
	if (!slice)
		return;

	if (!mData.count(type))
	{
		DataType data;
		data.mPointPos_normvp = mProperties.mPointPos_normvp;
		data.vp_s = vp_s;
		data.mSliceProxy = slice;
		data.mColor = mProperties.mColor[type];
		data.mSymbol = mProperties.mSymbol[type];

		connect(data.mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SIGNAL(changed()));
		mData[type] = data;
	}

	mData[type].vp_s = vp_s;

	emit changed();
}

void SlicePlanesProxy::addSimpleSlicePlane(PLANE_TYPE type)
{
	SliceProxyPtr slice = SliceProxy::New("sliceproxy_(" + qstring_cast(type) + ")");
	slice->initializeFromPlane(type, false, ssc::Vector3D(0, 0, 1), true, 1, 0.25);

	this->setViewportData(type, slice, ssc::DoubleBoundingBox3D(0, 1, 0, 1, 0, 1));
}

SlicePlanesProxy::DataMap SlicePlanesProxy::getData()
{
	return mData;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


SlicePlanes3DRepPtr SlicePlanes3DRep::New(const QString& uid, const QString& name)
{
	SlicePlanes3DRepPtr retval(new SlicePlanes3DRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

SlicePlanes3DRep::SlicePlanes3DRep(const QString& uid, const QString& name) :
	RepImpl(uid, name), mView(NULL)
{
//	this->setDynamicLabelSize(true);
}

SlicePlanes3DRep::~SlicePlanes3DRep()
{
	if (mProxy)
		mProxy->connectTo3D(false);
}

/** Set this to get a label size that keeps a
 *  maximum size relative to the viewport.
 *
 */
void SlicePlanes3DRep::setDynamicLabelSize(bool on)
{
	if (on)
	{
		mViewportListener.reset(new ssc::ViewportListener);
		mViewportListener->setCallback(boost::bind(&SlicePlanes3DRep::rescale, this));
	}
	else
	{
		mViewportListener.reset();
	}
}

void SlicePlanes3DRep::addRepActorsToViewRenderer(ssc::View* view)
{
	mView = view;
	this->changedSlot();
	if (mViewportListener)
		mViewportListener->startListen(view->getRenderer());
}

void SlicePlanes3DRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
	if (mViewportListener)
		mViewportListener->stopListen();
	this->clearActors();
	mView = NULL;
}

void SlicePlanes3DRep::clearActors()
{
	if (!mView)
		return;

	for (DataMap::iterator i = mData.begin(); i != mData.end(); ++i)
	{
		mView->getRenderer()->RemoveActor(i->second.mText);
		i->second.mPoint.reset();
		i->second.mRect.reset();
		i->second.mAxes.reset();
	}
	mData.clear();
}

void SlicePlanes3DRep::rescale()
{
	this->changedSlot();
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

	for (SlicePlanesProxy::DataMap::iterator i = baseData.begin(); i != baseData.end(); ++i)
	{
		SlicePlanesProxy::DataType& base = i->second;
		DataType& data = mData[i->first];

		if (!data.mText)
		{
			data.mText = vtkTextActor3DPtr::New();
			data.mText->SetInput(cstring_cast(base.mSymbol));
			data.mText->GetTextProperty()->SetColor(base.mColor.begin());
			data.mText->GetTextProperty()->SetFontSize(mProxy->getProperties().m3DFontSize);
			data.mText->GetTextProperty()->BoldOn();
			data.mText->GetTextProperty()->SetVerticalJustificationToBottom();
			data.mText->GetTextProperty()->SetJustificationToLeft();
			data.mText->GetTextProperty()->ShadowOff();
			mView->getRenderer()->AddActor(data.mText);
		}
		if (!data.mRect)
		{
			data.mRect.reset(new Rect3D(mView->getRenderer(), base.mColor));
		}

		Transform3D rMs = base.mSliceProxy->get_sMr().inv();
		Transform3D vpMnvp = createTransformNormalize(DoubleBoundingBox3D(0, 1, 0, 1, 0, 1), base.vp_s);
		Vector3D pos_s = vpMnvp.coord(base.mPointPos_normvp);
		Transform3D T = createTransformTranslate(pos_s);

		double scale = 1.0;
		if (data.mText)
		{
			if (mViewportListener)
			{
				double size = mViewportListener->getVpnZoom();
				double planeSize = (i->second.vp_s.range()[0] + i->second.vp_s.range()[1]) / 2.0;
				double sphereSize = std::min(0.1 / size, planeSize/5); // set to 20% of 2D plane size, but constrain upwards to 0.1/s.
				sphereSize = sphereSize/50;
				data.mText->GetTextProperty()->SetFontSize(mProxy->getProperties().m3DFontSize);
//				std::cout << "set font size " << sphereSize << ", s=" << size << ", plane="<< planeSize <<  std::endl;
				scale = sphereSize;
			}
		}

		if (data.mText)
		{
			Transform3D T2 = createTransformTranslate(Vector3D(0,-mProxy->getProperties().m3DFontSize,0));
			Transform3D S = createTransformScale(Vector3D(scale,scale,scale));
//			data.mText->SetUserMatrix((rMs * T).getVtkMatrix());
			data.mText->SetUserMatrix((rMs * T * S * T2).getVtkMatrix());
		}

		if (data.mPoint)
		{
			data.mPoint->setColor(base.mColor);
			data.mPoint->setValue(rMs.coord(pos_s));
		}
		if (data.mRect)
		{
			data.mRect->updatePosition(base.vp_s, rMs);
			data.mRect->setLine(mProxy->getProperties().mLineWidth != 0, mProxy->getProperties().mLineWidth);
			data.mRect->setSurface(mProxy->getDrawPlanes());
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
	mProxy->connectTo3D(true);
	connect(mProxy.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
	changedSlot();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

SlicePlanes3DMarkerIn2DRepPtr SlicePlanes3DMarkerIn2DRep::New(const QString& uid, const QString& name)
{
	SlicePlanes3DMarkerIn2DRepPtr retval(new SlicePlanes3DMarkerIn2DRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

SlicePlanes3DMarkerIn2DRep::SlicePlanes3DMarkerIn2DRep(const QString& uid, const QString& name) :
	RepImpl(uid, name)
{
}

SlicePlanes3DMarkerIn2DRep::~SlicePlanes3DMarkerIn2DRep()
{
}

void SlicePlanes3DMarkerIn2DRep::addRepActorsToViewRenderer(ssc::View *view)
{
	SlicePlanesProxy::DataType baseData = mProxy->getData()[mType];

	mText.reset(new ssc::TextDisplay(baseData.mSymbol, baseData.mColor, mProxy->getProperties().m2DFontSize));
	mText->textProperty()->BoldOn();
	mText->textProperty()->SetVerticalJustificationToTop();
	mText->textProperty()->SetJustificationToLeft();
	mText->setPosition(baseData.mPointPos_normvp);
	mText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	view->getRenderer()->AddActor2D(mText->getActor());
	this->changedSlot();
}

void SlicePlanes3DMarkerIn2DRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
	view->getRenderer()->RemoveActor(mText->getActor());
	mText.reset();
}

void SlicePlanes3DMarkerIn2DRep::changedSlot()
{
	if (mText)
	{
		mText->getActor()->SetVisibility(mProxy->getVisible() && mProxy->getData().count(mType));
		//    std::cout << "SlicePlanes3DMarkerIn2DRep::changedSlot() " << this << " " << mProxy.get() << " - " << mProxy->getVisible() << " " << mProxy->getData().count(mType) << std::endl;
	}

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
