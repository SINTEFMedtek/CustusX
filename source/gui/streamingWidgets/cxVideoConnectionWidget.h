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

#ifndef CXIGTLINKWIDGET_H_
#define CXIGTLINKWIDGET_H_

#include "cxGuiExport.h"

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
class SimulateUSWidget;
class FileInputWidget;
class StreamerService;
class XmlOptionFile;
class DetailedLabeledComboBoxWidget;
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
typedef boost::shared_ptr<class StringPropertyActiveVideoSource> StringPropertyActiveVideoSourcePtr;
typedef boost::shared_ptr<class StringProperty> StringPropertyPtr;
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/**
 * \brief GUI for setting up a connection to a video stream
 *
 * \ingroup cx_gui
 *
 * \date 2010.10.27
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 *
 */
class cxGui_EXPORT VideoConnectionWidget : public BaseWidget
{
  Q_OBJECT

public:
	VideoConnectionWidget(VisServicesPtr services, QWidget* parent);
	virtual ~VideoConnectionWidget();

protected slots:
	void toggleConnectServer();
	void serverStatusChangedSlot();
	void importStreamImageSlot();
	void selectGuiForConnectionMethodSlot();
	void onServiceAdded(StreamerService *service);
	void onServiceRemoved(StreamerService *service);

protected:
	StringPropertyActiveVideoSourcePtr initializeActiveVideoSourceSelector();
	QFrame* wrapStackedWidgetInAFrame();
	QPushButton* initializeConnectButton();
	QPushButton* initializeImportStreamImageButton();
	QWidget* wrapVerticalStretch(QWidget* input);
	Transform3D calculate_rMd_ForAProbeImage(ToolPtr probe);
	QString generateFilename(VideoSourcePtr videoSource);
	void saveAndImportSnapshot(vtkImageDataPtr input, QString filename, Transform3D rMd);

	QPushButton* mConnectButton;
	QPushButton* mImportStreamImageButton;
	QVBoxLayout* mToptopLayout;
	QStackedWidget* mStackedWidget;
	StringPropertyPtr mConnectionSelector;
	StringPropertyActiveVideoSourcePtr mActiveVideoSourceSelector;
	XmlOptionFile mOptions;
	DetailedLabeledComboBoxWidget* mConnectionSelectionWidget;
	QFrame* mStackedWidgetFrame;

	VisServicesPtr mServices;

private:
	QWidget* createStreamerWidget(StreamerService* service);
	void addServiceToSelector(StreamerService *service);
	void removeServiceFromSelector(StreamerService *service);
	void removeServiceWidget(QString name);
	void addExistingStreamerServices();

	std::map<QString, QWidget*> mStreamerServiceWidgets;
};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
