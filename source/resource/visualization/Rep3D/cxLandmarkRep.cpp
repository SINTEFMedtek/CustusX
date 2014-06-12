// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
#include "cxView.h"
#include "cxLandmark.h"
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxTypeConversions.h"
#include "boost/bind.hpp"
#include "cxVtkHelperClasses.h"

namespace cx
{

PatientLandmarksSource::PatientLandmarksSource(DataServicePtr dataManager) : mDataManager(dataManager)
{
	connect(mDataManager->getPatientLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(changed()));
	connect(mDataManager->getPatientLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(changed()));
	connect(mDataManager.get(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
}
LandmarkMap PatientLandmarksSource::getLandmarks() const
{
	return mDataManager->getPatientLandmarks()->getLandmarks();
}
Transform3D PatientLandmarksSource::get_rMl() const
{
	return mDataManager->get_rMpr();
}
// --------------------------------------------------------
Vector3D PatientLandmarksSource::getTextPos(Vector3D p_l) const
{
	return p_l;
}
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

ImageLandmarksSource::ImageLandmarksSource()
{
}

DataPtr ImageLandmarksSource::getData()
{
	return mData;
}

void ImageLandmarksSource::setData(DataPtr image)
{
	if (image == mData)
		return;

	if (mData)
	{
		disconnect(mData->getLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(changed()));
		disconnect(mData->getLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(changed()));
		disconnect(mData.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
	}

	mData = image;

	if (mData)
	{
		connect(mData->getLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(changed()));
		connect(mData->getLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(changed()));
		connect(mData.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
	}

	emit changed();
}

LandmarkMap ImageLandmarksSource::getLandmarks() const
{
	if (!mData)
		return LandmarkMap();
	return mData->getLandmarks()->getLandmarks();
}

Transform3D ImageLandmarksSource::get_rMl() const
{
	if (!mData)
		return Transform3D::Identity();
	return mData->get_rMd();
}

Vector3D ImageLandmarksSource::getTextPos(Vector3D p_l) const
{
	Vector3D imageCenter = mData->boundingBox().center();
	Vector3D centerToSkinVector = (p_l - imageCenter).normal();
	Vector3D numberPosition = p_l + 10.0 * centerToSkinVector;
	return numberPosition;
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

LandmarkRepPtr LandmarkRep::New(DataServicePtr dataManager, const QString& uid, const QString& name)
{
	LandmarkRepPtr retval(new LandmarkRep(dataManager, uid, name));
	retval->mSelf = retval;
	return retval;
}

LandmarkRep::LandmarkRep(DataServicePtr dataManager, const QString& uid, const QString& name) :
	RepImpl(uid, name),
	mDataManager(dataManager),
	mInactiveColor(QColor::fromRgbF(0.5,0.5,0.5)),
	mColor(QColor(Qt::green)),
				//  mSecondaryColor(0,0.6,0.8),
	mSecondaryColor(QColor::fromRgbF(0, 0.9, 0.5)),
	mShowLandmarks(true),
	mGraphicsSize(1),
	mLabelSize(2.5)
{
	connect(mDataManager.get(), SIGNAL(landmarkPropertiesChanged()), this, SLOT(internalUpdate()));

	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&LandmarkRep::rescale, this));
}

LandmarkRep::~LandmarkRep()
{
}

void LandmarkRep::setPrimarySource(LandmarksSourcePtr primary)
{
//  std::cout << this <<  "  LandmarkRep::setPrimarySource " << primary.get() << std::endl;

	if (mPrimary)
		disconnect(mPrimary.get(), SIGNAL(changed()), this, SLOT(internalUpdate()));
	mPrimary = primary;
	if (mPrimary)
		connect(mPrimary.get(), SIGNAL(changed()), this, SLOT(internalUpdate()));

	this->internalUpdate();
}

void LandmarkRep::setSecondarySource(LandmarksSourcePtr secondary)
{
	if (mSecondary)
		disconnect(mSecondary.get(), SIGNAL(changed()), this, SLOT(internalUpdate()));
	mSecondary = secondary;
	if (mSecondary)
		connect(mSecondary.get(), SIGNAL(changed()), this, SLOT(internalUpdate()));

	this->internalUpdate();
}

void LandmarkRep::setColor(QColor color)
{
	mColor = color;
	this->internalUpdate();
}

void LandmarkRep::setSecondaryColor(QColor color)
{
	mSecondaryColor = color;
	this->internalUpdate();
}

void LandmarkRep::setGraphicsSize(double size)
{
	mGraphicsSize = size;
	this->internalUpdate();
}

void LandmarkRep::setLabelSize(double size)
{
	mLabelSize = size;
	this->internalUpdate();
}

void LandmarkRep::showLandmarks(bool on)
{
	if (on == mShowLandmarks)
		return;

	for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
	{
		if (iter->second.mPrimaryPoint)
			iter->second.mPrimaryPoint->getActor()->SetVisibility(on);
		if (iter->second.mSecondaryPoint)
			iter->second.mSecondaryPoint->getActor()->SetVisibility(on);
		if (iter->second.mText)
			iter->second.mText->getActor()->SetVisibility(on);
		if (iter->second.mLine)
			iter->second.mLine->getActor()->SetVisibility(on);
	}
	mShowLandmarks = on;
}

void LandmarkRep::addAll()
{
//  std::cout << this << " LandmarkRep::addLandmark ADD ALL" << std::endl;

	LandmarkPropertyMap props = mDataManager->getLandmarkProperties();

	for (LandmarkPropertyMap::iterator it = props.begin(); it != props.end(); ++it)
	{
		this->addLandmark(it->first);
	}
}

void LandmarkRep::internalUpdate()
{
//  std::cout << this << " LandmarkRep::internalUpdate()" << std::endl;

	this->clearAll();
	this->addAll();
}

void LandmarkRep::clearAll()
{
//  std::cout << this << " LandmarkRep::addLandmark CLEAR ALL" << std::endl;
	mGraphics.clear();
}

void LandmarkRep::addRepActorsToViewRenderer(View* view)
{
	if (!view || !view->getRenderer())
		return;

	this->addAll();
	mViewportListener->startListen(view->getRenderer());
}

void LandmarkRep::removeRepActorsFromViewRenderer(View* view)
{
	this->clearAll();
	mViewportListener->stopListen();
}

/**
 * Use the inpup coord in ref space to render a landmark.
 */
void LandmarkRep::addLandmark(QString uid)
{
//  std::cout << this << " LandmarkRep::addLandmark init" << uid << std::endl;
	vtkRendererPtr renderer = this->getRenderer();

	LandmarkProperty property = mDataManager->getLandmarkProperties()[uid];
	if (property.getUid().isEmpty())
	{
//    std::cout << "LandmarkRep::addLandmark CLEAR" << uid << std::endl;
		mGraphics.erase(uid);
		return;
	}

	double radius = 2;
	QColor color = mColor;
	QColor secondaryColor = mSecondaryColor;

	if (!property.getActive())
	{
		radius = 1;
		color = mInactiveColor;
		secondaryColor = mInactiveColor;
	}

	LandmarkGraphics current;

	// primary point
	Landmark primary;
	Vector3D primary_r(0, 0, 0);
	if (mPrimary)
	{
//    std::cout << this << "   LandmarkRep::addLandmark found mPrimary" << uid << std::endl;
		primary = mPrimary->getLandmarks()[uid];
		if (!primary.getUid().isEmpty())
		{
//      std::cout << this << "  LandmarkRep::addLandmark" << uid << std::endl;

			primary_r = mPrimary->get_rMl().coord(primary.getCoord());

			current.mPrimaryPoint.reset(new GraphicalPoint3D(renderer));
			current.mPrimaryPoint->setColor(color);
			current.mPrimaryPoint->setRadius(radius);

			current.mText.reset(new FollowerText3D(renderer));
			current.mText->setText(property.getName());
			current.mText->setSizeInNormalizedViewport(true, 0.025);
			current.mText->setColor(color);

			Vector3D text_r = mPrimary->get_rMl().coord(mPrimary->getTextPos(primary.getCoord()));

			current.mPrimaryPoint->setValue(primary_r);
			current.mText->setPosition(text_r);
		}
	}

	// secondary point
	Vector3D secondary_r(0, 0, 0);
	Landmark secondary;
	if (mSecondary)
	{
		secondary = mSecondary->getLandmarks()[uid];
		if (!secondary.getUid().isEmpty())
		{
			secondary_r = mSecondary->get_rMl().coord(secondary.getCoord());

			current.mSecondaryPoint.reset(new GraphicalPoint3D(renderer));
			current.mSecondaryPoint->setColor(secondaryColor);
			current.mSecondaryPoint->setRadius(radius);
			current.mSecondaryPoint->setValue(secondary_r);
		}
	}

	// connecting line
	if (!secondary.getUid().isEmpty() && !secondary.getUid().isEmpty())
	{
		current.mLine.reset(new GraphicalLine3D(renderer));
		current.mLine->setColor(secondaryColor);
		current.mLine->setStipple(0x0F0F);

		current.mLine->setValue(primary_r, secondary_r);
	}

	mGraphics[uid] = current;
	this->rescale();
}

void LandmarkRep::rescale()
{
	if (!mViewportListener->isListening())
		return;
	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;

	for (LandmarkGraphicsMapType::iterator iter = mGraphics.begin(); iter != mGraphics.end(); ++iter)
	{
		if (iter->second.mSecondaryPoint)
			iter->second.mSecondaryPoint->setRadius(sphereSize);
		if (iter->second.mPrimaryPoint)
			iter->second.mPrimaryPoint->setRadius(sphereSize);
	}
}

} //namespace cx
