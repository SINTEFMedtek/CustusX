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

#ifndef CXGUIEXTENDERSERVICE_H_
#define CXGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderServiceExport.h"

#include <QObject>
#include "boost/shared_ptr.hpp"
class QWidget;
class QToolBar;
#include <vector>

#define GUIExtenderService_iid "cx::GUIExtenderService"

namespace cx
{
typedef boost::shared_ptr<class GUIExtenderService> GUIExtenderServicePtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;
typedef boost::shared_ptr<class VideoService> VideoServicePtr;

/** Interface for service that extends the user interface with more widgets.
 *
 * Provide a collection of widgets and toolbars that can be added to for
 * example the MainWindow. This is the most generic extension point and can
 * be used for anything that can be accessed from a GUI.
 */
class cxGUIExtenderService_EXPORT GUIExtenderService : public QObject
{
	Q_OBJECT
public:
	struct CategorizedWidget
	{
		CategorizedWidget() {}
		CategorizedWidget(QWidget* widget, QString category) : mWidget(widget), mCategory(category) {}
		QWidget* mWidget;
		QString mCategory;
	};
	virtual ~GUIExtenderService() {}

	virtual std::vector<CategorizedWidget> createWidgets() const = 0;
	virtual std::vector<QToolBar*> createToolBars() const { return std::vector<QToolBar*>(); }
};

} // namespace cx
Q_DECLARE_INTERFACE(cx::GUIExtenderService, GUIExtenderService_iid)


#endif /* CXGUIEXTENDERSERVICE_H_ */
