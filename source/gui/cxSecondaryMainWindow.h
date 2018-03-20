/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSECONDARYMAINWINDOW_H_
#define CXSECONDARYMAINWINDOW_H_

#include "cxGuiExport.h"

#include <QMainWindow>

namespace cx
{

/**
 * \brief Experimental class for IPad usage.
 * \ingroup cx_gui
 *
 * This detached main window can be moved onto a secondary screen.
 * The use case was a IPad where scrolling etc could control the
 * CustusX scene.
 *
 */
class cxGui_EXPORT SecondaryMainWindow: public QMainWindow
{
Q_OBJECT

public:
	SecondaryMainWindow(QWidget* parent, QWidget *widget);
	~SecondaryMainWindow();
private:
	void addAsDockWidget(QWidget* widget, bool visible);
};

} // namespace cx

#endif /* CXSECONDARYMAINWINDOW_H_ */
