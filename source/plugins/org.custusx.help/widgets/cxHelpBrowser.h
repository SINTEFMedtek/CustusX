/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXHELPBROWSER_H
#define CXHELPBROWSER_H

#include "boost/shared_ptr.hpp"
#include <QTextBrowser>
#include "org_custusx_help_Export.h"

namespace cx
{
typedef boost::shared_ptr<class HelpEngine> HelpEnginePtr;

/**
 *
 * \ingroup org_custusx_help
 *
 * \date 2014-09-30
 * \author Christian Askeland
 */
class org_custusx_help_EXPORT HelpBrowser : public QTextBrowser
{
	Q_OBJECT

public:
	HelpBrowser(QWidget *parent, HelpEnginePtr engine);
	virtual void setSource(const QUrl& name);
	void listenToEngineKeywordActivated();

public slots:
	void showHelpForKeyword(const QString &id);

private:
	virtual QVariant loadResource(int type, const QUrl &name);
	HelpEnginePtr mEngine;
};


}//end namespace cx

#endif // CXHELPBROWSER_H
