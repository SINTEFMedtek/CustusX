#ifndef UR5MANUALMOVE_H
#define UR5MANUALMOVE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5ManualMoveTab : public QWidget
{
    Q_OBJECT
public:
    Ur5ManualMoveTab(Ur5RobotPtr Ur5Robot,QWidget *parent = 0);
    virtual ~Ur5ManualMoveTab();

    QPushButton *negZButton, *posZButton, *posXButton, *negYButton, *posYButton, *negXButton;
    QPushButton *rotNegZButton, *rotPosZButton, *rotPosXButton, *rotNegYButton, *rotPosYButton, *rotNegXButton;

    QLineEdit *xPosLineEdit, *yPosLineEdit, *zPosLineEdit;
    QLineEdit *rxLineEdit, *ryLineEdit, *rzLineEdit;

    QLineEdit *jxPosLineEdit, *jyPosLineEdit, *jzPosLineEdit;
    QLineEdit *jrxLineEdit, *jryLineEdit, *jrzLineEdit;

    QLineEdit *accelerationLineEdit, *velocityLineEdit, *timeLineEdit;

    void coordButtonPressed(int axis,int sign);
    void rotButtonPressed(int axis,int sign);


public slots:
    void moveButtonReleasedSlot();
    void posZButtonPressedSlot();
    void negZButtonPressedSlot();
    void posYButtonPressedSlot();
    void negYButtonPressedSlot();
    void posXButtonPressedSlot();
    void negXButtonPressedSlot();

    void posRXButtonPressedSlot();
    void negRXButtonPressedSlot();
    void posRYButtonPressedSlot();
    void negRYButtonPressedSlot();
    void posRZButtonPressedSlot();
    void negRZButtonPressedSlot();


    void updatePositionSlot();


private:
    QHBoxLayout *mainLayout;
    void setupUi(QWidget *parent);
    Ur5RobotPtr mUr5Robot;
    void connectMovementButtons();

    void setMoveToolLayout(QVBoxLayout *vLayout);
    void setMoveSettingsWidget(QVBoxLayout *vLayout);
    void setCoordInfoWidget(QVBoxLayout *vLayout);
    void setJointMoveWidget(QVBoxLayout *vLayout);
};

} // cx

#endif // UR5MANUALMOVE_H
