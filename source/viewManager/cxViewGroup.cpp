#include "cxViewGroup.h"

#include <vector>
#include <QtGui>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscTypeConversions.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "sscUtilHelpers.h"
#include "cxRepManager.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxInriaRep2D.h"
#include "cxLandmarkRep.h"
#include "cxViewWrapper.h"

namespace cx
{


/**Find the center of all images, defined as the center
 * of the smallest bounding box enclosing all images.
 */
ssc::Vector3D Navigation::findGlobalImageCenter()
{
  ssc::Vector3D p_r(0,0,0);
  if (DataManager::getInstance()->getImages().empty())
    return p_r;

    //TODO: move this to suitable place... (CA)
  // must use mean center at the least.
  std::vector<ssc::Vector3D> coord;

  ssc::DataManager::ImagesMap images = dataManager()->getImages();
  ssc::DataManager::ImagesMap::iterator iter;

  for (iter=images.begin(); iter!=images.end(); ++iter)
  {
    ssc::ImagePtr image = iter->second;
    ssc::Transform3D rMd = image->get_rMd();
    ssc::DoubleBoundingBox3D bb = image->boundingBox();

    coord.push_back(rMd.coord(bb.corner(0,0,0)));
    coord.push_back(rMd.coord(bb.corner(0,0,1)));
    coord.push_back(rMd.coord(bb.corner(0,1,0)));
    coord.push_back(rMd.coord(bb.corner(0,1,1)));
    coord.push_back(rMd.coord(bb.corner(1,0,0)));
    coord.push_back(rMd.coord(bb.corner(1,0,1)));
    coord.push_back(rMd.coord(bb.corner(1,1,0)));
    coord.push_back(rMd.coord(bb.corner(1,1,1)));
  }

  //p_r = image->get_rMd().coord(image->boundingBox().center());
  ssc::Vector3D p_min = coord[0];
  ssc::Vector3D p_max = coord[0];

  for (unsigned i=0; i<coord.size(); ++i)
  {
    for (unsigned j=0; j<3; ++j)
    {
      p_min[j] = std::min(p_min[j], coord[i][j]);
      p_max[j] = std::max(p_max[j], coord[i][j]);
    }
  }

  p_r = (p_min+p_max)/2.0;

  return p_r;
}

/**Place the global center to the mean center of
 * all the loaded images.
 */
void Navigation::centerToImageCenter()
{
  ssc::Vector3D p_r = findGlobalImageCenter();

  // set center to calculated position
  DataManager::getInstance()->setCenter(p_r);

  // move the manual tool to the same position. (this is a side effect... do we want it?)
  ssc::Vector3D p_pr = ToolManager::getInstance()->get_rMpr()->inv().coord(p_r);
  ssc::Transform3D prM0t = ToolManager::getInstance()->getManualTool()->get_prMt(); // modify old pos in order to keep orientation
  ssc::Vector3D t_pr = prM0t.coord(ssc::Vector3D(0,0,0));
  ssc::Transform3D prM1t = createTransformTranslate(p_pr-t_pr) * prM0t;
  //ToolManager::getInstance()->getManualTool()->set_prMt(ssc::createTransformTranslate(p_pr));
  ToolManager::getInstance()->getManualTool()->set_prMt(prM1t);
}

/**Place the global center at the current position of the
 * tooltip of the dominant tool.
 */
void Navigation::centerToTooltip()
{
  ssc::Vector3D p_pr = ToolManager::getInstance()->getDominantTool()->get_prMt().coord(ssc::Vector3D(0,0,0));
  ssc::Vector3D p_r = ToolManager::getInstance()->get_rMpr()->coord(p_pr);
  // set center to calculated position
  DataManager::getInstance()->setCenter(p_r);
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ViewGroup::ViewGroup()
{
  mZoomFactor2D = 0.5;
}

ViewGroup::~ViewGroup()
{
}

/**Add one view wrapper and setup the necessary connections.
 */
void ViewGroup::addViewWrapper(ViewWrapperPtr wrapper)
{
  mViews.push_back(wrapper->getView());
  mElements.push_back(wrapper);

  connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)),
          this, SLOT(activateManualToolSlot()));

  connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)),
          this, SLOT(activeImageChangeSlot()));
  connect(wrapper->getView(), SIGNAL(focusInSignal(QFocusEvent*)),
          this, SLOT(activeImageChangeSlot()));

  connect(wrapper.get(), SIGNAL(zoom2DChange(double)), this, SLOT(zoom2DChangeSlot(double)));
}

/**Set the zoom2D factor, only.
 */
void ViewGroup::setZoom2D(double newZoom)
{
  mZoomFactor2D = newZoom;
  mZoomFactor2D = ssc::constrainValue(mZoomFactor2D, 0.2, 10.0); // constrain zoom to a sensible interval

  for (unsigned i=0; i<mElements.size(); ++i)
  {
    mElements[i]->setZoom2D(mZoomFactor2D);
  }
}

/**Called when a zoom change is requested from one view wrapper
 *
 */
void ViewGroup::zoom2DChangeSlot(double newZoom)
{
  Navigation().centerToTooltip(); // side effect: center on tool
  setZoom2D(newZoom);
}

void ViewGroup::activeImageChangeSlot()
{
  messageManager()->sendInfo("MousePressEvent and focusInEvent in a viewgroup calls setActiveImage()");
  dataManager()->setActiveImage(mImage);
}

std::vector<ssc::View*> ViewGroup::getViews() const
{
  return mViews;
}

/**Call this on an initialized view, when the plane type is changed.
 *
 */
ssc::View* ViewGroup::initializeView(int index, ssc::PLANE_TYPE plane)
{
  if (index<0 || index>=(int)mElements.size())
  {
    messageManager()->sendError("invalid index in ViewGroup2D");
  }

  mElements[index]->initializePlane(plane);
  return mViews[index];
}

void ViewGroup::setImage(ssc::ImagePtr image)
{
  if(mImage == image)
    return;
  mImage = image;
  for (unsigned i=0; i<mElements.size(); ++i)
    mElements[i]->setImage(image);
}

void ViewGroup::removeImage(ssc::ImagePtr image)
{
  if(mImage != image)
    return;
  mImage = image;
  for (unsigned i=0; i<mElements.size(); ++i)
    mElements[i]->removeImage(image);
}

void ViewGroup::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  for (unsigned i=0; i<mElements.size(); ++i)
    mElements[i]->setRegistrationMode(mode);
}

void ViewGroup::activateManualToolSlot()
{
  toolManager()->dominantCheckSlot();
}

void ViewGroup::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();

  if (mImage)
  {
    QDomElement imageNode = doc.createElement("image");
    imageNode.appendChild(doc.createTextNode(qstring_cast(mImage->getUid())));
    dataNode.appendChild(imageNode);
  }

  QDomElement zoom2DNode = doc.createElement("zoomFactor2D");
  zoom2DNode.appendChild(doc.createTextNode(qstring_cast(mZoomFactor2D)));
  dataNode.appendChild(zoom2DNode);
}

void ViewGroup::parseXml(QDomNode dataNode)
{
  QString imageUid = dataNode.namedItem("image").toElement().text();

  if (!imageUid.isEmpty())
  {
    ssc::ImagePtr image = dataManager()->getImage(string_cast(imageUid));
    if (image)
      this->setImage(image);
    else
      messageManager()->sendError("Couldn't find the image: "+string_cast(imageUid)+" in the datamanager.");
  }

  QString zoom2D = dataNode.namedItem("zoomFactor2D").toElement().text();
  bool ok;
  zoom2D.toDouble(&ok);
  if (ok)
    this->setZoom2D(zoom2D.toDouble());
  else
    messageManager()->sendError("Couldn't convert the zoomfactor to a double: "+string_cast(zoom2D)+"");
}

}//cx
