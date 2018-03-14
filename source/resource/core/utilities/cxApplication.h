/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXAPPLICATION_H_
#define CXAPPLICATION_H_

#include "cxResourceExport.h"

#include <QApplication>

namespace cx
{

/**
 * Override of QApplication,
 * Use when you want to catch exceptions thrown
 * from within an event handler and not caught
 * anywhere else.
 *
 * \ingroup cx_resource_core_utilities
 * \date March 6, 2013
 * \author christiana
 */
class cxResource_EXPORT Application : public QApplication
{
public:
	Application(int& argc, char** argv);
	virtual ~Application() {}
	virtual bool notify(QObject *rec, QEvent *ev);
private:
	void reportException(QString text);
	void force_C_locale_decimalseparator();
};

void cxResource_EXPORT bringWindowToFront(QWidget* window);
cxResource_EXPORT QWidget* getMainWindow();
template<typename T>
cxResource_EXPORT T findMainWindowChildWithObjectName(QString objectName);
void cxResource_EXPORT triggerMainWindowActionWithObjectName(QString actionName);

} // namespace cx

#endif // CXAPPLICATION_H_
