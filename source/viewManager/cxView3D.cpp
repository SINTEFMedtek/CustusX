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
  ssc::View(parent, f),
  mContextMenu(new QMenu(this)),
  mMakeVolumetricRepMenu(new QMenu(tr("Add volumetric representation of...") ,mContextMenu)),
  mMakeGeometricRepMenu(new QMenu(tr("Add geometric representation of...") ,mContextMenu)),
  mDataManager(DataManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mMessageManager(MessageManager::getInstance())
{
  mUid = uid;
  mName = name;

  mContextMenu->addMenu(mMakeVolumetricRepMenu);
  mContextMenu->addMenu(mMakeGeometricRepMenu);
}
View3D::~View3D()
{}
void View3D::contextMenuEvent(QContextMenuEvent *event)
{
    mMakeVolumetricRepMenu->clear();
    mMakeGeometricRepMenu->clear();

    mMakeGeometricRepMenu->setEnabled(false); //TODO remove when we know what to do with meshes

    //Get a list of available image and meshes names
    std::map<std::string, std::string> imageUidsAndNames = mDataManager->getImageUidsAndNames();
    std::map<std::string, std::string> meshUidsAndNames = mDataManager->getMeshUIDsWithNames();

    //Display the lists to the user
    //Extract to own function if often reused...
    std::map<std::string, std::string>::iterator itImages = imageUidsAndNames.begin();
    while(itImages != imageUidsAndNames.end())
    {
      const QString id = itImages->first.c_str();
      QAction* imageIdAction = new QAction(id, mMakeVolumetricRepMenu);
      mMakeVolumetricRepMenu->addAction(imageIdAction);
      itImages++;
    }
    std::map<std::string, std::string>::iterator itMeshes = meshUidsAndNames.begin();
    while(itMeshes != meshUidsAndNames.end())
    {
      const QString id = itMeshes->first.c_str();
      QAction* meshIdAction = new QAction(id, mMakeGeometricRepMenu);
      mMakeGeometricRepMenu->addAction(meshIdAction);
      itMeshes++;
    }

    //Find out which the user chose
    //TODO: IMAGE OR MESH??? theAction->parent()?
    QAction* theAction = mContextMenu->exec(event->globalPos());
    if(theAction == NULL)
      return;

    std::string actionId = theAction->text().toStdString();
    std::string info = "Making a volumetricrep of data with name: "+actionId;
    mMessageManager->sendInfo(info);

    //Make a volumetric rep out of the image
    VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");//TODO: REMOVE HACK???
    ssc::ImagePtr image = mDataManager->getImage(actionId);
    volumetricRep->setImage(image);

    //Show the rep in this view
    this->setRep(volumetricRep);
}
}//namespace cx
