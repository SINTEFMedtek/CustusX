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
#ifndef CXNETWORKCONNECTIONSWIDGET_H
#define CXNETWORKCONNECTIONSWIDGET_H

#include "org_custusx_core_openigtlink_Export.h"

#include "cxBaseWidget.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <QProcess>
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxVisServices.h"

class QPushButton;
class QComboBox;
class QLineEdit;
class QStackedWidget;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{
//class SimulateUSWidget;
//class FileInputWidget;
//class StreamerService;
//class XmlOptionFile;
class DetailedLabeledComboBoxWidget;
//typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
//typedef boost::shared_ptr<class StringPropertyActiveVideoSource> StringPropertyActiveVideoSourcePtr;
//typedef boost::shared_ptr<class StringProperty> StringPropertyPtr;
//typedef boost::shared_ptr<class Tool> ToolPtr;
//typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class NetworkServiceImpl> NetworkConnectionManagerPtr;

/**
 * \brief GUI for managing network connections
 *
 * \date 2015-09-23
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_openigtlink_EXPORT NetworkConnectionsWidget : public BaseWidget
{
  Q_OBJECT

public:
	NetworkConnectionsWidget(NetworkConnectionManagerPtr connections, QWidget* parent=NULL);
	virtual ~NetworkConnectionsWidget() {}

private:
	NetworkConnectionManagerPtr mConnections;
	StringPropertyPtr mConnectionSelector;

	void onNetworkManagerChanged();
	void onConnectionSelected();
	void updateConnectionWidget();

	XmlOptionFile mOptions;
	DetailedLabeledComboBoxWidget* mConnectionSelectionWidget;
	class WidgetInGroupBox* mOptionsWidget;

};

}//end namespace cx

#endif // CXNETWORKCONNECTIONSWIDGET_H
