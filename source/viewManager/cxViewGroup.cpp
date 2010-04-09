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

/**Place the global center to the mean center of
 * all the loaded images.
 */
void Navigation::centerToImageCenter()
{
  //TODO: move this to suitable place... (CA)
  // must use mean center at the least.
  ssc::Vector3D p_r(0,0,0);
  if (!DataManager::getInstance()->getImages().empty())
  {
    ssc::ImagePtr image = DataManager::getInstance()->getImages().begin()->second;
    p_r = image->get_rMd().coord(image->boundingBox().center());
  }

  // set center to calculated position
  DataManager::getInstance()->setCenter(p_r);

  // move the manual tool to the same position. (this is a side effect... do we want it?)
  ssc::Vector3D p_pr = ToolManager::getInstance()->get_rMpr()->inv().coord(p_r);
  ToolManager::getInstance()->getManualTool()->set_prMt(ssc::createTransformTranslate(p_pr));
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
  this->connectContextMenu();
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
  connectContextMenu(wrapper->getView());
}

/**Called when a zoom change is requested from one view wrapper
 *
 */
void ViewGroup::zoom2DChangeSlot(double newZoom)
{
  mZoomFactor2D = newZoom;
  mZoomFactor2D = ssc::constrainValue(mZoomFactor2D, 0.2, 10.0); // constrain zoom to a sensible interval

  Navigation().centerToTooltip();

  for (unsigned i=0; i<mElements.size(); ++i)
  {
    mElements[i]->setZoom2D(mZoomFactor2D);
  }
}
void ViewGroup::activeImageChangeSlot()
{
  messageManager()->sendInfo("MousePressEvent and focusInEvent in a viewgroup calls setActiveImage()");
  dataManager()->setActiveImage(mImage);
}
void ViewGroup::connectContextMenu()
{
  for(unsigned int i=0;i<mViews.size();++i)
  {
    connectContextMenu(mViews[i]);
  }
}

void ViewGroup::connectContextMenu(ssc::View* view)
{
   connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
       this, SLOT(contexMenuSlot(const QPoint &)));
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

  Navigation().centerToImageCenter();
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


void ViewGroup::contexMenuSlot(const QPoint& point)
{
  //NOT SUPPORTING MESHES IN 3D VIEW YET

  QWidget* sender = dynamic_cast<QWidget*>(this->sender());
  QPoint pointGlobal = sender->mapToGlobal(point);
  /*ssc::View* senderView = dynamic_cast<ssc::View*>(sender);
  if(senderView)
    std::cout << senderView->getUid() << std::endl;*/
  QMenu contextMenu(sender);

    //Get a list of available image and meshes names
    std::map<std::string, std::string> imageUidsAndNames = dataManager()->getImageUidsAndNames();
    std::map<std::string, std::string> meshUidsAndNames = dataManager()->getMeshUidsWithNames();

    //Display the lists to the user
    std::map<std::string, std::string>::iterator imageIt = imageUidsAndNames.begin();
    while(imageIt != imageUidsAndNames.end())
    {
      const QString uid = imageIt->first.c_str();
      const QString name = imageIt->second.c_str();
      QAction* imageAction = new QAction(name, &contextMenu);
      imageAction->setStatusTip(uid.toStdString().c_str());
      contextMenu.addAction(imageAction);
      imageIt++;
    }

    //Find out which the user chose
    QAction* theAction = contextMenu.exec(pointGlobal);
    if(!theAction)//this happens if you rightclick in the view and then don't select a action
      return;

    QString imageName = theAction->text();
    QString imageUid = theAction->statusTip();
    ssc::ImagePtr image = dataManager()->getImage(imageUid.toStdString());

    if(!image)
    {
      std::string error = "Couldn't find image with uid "+imageUid.toStdString()+" to set in View.";
      messageManager()->sendError(error);
      return;
    }

    this->setImage(image);

    //TODO remove/move? (JB)
    //test to see if the contextdockwidgets combobox will respond
    //it did
    //dataManager()->setActiveImage(image);
}
void ViewGroup::activateManualToolSlot()
{
  toolManager()->dominantCheckSlot();
}

void ViewGroup::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();
//  QDomElement viewManagerNode = doc.createElement("viewManager");
//  parentNode.appendChild(viewManagerNode);

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
  ssc::ImagePtr image = dataManager()->getImage(string_cast(imageUid));
  if (image)
    setImage(image);

  QString zoom2D = dataNode.namedItem("zoomFactor2D").toElement().text();
  bool ok;
  zoom2D.toDouble(&ok);
  if (ok)
    zoom2DChangeSlot(zoom2D.toDouble());
}

}//cx
