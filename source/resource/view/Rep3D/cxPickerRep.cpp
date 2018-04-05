/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxPickerRep.h"

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
#include <vtkCallbackCommand.h>
#include <vtkPolyDataMapper.h>
//#include <vtkDataSetAttributes.h>
#include <vtkEventQtSlotConnect.h>
//#include <vtkCellPicker.h>
#include "cxMesh.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxImage.h"
#include "cxView.h"
#include "cxTool.h"
#include "cxRegistrationTransform.h"
#include "cxGeometricRep.h"
#include <vtkRenderWindowInteractor.h>
#include "cxLogger.h"
#include "vtkVolumePicker.h"

#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
	#include "vtkMultiVolumePicker.h"
		typedef vtkSmartPointer<class vtkMultiVolumePicker> vtkMultiVolumePickerPtr;
//		typedef vtkSmartPointer<class vtkVolumePicker> vtkMultiVolumePickerPtr;
#else
    typedef vtkSmartPointer<class vtkVolumePicker> vtkMultiVolumePickerPtr;
#endif

namespace cx
{
PickerRepPtr PickerRep::New(PatientModelServicePtr dataManager, const QString& uid)
{
	return wrap_new(new PickerRep(dataManager), uid);
}
PickerRep::PickerRep(PatientModelServicePtr dataManager) :
	RepImpl(),
	mDataManager(dataManager),
	mPickedPoint(), mSphereRadius(2) //, mConnections(vtkEventQtSlotConnectPtr::New())
{
	mIsDragging = false;
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&PickerRep::scaleSphere, this));

	this->mCallbackCommand = vtkCallbackCommandPtr::New();
	this->mCallbackCommand->SetClientData(this);
	this->mCallbackCommand->SetCallback(PickerRep::ProcessEvents);

	mEnabled = true;
	mConnected = false;
	mSnapToSurface = false;
}

void PickerRep::scaleSphere()
{
	if (!mGraphicalPoint)
		return;

	double size = mViewportListener->getVpnZoom(mPickedPoint);
	double sphereSize = mSphereRadius / 100 / size;
	mGraphicalPoint->setRadius(sphereSize);
}

PickerRep::~PickerRep()
{
}
QString PickerRep::getType() const
{
	return "PickerRep";
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
				SLOT(setModified()));
	}

	mTool = tool;

	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
				SLOT(setModified()));
		this->setModified();
	}
}

void PickerRep::setGlyph(MeshPtr glyph)
{
	if (!mGlyph)
		mGlyph = glyph;

	 if (!mGlyphRep)
	 {
		 mGlyphRep = GeometricRep::New("PickerGlyphRep");
		 if (this->getView())
		 {
			 this->getView()->addRep(mGlyphRep);
		 }
	 }

	 mGlyphRep->setMesh(mGlyph);
}

typedef vtkSmartPointer<class vtkDataSet> vtkDataSetPtr;

/**
 * Trace a ray from clickPosition along the camera view direction and intersect
 * the image.
 * \param[in] clickPosition the click position in DISPLAY coordinates
 * \param[in] renderer the renderer from which to get the camera
 * \return the point where the ray intersects the image
 */
void PickerRep::pickLandmark(const Vector3D& clickPosition, vtkRendererPtr renderer)
{
	if (!this->mEnabled)
		return;
	vtkMultiVolumePickerPtr picker = vtkMultiVolumePickerPtr::New();
	int hit = picker->Pick(clickPosition[0], clickPosition[1], 0, renderer);
	if (!hit)
	{
		mIsDragging = false;
		return;
	}

	// search for picked data in manager, emit uid if found.
	vtkDataSetPtr data = picker->GetDataSet();
	if (data)
	{
		std::map<QString, DataPtr> allData = mDataManager->getDatas();
		for (std::map<QString, DataPtr>::iterator iter = allData.begin(); iter != allData.end(); ++iter)
		{
			MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(iter->second);
			if (mesh && mesh->getVtkPolyData() == data)
				emit dataPicked(iter->first);

			ImagePtr image = boost::dynamic_pointer_cast<Image>(iter->second);
			if (image && image->getBaseVtkImageData() == data)
				emit dataPicked(iter->first);
		}
	}

	Vector3D pick_w(picker->GetPickPosition());

	if ( data &&
		((mGraphicalPoint && (data == mGraphicalPoint->getPolyData() ))
	   ||(mGlyph          && (data == mGlyph->getVtkPolyData()       ))
	   ||(mTool           && (data == mTool->getGraphicsPolyData()   )))
	   )
	{
		// We have clicked the picker/tool itself.
		// Store click pos and wait for dragging.
		mClickedPoint = pick_w;
		mIsDragging = true;
		mCallbackCommand->SetAbortFlag(1); // abort this event: interactor does not receive it.
		return;
	}
	else
	{
		mIsDragging = false;
	}

	if (hit && mSnapToSurface)
	{
		mPickedPoint = pick_w;

		if (mGraphicalPoint)
			mGraphicalPoint->setValue(mPickedPoint);
		this->setGlyphCenter(mPickedPoint);

		emit pointPicked(mPickedPoint);
	}
}

void PickerRep::pickLandmarkSlot(vtkObject* renderWindowInteractor)
{
	vtkRenderWindowInteractorPtr iren = vtkRenderWindowInteractor::SafeDownCast(renderWindowInteractor);

	if (iren == NULL)
		return;

	int pickedPoint[2]; //<x,y>
	iren->GetEventPosition(pickedPoint); //mouse positions are measured in pixels

	vtkRendererPtr renderer = this->getRenderer();
	if (renderer == NULL)
		return;

	Vector3D clickPoint(pickedPoint[0], pickedPoint[1], 0);

	this->pickLandmark(clickPoint, renderer);
}

void PickerRep::onModifiedStartRender()
{
	this->toolHasChanged();
}

void PickerRep::toolHasChanged()
{
	if (!mTool)
		return;
	Transform3D prMt = mTool->get_prMt();
	Transform3D rMpr = mDataManager->get_rMpr();
	Transform3D rMt = rMpr * prMt;
	Vector3D p_r = rMt.coord(Vector3D(0, 0, mTool->getTooltipOffset()));

	mPickedPoint = p_r;
	if (mGraphicalPoint)
		mGraphicalPoint->setValue(mPickedPoint);
	this->setGlyphCenter(mPickedPoint);
}

void PickerRep::setEnabled(bool on)
{
	mEnabled = on;
	if (mSnapToSurface == on)
		return;

	mSnapToSurface = on;

	if (mSnapToSurface)
	{
		if (mGraphicalPoint)
			mGraphicalPoint->getActor()->SetVisibility(true);
	}
	else
	{
		if (mGraphicalPoint)
			mGraphicalPoint->getActor()->SetVisibility(false);
	}
}

void PickerRep::ProcessEvents(vtkObject* vtkNotUsed(object), unsigned long event, void* clientdata,
		void* vtkNotUsed(calldata))
{
	PickerRep* self = reinterpret_cast<PickerRep *>(clientdata);

	//okay, let's do the right thing
	switch (event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->OnLeftButtonDown();
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;
	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;
	}
}

/**Convert a point in display to world.
 * Based on method in vtkInteractorObserver
 */
Vector3D PickerRep::ComputeDisplayToWorld(Vector3D p_d)
{
	double worldPt[4];
	vtkRendererPtr ren = this->getRenderer();
	ren->SetDisplayPoint(p_d.data());
	ren->DisplayToWorld();
	ren->GetWorldPoint(worldPt);
	return Vector3D(worldPt)/worldPt[3];
}

/**Convert a point in world to display
 * Based on method in vtkInteractorObserver
 */
Vector3D PickerRep::ComputeWorldToDisplay(Vector3D p_w)
{
	Vector3D p_d;
	vtkRendererPtr ren = this->getRenderer();
	ren->SetWorldPoint(p_w[0], p_w[1], p_w[2], 1.0);
	ren->WorldToDisplay();
	ren->GetDisplayPoint(p_d.data());
	return p_d;
}

/** Get the displacement in the view plane since last event.
 */
Vector3D PickerRep::getDisplacement()
{
	vtkRenderWindowInteractorPtr interactor = this->getView()->getRenderWindow()->GetInteractor();

//	// find previous pos in world and display:
	Vector3D p_prev_w = mClickedPoint;
//	std::cout << "  p_prev_w  = \t" << p_prev_w << std::endl;
	Vector3D p_prev_d = this->ComputeWorldToDisplay(p_prev_w);
//	std::cout << "  p_prev_d  = \t" << p_prev_d << std::endl;

	// find current pos in world and display, set z-pos in d equal to previous z-pos:
	Vector3D p_current_d(interactor->GetEventPosition()[0], interactor->GetEventPosition()[1], p_prev_d[2]);
//	std::cout << "  p_current_d = \t" << p_current_d << std::endl;
	Vector3D p_current_w = this->ComputeDisplayToWorld(p_current_d);
//	std::cout << "  p_current_w  = \t" << p_current_w << std::endl;

	// both positions are now in the camera focal plane: the diff lies in the view plane.
//	std::cout << "  diff_d   = \t" << p_current_d - p_prev_d << std::endl;
//	std::cout << "  diff_w   = \t" << p_current_w - p_prev_w << std::endl;
	return p_current_w - p_prev_w;
}

void PickerRep::OnLeftButtonDown()
{
	this->pickLandmarkSlot(this->getView()->getRenderWindow()->GetInteractor());
}

void PickerRep::OnMouseMove()
{
	if (mIsDragging)
	{
		mPickedPoint += this->getDisplacement();
		mClickedPoint = mPickedPoint;

		if (mGraphicalPoint)
			mGraphicalPoint->setValue(mPickedPoint);
		this->setGlyphCenter(mPickedPoint);
		emit pointPicked(mPickedPoint);

		mCallbackCommand->SetAbortFlag(1);
	}
}

void PickerRep::setGlyphCenter(Vector3D pos)
{
	if (mGlyph)
	{
		mGlyph->get_rMd_History()->setRegistration(createTransformTranslate(pos));
	}
}


void PickerRep::OnLeftButtonUp()
{
	if (mIsDragging)
	{
		mIsDragging = false;
		mCallbackCommand->SetAbortFlag(1); // abort this event: interactor does not receive it.
	}
}

void PickerRep::connectInteractor()
{
	if (!this->getView())
		return;
	if (mConnected)
		return;

	vtkRenderWindowInteractorPtr i = this->getView()->getRenderWindow()->GetInteractor();
	i->AddObserver(vtkCommand::MouseMoveEvent, this->mCallbackCommand, 1.0);
	i->AddObserver(vtkCommand::LeftButtonPressEvent, this->mCallbackCommand, 1.0);
	i->AddObserver(vtkCommand::LeftButtonReleaseEvent, this->mCallbackCommand, 1.0);

	mConnected = true;
}

void PickerRep::disconnectInteractor()
{
	if (!this->getView())
		return;
	if (!mConnected)
		return;

	// don't listen for events any more
	this->getView()->getRenderWindow()->GetInteractor()->RemoveObserver(this->mCallbackCommand);

	mConnected = false;
}

void PickerRep::addRepActorsToViewRenderer(ViewPtr view)
{
	if (view == NULL)
	{
		reportDebug("Cannot add rep actor to a NULL view.");
		return;
	}

	this->connectInteractor();

	mGraphicalPoint.reset(new GraphicalPoint3D(this->getRenderer()));
	mGraphicalPoint->setColor(QColor(Qt::blue));
	mGraphicalPoint->setRadius(mSphereRadius);
	mGraphicalPoint->getActor()->SetVisibility(mSnapToSurface);

	// show even if disabled
	if (mGlyphRep)
	{
		this->getView()->addRep(mGlyphRep);
	}

	mViewportListener->startListen(this->getRenderer());
	this->scaleSphere();
}

void PickerRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	if (view == NULL)
		return;

	this->disconnectInteractor();
	mViewportListener->stopListen();
	mGraphicalPoint.reset();

	if (mGlyphRep)
		view->removeRep(mGlyphRep);
}

Vector3D PickerRep::getPosition() const
{
	return mPickedPoint;
}

} //namespace cx
