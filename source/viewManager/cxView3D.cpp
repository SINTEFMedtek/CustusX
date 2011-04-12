#include "cxView3D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include "sscImage.h"
#include "sscVolumetricRep.h"
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscToolRep3D.h"
#include "sscTypeConversions.h"
#include "cxViewManager.h"

namespace cx
{
/**
 * \param uid the views unique id
 * \param name the views name
 * \param parent the views parent
 * \param f flags
 */
View3D::View3D(const QString& uid, const QString& name, QWidget *parent, Qt::WFlags f) :
  ssc::View(parent, f)
{
  mUid = uid;
  mName = name;
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  mRenderer->GetActiveCamera()->SetClippingRange(1, 2000);
}

View3D::~View3D()
{
}


}//namespace cx
