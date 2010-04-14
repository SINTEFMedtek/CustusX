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
#include "cxMessageManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxViewGroup.h"
#include "cxViewGroupInria.h"
#include "cxViewWrapper2D.h"
#include "cxViewWrapper3D.h"
#include "cxDataManager.h"
#include "cxToolManager.h"

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(cx, LayoutType, LAYOUT_COUNT)
{
  "No_layout",
  "3D_1X1",
  "3DACS_2X2",
  "3DACS_1X3",
  "ACSACS_2X3",
  "3DACS_2X2_SNW",
  "3DAny_1X2_SNW",
  "ACSACS_2X3_SNW",
  "Any_2x3_SNW"
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
  mActiveView(NULL),
  MAX_3DVIEWS(2),
  MAX_2DVIEWS(15),
  mRenderingTimer(new QTimer(this)),
  mSettings(new QSettings()),
  mRenderingTime(new QTime()),
  mNumberOfRenderings(0)
{
  mLayout->setSpacing(1);
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

  group.reset(new ViewGroupInria(1,mView2DMap["View2D_1"], mView2DMap["View2D_2"],mView2DMap["View2D_3"]));
  mViewGroups.push_back(group);
  group.reset(new ViewGroupInria(2,mView2DMap["View2D_4"], mView2DMap["View2D_5"],mView2DMap["View2D_6"]));
  mViewGroups.push_back(group);

  // set start layout
  this->setActiveLayout(LAYOUT_3DACS_2X2);

  mRenderingTimer->start(mSettings->value("renderingInterval").toInt());
  connect(mRenderingTimer, SIGNAL(timeout()),
          this, SLOT(renderAllViewsSlot()));
  
  mShadingOn = mSettings->value("shadingOn").toBool();
  
  mRenderingTime->start();
}
ViewManager::~ViewManager()
{}

std::string ViewManager::layoutText(LayoutType type)
{

  return string_cast(type);
//  switch (type)
//  {
//  case LAYOUT_NONE :          return "No_layout";
//  case LAYOUT_3D_1X1 :        return "3D_1X1";
//  case LAYOUT_3DACS_2X2 :     return "3DACS_2X2";
//  case LAYOUT_3DACS_1X3 :     return "3DACS_1X3";
//  case LAYOUT_ACSACS_2X3 :    return "ACSACS_2X3";
//  case LAYOUT_3DACS_2X2_SNW : return "3DACS_2X2_SNW";
//  case LAYOUT_3DAny_1X2_SNW : return "3DAny_1X2_SNW";
//  case LAYOUT_ACSACS_2X3_SNW : return "ACSACS_2X3_SNW";
//  default: return "Undefined layout";
//  }
}

//ViewManager::LayoutType ViewManager::layoutTypeFromText(std::string text)
//{
//  if(text == "No_layout")
//  {
//    return LAYOUT_NONE;
//  } else if(text == "3D_1X1")
//  {
//    return LAYOUT_3D_1X1;
//  } else if(text == "3DACS_2X2")
//  {
//    return LAYOUT_3DACS_2X2;
//  } else if(text == "3DACS_1X3")
//  {
//    return LAYOUT_3DACS_1X3;
//  } else if(text == "ACSACS_2X3")
//  {
//    return LAYOUT_ACSACS_2X3;
//  } else if(text == "3DACS_2X2_SNW")
//  {
//    return LAYOUT_3DACS_2X2_SNW;
//  } else if(text == "3DAny_1X2_SNW")
//  {
//    return LAYOUT_3DAny_1X2_SNW;
//  } else if (text == "ACSACS_2X3_SNW")
//  {
//    return LAYOUT_ACSACS_2X3_SNW;
//  } else
//  {
//    return LAYOUT_NONE;
//  }
//}

std::vector<LayoutType> ViewManager::availableLayouts() const
{
  std::vector<LayoutType> retval;
  retval.push_back(LAYOUT_3D_1X1);
  retval.push_back(LAYOUT_3DACS_2X2);
  retval.push_back(LAYOUT_3DACS_1X3);
  retval.push_back(LAYOUT_ACSACS_2X3);
  retval.push_back(LAYOUT_3DACS_2X2_SNW);
  retval.push_back(LAYOUT_3DAny_1X2_SNW);
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
  //std::cout << "Setting active layout to "<< layoutText(layout) << std::endl;
  if (mActiveLayout==layout)
    return;

  deactivateCurrentLayout();
  activateLayout(layout);
}
ssc::View* ViewManager::getActiveView() const
{
  return mActiveView;
}
void ViewManager::setActiveView(ssc::View* view)
{
  if(mActiveView && view && mActiveView->getUid() != view->getUid())
    return;

  mActiveView = view;
  emit activeViewChanged();
}
void ViewManager::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement viewManagerNode = doc.createElement("viewManager");
  parentNode.appendChild(viewManagerNode);

  QDomElement activeLayoutNode = doc.createElement("activeLayout");
  activeLayoutNode.appendChild(doc.createTextNode(layoutText(mActiveLayout).c_str()));
  viewManagerNode.appendChild(activeLayoutNode);

  QDomElement activeViewNode = doc.createElement("activeView");
  if(mActiveView)
    activeViewNode.appendChild(doc.createTextNode(mActiveView->getUid().c_str()));
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
  QDomNode child = viewmanagerNode.firstChild();
  while(!child.isNull())
  {
    if(child.toElement().tagName() == "activeLayout")
    {
      //std::cout << "Found activeLayout tag." << std::endl;
      const QString activeLayoutString = child.toElement().text();
      if(!activeLayoutString.isEmpty())
        this->setActiveLayout(string2enum<LayoutType>(activeLayoutString.toStdString()));
    }
    else if(child.toElement().tagName() == "activeView")
    {
      const QString activeViewString = child.toElement().text();
      if(!activeViewString.isEmpty())
        this->setActiveView(getView(activeViewString.toStdString()));
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
/*void ViewManager::changeLayout(LayoutType toType)
{
  if (mActiveLayout==toType)
    return;

  deactivateCurrentLayout();
  activateLayout(toType);
}*/

/**activate a layout. Assumes the previous layout is already deactivated.
 */
void ViewManager::activateLayout(LayoutType toType)
{
  // reset the center for easier viewing.
  //Navigation().centerToImageCenter(); // we have buttons for this.

  switch(toType)
  {
  case LAYOUT_NONE:
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_3D_1X1:
    this->activateLayout_3D_1X1();
    break;
  case LAYOUT_3DACS_2X2:
    this->activateLayout_3DACS_2X2();
    break;
  case LAYOUT_3DACS_1X3:
    this->activateLayout_3DACS_1X3();
    break;
  case LAYOUT_ACSACS_2X3:
    this->activateLayout_ACSACS_2X3();
    break;
  case LAYOUT_3DACS_2X2_SNW:
    this->activateLayout_3DACS_2X2_SNW();
    break;
  case LAYOUT_3DAny_1X2_SNW:
    this->activateLayout_3DAny_1X2_SNW();
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

  messageManager()->sendInfo("Layout changed to "+ layoutText(mActiveLayout));
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
  
  ssc::VolumetricRepPtr volumetricRep
  = RepManager::getInstance()->getVolumetricRep("VolumetricRep_1");
  if(volumetricRep->getImage())
    if(shadingOn)
      volumetricRep->getVtkVolume()->GetProperty()->ShadeOn();
    else
      volumetricRep->getVtkVolume()->GetProperty()->ShadeOff();
}
  
void ViewManager::activateView(ssc::View* view, int row, int col, int rowSpan, int colSpan)
{
  mLayout->addWidget(view, row, col, rowSpan, colSpan );
  view->show();
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

void ViewManager::activateLayout_3DACS_2X2()
{
  activate3DView(0, 0,                  0, 0);

  activateView(mView2DMap[mView2DNames[0]],   0, 1);
  activateView(mView2DMap[mView2DNames[1]],   1, 0);
  activateView(mView2DMap[mView2DNames[2]],   1, 1);

  mActiveLayout = LAYOUT_3DACS_2X2;
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
  activate2DView(0, 2, ssc::ptANYPLANE, 1, 0);
  activate2DView(1, 1, ssc::ptANYPLANE, 0, 1);
  activate2DView(1, 2, ssc::ptANYPLANE, 1, 1);
  activate2DView(2, 1, ssc::ptANYPLANE, 0, 2);
  activate2DView(2, 2, ssc::ptANYPLANE, 1, 2);

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

void ViewManager::activateLayout_3DACS_1X3()
{
  activate3DView(0, 0,                  0, 0, 3, 1);

  activateView(mView2DMap[mView2DNames[0]],   0, 1);
  activateView(mView2DMap[mView2DNames[1]],   1, 1);
  activateView(mView2DMap[mView2DNames[2]],   2, 1);

  mActiveLayout = LAYOUT_3DACS_1X3;
  emit activeLayoutChanged();
}

void ViewManager::activateLayout_ACSACS_2X3()
{
  activateView(mView2DMap[mView2DNames[0]],   0, 0);
  activateView(mView2DMap[mView2DNames[1]],   0, 1);
  activateView(mView2DMap[mView2DNames[2]],   0, 2);

  activateView(mView2DMap[mView2DNames[3]],   1, 0);
  activateView(mView2DMap[mView2DNames[4]],   1, 1);
  activateView(mView2DMap[mView2DNames[5]],   1, 2);

  mActiveLayout = LAYOUT_ACSACS_2X3;
  emit activeLayoutChanged();
}
  
/*void ViewManager::removeRepFromViews(ssc::RepPtr rep)
{
  View3DMap::iterator it3D = mView3DMap.begin();
  for(; it3D != mView3DMap.end(); ++it3D)
    it3D->second->removeRep(rep);
  View2DMap::iterator it2D = mView2DMap.begin();
  for(; it2D != mView2DMap.end(); ++it2D)
    it2D->second->removeRep(rep);
}*/
/*void ViewManager::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  for (unsigned i=0; i<mViewGroups.size(); ++i)
  {
    mViewGroups[i]->setImage(currentImage);
  }
}*/
void ViewManager::renderAllViewsSlot()
{
  for(ViewMap::iterator iter=mViewMap.begin(); iter != mViewMap.end(); ++iter)
  {
    if(iter->second->isVisible())
      iter->second->getRenderWindow()->Render();
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
//void ViewManager::addXml(QDomNode& parentNode)
//{
//  QDomDocument doc = parentNode.ownerDocument();
//  QDomElement base = doc.createElement("viewManager");
//  parentNode.appendChild(base);
//
//  base.setAttribute("layoutType", qstring_cast(mCurrentLayoutType));
//
//
////  m_rMpr_History->addXml(base);
////
////  QDomDocument doc = parentNode.ownerDocument();
////  QDomElement base = doc.createElement("registrationTransform");
////  parentNode.appendChild(base);
////
////  base.setAttribute("timestamp", mTimestamp.toString(timestampSecondsFormat()));
////  base.setAttribute("type", mType);
////  base.appendChild(doc.createTextNode(qstring_cast(mValue)));
//}
//
//void ViewManager::parseXml(QDomNode dataNode)
//{
//  if (dataNode.isNull())
//    return;
//
//  QDomElement base = dataNode.toElement();
//  QString layout = base.attribute("layoutType");
//  changeLayout(string2enum<LayoutType>(string_cast(layout)));
////  LayoutType = base
//
////  QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
////  m_rMpr_History->parseXml(registrationHistory);
////
////
////  QDomElement base = dataNode.toElement();
////
////  mTimestamp = QDateTime::fromString(base.attribute("timestamp"), timestampSecondsFormat());
////  mType = base.attribute("type");
////  mValue = Transform3D::fromString(base.text());
//
//
//}


}//namespace cx
