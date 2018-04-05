/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxSlicePlanes3DRep.h"

#include <boost/bind.hpp>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextActor3D.h>

#include "cxView.h"
#include "cxSliceProxy.h"
#include "cxVtkHelperClasses.h"
#include "cxTypeConversions.h"

namespace cx
{

SlicePlanesProxy::SlicePlanesProxy()
{
	mConnectedTo3D = false;
	mVisible = true;
	mDrawPlane = false;

	QColor color1 = QColor::fromRgbF(0, 1, 1);
	QColor color2 = QColor::fromRgbF(0, 0.6, 1);
	QColor color3 = QColor::fromRgbF(0.5, 0.5, 1);
    QColor color4 = QColor::fromRgbF(0.75, 0.75, 1);

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

    mProperties.mColor[ptTOOLSIDEPLANE] = color4;
    mProperties.mSymbol[ptTOOLSIDEPLANE] = "T";

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

void SlicePlanesProxy::setViewportData(PLANE_TYPE type, SliceProxyPtr slice, const DoubleBoundingBox3D& vp_s)
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

void SlicePlanesProxy::addSimpleSlicePlane(PLANE_TYPE type, PatientModelServicePtr dataManager)
{
	SliceProxyPtr slice = SliceProxy::create(dataManager);
	slice->initializeFromPlane(type, false, true, 1, 0.25);

	this->setViewportData(type, slice, DoubleBoundingBox3D(0, 1, 0, 1, 0, 1));
}

SlicePlanesProxy::DataMap SlicePlanesProxy::getData()
{
	return mData;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


SlicePlanes3DRepPtr SlicePlanes3DRep::New(const QString& uid)
{
	return wrap_new(new SlicePlanes3DRep(), uid);
}

SlicePlanes3DRep::SlicePlanes3DRep() :
	RepImpl()
{
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
		mViewportListener.reset(new ViewportListener);
		mViewportListener->setCallback(boost::bind(&SlicePlanes3DRep::rescale, this));
	}
	else
	{
		mViewportListener.reset();
	}
}

void SlicePlanes3DRep::addRepActorsToViewRenderer(ViewPtr view)
{
	this->changedSlot();
	if (mViewportListener)
		mViewportListener->startListen(view->getRenderer());
}

void SlicePlanes3DRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	if (mViewportListener)
		mViewportListener->stopListen();
	this->clearActors();
}

void SlicePlanes3DRep::clearActors()
{
	if (!this->getView())
		return;

	for (DataMap::iterator i = mData.begin(); i != mData.end(); ++i)
	{
		this->getRenderer()->RemoveActor(i->second.mText);
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
	if (!this->getView())
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
			data.mText->GetTextProperty()->SetColor(getColorAsVector3D(base.mColor).begin());
			data.mText->GetTextProperty()->SetFontSize(mProxy->getProperties().m3DFontSize);
			data.mText->GetTextProperty()->BoldOn();
			data.mText->GetTextProperty()->SetVerticalJustificationToBottom();
			data.mText->GetTextProperty()->SetJustificationToLeft();
			data.mText->GetTextProperty()->ShadowOff();
			this->getRenderer()->AddActor(data.mText);
		}
		if (!data.mRect)
		{
			data.mRect.reset(new Rect3D(this->getRenderer(), base.mColor));
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
				Vector3D focus = rMs.translation();
				double size = mViewportListener->getVpnZoom(focus);
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

SlicePlanes3DMarkerIn2DRepPtr SlicePlanes3DMarkerIn2DRep::New(const QString& uid)
{
	return wrap_new(new SlicePlanes3DMarkerIn2DRep(), uid);
}

SlicePlanes3DMarkerIn2DRep::SlicePlanes3DMarkerIn2DRep() :
	RepImpl()
{
}

SlicePlanes3DMarkerIn2DRep::~SlicePlanes3DMarkerIn2DRep()
{
}

void SlicePlanes3DMarkerIn2DRep::addRepActorsToViewRenderer(ViewPtr view)
{
	SlicePlanesProxy::DataType baseData = mProxy->getData()[mType];

	mText.reset(new TextDisplay(baseData.mSymbol, baseData.mColor, mProxy->getProperties().m2DFontSize));
	mText->textProperty()->BoldOn();
	mText->textProperty()->SetVerticalJustificationToTop();
	mText->textProperty()->SetJustificationToLeft();
	mText->setPosition(baseData.mPointPos_normvp);
	mText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	view->getRenderer()->AddActor2D(mText->getActor());
	this->changedSlot();
}

void SlicePlanes3DMarkerIn2DRep::removeRepActorsFromViewRenderer(ViewPtr view)
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
