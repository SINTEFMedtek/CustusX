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
#ifndef CXAPPLICATION_H
#define CXAPPLICATION_H

#include <QApplication>

namespace cx
{

/**
 * Override of QApplication,
 * Use when you want to catch exceptions thrown
 * from within an event handler and not caught
 * anywhere else.
 *
 * \ingroup cxGUI
 * \date March 6, 2013
 * \author christiana
 */
class Application : public QApplication
{
public:
	Application(int& argc, char** argv);
	virtual ~Application() {}
	virtual bool notify(QObject *rec, QEvent *ev);
};

} // namespace cx

#endif // CXAPPLICATION_H
