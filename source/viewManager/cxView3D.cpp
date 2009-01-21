#include "cxView3D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

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
  mContextMenu(new QMenu(this))
{
  mUid = uid;
  mName = name;
}
View3D::~View3D()
{}
void View3D::contextMenuEvent(QContextMenuEvent *event)
{
    mContextMenu->clear();

    QAction *enAction = new QAction( "View3D::En", this );
    QAction *toAction = new QAction( "View3D::To", this );
    QAction *treAction = new QAction( "View3D::Tre", this );
    mContextMenu->addAction( enAction );
    mContextMenu->addAction( toAction );
    mContextMenu->addAction( treAction );

   QAction *theAction = mContextMenu->exec(event->globalPos());
}
}//namespace cx
