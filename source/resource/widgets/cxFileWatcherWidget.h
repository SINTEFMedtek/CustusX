/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXFILEWATCHERWIDGET_H_
#define CXFILEWATCHERWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"

class QFileSystemWatcher;
class QFile;

namespace cx
{

/**
 * \class FileWatcherWidget
 *
 * \brief Baseclass for widgets that should watch a file.
 *
 * \ingroup cx_resource_widgets
 * \date Nov 7, 2012
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceWidgets_EXPORT FileWatcherWidget : public BaseWidget
{
	Q_OBJECT

public:
	FileWatcherWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~FileWatcherWidget();

public slots:
	virtual void previewFileSlot(const QString& absoluteFilePath) = 0;

protected:
	bool internalOpenNewFile(const QString absoluteFilePath);
	void watchFile(bool on);

	QFileSystemWatcher*				mFileSystemWatcher;
	boost::shared_ptr<QFile>	mCurrentFile;
};

} /* namespace cx */
#endif /* CXFILEWATCHERWIDGET_H_ */
