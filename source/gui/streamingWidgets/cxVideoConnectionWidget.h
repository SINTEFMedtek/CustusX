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

#include "cxBaseWidget.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <QProcess>
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxServiceTrackerListener.h"
#include "cxStreamerService.h"
#include "cxXmlOptionItem.h"
#include "cxDetailedLabeledComboBoxWidget.h"

class QPushButton;
class QComboBox;
class QLineEdit;
class QStackedWidget;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{
	class FileSelectWidget;
	typedef boost::shared_ptr<class StringDataAdapterXml> StringDataAdapterXmlPtr;
	typedef boost::shared_ptr<class Tool> ToolPtr;
}

namespace cx
{
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
typedef boost::shared_ptr<class ActiveVideoSourceStringDataAdapter> ActiveVideoSourceStringDataAdapterPtr;

class SimulateUSWidget;
class FileInputWidget;

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
class VideoConnectionWidget : public BaseWidget
{
  Q_OBJECT

public:
	VideoConnectionWidget(VisualizationServicePtr visualizationService, PatientModelServicePtr patientModelService, VideoServicePtr videoService, QWidget* parent);
	virtual ~VideoConnectionWidget();
	virtual QString defaultWhatsThis() const;

protected slots:
	void toggleLaunchServer();
	void launchServer();
	void toggleConnectServer();
	void serverProcessStateChanged(QProcess::ProcessState newState);
	void serverStatusChangedSlot();
	void importStreamImageSlot();
	void selectGuiForConnectionMethodSlot();
	void initScriptSelected(QString filename);

protected:
	void connectServer();
	void disconnectServer();
	void initializeScriptWidget();
	ActiveVideoSourceStringDataAdapterPtr initializeActiveVideoSourceSelector();
	QFrame* wrapStackedWidgetInAFrame();
	void updateHostHistory();
	void updateDirectLinkArgumentHistory();
	QProcess* getServerProcess();
	bool serverIsRunning();
	VideoConnectionManagerPtr getVideoConnectionManager();
	void writeSettings();
	QPushButton* initializeConnectButton();
	QPushButton* initializeImportStreamImageButton();
	QStackedWidget* initializeStackedWidget();
	QWidget* createDirectLinkWidget();
	QWidget* createLocalServerWidget();
	QWidget* createRemoteWidget();
	QWidget* wrapVerticalStretch(QWidget* input);
	Transform3D calculate_rMd_ForAProbeImage(ToolPtr probe);
	QString generateFilename(VideoSourcePtr videoSource);
	void saveAndImportSnapshot(vtkImageDataPtr input, QString filename, Transform3D rMd);

	QPushButton* mConnectButton;
	QPushButton* mImportStreamImageButton;
	QVBoxLayout* mToptopLayout;
	FileInputWidget* mInitScriptWidget;
	QComboBox* mAddressEdit;
	QLineEdit* mPortEdit;
	QLineEdit* mLocalServerArguments;
	QPushButton* mLaunchServerButton;
	QComboBox* mDirectLinkArguments;
	QStackedWidget* mStackedWidget;
	StringDataAdapterXmlPtr mConnectionSelector;
	ActiveVideoSourceStringDataAdapterPtr mActiveVideoSourceSelector;
	FileInputWidget* mLocalServerFile;
	XmlOptionFile mOptions;
	DetailedLabeledComboBoxWidget* mConnectionSelectionWidget;
	VisualizationServicePtr mVisualizationService;
	PatientModelServicePtr mPatientModelService;
	VideoServicePtr mVideoService;

private:
	QWidget* createStreamerWidget(StreamerService* service);
	void onServiceAdded(StreamerService *service);
	void onServiceRemoved(StreamerService *service);
	void addServiceToSelector(QString name);
	void removeServiceFromSelector(QString name);
	void removeServiceWidget(QString name);

	boost::shared_ptr<ServiceTrackerListener<StreamerService> > mServiceListener;
	std::map<QString, QWidget*> mStreamerServiceWidgets;
};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
