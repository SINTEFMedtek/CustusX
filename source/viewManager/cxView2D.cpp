#include "cxView2D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
//#include "sscMessageManager.h"
//#include "sscDataManager.h"
//#include "cxRepManager.h"
//#include "cxViewManager.h"

namespace cx
{
/**
 * \param uid the views unique id
 * \param name the views name
 * \param parent the views parent
 * \param f flags
 */
View2D::View2D(const QString& uid, const QString& name,
    QWidget *parent, Qt::WFlags f) :
  View(parent, f)
{
  mUid = uid;
  mName = name;
  this->setContextMenuPolicy(Qt::CustomContextMenu);
}
View2D::~View2D()
{
}

}//namespace cx
