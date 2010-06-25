#include "cxViewManager.h"

#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include <QSettings>
#include <QTime>
#include <QAction>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include "sscProbeRep.h"
#include "sscVolumetricRep.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "cxViewWrapper2D.h"
#include "cxViewWrapper3D.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxDataLocations.h"

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, LayoutType, LAYOUT_COUNT)
{
  "No_layout",
  "3D_1X1",
  "3DACS_2X2",
  "3DACS_1X3",
  "3DAny_1X2",
  "ACS_1X3",
  "ACSACS_2X3",
  "Any_2x3"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(cx, LayoutType, LAYOUT_COUNT);

namespace cx
{

ViewManager *ViewManager::mTheInstance = NULL;
ViewManager* viewManager() { return ViewManager::getInstance(); }
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
  mActiveLayout(LAYOUT_NONE),
  mLayout(new QGridLayout()),
  mMainWindowsCentralWidget(new QWidget()),
  MAX_3DVIEWS(2),
  MAX_2DVIEWS(15),
  mRenderingTimer(new QTimer(this)),
  mSettings(DataLocations::getSettings()),
  mRenderingTime(new QTime()),
  mNumberOfRenderings(0),
  mGlobal2DZoom(true),
  mGlobalObliqueOrientation(false)
{
  mLayout->setSpacing(2);
  mLayout->setMargin(4);
  mMainWindowsCentralWidget->setLayout(mLayout);

  mView3DNames.resize(MAX_3DVIEWS);
  for (unsigned i=0; i<mView3DNames.size(); ++i)
    mView3DNames[i] = "View3D_"+string_cast(i+1);

  mView2DNames.resize(MAX_2DVIEWS);
  for (unsigned i=0; i<mView2DNames.size(); ++i)
    mView2DNames[i] = "View2D_"+string_cast(i+1);

  for(int i=0; i<MAX_3DVIEWS; i++)
  {
    View3D* view = new View3D(mView3DNames[i], mView3DNames[i],
                              mMainWindowsCentralWidget);
    view->hide();
    mView3DMap[view->getUid()] = view;
    mViewMap[view->getUid()] = view;
    
    //Turn off rendering in vtkRenderWindowInteractor
    view->getRenderWindow()->GetInteractor()->EnableRenderOff();
  }
  for(int i=0; i<MAX_2DVIEWS; i++)
  {
    View2D* view = new View2D(mView2DNames[i], mView2DNames[i],
                              mMainWindowsCentralWidget);
    view->hide();
    mView2DMap[view->getUid()] = view;
    mViewMap[view->getUid()] = view;
    
    //Turn off rendering in vtkRenderWindowInteractor
    view->getRenderWindow()->GetInteractor()->EnableRenderOff();
  }

  // initialize view groups:
  ViewGroupPtr group;

  group.reset(new ViewGroup());
  group->addViewWrapper(ViewWrapper3DPtr(new ViewWrapper3D(1, mView3DMap["View3D_1"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_7"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_8"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_9"])));
  mViewGroups.push_back(group);

  group.reset(new ViewGroup());
  group->addViewWrapper(ViewWrapper3DPtr(new ViewWrapper3D(2, mView3DMap["View3D_2"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_10"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_11"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_12"])));
  mViewGroups.push_back(group);

  group.reset(new ViewGroup());
  //group->addViewWrapper(ViewWrapper3DPtr(new ViewWrapper3D(3, mView3DMap["View3D_3"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_13"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_14"])));
  group->addViewWrapper(ViewWrapper2DPtr(new ViewWrapper2D(mView2DMap["View2D_15"])));
  mViewGroups.push_back(group);

  this->syncOrientationMode(SyncedValue::create(0));

  // set start layout
  this->setActiveLayout(LAYOUT_3DACS_2X2_SNW);

  mRenderingTimer->start(mSettings->value("renderingInterval").toInt());
  connect(mRenderingTimer, SIGNAL(timeout()),
          this, SLOT(renderAllViewsSlot()));
  
  mShadingOn = mSettings->value("shadingOn").toBool();

  mGlobalZoom2DVal = SyncedValue::create(1);
  this->setGlobal2DZoom(mGlobal2DZoom);

  mRenderingTime->start();
}
ViewManager::~ViewManager()
{}

std::string ViewManager::layoutText(LayoutType type)
{
  return string_cast(type);
}

std::vector<LayoutType> ViewManager::availableLayouts() const
{
  std::vector<LayoutType> retval;
  retval.push_back(LAYOUT_3D_1X1);
  retval.push_back(LAYOUT_3DACS_2X2_SNW);
  retval.push_back(LAYOUT_3DACS_1X3_SNW);
  retval.push_back(LAYOUT_3DAny_1X2_SNW);
  retval.push_back(LAYOUT_ACS_1X3_SNW);
  retval.push_back(LAYOUT_ACSACS_2X3_SNW);
  retval.push_back(LAYOUT_Any_2x3_SNW);
  return retval;
}

void ViewManager::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  for (unsigned i=0; i<mViewGroups.size(); ++i)
    mViewGroups[i]->setRegistrationMode(mode);
}

LayoutType ViewManager::getActiveLayout() const
{
  return mActiveLayout;
}

/**Change layout from current to layout.
 */
void ViewManager::setActiveLayout(LayoutType layout)
{
  if (mActiveLayout==layout)
    return;

  deactivateCurrentLayout();
  activateLayout(layout);
}

ViewWrapperPtr ViewManager::getActiveView() const
{
  return mActiveView;
}

void ViewManager::setActiveView(ViewWrapperPtr viewWrapper)
{
  if(mActiveView && viewWrapper &&
      (mActiveView->getView()->getUid() == viewWrapper->getView()->getUid()))
    return;

  mActiveView = viewWrapper;
  emit activeViewChanged();
  ssc::messageManager()->sendInfo("Active view set to "+mActiveView->getView()->getUid());
}

void ViewManager::setActiveView(std::string viewUid)
{
  for(unsigned i=0; i<mViewGroups.size(); ++i)
  {
    ViewWrapperPtr viewWrapper = mViewGroups[i]->getViewWrapperFromViewUid(viewUid);
    if(viewWrapper)
    {
      this->setActiveView(viewWrapper);
      return;
    }
  }
}

void ViewManager::syncOrientationMode(SyncedValuePtr val)
{
  for(unsigned i=0; i<mViewGroups.size(); ++i)
  {
    mViewGroups[i]->syncOrientationMode(val);
  }
}


void ViewManager::setGlobal2DZoom(bool global)
{
  mGlobal2DZoom = global;

  for (unsigned i=0; i<mViewGroups.size(); ++i)
  {
    mViewGroups[i]->setGlobal2DZoom(mGlobal2DZoom, mGlobalZoom2DVal);
  }
}

bool ViewManager::getGlobal2DZoom()
{
  return mGlobal2DZoom;
}

void ViewManager::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement viewManagerNode = doc.createElement("viewManager");
  parentNode.appendChild(viewManagerNode);

  QDomElement global2DZoomNode = doc.createElement("global2DZoom");
  global2DZoomNode.appendChild(doc.createTextNode(string_cast(mGlobal2DZoom).c_str()));
  viewManagerNode.appendChild(global2DZoomNode);

  QDomElement activeLayoutNode = doc.createElement("activeLayout");
  activeLayoutNode.appendChild(doc.createTextNode(layoutText(mActiveLayout).c_str()));
  viewManagerNode.appendChild(activeLayoutNode);

  QDomElement activeViewNode = doc.createElement("activeView");
  if(mActiveView)
    activeViewNode.appendChild(doc.createTextNode(mActiveView->getView()->getUid().c_str()));
  viewManagerNode.appendChild(activeLayoutNode);

  QDomElement viewGroupsNode = doc.createElement("viewGroups");
  viewManagerNode.appendChild(viewGroupsNode);
  for (unsigned i=0; i<mViewGroups.size(); ++i)
  {
    QDomElement viewGroupNode = doc.createElement("viewGroup");
    viewGroupNode.setAttribute("index", i);
    viewGroupsNode.appendChild(viewGroupNode);

    mViewGroups[i]->addXml(viewGroupNode);
  }
}

void ViewManager::parseXml(QDomNode viewmanagerNode)
{
  QString activeViewString;
  QDomNode child = viewmanagerNode.firstChild();
  while(!child.isNull())
  {
    if(child.toElement().tagName() == "global2DZoom")
    {
      const QString global2DZoomString = child.toElement().text();
      if(!global2DZoomString.isEmpty() && global2DZoomString.toInt() == 0)
        this->setGlobal2DZoom(false);
      else
        this->setGlobal2DZoom(true);
    }else if(child.toElement().tagName() == "activeLayout")
    {
      const QString activeLayoutString = child.toElement().text();
      if(!activeLayoutString.isEmpty())
        this->setActiveLayout(string2enum<LayoutType>(activeLayoutString.toStdString()));
    }
    else if(child.toElement().tagName() == "activeView")
    {
      activeViewString = child.toElement().text();
      //set active view after all viewgroups are properly set up
      //if(!activeViewString.isEmpty())
        //this->setActiveView(getViewWrapper(activeViewString.toStdString()));
    }
    child = child.nextSibling();
  }

  QDomElement viewgroups = viewmanagerNode.namedItem("viewGroups").toElement();

  QDomNode viewgroup = viewgroups.firstChild();
  while(!viewgroup.isNull())
  {
    if (viewgroup.toElement().tagName()!="viewGroup")
    {
      viewgroup = viewgroup.nextSibling();
      continue;
    }
    int index = viewgroup.toElement().attribute("index").toInt();

    if (index<0 || index>=int(mViewGroups.size()))
    {
      viewgroup = viewgroup.nextSibling();
      continue;
    }

    mViewGroups[index]->parseXml(viewgroup);

    viewgroup = viewgroup.nextSibling();
  }

  this->setActiveView(activeViewString.toStdString());
}

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

/**deactivate the current layout, leaving an empty layout
 */
void ViewManager::deactivateCurrentLayout()
{
  for (ViewMap::iterator iter=mViewMap.begin(); iter!=mViewMap.end(); ++iter)
    deactivateView(iter->second);
}

/**activate a layout. Assumes the previous layout is already deactivated.
 */
void ViewManager::activateLayout(LayoutType toType)
{
  switch(toType)
  {
  case LAYOUT_NONE:
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_3D_1X1:
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_3DACS_2X2_SNW:
    this->activateLayout_3DACS_2X2_SNW();
    break;
  case LAYOUT_3DACS_1X3_SNW:
    this->activateLayout_3DACS_1X3_SNW();
    break;
  case LAYOUT_3DAny_1X2_SNW:
    this->activateLayout_3DAny_1X2_SNW();
    break;
  case LAYOUT_ACS_1X3_SNW:
    this->activateLayout_ACS_1X3_SNW();
    break;
  case LAYOUT_ACSACS_2X3_SNW:
    this->activateLayout_ACSACS_2X3_SNW();
    break;
  case LAYOUT_Any_2x3_SNW:
    this->activateLayout_Any_2X3_SNW();
    break;
  default:
    return;
    break;
  }

  ssc::messageManager()->sendInfo("Layout changed to "+ layoutText(mActiveLayout));
}
  
void ViewManager::deleteImageSlot(ssc::ImagePtr image)
{
  for (unsigned i=0; i<mViewGroups.size(); ++i)
    mViewGroups[i]->removeImage(image);

  emit imageDeletedFromViews(image);
}

void ViewManager::renderingIntervalChangedSlot(int interval)
{
  mRenderingTimer->stop();
  mRenderingTimer->start(interval);
}

void ViewManager::shadingChangedSlot(bool shadingOn)
{
  mShadingOn = shadingOn;

  // currently disabled: shading is now a property in ssc::Image.
  // Remove this method???
  
//  ssc::VolumetricRepPtr volumetricRep
//  = RepManager::getInstance()->getVolumetricRep("VolumetricRep_1");
//  if(volumetricRep->getImage())
//  {
//    if(shadingOn)
//      volumetricRep->getVtkVolume()->GetProperty()->ShadeOn();
//    else
//      volumetricRep->getVtkVolume()->GetProperty()->ShadeOff();
//  }
}
  
void ViewManager::activate2DView(int group, int index, ssc::PLANE_TYPE plane, int row, int col, int rowSpan, int colSpan)
{
  mViewGroups[group]->initializeView(index, plane);
  ssc::View* view = mViewGroups[group]->getViews()[index];
  mLayout->addWidget(view, row, col, rowSpan, colSpan );
  view->show();
}
void ViewManager::activate3DView(int group, int index, int row, int col, int rowSpan, int colSpan)
{
  ssc::View* view = mViewGroups[group]->getViews()[index];
  mLayout->addWidget(view, row, col, rowSpan, colSpan );
  view->show();
}

void ViewManager::deactivateView(ssc::View* view)
{
  view->hide();
  mLayout->removeWidget(view);
}

void ViewManager::activateLayout_3D_1X1()
{
  activate3DView(0, 0,                  0, 0);

  mActiveLayout = LAYOUT_3D_1X1;
  emit activeLayoutChanged();
}

void ViewManager::activateLayout_3DACS_2X2_SNW()
{
  activate3DView(0, 0,                  0, 0);
  activate2DView(0, 1, ssc::ptAXIAL,    0, 1);
  activate2DView(0, 2, ssc::ptCORONAL,  1, 0);
  activate2DView(0, 3, ssc::ptSAGITTAL, 1, 1);

  mActiveLayout = LAYOUT_3DACS_2X2_SNW;
  emit activeLayoutChanged();
}

void ViewManager::activateLayout_3DACS_1X3_SNW()
{
  activate3DView(0, 0,                  0, 0, 3, 1);
  activate2DView(0, 1, ssc::ptAXIAL,    0, 1);
  activate2DView(0, 2, ssc::ptCORONAL,  1, 1);
  activate2DView(0, 3, ssc::ptSAGITTAL, 2, 1);

  mActiveLayout = LAYOUT_3DACS_1X3_SNW;
  emit activeLayoutChanged();
}

void ViewManager::activateLayout_ACS_1X3_SNW()
{
  activate2DView(0, 1, ssc::ptAXIAL,    0, 0);
  activate2DView(0, 2, ssc::ptCORONAL,  0, 1);
  activate2DView(0, 3, ssc::ptSAGITTAL, 0, 2);

  mActiveLayout = LAYOUT_ACS_1X3_SNW;
  emit activeLayoutChanged();
}

void ViewManager::activateLayout_ACSACS_2X3_SNW()
{
  activate2DView(0, 1, ssc::ptAXIAL,    0, 0);
  activate2DView(0, 2, ssc::ptCORONAL,  0, 1);
  activate2DView(0, 3, ssc::ptSAGITTAL, 0, 2);
  activate2DView(1, 1, ssc::ptAXIAL,    1, 0);
  activate2DView(1, 2, ssc::ptCORONAL,  1, 1);
  activate2DView(1, 3, ssc::ptSAGITTAL, 1, 2);

  mActiveLayout = LAYOUT_ACSACS_2X3_SNW;
  emit activeLayoutChanged();
}

void ViewManager::activateLayout_Any_2X3_SNW()
{
  activate2DView(0, 1, ssc::ptANYPLANE, 0, 0);
  activate2DView(0, 2, ssc::ptSIDEPLANE, 1, 0);
  activate2DView(1, 1, ssc::ptANYPLANE, 0, 1);
  activate2DView(1, 2, ssc::ptSIDEPLANE, 1, 1);
  activate2DView(2, 1, ssc::ptANYPLANE, 0, 2);
  activate2DView(2, 2, ssc::ptSIDEPLANE, 1, 2);

  mActiveLayout = LAYOUT_Any_2x3_SNW;
  emit activeLayoutChanged();
}

void ViewManager::activateLayout_3DAny_1X2_SNW()
{
  activate3DView(0, 0,                  0, 0);
  activate2DView(0, 1, ssc::ptANYPLANE, 0, 1);

  mActiveLayout = LAYOUT_3DAny_1X2_SNW;
  emit activeLayoutChanged();
}

void ViewManager::renderAllViewsSlot()
{
  for(ViewMap::iterator iter=mViewMap.begin(); iter != mViewMap.end(); ++iter)
  {
    if(iter->second->isVisible())
    {
      iter->second->getRenderWindow()->Render(); // previous version: renders even when nothing is changed
      //iter->second->render(); // render only changed scenegraph
    }
  }
  
  if(mRenderingTime->elapsed()>1000)
  {
    emit fps(mNumberOfRenderings);
    mRenderingTime->restart();
    mNumberOfRenderings = 1;
  }
  else
    mNumberOfRenderings++;
}

}//namespace cx
