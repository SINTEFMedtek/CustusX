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

namespace ssc
{
	class FileSelectWidget;
}

namespace cx
{
typedef boost::shared_ptr<class IGTLinkClient> GrabberReceiveThreadIGTLinkPtr;
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;

/**
 * \class IGTLinkWidget
 * \brief GUI for setup of a IGTLink connection.
 * \ingroup cxGUI
 *
 * \date 2010.10.27
 * \\author Christian Askeland, SINTEF
 */
class VideoConnectionWidget : public BaseWidget
{
  Q_OBJECT

public:
  VideoConnectionWidget(QWidget* parent);
  virtual ~VideoConnectionWidget();

  virtual QString defaultWhatsThis() const;

private slots:
  void toggleLaunchServer();
  void launchServer();
  void toggleConnectServer();
  void connectServer();
  void serverProcessStateChanged(QProcess::ProcessState newState);
  void serverStatusChangedSlot();
  void browseLocalServerSlot();
  void saveSnapshotSlot(); ///<Save snapshot of RT image/volume

  void dataChanged();
  void initScriptSelected(QString filename);

private:
  void updateHostHistory();
  void updateDirectLinkArgumentHistory();
  QProcess* getServer();
  VideoConnectionManagerPtr getConnection();
  void writeSettings();

  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event); ///<disconnects stuff

  QPushButton* mConnectButton;
  QPushButton* mSnapshotButton;
  QVBoxLayout* mToptopLayout;
  ssc::FileSelectWidget* mInitScriptWidget;
  // remote server widgets:
  QComboBox* mAddressEdit;
  QLineEdit* mPortEdit;
  // local server widgets:
  QLineEdit* mLocalServerEdit;
  QLineEdit* mLocalServerArguments;
  QPushButton* mLaunchServerButton;
  // direct link widgets:
  QComboBox* mDirectLinkArguments;

  QStackedWidget* mStackedWidget;
  QWidget* createDirectLinkWidget();
  QWidget* createLocalServerWidget();
  QWidget* createRemoteWidget();
  QWidget* wrapVerticalStretch(QWidget* input);

};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
