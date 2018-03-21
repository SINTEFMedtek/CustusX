/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxHelpBrowser.h"

#include <QHelpEngine>
#include "cxHelpEngine.h"
#include "cxTypeConversions.h"
#include <iostream>
#include <QDesktopServices>

namespace cx
{
HelpBrowser::HelpBrowser(QWidget *parent, HelpEnginePtr engine)
	: QTextBrowser(parent), mEngine(engine)
{
}

void HelpBrowser::showHelpForKeyword(const QString &id)
{
	if (mEngine->engine())
	{
		QMap<QString, QUrl> links = mEngine->engine()->linksForIdentifier(id);
		if (links.count())
		{
			setSource(links.first());
		}
	}
}

void HelpBrowser::setSource(const QUrl& name)
{
	if (name.scheme() == "qthelp")
		QTextBrowser::setSource(name);
	else
	{
		QDesktopServices::openUrl(name);
	}

}

void HelpBrowser::listenToEngineKeywordActivated()
{
	connect(mEngine.get(), SIGNAL(keywordActivated(QString)), this, SLOT(showHelpForKeyword(const QString&)));
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
	if (type < 4 && mEngine->engine())
	{
		QUrl url(name);
		if (name.isRelative())
			url = source().resolved(url);

		if (url.scheme() == "qthelp")
			return QVariant(mEngine->engine()->fileData(url));
		else
			return QTextBrowser::loadResource(type, url);
	}
	return QVariant();
}

}//end namespace cx
