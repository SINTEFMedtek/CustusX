// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxView2D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

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
    ViewWidget(parent, f)
{
  mUid = uid;
  mName = name;
  this->setContextMenuPolicy(Qt::CustomContextMenu);
}
View2D::~View2D()
{
}

}//namespace cx
