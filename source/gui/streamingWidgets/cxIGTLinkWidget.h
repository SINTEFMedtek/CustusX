#ifndef CXIGTLINKWIDGET_H_
#define CXIGTLINKWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtGui>
#include "sscDoubleWidgets.h"
#include "sscView.h"
#include "cxOpenIGTLinkRTSource.h"
#include "cxRenderTimer.h"
namespace cx
{
typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;
typedef boost::shared_ptr<class VideoConnection> VideoConnectionPtr;


/**
 * \class IGTLinkWidget
 * \brief GUI for setup of a IGTLink connection.
 * \ingroup cxGUI
 *
 * \date 2010.10.27
 * \\author Christian Askeland, SINTEF
 */
class IGTLinkWidget : public BaseWidget
{
  Q_OBJECT

public:
  IGTLinkWidget(QWidget* parent);
  virtual ~IGTLinkWidget();

  virtual QString defaultWhatsThis() const;

private slots:
  void toggleLaunchServer();
  void launchServer();
  void toggleConnectServer();
  void connectServer();
  void serverProcessStateChanged(QProcess::ProcessState newState);
  void serverStatusChangedSlot();
  void browseLocalServerSlot();

//  void useLocalServerChanged();
//  void useDirectLinkChanged();
  void dataChanged();

private:
  void updateHostHistory();
  QProcess* getServer();
  OpenIGTLinkRTSourcePtr getRTSource();
  VideoConnectionPtr getConnection();
  void writeSettings();

  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event); ///<disconnects stuff
  QComboBox* mAddressEdit;
  QLineEdit* mPortEdit;
//  QCheckBox* mUseLocalServer;
//  QCheckBox* mUseDirectLink;
  QPushButton* mConnectButton;
  QPushButton* mLaunchServerButton;
//  QGridLayout* mGridLayout;
  QVBoxLayout* mToptopLayout;

  QLineEdit* mDirectLinkArguments;
  QLineEdit* mLocalServerArguments;

  QLineEdit* mLocalServerEdit;

  QStackedWidget* mStackedWidget;
  QWidget* createDirectLinkWidget();
  QWidget* createLocalServerWidget();
  QWidget* createRemoteWidget();
  QWidget* wrapVerticalStretch(QWidget* input);

};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
