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
#ifndef CXSECONDARYVIEWLAYOUTWINDOW_H
#define CXSECONDARYVIEWLAYOUTWINDOW_H

#include <QMainWindow>

namespace cx
{

/**
 * \brief Experimental class for IPad usage.
 *
 * This detached main window can be moved onto a secondary screen.
 * The use case was a IPad where scrolling etc could control the
 * CustusX scene.
 *
 * \sa SecondaryMainWindow
 *
 * \ingroup cxGUI
 * \date 05.11.2013
 * \author christiana
 */
class SecondaryViewLayoutWindow: public QMainWindow
{
Q_OBJECT

public:
	SecondaryViewLayoutWindow(QWidget* parent);
	~SecondaryViewLayoutWindow();
private:
};

} // namespace cx

#endif // CXSECONDARYVIEWLAYOUTWINDOW_H
