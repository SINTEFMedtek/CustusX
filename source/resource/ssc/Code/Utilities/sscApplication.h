// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.
#ifndef SSCAPPLICATION_H
#define SSCAPPLICATION_H

#include <QApplication>

namespace ssc
{

/**
 * Override of QApplication,
 * Use when you want to catch exceptions thrown
 * from within an event handler and not caught
 * anywhere else.
 *
 * \ingroup sscUtility
 * \date March 6, 2013
 * \author christiana
 */
class Application : public QApplication
{
public:
	Application(int& argc, char** argv);
	virtual ~Application() {}
	virtual bool notify(QObject *rec, QEvent *ev);
private:
	void reportException(QString text);
};

} // namespace ssc

#endif // SSCAPPLICATION_H
