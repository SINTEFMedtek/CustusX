/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXHELPENGINE_H
#define CXHELPENGINE_H

#include <map>
#include "boost/shared_ptr.hpp"
#include "org_custusx_help_Export.h"

#include <QString>
#include <QObject>

class QWidget;
class QHelpEngineCore;
class QHelpEngine;
class QHelpSearchEngine;

namespace cx
{

/**
 * Core functionality and shared resource for help plugin.
 *
 * \ingroup org_custusx_help
 *
 * \date 2014-09-30
 * \author Christian Askeland
 */
class org_custusx_help_EXPORT HelpEngine : public QObject
{
	Q_OBJECT
public:
	HelpEngine();
	~HelpEngine();
	QHelpEngine* engine() { return helpEngine; }
signals:
	void keywordActivated(QString);
private slots:
	void focusObjectChanged(QObject* newFocus);
	void focusChanged(QWidget * old, QWidget * now);
	void setInitialPage();
private:
	QString findBestMatchingKeyword(QObject* object);
	QHelpEngine* helpEngine;
	void setupDataWithWarning();

	bool isBreakChar(QChar c) const;
	bool isBreakChar(QString text, int index) const;

	void setupDocFile();
};
typedef boost::shared_ptr<HelpEngine> HelpEnginePtr;

}

#endif // CXHELPENGINE_H
