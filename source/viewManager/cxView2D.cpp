#include "cxView2D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

/**
 * cxView2D2.cpp
 *
 * \brief
 *
 * \date Dec 9, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
View2D::View2D(const std::string& uid, const std::string& name, QWidget *parent, Qt::WFlags f) :
  View(parent, f),
  mContextMenu(new QMenu(this))
{
    mUid = uid;
    mName = name;
}
View2D::~View2D()
{}
void View2D::contextMenuEvent(QContextMenuEvent *event)
{
    mContextMenu->clear();

    QAction *enAction = new QAction( "View2D::En", this );
    QAction *toAction = new QAction( "View2D::To", this );
    mContextMenu->addAction( enAction );
    mContextMenu->addAction( toAction );

   QAction *theAction = mContextMenu->exec(event->globalPos());
}

}//namespace cx
