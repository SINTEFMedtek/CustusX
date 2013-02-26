#ifndef CXIGTLINKWIDGET_H_
#define CXIGTLINKWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtGui>
#include "sscDoubleWidgets.h"
#include "sscView.h"
#include "sscFileSelectWidget.h"


namespace cx
{
typedef boost::shared_ptr<class IGTLinkClient> GrabberReceiveThreadIGTLinkPtr;
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
typedef boost::shared_ptr<class GrabberVideoSource> GrabberVideoSourcePtr;


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
  void saveSnapshotSlot(); ///<Save snapshot of RT image/volume

//  void useLocalServerChanged();
//  void useDirectLinkChanged();
  void dataChanged();
  void initScriptSelected(QString filename);

private:
  void updateHostHistory();
  void updateDirectLinkArgumentHistory();
  QProcess* getServer();
  GrabberVideoSourcePtr getRTSource();
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
