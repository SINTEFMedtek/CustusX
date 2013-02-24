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

#include "cxViewGroup.h"

#include <vector>
#include <QtGui>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscToolRep2D.h"
#include "sscUtilHelpers.h"
#include "sscSlicePlanes3DRep.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxToolManager.h"
#include "cxViewWrapper2D.h"
#include "cxViewManager.h"
#include "cxCameraControl.h"
#include "sscData.h"
#include "cxViewWrapper.h"
#include "sscManualTool.h"

namespace cx
{

/**Place the global center to the center of the image.
 */
void Navigation::centerToData(ssc::DataPtr image)
{
	if (!image)
		return;
	ssc::Vector3D p_r = image->get_rMd().coord(image->boundingBox().center());

	// set center to calculated position
	ssc::dataManager()->setCenter(p_r);
	CameraControl().translateByFocusTo(p_r);
	this->centerManualTool(p_r);
}

/**Place the global center to the mean center of
 * all the images in a view(wrapper).
 */
void Navigation::centerToView(const std::vector<ssc::DataPtr>& images)
{
	ssc::Vector3D p_r = findViewCenter(images);
	std::cout << "center ToView: " << images.size() << " - " << p_r << std::endl;

	// set center to calculated position
	ssc::dataManager()->setCenter(p_r);
	CameraControl().translateByFocusTo(p_r);
	this->centerManualTool(p_r);
//  std::cout << "Centered to view." << std::endl;
}

/**Place the global center to the mean center of
 * all the loaded images.
 */
void Navigation::centerToGlobalDataCenter()
{
	if (ssc::dataManager()->getData().empty())
		return;

	ssc::Vector3D p_r = this->findGlobalDataCenter();

	// set center to calculated position
	ssc::dataManager()->setCenter(p_r);
	CameraControl().translateByFocusTo(p_r);
	this->centerManualTool(p_r);
//  std::cout << "Centered to all images." << std::endl;
}

/**Place the global center at the current position of the
 * tooltip of the dominant tool.
 */
void Navigation::centerToTooltip()
{
	ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
	ssc::Vector3D p_pr = tool->get_prMt().coord(ssc::Vector3D(0, 0, tool->getTooltipOffset()));
	ssc::Vector3D p_r = ssc::toolManager()->get_rMpr()->coord(p_pr);

	// set center to calculated position
	ssc::dataManager()->setCenter(p_r);
	CameraControl().translateByFocusTo(p_r);
}

/**Find the center of all images in the view(wrapper), defined as the mean of
 * all the images center.
 */
ssc::Vector3D Navigation::findViewCenter(const std::vector<ssc::DataPtr>& images)
{
	return this->findDataCenter(images);
}

/**Find the center of all images, defined as the mean of
 * all the images center.
 */
ssc::Vector3D Navigation::findGlobalDataCenter()
{
	ssc::DataManager::DataMap images = ssc::dataManager()->getData();
	if (images.empty())
		return ssc::Vector3D(0, 0, 0);

	ssc::DataManager::DataMap::iterator iter;
	std::vector<ssc::DataPtr> dataVector;

	for (iter = images.begin(); iter != images.end(); ++iter)
	{
		dataVector.push_back(iter->second);
	}
//  std::cout << "findGlobalDataCenter() " << dataVector.size() << std::endl;
	return findDataCenter(dataVector);
}

/**Find the center of the images, defined as the center
 * of the smallest bounding box enclosing the images.
 */
ssc::Vector3D Navigation::findDataCenter(std::vector<ssc::DataPtr> data)
{
	if (data.empty())
		return ssc::Vector3D(0, 0, 0);

	std::vector<ssc::Vector3D> corners_r;

	for (unsigned i = 0; i < data.size(); ++i)
	{
		ssc::Transform3D rMd = data[i]->get_rMd();
		ssc::DoubleBoundingBox3D bb = data[i]->boundingBox();

		corners_r.push_back(rMd.coord(bb.corner(0, 0, 0)));
		corners_r.push_back(rMd.coord(bb.corner(0, 0, 1)));
		corners_r.push_back(rMd.coord(bb.corner(0, 1, 0)));
		corners_r.push_back(rMd.coord(bb.corner(0, 1, 1)));
		corners_r.push_back(rMd.coord(bb.corner(1, 0, 0)));
		corners_r.push_back(rMd.coord(bb.corner(1, 0, 1)));
		corners_r.push_back(rMd.coord(bb.corner(1, 1, 0)));
		corners_r.push_back(rMd.coord(bb.corner(1, 1, 1)));
	}

	ssc::DoubleBoundingBox3D bb_sigma = ssc::DoubleBoundingBox3D::fromCloud(corners_r);
	return bb_sigma.center();
}

void Navigation::centerManualTool(ssc::Vector3D& p_r)
{
	// move the manual tool to the same position. (this is a side effect... do we want it?)
	ssc::ManualToolPtr manual = ToolManager::getInstance()->getManualTool();
	ssc::Vector3D p_pr = ssc::toolManager()->get_rMpr()->inv().coord(p_r);
	ssc::Transform3D prM0t = manual->get_prMt(); // modify old pos in order to keep orientation
	ssc::Vector3D t_pr = prM0t.coord(ssc::Vector3D(0, 0, manual->getTooltipOffset()));
	ssc::Transform3D prM1t = ssc::createTransformTranslate(p_pr - t_pr) * prM0t;

	manual->set_prMt(prM1t);
//  std::cout << "center manual tool" << std::endl;
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ViewGroup::ViewGroup()
{
//  mRegistrationMode = ssc::rsNOT_REGISTRATED;
	mZoom2D.mLocal = SyncedValue::create(1.0);
	mZoom2D.activateGlobal(false);

	mViewGroupData.reset(new ViewGroupData());

//  this->setSlicePlanesProxy();
}

ViewGroup::~ViewGroup()
{
}

//void ViewGroup::setSlicePlanesProxy()
//{
//  mSlicePlanesProxy.reset(new ssc::SlicePlanesProxy());
//}

/**Add one view wrapper and setup the necessary connections.
 */
void ViewGroup::addView(ViewWrapperPtr wrapper)
{
	mViews.push_back(wrapper->getView());
	mViewWrappers.push_back(wrapper);

	// add state
	wrapper->setZoom2D(mZoom2D.mActive);

	wrapper->setViewGroup(mViewGroupData);
//  wrapper->setSlicePlanesProxy(mSlicePlanesProxy);

	// connect signals
	connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(activateManualToolSlot()));
	connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(mouseClickInViewGroupSlot()));
	connect(wrapper->getView(), SIGNAL(focusInSignal(QFocusEvent*)), this, SLOT(mouseClickInViewGroupSlot()));
}

void ViewGroup::removeViews()
{
	for (unsigned i = 0; i < mViewWrappers.size(); ++i)
	{
		ViewWrapperPtr wrapper = mViewWrappers[i];

		disconnect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(activateManualToolSlot()));
		disconnect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(mouseClickInViewGroupSlot()));
		disconnect(wrapper->getView(), SIGNAL(focusInSignal(QFocusEvent*)), this, SLOT(mouseClickInViewGroupSlot()));
	}

	mViews.clear();
	mViewWrappers.clear();
//  mSlicePlanesProxy->clearViewports();
}

ViewWrapperPtr ViewGroup::getViewWrapperFromViewUid(QString viewUid)
{
	for (unsigned i = 0; i < mViewWrappers.size(); ++i)
	{
		if (mViewWrappers[i]->getView()->getUid() == viewUid)
			return mViewWrappers[i];
	}
	return ViewWrapperPtr();
}

void ViewGroup::setGlobal2DZoom(bool use, SyncedValuePtr val)
{
	mZoom2D.mGlobal = val;
	mZoom2D.activateGlobal(use);

	for (unsigned i = 0; i < mViewWrappers.size(); ++i)
		mViewWrappers[i]->setZoom2D(mZoom2D.mActive);
}

/**Set the zoom2D factor, only.
 */
void ViewGroup::setZoom2D(double newZoom)
{
	mZoom2D.mActive->set(newZoom);
}

double ViewGroup::getZoom2D()
{
	return mZoom2D.mActive->get().toDouble();
}

void ViewGroup::syncOrientationMode(SyncedValuePtr val)
{
	for (unsigned i = 0; i < mViewWrappers.size(); ++i)
	{
		mViewWrappers[i]->setOrientationMode(val);
	}
}

void ViewGroup::mouseClickInViewGroupSlot()
{
	std::vector<ssc::ImagePtr> images = mViewGroupData->getImages();
	if (images.empty())
	{
		//Don't remove active image too easily
		//ssc::dataManager()->setActiveImage(ssc::ImagePtr());
	}
	else
	{
		if (!std::count(images.begin(), images.end(), ssc::dataManager()->getActiveImage()))
		{
			ssc::dataManager()->setActiveImage(images.front());
		}
	}

	ViewWidgetQPtr view = static_cast<ssc::ViewWidget*>(this->sender());
	if (view)
		viewManager()->setActiveView(view->getUid());
}

std::vector<ViewWidgetQPtr> ViewGroup::getViews() const
{
	return mViews;
}

void ViewGroup::activateManualToolSlot()
{
	ToolManager::getInstance()->dominantCheckSlot();
}

void ViewGroup::addXml(QDomNode& dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();

	std::vector<ssc::DataPtr> data = mViewGroupData->getData();

	for (unsigned i = 0; i < data.size(); ++i)
	{
		QDomElement imageNode = doc.createElement("data");
		imageNode.appendChild(doc.createTextNode(qstring_cast(data[i]->getUid())));
		dataNode.appendChild(imageNode);
	}

	QDomElement cameraNode = doc.createElement("camera3D");
	mViewGroupData->getCamera3D()->addXml(cameraNode);
	dataNode.appendChild(cameraNode);

	QDomElement zoom2DNode = doc.createElement("zoomFactor2D");
	zoom2DNode.appendChild(doc.createTextNode(qstring_cast(this->getZoom2D())));
	dataNode.appendChild(zoom2DNode);

//  QDomElement slicePlanes3DNode = doc.createElement("slicePlanes3D");
//  slicePlanes3DNode.setAttribute("use", mSlicePlanesProxy->getVisible());
//  slicePlanes3DNode.setAttribute("opaque", mSlicePlanesProxy->getDrawPlanes());
//  dataNode.appendChild(slicePlanes3DNode);

}

void ViewGroup::clearPatientData()
{
	mViewGroupData->clearData();
	this->setZoom2D(1.0);
}

void ViewGroup::parseXml(QDomNode dataNode)
{
	for (QDomElement elem = dataNode.firstChildElement("data"); !elem.isNull(); elem = elem.nextSiblingElement("data"))
	{
		QString uid = elem.text();
		ssc::DataPtr data = ssc::dataManager()->getData(uid);

		mViewGroupData->addData(data);
		if (!data)
			ssc::messageManager()->sendError("Couldn't find the data: [" + uid + "] in the datamanager.");
	}

	mViewGroupData->getCamera3D()->parseXml(dataNode.namedItem("camera3D"));

	QString zoom2D = dataNode.namedItem("zoomFactor2D").toElement().text();
	bool ok;
	double zoom2Ddouble = zoom2D.toDouble(&ok);
	if (ok)
		this->setZoom2D(zoom2Ddouble);
	else
		ssc::messageManager()->sendError("Couldn't convert the zoomfactor to a double: " + qstring_cast(zoom2D) + "");

//  QDomElement slicePlanes3DNode = dataNode.namedItem("slicePlanes3D").toElement();
//  mSlicePlanesProxy->setVisible(slicePlanes3DNode.attribute("use").toInt());
//  mSlicePlanesProxy->setDrawPlanes(slicePlanes3DNode.attribute("opaque").toInt());
//  dataNode.appendChild(slicePlanes3DNode);

}

std::vector<ssc::ImagePtr> ViewGroup::getImages()
{
	return mViewGroupData->getImages();
}

} //cx
