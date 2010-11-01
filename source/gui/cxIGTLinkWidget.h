#ifndef CXIGTLINKWIDGET_H_
#define CXIGTLINKWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscDoubleWidgets.h"
#include "sscView.h"
#include "RTSource/sscOpenIGTLinkRTSource.h"

namespace cx
{
typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;

/**
 * \class IGTLinkWidget
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
  void launchServer();
  void toggleConnect();
  void clientFinishedSlot();
  void renderSlot();
  void imageReceivedSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  void connectServer();
  void disconnectServer();

  QLineEdit* mAddressEdit;
  QLineEdit* mPortEdit;
  QPushButton* mConnectButton;
  QPushButton* mLaunchServerButton;
  IGTLinkClientPtr mClient;
  ssc::OpenIGTLinkRTSourcePtr mRTSource;

  ssc::View* mView;
  QTimer* mRenderTimer;
};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
