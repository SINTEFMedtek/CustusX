#include "cxViewGroup.h"

#include <vector>
#include <QString>
#include <QPoint>
#include <QMenu>
#include <QSettings>
#include <QAction>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscTypeConversions.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "cxRepManager.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxInriaRep2D.h"
#include "cxLandmarkRep.h"
#include "cxViewWrapper.h"

namespace cx
{

ViewGroup::ViewGroup()
{
  this->connectContextMenu();
}

ViewGroup::~ViewGroup()
{
}

void ViewGroup::addViewWrapper(ViewWrapperPtr wrapper)
{
  mViews.push_back(wrapper->getView());
  mElements.push_back(wrapper);

  connect(wrapper->getView(), SIGNAL(mousePressSignal(QMouseEvent*)),
          this, SLOT(activateManualToolSlot()));
}

std::string ViewGroup::toString(int i) const
{
  return QString::number(i).toStdString();
}

void ViewGroup::connectContextMenu()
{
  for(unsigned int i=0;i<mViews.size();++i)
  {
   connect(mViews[i], SIGNAL(customContextMenuRequested(const QPoint &)),
       this, SLOT(contexMenuSlot(const QPoint &)));
  }
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
      std::string error = "Couldn't find image with uid "+imageUid.toStdString()+" to set in View3D.";
      messageManager()->sendError(error);
      return;
    }

    this->setImage(image);
}
void ViewGroup::activateManualToolSlot()
{
  toolManager()->setDominantTool(toolManager()->getManualTool()->getUid());
}
}//cx
