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

#include "sscPickerRep.h"

#include "boost/bind.hpp"
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkLineSource.h>
//#include <vtkDoubleArray.h>
#include <vtkProbeFilter.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
//#include <vtkDataSetAttributes.h>
#include <vtkEventQtSlotConnect.h>
//#include <vtkCellPicker.h>
#include <vtkVolumePicker.h>
#include "sscMesh.h"
#include "sscDataManager.h"
#include "sscMessageManager.h"
#include "sscImage.h"
#include "sscView.h"
#include "sscTool.h"
#include "sscToolManager.h"

namespace ssc
{
PickerRepPtr PickerRep::New(const QString& uid, const QString& name)
{
	PickerRepPtr retval(new PickerRep(uid, name));
	retval->mSelf = retval;
	return retval;
}
PickerRep::PickerRep(const QString& uid, const QString& name) :
	RepImpl(uid, name), mPickedPoint(), mSphereRadius(2), mConnections(vtkEventQtSlotConnectPtr::New())
{
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&PickerRep::scaleSphere, this));

	mView = NULL;
	mEnabled = false;
	mConnected = false;
}

void PickerRep::scaleSphere()
{
	if (!mGraphicalPoint)
		return;

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mSphereRadius / 100 / size;
	mGraphicalPoint->setRadius(sphereSize);
}

PickerRep::~PickerRep()
{
}
QString PickerRep::getType() const
{
	return "ssc::PickerRep";
}

void PickerRep::setSphereRadius(double radius)
{
	mSphereRadius = radius;
	if (mGraphicalPoint)
		mGraphicalPoint->setRadius(mSphereRadius);
}

void PickerRep::setTool(ToolPtr tool)
{
	if (tool == mTool)
		return;

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
			SLOT(receiveTransforms(Transform3D, double)));
	}

	mTool = tool;

	if (mTool)
	{
		receiveTransforms(mTool->get_prMt(), 0);

		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
			SLOT(receiveTransforms(Transform3D, double)));
	}
}

typedef vtkSmartPointer<class vtkVolumePicker> vtkVolumePickerPtr;
typedef vtkSmartPointer<class vtkDataSet> vtkDataSetPtr;


/**
 * Trace a ray from clickPosition along the camera view direction and intersect
 * the image.
 * \param[in] clickPosition the click position in DISPLAY coordinates
 * \param[in] renderer the renderer from which to get the camera
 * \return the point where the ray intersects the image
 */
Vector3D PickerRep::pickLandmark(const Vector3D& clickPosition, vtkRendererPtr renderer)
{
	vtkVolumePickerPtr picker = vtkVolumePickerPtr::New();
// default values:
//	picker->SetVolumeOpacityIsovalue(0.05);
//	picker->SetTolerance(1.0E-6);
	int hit = picker->Pick(clickPosition[0], clickPosition[1], 0, renderer);

	// search for picked data in manager, emit uid if found.
	vtkDataSetPtr data = picker->GetDataSet();
//	std::cout << "data " << data << std::endl;
	if (data)
	{
//		data->Print(std::cout);
//		std::cout << "looking for " << data.GetPointer() << std::endl;

		std::map<QString, DataPtr> allData = ssc::dataManager()->getData();
		for (std::map<QString, DataPtr>::iterator iter=allData.begin(); iter!=allData.end(); ++iter)
		{
			ssc::MeshPtr mesh = boost::shared_dynamic_cast<ssc::Mesh>(iter->second);
			if (mesh && mesh->getVtkPolyData()==data)
				emit dataPicked(iter->first);

			ssc::ImagePtr image = boost::shared_dynamic_cast<ssc::Image>(iter->second);
//			if (image)
//				std::cout << "  checking " << image->getBaseVtkImageData().GetPointer() << std::endl;
			if (image && image->getBaseVtkImageData()==data)
				emit dataPicked(iter->first);
		}
	}
//	picker->Print(std::cout);
	ssc::Vector3D pick_w(picker->GetPickPosition());
	mPickedPoint = pick_w;

	if (!hit)
		return pick_w;

	if (mGraphicalPoint)
		mGraphicalPoint->setValue(mPickedPoint);

	emit pointPicked(mPickedPoint);
	return mPickedPoint;
}

void PickerRep::pickLandmarkSlot(vtkObject* renderWindowInteractor)
{
	vtkRenderWindowInteractorPtr iren = vtkRenderWindowInteractor::SafeDownCast(renderWindowInteractor);

	if (iren == NULL)
		return;

	int pickedPoint[2]; //<x,y>
	iren->GetEventPosition(pickedPoint); //mouse positions are measured in pixels

	vtkRendererPtr renderer = this->getRendererFromRenderWindow(*iren);
	if (renderer == NULL)
		return;

	Vector3D clickPoint(pickedPoint[0], pickedPoint[1], 0);
	this->pickLandmark(clickPoint, renderer);
}

void PickerRep::receiveTransforms(Transform3D prMt, double timestamp)
{
	Transform3D rMpr = *ToolManager::getInstance()->get_rMpr();
	Transform3D rMt = rMpr * prMt;
	Vector3D p_r = rMt.coord(ssc::Vector3D(0, 0, mTool->getTooltipOffset()));

	mPickedPoint = p_r;
	if (mGraphicalPoint)
		mGraphicalPoint->setValue(mPickedPoint);

}

void PickerRep::setEnabled(bool on)
{
	if (mEnabled == on)
		return;

	mEnabled = on;

	if (mEnabled)
	{
		this->connectInteractor();
		if (mGraphicalPoint)
			mGraphicalPoint->getActor()->SetVisibility(true);
	}
	else
	{
		this->disconnectInteractor();
		if (mGraphicalPoint)
			mGraphicalPoint->getActor()->SetVisibility(false);
	}
}

void PickerRep::connectInteractor()
{
	if (!mView)
		return;
	if (mConnected)
		return;
	mConnections->Connect(mView->getRenderWindow()->GetInteractor(), vtkCommand::LeftButtonPressEvent, this,
		SLOT(pickLandmarkSlot(vtkObject*)));
	mConnected = true;
}

void PickerRep::disconnectInteractor()
{
	if (!mView)
		return;
	if (!mConnected)
		return;
	mConnections->Disconnect(mView->getRenderWindow()->GetInteractor(), vtkCommand::LeftButtonPressEvent, this,
		SLOT(pickLandmarkSlot(vtkObject*)));
	mConnected = false;
}

void PickerRep::addRepActorsToViewRenderer(ViewBase *view)
{
	if (view == NULL)
	{
		messageManager()->sendDebug("Cannot add rep actor to a NULL view.");
		return;
	}

	if (mEnabled)
		this->connectInteractor();

	mView = view;

	mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));
	mGraphicalPoint->setColor(ssc::Vector3D(0, 0, 1));
	mGraphicalPoint->setRadius(mSphereRadius);
	mGraphicalPoint->getActor()->SetVisibility(false);

	mViewportListener->startListen(mView->getRenderer());
	this->scaleSphere();
}

void PickerRep::removeRepActorsFromViewRenderer(ViewBase *view)
{
	if (view == NULL)
		return;

	this->disconnectInteractor();
	mViewportListener->stopListen();
	mGraphicalPoint.reset();
	mView = NULL;
}

vtkRendererPtr PickerRep::getRendererFromRenderWindow(vtkRenderWindowInteractor& iren)
{
	vtkRendererPtr renderer = NULL;
	std::set<ssc::ViewBase *>::const_iterator it = mViews.begin();
	for (; it != mViews.end(); ++it)
	{
		if (iren.GetRenderWindow() == (*it)->getRenderWindow())
			renderer = (*it)->getRenderer();
	}
	return renderer;
}

Vector3D PickerRep::getPosition() const
{
	return mPickedPoint;
}

}//namespace ssc
