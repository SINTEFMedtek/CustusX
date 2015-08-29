#ifndef UR5INITIALIZE_H
#define UR5INITIALIZE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

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

protected slots:
    void connectButtonSlot();
    void initializeButtonSlot();
    void disconnectButtonSlot();
    void shutdownButtonSlot();

private:
    void setupUi(QWidget *parent);
    Ur5RobotPtr mUr5Robot;

    QLineEdit *ipLineEdit, *manualCoordinatesLineEdit;
    QPushButton *connectButton, *disconnectButton, *initializeButton, *initializeButton_2, *shutdownButton;
    QComboBox *presetOrigoComboBox;

};

} // cx

#endif // UR5INITIALIZE_H
