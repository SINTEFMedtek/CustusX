#ifndef UR5MANUALMOVE_H
#define UR5MANUALMOVE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollBar>
#include <QButtonGroup>

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5ManualMoveTab : public QWidget
{
    Q_OBJECT
public:
    Ur5ManualMoveTab(Ur5RobotPtr Ur5Robot,QWidget *parent = 0);
    virtual ~Ur5ManualMoveTab();

private slots:
    void moveButtonReleased();
    void jointButtonReleased();

    void posZButtonPressed();
    void negZButtonPressed();
    void posYButtonPressed();
    void negYButtonPressed();
    void posXButtonPressed();
    void negXButtonPressed();
    void posRXButtonPressed();
    void negRXButtonPressed();
    void posRYButtonPressed();
    void negRYButtonPressed();
    void posRZButtonPressed();
    void negRZButtonPressed();

    void q1PosButtonPressed();
    void q2PosButtonPressed();
    void q3PosButtonPressed();
    void q4PosButtonPressed();
    void q5PosButtonPressed();
    void q6PosButtonPressed();
    void q1NegButtonPressed();
    void q2NegButtonPressed();
    void q3NegButtonPressed();
    void q4NegButtonPressed();
    void q5NegButtonPressed();
    void q6NegButtonPressed();

    void updatePositions();

private:
    QHBoxLayout *mainLayout;
    void setupUi(QWidget *parent);
    Ur5RobotPtr mUr5Robot;

    void connectMovementButtons();
    void connectJointButtons();

    void setMoveToolLayout(QVBoxLayout *vLayout);
    void setMoveSettingsWidget(QVBoxLayout *vLayout);
    void setCoordInfoWidget(QVBoxLayout *vLayout);
    void setJointMoveWidget(QVBoxLayout *vLayout);

    QPushButton *negZButton, *posZButton, *posXButton, *negYButton, *posYButton, *negXButton;
    QPushButton *rotNegZButton, *rotPosZButton, *rotPosXButton, *rotNegYButton, *rotPosYButton, *rotNegXButton;

    QLineEdit *xPosLineEdit, *yPosLineEdit, *zPosLineEdit;
    QLineEdit *rxLineEdit, *ryLineEdit, *rzLineEdit;

    QLineEdit *q1LineEdit, *q2LineEdit, *q3LineEdit;
    QLineEdit *q4LineEdit, *q5LineEdit, *q6LineEdit;

    QPushButton *q1PosButton, *q1NegButton;
    QPushButton *q2PosButton, *q2NegButton;
    QPushButton *q3PosButton, *q3NegButton;
    QPushButton *q4PosButton, *q4NegButton;
    QPushButton *q5PosButton, *q5NegButton;
    QPushButton *q6PosButton, *q6NegButton;

    QButtonGroup *linearMotionButtons, *rotationMotionButtons, *jointConfigurationButtons;

    QLineEdit *accelerationLineEdit, *velocityLineEdit, *timeLineEdit;

    void coordButtonPressed(int axis,int sign);
    void rotButtonPressed(int axis,int sign);
    void jointButtonPressed(int joint,int sign);
    
    void setAutoRepeat(bool isRepeated, QButtonGroup *buttons);
    void setAutoRepeatInterval(int intervalms, QButtonGroup *buttons);
    void setMaximumWidth(int width, QButtonGroup *buttons);

};

} // cx

#endif // UR5MANUALMOVE_H
