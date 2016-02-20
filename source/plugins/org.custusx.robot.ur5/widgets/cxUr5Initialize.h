#ifndef UR5INITIALIZE_H
#define UR5INITIALIZE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QCheckBox>

#include "cxUr5Robot.h"

#include "org_custusx_robot_ur5_Export.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5InitializeTab : public QWidget
{
    Q_OBJECT
public:
    Ur5InitializeTab(Ur5RobotPtr Ur5Robot,QWidget *parent = 0);
    virtual ~Ur5InitializeTab();

signals:
    void addApplicationTab(QString typeOfTab);
    void removeApplicationTab(QString typeOfTab);

private slots:
    void connectButtonSlot();
    void disconnectButtonSlot();
    void shutdownButtonSlot();

    void startTrackingSlot();
    void stopTrackingSlot();
    void addRobotLinkSlot();
    void removeRobotLinkSlot();

    void trackingSlot();
    void robotLinkSlot();

    void addCheckedApplicationSlot();

private:
    void setupUi(QWidget *parent);
    void setupConnections(QWidget *parent);

    Ur5RobotPtr mUr5Robot;

    QLineEdit *ipLineEdit, *manualCoordinatesLineEdit;
    QPushButton *connectButton, *disconnectButton,*shutdownButton;
    QComboBox *presetOrigoComboBox;
    QPushButton *startTrackingButton, *stopTrackingButton;
    QPushButton *addLinksButton, *removeLinksButton;

    QPushButton *trackingButton, *linksButton;

    void setRobotConnectionLayout(QVBoxLayout *parent);
    void setRobotTrackingLayout(QHBoxLayout *parent);
    void setRobotApplicationLayout(QHBoxLayout *parent);

    QPushButton *toggleManual, *togglePlanned, *toggleUr5Script;
    QPushButton *toggleLungSimulation, *toggleSettings, *toggleUSTracker;

    QString mGraphicsFolderName;

    QString onoffButtonStyleSheet();
};

} // cx

#endif // UR5INITIALIZE_H
