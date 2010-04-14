#include "cxViewWrapper.h"

#include <QMenu>
#include "cxDataManager.h"
#include "cxMessageManager.h"
#include "cxViewGroup.h" //for class Navigation

namespace cx
{
void ViewWrapper::contextMenuSlot(const QPoint& point)
{
  QWidget* sender = dynamic_cast<QWidget*>(this->sender());
  QPoint pointGlobal = sender->mapToGlobal(point);
  QMenu contextMenu(sender);

  QActionGroup* imageActionGroup = new QActionGroup(&contextMenu);

  //add actions to the actiongroups and the contextmenu
  std::map<std::string, std::string> imageUidsAndNames = dataManager()->getImageUidsAndNames();
  std::map<std::string, std::string> meshUidsAndNames = dataManager()->getMeshUidsWithNames();

  std::map<std::string, std::string>::iterator imageIt = imageUidsAndNames.begin();
  while(imageIt != imageUidsAndNames.end())
  {
    const QString uid = imageIt->first.c_str();
    const QString name = imageIt->second.c_str();
    QAction* imageAction = new QAction(name, &contextMenu);
    imageAction->setStatusTip(uid.toStdString().c_str());
    imageActionGroup->addAction(imageAction);
    contextMenu.addAction(imageAction);
    imageIt++;
  }

  this->appendToContextMenu(contextMenu);

  //Show the contextmenu
  QAction* theAction = contextMenu.exec(pointGlobal);
  if(!theAction)//this happens if you right click in the view and then don't select a action
    return;

  //take action :P
  QActionGroup* theActionGroup = theAction->actionGroup();
  if(!theActionGroup)
  {
    messageManager()->sendError("theAction: "+theAction->text().toStdString()+" was not in a actiongroup.");
    return;
  }
  if(theActionGroup == imageActionGroup)
  {
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
    Navigation().centerToImageCenter(); // reset center for convenience
  }

  this->checkFromContextMenu(theAction, theActionGroup);
}

/*void ViewWrapper::connectContextMenu()
{
  for(unsigned int i=0;i<mViews.size();++i)
  {
    connectContextMenu(mViews[i]);
  }
}*/

void ViewWrapper::connectContextMenu(ssc::View* view)
{
   connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
       this, SLOT(contextMenuSlot(const QPoint &)));
}
}//namespace cx
