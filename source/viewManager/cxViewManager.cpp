#include "cxViewManager.h"

#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include "vtkRenderWindow.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxMessageManager.h"

namespace cx
{

ViewManager *ViewManager::mTheInstance = NULL;

ViewManager::ViewManager* ViewManager::getInstance()
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
  MAX_3DVIEWS(2),
  MAX_2DVIEWS(9),
  mRenderingTimer(new QTimer(this))
{
  mLayout->setSpacing(1);

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
    View3D* view = new View3D(mView3DNames[i]);
    mView3DMap[view->getUid()] = view;
  }
  for(int i=0; i<MAX_2DVIEWS; i++)
  {
    View2D* view = new View2D(mView2DNames[i]);
    mView2DMap[view->getUid()] = view;
  }

  this->setLayoutTo_3DACS_2X2();

  mRenderingTimer->start(33);
  connect(mRenderingTimer, SIGNAL(timeout()),
          this, SLOT(renderAllViewsSlot()));

}
ViewManager::~ViewManager()
{}
void ViewManager::setCentralWidget(QWidget& centralWidget)
{
  mCentralWidget = &centralWidget;
  mCentralWidget->setLayout(mLayout);
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
}//namespace cx
