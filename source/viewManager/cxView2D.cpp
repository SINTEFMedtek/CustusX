#include "cxView2D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include "cxDataManager.h"
#include "cxRepManager.h"

namespace cx
{
/**
 * \param uid the views unique id
 * \param name the views name
 * \param parent the views parent
 * \param f flags
 */
View2D::View2D(const std::string& uid, const std::string& name, QWidget *parent, Qt::WFlags f) :
  View(parent, f),
  mContextMenu(new QMenu(this)),
  mDataManager(DataManager::getInstance()),
  mRepManager(RepManager::getInstance())
{
    mUid = uid;
    mName = name;
}
View2D::~View2D()
{}
void View2D::contextMenuEvent(QContextMenuEvent *event)
{
    mContextMenu->clear();

    //Get a list of available image and meshes names
    std::vector<std::string> imageNames = mDataManager->getImageNames();
    std::vector<std::string> meshNames = mDataManager->getMeshNames();

    //Display the lists to the user
    for(unsigned int i=0; i< imageNames.size(); i++)
    {
      const QString name = imageNames.at(i).c_str();
      QAction* imageNameAction = new QAction(name, mContextMenu);
      mContextMenu->addAction(imageNameAction);
    }
    for(unsigned int i=0; i< meshNames.size(); i++)
    {
      const QString name = meshNames.at(i).c_str();
      QAction* meshNameAction = new QAction(name, mContextMenu);
      mContextMenu->addAction(meshNameAction);
    }

    //Find out which the user chose
    QAction* theAction = mContextMenu->exec(event->globalPos());
    if(theAction != NULL)
      QString actionName = theAction->text();
}

}//namespace cx
