#include "cxView3D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include "sscDataManager.h"
#include "sscImage.h"
#include "cxRepManager.h"
#include "cxMessageManager.h"
#include "cxVolumetricRep.h"

/**
 * cxView3D.cpp
 *
 * \brief
 *
 * \date Dec 9, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
View3D::View3D(const std::string& uid, const std::string& name, QWidget *parent, Qt::WFlags f) :
  View(parent, f),
  mContextMenu(new QMenu(this)),
  mMakeRepMenu(new QMenu(tr("Add volumetric representation of...") ,mContextMenu)),
  mDataManager(DataManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mMessageManager(MessageManager::getInstance())
{
  mUid = uid;
  mName = name;
}
View3D::~View3D()
{}
void View3D::contextMenuEvent(QContextMenuEvent *event)
{
    mContextMenu->clear();
    mMakeRepMenu->clear();

    mContextMenu->addMenu(mMakeRepMenu);

    //Get a list of available image and meshes names
    std::map<std::string, std::string> imageUidsAndNames = mDataManager->getImageUidsAndNames();
/*    std::map<std::string, std::string> meshNames = mDataManager->getMeshUIDsWithNames();*/

    //Display the lists to the user
    //Extract to own function if often reused...
    std::map<std::string, std::string>::iterator itImages = imageUidsAndNames.begin();
    while(itImages != imageUidsAndNames.end())
    {
      const QString name = itImages->first.c_str();
      QAction* imageIdAction = new QAction(name, mContextMenu);
      mMakeRepMenu->addAction(imageIdAction);
      itImages++;
    }
    //TODO: What to do with meshes?
/*    std::map<std::string, std::string>::iterator itMeshes = meshNames.begin();
    while(itMeshes != imageNames.end())
    {
      const QString name = itMeshes->first.c_str();
      QAction* meshNameAction = new QAction(name, mContextMenu);
      mMakeRepMenu->addAction(meshNameAction);
      itMeshes++;
    }*/

    //Find out which the user chose
    QAction* theAction = mContextMenu->exec(event->globalPos());
    if(theAction == NULL)
      return;

    std::string actionId = theAction->text().toStdString();
    std::string info = "Making a volumetricrep of data with name: "+actionId;
    mMessageManager.sendInfo(info);

    //Make a volumetric rep out of the image
    VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");//TODO: REMOVE HACK!!!
    ssc::ImagePtr image = mDataManager->getImage(actionId);
    volumetricRep->setImage(image);

    //Show the rep in this view
    this->setRep(volumetricRep);
}
}//namespace cx
