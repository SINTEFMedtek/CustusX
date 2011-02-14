#ifndef CXIGTLINKWIDGET_H_
#define CXIGTLINKWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscDoubleWidgets.h"
#include "sscView.h"
#include "RTSource/sscOpenIGTLinkRTSource.h"
#include "cxRenderTimer.h"
namespace cx
{
typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;
typedef boost::shared_ptr<class RTSourceManager> IGTLinkConnectionPtr;


/**
 * \class IGTLinkWidget
 * GUI for setup of a IGTLink connection.
 *
 * \date 2010.10.27
 * \author: Christian Askeland, SINTEF
 */
class IGTLinkWidget : public QWidget
{
  Q_OBJECT

public:
  IGTLinkWidget(QWidget* parent);
  virtual ~IGTLinkWidget();

private slots:
  void toggleLaunchServer();
  void launchServer();
  void showStream();
  void toggleConnectServer();
  void connectServer();
  void renderSlot();
  void serverProcessStateChanged(QProcess::ProcessState newState);
  void serverStatusChangedSlot();
  void browseLocalServerSlot();

  void guiChanged();

private:
  void dataChanged();
  void updateHostHistory();
  QProcess* getServer();
  ssc::OpenIGTLinkRTSourcePtr getRTSource();
  IGTLinkConnectionPtr getConnection();
  void writeSettings();

  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event); ///<disconnects stuff
  QComboBox* mAddressEdit;
  QLineEdit* mPortEdit;
  QCheckBox* mUseLocalServer;
  QPushButton* mConnectButton;
  QPushButton* mShowStreamButton;
  QPushButton* mLaunchServerButton;
  QGridLayout* mGridLayout;
  QVBoxLayout* mToptopLayout;

  ssc::View* mView;
  QTimer* mRenderTimer;
  RenderTimer mRenderTimerW;
  QLabel* mRenderLabel;

  QLineEdit* mLocalServerEdit;
};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
