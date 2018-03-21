/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

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
		CategorizedWidget(QWidget* widget, QString category, bool placeInSeparateWindow = false) :
			mWidget(widget), mCategory(category), mPlaceInSeparateWindow(placeInSeparateWindow) {}
		QWidget* mWidget;
		QString mCategory;
		bool mPlaceInSeparateWindow;
	};
	virtual ~GUIExtenderService() {}

	virtual std::vector<CategorizedWidget> createWidgets() const = 0;
	virtual std::vector<QToolBar*> createToolBars() const { return std::vector<QToolBar*>(); }
};

} // namespace cx
Q_DECLARE_INTERFACE(cx::GUIExtenderService, GUIExtenderService_iid)


#endif /* CXGUIEXTENDERSERVICE_H_ */
