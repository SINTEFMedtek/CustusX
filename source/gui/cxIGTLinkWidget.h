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
  void renderSlot();
  void serverStatusChangedSlot();

private:
  QLineEdit* mAddressEdit;
  QLineEdit* mPortEdit;
  QPushButton* mConnectButton;
  QPushButton* mLaunchServerButton;
  ssc::OpenIGTLinkRTSourcePtr mRTSource;
  QGridLayout* mGridLayout;
  QVBoxLayout* mToptopLayout;
  ssc::View* mView;
  QTimer* mRenderTimer;
  RenderTimer mRenderTimerW;
  QLabel* mRenderLabel;
};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
