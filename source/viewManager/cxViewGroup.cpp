#include "cxViewGroup.h"

#include <vector>
#include <QtGui>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "sscUtilHelpers.h"
#include "sscSlicePlanes3DRep.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "sscDataManager.h"
#include "cxToolManager.h"
#include "cxLandmarkRep.h"
#include "cxViewWrapper2D.h"
#include "cxViewManager.h"
#include "cxCameraControl.h"

namespace cx
{

/**Place the global center to the center of the image.
 */
void Navigation::centerToImage(ssc::ImagePtr image)
{
  if(!image)
    return;
  ssc::Vector3D p_r = findImageCenter(image);

  // set center to calculated position
  ssc::dataManager()->setCenter(p_r);

  this->centerManualTool(p_r);
//  std::cout << "Centered to image." << std::endl;
}

/**Place the global center to the mean center of
 * all the images in a view(wrapper).
 */
void Navigation::centerToView(const std::vector<ssc::ImagePtr>& images)
{
  ssc::Vector3D p_r = findViewCenter(images);

  // set center to calculated position
  ssc::dataManager()->setCenter(p_r);

  this->centerManualTool(p_r);
//  std::cout << "Centered to view." << std::endl;
}

/**Place the global center to the mean center of
 * all the loaded images.
 */
void Navigation::centerToGlobalImageCenter()
{
  ssc::Vector3D p_r = findGlobalImageCenter();

  // set center to calculated position
  ssc::dataManager()->setCenter(p_r);

  this->centerManualTool(p_r);
//  std::cout << "Centered to all images." << std::endl;
}

/**Place the global center at the current position of the
 * tooltip of the dominant tool.
 */
void Navigation::centerToTooltip()
{
  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
  ssc::Vector3D p_pr = tool->get_prMt().coord(ssc::Vector3D(0,0,tool->getTooltipOffset()));
  ssc::Vector3D p_r = ssc::toolManager()->get_rMpr()->coord(p_pr);

  // set center to calculated position
  ssc::dataManager()->setCenter(p_r);
}
/**Find the center of the images, defined as the center
 * of the smallest bounding box enclosing the images.
 */
ssc::Vector3D Navigation::findImageCenter(ssc::ImagePtr image)
{
  if(!image)
    return ssc::Vector3D();
  std::vector<ssc::Vector3D> corners_r;
  ssc::Transform3D rMd = image->get_rMd();
  ssc::DoubleBoundingBox3D bb = image->boundingBox();

  corners_r.push_back(rMd.coord(bb.corner(0,0,0)));
  corners_r.push_back(rMd.coord(bb.corner(0,0,1)));
  corners_r.push_back(rMd.coord(bb.corner(0,1,0)));
  corners_r.push_back(rMd.coord(bb.corner(0,1,1)));
  corners_r.push_back(rMd.coord(bb.corner(1,0,0)));
  corners_r.push_back(rMd.coord(bb.corner(1,0,1)));
  corners_r.push_back(rMd.coord(bb.corner(1,1,0)));
  corners_r.push_back(rMd.coord(bb.corner(1,1,1)));

  ssc::Vector3D p_min = corners_r[0];
  ssc::Vector3D p_max = corners_r[0];

  for (unsigned i=0; i<corners_r.size(); ++i)
  {
    for (unsigned j=0; j<3; ++j)
    {
      p_min[j] = std::min(p_min[j], corners_r[i][j]);
      p_max[j] = std::max(p_max[j], corners_r[i][j]);
    }
  }

  ssc::Vector3D center_r = (p_min+p_max)/2.0;

  return center_r;
}
/**Find the center of all images in the view(wrapper), defined as the mean of
 * all the images center.
 */
ssc::Vector3D Navigation::findViewCenter(const std::vector<ssc::ImagePtr>& images)
{
  ssc::Vector3D center_r;
  //std::vector<ssc::ImagePtr> images = viewWrapper->getImages();
  std::vector<ssc::ImagePtr>::const_iterator iter;

  for (iter=images.begin(); iter!=images.end(); ++iter)
  {
    center_r += this->findImageCenter(*iter);
  }
  center_r = center_r/images.size();

  return center_r;
}

/**Find the center of all images, defined as the mean of
 * all the images center.
 */
ssc::Vector3D Navigation::findGlobalImageCenter()
{
  ssc::Vector3D p_r(0,0,0);
  if (ssc::dataManager()->getImages().empty())
    return p_r;

  ssc::DataManager::ImagesMap images = ssc::dataManager()->getImages();
  ssc::DataManager::ImagesMap::iterator iter;

  for (iter=images.begin(); iter!=images.end(); ++iter)
  {
    p_r += this->findImageCenter(iter->second);
  }
  p_r = p_r/images.size();

  return p_r;
}

void Navigation::centerManualTool(ssc::Vector3D& p_r)
{
  // move the manual tool to the same position. (this is a side effect... do we want it?)
  ssc::ManualToolPtr manual = ToolManager::getInstance()->getManualTool();
  ssc::Vector3D p_pr = ssc::toolManager()->get_rMpr()->inv().coord(p_r);
  ssc::Transform3D prM0t = manual->get_prMt(); // modify old pos in order to keep orientation
  ssc::Vector3D t_pr = prM0t.coord(ssc::Vector3D(0,0,manual->getTooltipOffset()));
  ssc::Transform3D prM1t = createTransformTranslate(p_pr-t_pr) * prM0t;

  manual->set_prMt(prM1t);
  std::cout << "center manual tool" << std::endl;
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ViewGroup::ViewGroup()
{
  mRegistrationMode = ssc::rsNOT_REGISTRATED;
  mZoom2D.mLocal = SyncedValue::create(1.0);
  mZoom2D.activateGlobal(false);

  mViewGroupData.reset(new ViewGroupData());

  this->setSlicePlanesProxy();
}

ViewGroup::~ViewGroup()
{}

void ViewGroup::setSlicePlanesProxy()
{
  mSlicePlanesProxy.reset(new ssc::SlicePlanesProxy());
}

/**Add one view wrapper and setup the necessary connections.
 */
void ViewGroup::addView(ViewWrapperPtr wrapper)
{
  mViews.push_back(wrapper->getView());
  mViewWrappers.push_back(wrapper);

  // add state
  wrapper->setZoom2D(mZoom2D.mActive);

  wrapper->setViewGroup(mViewGroupData);
  wrapper->setSlicePlanesProxy(mSlicePlanesProxy);

  // connect signals
  connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)),this, SLOT(activateManualToolSlot()));
  connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)),this, SLOT(mouseClickInViewGroupSlot()));
  connect(wrapper->getView(), SIGNAL(focusInSignal(QFocusEvent*)),this, SLOT(mouseClickInViewGroupSlot()));

  wrapper->setRegistrationMode(mRegistrationMode);
}

void ViewGroup::removeViews()
{
  for (unsigned i=0; i<mViewWrappers.size(); ++i)
  {
    ViewWrapperPtr wrapper = mViewWrappers[i];

    disconnect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)),this, SLOT(activateManualToolSlot()));
    disconnect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)),this, SLOT(mouseClickInViewGroupSlot()));
    disconnect(wrapper->getView(), SIGNAL(focusInSignal(QFocusEvent*)),this, SLOT(mouseClickInViewGroupSlot()));
  }

  mViews.clear();
  mViewWrappers.clear();
  mSlicePlanesProxy->clearViewports();
}

ViewWrapperPtr ViewGroup::getViewWrapperFromViewUid(QString viewUid)
{
  for(unsigned i=0; i<mViewWrappers.size(); ++i)
  {
    if(mViewWrappers[i]->getView()->getUid() == viewUid)
      return mViewWrappers[i];
  }
  return ViewWrapperPtr();
}

void ViewGroup::setGlobal2DZoom(bool use, SyncedValuePtr val)
{
  mZoom2D.mGlobal = val;
  mZoom2D.activateGlobal(use);

  for (unsigned i=0; i<mViewWrappers.size(); ++i)
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
  for(unsigned i=0; i<mViewWrappers.size(); ++i)
  {
    mViewWrappers[i]->setOrientationMode(val);
  }
}

void ViewGroup::mouseClickInViewGroupSlot()
{
  std::vector<ssc::ImagePtr> images = mViewGroupData->getImages();
  if (images.empty())
    ssc::dataManager()->setActiveImage(ssc::ImagePtr());
  else
  {
    if (!std::count(images.begin(), images.end(), ssc::dataManager()->getActiveImage()))
    {
      ssc::dataManager()->setActiveImage(images.front());
    }
  }

  ssc::View* view = static_cast<ssc::View*>(this->sender());
  if(view)
    viewManager()->setActiveView(view->getUid());
}

std::vector<ssc::View*> ViewGroup::getViews() const
{
  return mViews;
}

void ViewGroup::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  mRegistrationMode = mode;
  for (unsigned i=0; i<mViewWrappers.size(); ++i)
    mViewWrappers[i]->setRegistrationMode(mode);
}

void ViewGroup::activateManualToolSlot()
{
  ToolManager::getInstance()->dominantCheckSlot();
}

void ViewGroup::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();

  std::vector<ssc::DataPtr> data = mViewGroupData->getData();

  for (unsigned i=0; i<data.size(); ++i)
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

  QDomElement slicePlanes3DNode = doc.createElement("slicePlanes3D");
  slicePlanes3DNode.setAttribute("use", mSlicePlanesProxy->getVisible());
  slicePlanes3DNode.setAttribute("opaque", mSlicePlanesProxy->getDrawPlanes());
  dataNode.appendChild(slicePlanes3DNode);

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
      ssc::messageManager()->sendError("Couldn't find the data: ["+uid+"] in the datamanager.");
  }

  mViewGroupData->getCamera3D()->parseXml(dataNode.namedItem("camera3D"));

  QString zoom2D = dataNode.namedItem("zoomFactor2D").toElement().text();
  bool ok;
  double zoom2Ddouble = zoom2D.toDouble(&ok);
  if(ok)
    this->setZoom2D(zoom2Ddouble);
  else
    ssc::messageManager()->sendError("Couldn't convert the zoomfactor to a double: "+qstring_cast(zoom2D)+"");

  QDomElement slicePlanes3DNode = dataNode.namedItem("slicePlanes3D").toElement();
  mSlicePlanesProxy->setVisible(slicePlanes3DNode.attribute("use").toInt());
  mSlicePlanesProxy->setDrawPlanes(slicePlanes3DNode.attribute("opaque").toInt());
  dataNode.appendChild(slicePlanes3DNode);

}

std::vector<ssc::ImagePtr> ViewGroup::getImages()
{
  return mViewGroupData->getImages();
}


}//cx
