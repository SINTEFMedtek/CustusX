// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXIGTLINKWIDGET_H_
#define CXIGTLINKWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <QProcess>
class QPushButton;
class QComboBox;
class QLineEdit;
class QStackedWidget;

#include <QtTest/QtTest>

namespace ssc
{
	class FileSelectWidget;
	typedef boost::shared_ptr<class StringDataAdapterXml> StringDataAdapterXmlPtr;
}

namespace cx
{
//typedef boost::shared_ptr<class IGTLinkClient> GrabberReceiveThreadIGTLinkPtr;
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
typedef boost::shared_ptr<class ActiveVideoSourceStringDataAdapter> ActiveVideoSourceStringDataAdapterPtr;


/**
 * \brief GUI for setting up a connection to a video stream
 *
 * \ingroup cxGUI
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
  VideoConnectionWidget(QWidget* parent);
  virtual ~VideoConnectionWidget();

  virtual QString defaultWhatsThis() const;

protected slots:
	void toggleLaunchServer();
	void launchServer();
	void toggleConnectServer();
	void connectServer();
	void disconnectServer();
	void serverProcessStateChanged(QProcess::ProcessState newState);
	void serverStatusChangedSlot();
	void browseLocalServerSlot();
	void importStreamImageSlot();
	void dataChanged();
	void initScriptSelected(QString filename);

protected:
	QHBoxLayout* initializeScriptWidget();
	ssc::StringDataAdapterXmlPtr initializeConnectionSelector();
	ActiveVideoSourceStringDataAdapterPtr initializeActiveVideoSourceSelector();
	QFrame* wrapStackedWidgetInAFrame();
	void updateHostHistory();
	void updateDirectLinkArgumentHistory();
	QProcess* getServer();
	bool serverIsRunning();
	VideoConnectionManagerPtr getConnection();
	void writeSettings();
	QPushButton* initializeConnectButton();
	QPushButton* initializeImportStreamImageButton();
	QStackedWidget* initializeStackedWidget();
	QWidget* createDirectLinkWidget();
	QWidget* createLocalServerWidget();
	QWidget* createRemoteWidget();
	QWidget* wrapVerticalStretch(QWidget* input);

	QPushButton* mConnectButton;
	QPushButton* mImportStreamImageButton;
	QVBoxLayout* mToptopLayout;
	ssc::FileSelectWidget* mInitScriptWidget;
	QComboBox* mAddressEdit;
	QLineEdit* mPortEdit;
	QLineEdit* mLocalServerEdit;
	QLineEdit* mLocalServerArguments;
	QPushButton* mLaunchServerButton;
	QComboBox* mDirectLinkArguments;
	QStackedWidget* mStackedWidget;
	ssc::StringDataAdapterXmlPtr mConnectionSelector;
	ActiveVideoSourceStringDataAdapterPtr mActiveVideoSourceSelector;

};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
