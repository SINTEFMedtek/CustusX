/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSECONDARYVIEWLAYOUTWINDOW_H
#define CXSECONDARYVIEWLAYOUTWINDOW_H

#include "cxGuiExport.h"

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
 * \ingroup cx_gui
 * \date 05.11.2013
 * \author christiana
 */
class cxGui_EXPORT SecondaryViewLayoutWindow: public QMainWindow
{
Q_OBJECT

public:
	SecondaryViewLayoutWindow(QWidget* parent);
	~SecondaryViewLayoutWindow();

	void tryShowOnSecondaryScreen();

protected:
	virtual void showEvent(QShowEvent* event);
	virtual void hideEvent(QCloseEvent* event);
	virtual void closeEvent(QCloseEvent *event);
private:
	QString toString(QRect r) const;
	int findSmallestSecondaryScreen();
};

} // namespace cx

#endif // CXSECONDARYVIEWLAYOUTWINDOW_H
