/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLUSCONNECTWIDGET_H
#define CXPLUSCONNECTWIDGET_H

#include <QProcess>
#include "cxTabbedWidget.h"

class QPushButton;
class QDomElement;
class QComboBox;

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class OpenIGTLinkStreamerService> OpenIGTLinkStreamerServicePtr;
typedef boost::shared_ptr<class StreamerService> StreamerServicePtr;
typedef boost::shared_ptr<class ProcessWrapper> ProcessWrapperPtr;
typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;
typedef boost::shared_ptr<class BoolProperty> BoolPropertyPtr;
class FileSelectWidget;
class FilePreviewWidget;

class PlusConnectWidget : public BaseWidget
{
public:
	PlusConnectWidget(VisServicesPtr services, QWidget *parent);

private slots:
	void connectButtonClickedSlot();
	void browsePlusPathSlot();
	void plusAppStateChanged();

//	void browsePlusConfigFileSlot();
	void configFileFileSelected(QString filename);
	void processReadyRead();
private:
	VisServicesPtr mServices;
	QPushButton* mConnectButton;
	bool mPlusRunning;
	ProcessWrapperPtr mExternalProcess;

	QString mPlusPath;
	QString mPlusConfigFile;
	FileSelectWidget* mPlusConfigFileWidget;
	FilePreviewWidget* mFilePreviewWidget;
	QComboBox* mPlusPathComboBox;
	BoolPropertyPtr mShowPlusOutput;

	StreamerServicePtr getStreamerService();
	bool startPlus();
	bool stopPlus();
	QDomElement getXmlVideoElement();
	bool startExternalPlusServer();
	void searchForPlus();
	void searchForPlusConfigFile();
	QStringList getPlusConfigFilePaths();
	bool configFileIsValid();
	void startOpenIGTLink3VideoStreaming();
	void changeOpenIGTLinkStreamerParameter(StreamerServicePtr streamerService, QString parameterName, QVariant value);
};

}//namespace cx
#endif // CXPLUSCONNECTWIDGET_H
