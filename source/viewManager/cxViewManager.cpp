#include "cxViewManager.h"

#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include "sscProbeRep.h"
#include "sscVolumetricRep.h"
#include "cxMessageManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"

namespace cx
{

ViewManager *ViewManager::mTheInstance = NULL;

ViewManager* ViewManager::getInstance()
{
  if(mTheInstance == NULL)
   {
     mTheInstance = new ViewManager();
   }
   return mTheInstance;
}
void ViewManager::destroyInstance()
{}
ViewManager::ViewManager() :
  mMessageManager(MessageManager::getInstance()),
  mCurrentLayoutType(LAYOUT_NONE),
  mLayout(new QGridLayout()),
  mMainWindowsCentralWidget(new QWidget()),
  MAX_3DVIEWS(2),
  MAX_2DVIEWS(9),
  mRenderingTimer(new QTimer(this))
{
  mLayout->setSpacing(1);
  mMainWindowsCentralWidget->setLayout(mLayout);

  mView3DNames[0] = "View3D_1";
  mView3DNames[1] = "View3D_2";
  mView2DNames[0] = "View2D_1";
  mView2DNames[1] = "View2D_2";
  mView2DNames[2] = "View2D_3";
  mView2DNames[3] = "View2D_4";
  mView2DNames[4] = "View2D_5";
  mView2DNames[5] = "View2D_6";
  mView2DNames[6] = "View2D_7";
  mView2DNames[7] = "View2D_8";
  mView2DNames[8] = "View2D_9";

  for(int i=0; i<MAX_3DVIEWS; i++)
  {
    View3D* view = new View3D(mView3DNames[i], mView3DNames[i],
                              mMainWindowsCentralWidget);
    mView3DMap[view->getUid()] = view;
  }
  for(int i=0; i<MAX_2DVIEWS; i++)
  {
    View2D* view = new View2D(mView2DNames[i], mView2DNames[i],
                              mMainWindowsCentralWidget);
    mView2DMap[view->getUid()] = view;
  }

  this->setLayoutTo_3DACS_2X2();

  mRenderingTimer->start(33);
  connect(mRenderingTimer, SIGNAL(timeout()),
          this, SLOT(renderAllViewsSlot()));
}
ViewManager::~ViewManager()
{}
QWidget* ViewManager::stealCentralWidget()
{
  return mMainWindowsCentralWidget;
}
ViewManager::View2DMap* ViewManager::get2DViews()
{
  return &mView2DMap;
}
ViewManager::View3DMap* ViewManager::get3DViews()
{
  return &mView3DMap;
}
ViewManager::LayoutType ViewManager::getCurrentLayoutType()
{
  return mCurrentLayoutType;
}
ssc::View* ViewManager::getView(const std::string& uid)
{
  ssc::View* view = NULL;
  View2DMap::iterator it2 = mView2DMap.find(uid);
  if(it2 != mView2DMap.end())
  {
    view = (*it2).second;
  }
  View3DMap::iterator it3 = mView3DMap.find(uid);
  if(it3 != mView3DMap.end())
  {
    view = (*it3).second;
  }
  return view;
}
View2D* ViewManager::get2DView(const std::string& uid)
{
  View2D* view = NULL;
  View2DMap::iterator it2 = mView2DMap.find(uid);
  if(it2 != mView2DMap.end())
  {
    view = (*it2).second;
  }
  return view;
}
View3D* ViewManager::get3DView(const std::string& uid)
{
  View3D* view = NULL;
  View3DMap::iterator it3 = mView3DMap.find(uid);
  if(it3 != mView3DMap.end())
  {
    view = (*it3).second;
  }
  return view;
}
void ViewManager::setLayoutTo_3D_1X1()
{
  switch(mCurrentLayoutType)
  {
  case LAYOUT_NONE:
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_3D_1X1:
    break;
  case LAYOUT_3DACS_2X2:
    this->deactivateLayout_3DACS_2X2();
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_3DACS_1X3:
    this->deactivateLayout_3DACS_1X3();
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_ACSACS_2X3:
    this->deactivateLayout_ACSACS_2X3();
    this->activateLayout_3D_1X1();
    break;
  default:
    return;
    break;
  }
  mMessageManager->sendInfo("Layout changed to 3D_1X1");
}
void ViewManager::setLayoutTo_3DACS_2X2()
{
  switch(mCurrentLayoutType)
  {
  case LAYOUT_NONE:
    this->activateLayout_3DACS_2X2();
    break;
  case LAYOUT_3D_1X1:
    this->deactivatLayout_3D_1X1();
    this->activateLayout_3DACS_2X2();
    break;
  case LAYOUT_3DACS_2X2:
    break;
  case LAYOUT_3DACS_1X3:
    this->deactivateLayout_3DACS_1X3();
    this->activateLayout_3DACS_2X2();
    break;
  case LAYOUT_ACSACS_2X3:
    this->deactivateLayout_ACSACS_2X3();
    this->activateLayout_3DACS_2X2();
    break;
  default:
    return;
    break;
  }
  mMessageManager->sendInfo("Layout changed to 3DACS_2X2");
}
void ViewManager::setLayoutTo_3DACS_1X3()
{
  switch(mCurrentLayoutType)
  {
  case LAYOUT_NONE:
    this->activateLayout_3DACS_1X3();
    break;
  case LAYOUT_3D_1X1:
    this->deactivatLayout_3D_1X1();
    this->activateLayout_3DACS_1X3();
    break;
  case LAYOUT_3DACS_2X2:
    this->deactivateLayout_3DACS_2X2();
    this->activateLayout_3DACS_1X3();
    break;
  case LAYOUT_3DACS_1X3:
    break;
  case LAYOUT_ACSACS_2X3:
    this->deactivateLayout_ACSACS_2X3();
    this->activateLayout_3DACS_1X3();
    break;
  default:
    return;
    break;
  }
  mMessageManager->sendInfo("Layout changed to 3DACS_1X3");
}
void ViewManager::setLayoutTo_ACSACS_2X3()
{
  switch(mCurrentLayoutType)
  {
  case LAYOUT_NONE:
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_3D_1X1:
    this->deactivatLayout_3D_1X1();
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_3DACS_2X2:
    this->deactivateLayout_3DACS_2X2();
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_3DACS_1X3:
    this->deactivateLayout_3DACS_1X3();
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_ACSACS_2X3:
    break;
  default:
    return;
    break;
  }
  mMessageManager->sendInfo("Layout changed to ACSACS_2X3");
}
void ViewManager::activateLayout_3D_1X1()
{
  mLayout->addWidget(mView3DMap[mView3DNames[0]]);
  mView3DMap[mView3DNames[0]]->show();
  mCurrentLayoutType = LAYOUT_3D_1X1;
}
void ViewManager::deactivatLayout_3D_1X1()
{
  mView3DMap[mView3DNames[0]]->hide();
  mLayout->removeWidget(mView3DMap[mView3DNames[0]]);
}
void ViewManager::activateLayout_3DACS_2X2()
{
  mLayout->addWidget( mView3DMap[mView3DNames[0]],   0, 0 );
  mLayout->addWidget( mView2DMap[mView2DNames[0]],   0, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[1]],   1, 0 );
  mLayout->addWidget( mView2DMap[mView2DNames[2]],   1, 1 );
  if(mCurrentLayoutType != LAYOUT_NONE)
  {
    mView3DMap[mView3DNames[0]]->show();
    mView2DMap[mView2DNames[0]]->show();
    mView2DMap[mView2DNames[1]]->show();
    mView2DMap[mView2DNames[2]]->show();
  }
  mCurrentLayoutType = LAYOUT_3DACS_2X2;
}
void ViewManager::deactivateLayout_3DACS_2X2()
{
  mView3DMap[mView3DNames[0]]->hide();
  mView2DMap[mView2DNames[0]]->hide();
  mView2DMap[mView2DNames[1]]->hide();
  mView2DMap[mView2DNames[2]]->hide();
  mLayout->removeWidget( mView3DMap[mView3DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[1]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[2]]);
}
void ViewManager::activateLayout_3DACS_1X3()
{
  mLayout->addWidget( mView3DMap[mView3DNames[0]],   0, 0, 3, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[0]],   0, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[1]],   1, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[2]],   2, 1 );
  mView3DMap[mView3DNames[0]]->show();
  mView2DMap[mView2DNames[0]]->show();
  mView2DMap[mView2DNames[1]]->show();
  mView2DMap[mView2DNames[2]]->show();
  mCurrentLayoutType = LAYOUT_3DACS_1X3;
}
void ViewManager::deactivateLayout_3DACS_1X3()
{
  mView3DMap[mView3DNames[0]]->hide();
  mView2DMap[mView2DNames[0]]->hide();
  mView2DMap[mView2DNames[1]]->hide();
  mView2DMap[mView2DNames[2]]->hide();
  mLayout->removeWidget( mView3DMap[mView3DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[1]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[2]]);
}
void ViewManager::activateLayout_ACSACS_2X3()
{
  mLayout->addWidget( mView2DMap[mView2DNames[0]],   0, 0);
  mLayout->addWidget( mView2DMap[mView2DNames[1]],   0, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[2]],   0, 2 );
  mLayout->addWidget( mView2DMap[mView2DNames[3]],   1, 0 );
  mLayout->addWidget( mView2DMap[mView2DNames[4]],   1, 1 );
  mLayout->addWidget( mView2DMap[mView2DNames[5]],   1, 2 );
  mView2DMap[mView2DNames[0]]->show();
  mView2DMap[mView2DNames[1]]->show();
  mView2DMap[mView2DNames[2]]->show();
  mView2DMap[mView2DNames[3]]->show();
  mView2DMap[mView2DNames[4]]->show();
  mView2DMap[mView2DNames[5]]->show();
  mCurrentLayoutType = LAYOUT_ACSACS_2X3;
}
void ViewManager::deactivateLayout_ACSACS_2X3()
{
  mView2DMap[mView2DNames[0]]->hide();
  mView2DMap[mView2DNames[1]]->hide();
  mView2DMap[mView2DNames[2]]->hide();
  mView2DMap[mView2DNames[3]]->hide();
  mView2DMap[mView2DNames[4]]->hide();
  mView2DMap[mView2DNames[5]]->hide();
  mLayout->removeWidget( mView2DMap[mView2DNames[0]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[1]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[2]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[3]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[4]]);
  mLayout->removeWidget( mView2DMap[mView2DNames[5]]);
}
void ViewManager::renderAllViewsSlot()
{
  View3DMap::iterator it3D = mView3DMap.begin();
  for(; it3D != mView3DMap.end(); ++it3D)
  {
    it3D->second->getRenderWindow()->Render();
  }
  View2DMap::iterator it2D = mView2DMap.begin();
  for(; it2D != mView2DMap.end(); ++it2D)
  {
    it2D->second->getRenderWindow()->Render();
  }
}
	
void ViewManager::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  if (!currentImage.get())
    return;
  if (!currentImage->getRefVtkImageData().GetPointer())
    return;
  // Update 3D view
  RepManager* repManager = RepManager::getInstance();
  ssc::ProbeRepPtr probeRep = repManager->getProbeRep("ProbeRep_1");
  ssc::VolumetricRepPtr volumetricRep 
  = repManager->getVolumetricRep("VolumetricRep_1");
  LandmarkRepPtr landmarkRep = repManager->getLandmarkRep("LandmarkRep_1");
  
  volumetricRep->setImage(currentImage);
  probeRep->setImage(currentImage);
  landmarkRep->setImage(currentImage);
  
  //View3D* view3D_1 = this->get3DView("View3D_1");
  View3D* view3D_1 = mView3DMap[mView3DNames[0]];
  view3D_1->setRep(volumetricRep);
  //view3D_1->addRep(landmarkRep);
  //view3D_1->addRep(probeRep);
  view3D_1->getRenderer()->ResetCamera();
  view3D_1->getRenderer()->Render();
  
  // Update 2D views
  InriaRep2DPtr inriaRep2D_1 = repManager->getInria2DRep("InriaRep2D_1");
  InriaRep2DPtr inriaRep2D_2 = repManager->getInria2DRep("InriaRep2D_2");
  InriaRep2DPtr inriaRep2D_3 = repManager->getInria2DRep("InriaRep2D_3");

  //View2D* view2D_1 = this->get2DView("View2D_1");
  //View2D* view2D_2 = this->get2DView("View2D_2");
  //View2D* view2D_3 = this->get2DView("View2D_3");
  View2D* view2D_1 = mView2DMap[mView2DNames[0]];
  View2D* view2D_2 = mView2DMap[mView2DNames[1]];
  View2D* view2D_3 = mView2DMap[mView2DNames[2]];
  view2D_1->setRep(inriaRep2D_1);
  view2D_2->setRep(inriaRep2D_2);
  view2D_3->setRep(inriaRep2D_3);
  
  //test: Is render set?
  if (inriaRep2D_1->getVtkViewImage2D()->GetRenderer() == NULL)
    std::cout << "inriaRep2D_1: Lost renderer" << std::endl;
  if (inriaRep2D_2->getVtkViewImage2D()->GetRenderer() == NULL)
    std::cout << "inriaRep2D_2: Lost renderer" << std::endl;
  if (inriaRep2D_3->getVtkViewImage2D()->GetRenderer() == NULL)
    std::cout << "inriaRep2D_3: Lost renderer" << std::endl;
  
  inriaRep2D_1->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
  inriaRep2D_2->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
  inriaRep2D_3->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
  inriaRep2D_1->getVtkViewImage2D()->AddChild(inriaRep2D_2->getVtkViewImage2D());
  inriaRep2D_2->getVtkViewImage2D()->AddChild(inriaRep2D_3->getVtkViewImage2D());
  inriaRep2D_3->getVtkViewImage2D()->AddChild(inriaRep2D_1->getVtkViewImage2D());
  inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
  //TODO: ...or getBaseVtkImageData()???
  inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(currentImage->getRefVtkImageData());
  inriaRep2D_1->getVtkViewImage2D()->SyncReset();
  
  connect(probeRep.get(), SIGNAL(pointPicked(double,double,double)),
          inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
  connect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  connect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  connect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),
          probeRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
}	
}//namespace cx
