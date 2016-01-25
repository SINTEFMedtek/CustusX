#include "cxUr5ManualMove.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QGroupBox>

namespace cx
{

Ur5ManualMoveTab::Ur5ManualMoveTab(Ur5RobotPtr Ur5Robot,QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
{
    setupUi(this);

    connectMovementButtons();
    connectJointButtons();

    connect(mUr5Robot.get(),&Ur5Robot::stateUpdated,this,&Ur5ManualMoveTab::updatePositions);
}

Ur5ManualMoveTab::~Ur5ManualMoveTab()
{
}

void Ur5ManualMoveTab::setupUi(QWidget *parent)
{
   mainLayout = new QHBoxLayout(this);

   QWidget *leftColumnWidgets = new QWidget();
   QVBoxLayout *leftColumnLayout = new QVBoxLayout(leftColumnWidgets);

   QWidget *rightColumnWidgets = new QWidget();
   QVBoxLayout *rightColumnLayout = new QVBoxLayout(rightColumnWidgets);

   setMoveToolLayout(leftColumnLayout);
   setCoordInfoWidget(rightColumnLayout);
   setMoveSettingsWidget(leftColumnLayout);
   setJointMoveWidget(rightColumnLayout);

   mainLayout->addWidget(leftColumnWidgets,0,Qt::AlignTop|Qt::AlignLeft);
   mainLayout->addWidget(rightColumnWidgets,0,Qt::AlignTop|Qt::AlignRight);

   mainLayout->setSpacing(5);
   mainLayout->setMargin(5);
}

void Ur5ManualMoveTab::setMoveToolLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move tool");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *keyLayout = new QGridLayout();
    group->setLayout(keyLayout);
    keyLayout->setSpacing(0);
    keyLayout->setMargin(0);
    keyLayout->setContentsMargins(0,0,0,0);

    posZButton = new QPushButton(tr("+z")); //QIcon("/icons/arrow-up-double.png"),"");
    negZButton = new QPushButton(tr("-z")); //QIcon("/icons/arrow-down-double.png"),"");
    posXButton = new QPushButton("+x"); //QIcon("/icons/arrow-up.png"),"");
    negXButton = new QPushButton("-x"); //QIcon("/icons/arrow-down.png"),"");
    posYButton = new QPushButton("+y"); //QIcon("/icons/arrow-left.png"),"");
    negYButton = new QPushButton(tr("-y")); // QIcon("/icons/arrow-right.png"),"");

    posZButton->setAutoRepeat(true);
    posZButton->setToolTip("Move in positive Z direction");
    posZButton->setMaximumWidth(32);

    negZButton->setAutoRepeat(true);
    negZButton->setToolTip("Move in negative Z direction");
    negZButton->setMaximumWidth(32);

    posXButton->setAutoRepeat(true);
    posXButton->setToolTip("Move in positive Y direction");
    posXButton->setMaximumWidth(32);

    negXButton->setAutoRepeat(true);
    negXButton->setToolTip("Move in negative Y direction");
    negXButton->setMaximumWidth(32);

    posYButton->setAutoRepeat(true);
    posYButton->setToolTip("Move in positive X direction");
    posYButton->setMaximumWidth(32);

    negYButton->setAutoRepeat(true);
    negYButton->setToolTip("Move in negative X direction");
    negYButton->setMaximumWidth(32);

    rotPosXButton = new QPushButton(tr("+rx"));
    rotNegXButton = new QPushButton(tr("-rx"));
    rotPosYButton = new QPushButton(tr("+ry"));
    rotNegYButton = new QPushButton(tr("-ry"));
    rotPosZButton = new QPushButton(tr("+rz"));
    rotNegZButton = new QPushButton(tr("-rz"));

    rotPosXButton->setAutoRepeat(true);
    rotPosXButton->setToolTip("Rotate counter-clockwise around X axis");
    rotPosXButton->setMaximumWidth(32);

    rotNegXButton->setAutoRepeat(true);
    rotNegXButton->setToolTip("Rotate clockwise around X axis");
    rotNegXButton->setMaximumWidth(32);

    rotPosYButton->setAutoRepeat(true);
    rotPosYButton->setToolTip("Rotate counter-clockwise around Y axis");
    rotPosYButton->setMaximumWidth(32);

    rotNegYButton->setAutoRepeat(true);
    rotNegYButton->setToolTip("Rotate clockwise around Y axis");
    rotNegYButton->setMaximumWidth(32);

    rotPosZButton->setAutoRepeat(true);
    rotPosZButton->setToolTip("Rotate counter-clockwise around Z axis");
    rotPosZButton->setMaximumWidth(32);

    rotNegZButton->setAutoRepeat(true);
    rotNegZButton->setToolTip("Rotate clockwise around Z axis");
    rotNegZButton->setMaximumWidth(32);

    int krow=0;
    keyLayout->addWidget(posZButton, krow, 0, 1, 1,Qt::AlignBottom);
    keyLayout->addWidget(negZButton, krow,2,1,1,Qt::AlignBottom);

    krow++;
    keyLayout->addWidget(posXButton,krow,1,1,1,Qt::AlignBottom);

    krow++;
    keyLayout->addWidget(posYButton,krow,0,1,1,Qt::AlignRight);
    keyLayout->addWidget(negYButton,krow,2,1,1,Qt::AlignLeft);

    krow++;
    keyLayout->addWidget(negXButton,krow,1,1,1,Qt::AlignTop);

    krow++;
    keyLayout->addWidget(rotNegXButton, krow, 0, 1, 1,Qt::AlignBottom);
    keyLayout->addWidget(rotPosXButton, krow,2,1,1,Qt::AlignBottom);

    krow++;
    keyLayout->addWidget(rotPosYButton,krow,1,1,1,Qt::AlignBottom);

    krow++;
    keyLayout->addWidget(rotNegZButton,krow,0,1,1,Qt::AlignRight);
    keyLayout->addWidget(rotPosZButton,krow,2,1,1,Qt::AlignLeft);

    krow++;
    keyLayout->addWidget(rotNegYButton,krow,1,1,1,Qt::AlignTop);

    posZButton->setToolTip("Move in positive Z direction");
    negZButton->setToolTip("Move in negative Z direction");

    // Style sheet for buttons
//    posXButton->setStyleSheet("border:none");
//    negXButton->setStyleSheet("border:none");
//    posYButton->setStyleSheet("border:none");
//    negYButton->setStyleSheet("border:none");
//    posZButton->setStyleSheet("border:none");
//    negZButton->setStyleSheet("border:none");

    //rotPosZButton->setIconSize(QSize(32, 32));
    rotPosZButton->setToolTip("Move in rotPositive Z direction");

    //rotNegZButton->setIconSize(QSize(32, 32));
    rotNegZButton->setToolTip("Move in rotNegative Z direction");

//    rotPosXButton->setIconSize(QSize(32, 32));
//    rotNegYButton->setIconSize(QSize(32, 32));
//    rotPosYButton->setIconSize(QSize(32, 32));
//    rotNegXButton->setIconSize(QSize(32, 32));

    // Style sheet for buttons
//    rotPosXButton->setStyleSheet("border:none");
//    rotNegXButton->setStyleSheet("border:none");
//    rotPosYButton->setStyleSheet("border:none");
//    rotNegYButton->setStyleSheet("border:none");
//    rotPosZButton->setStyleSheet("border:none");
//    rotNegZButton->setStyleSheet("border:none");
}

void Ur5ManualMoveTab::setMoveSettingsWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move settings");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *velAccLayout = new QGridLayout();
    group->setLayout(velAccLayout);

    velAccLayout->setSpacing(5);
    velAccLayout->setMargin(5);
    //velAccLayout->setContentsMargins(0,0,0,0);

    // Velocity
    velAccLayout->addWidget(new QLabel("Vel"), 0, 0, 1, 1);
    velocityLineEdit = new QLineEdit();
    velAccLayout->addWidget(velocityLineEdit, 0, 1, 1, 1);
    velocityLineEdit->setText(QApplication::translate("Ur5Widget", "0.1", 0));
    velAccLayout->addWidget(new QLabel("m/s"), 0, 2, 1, 1);

    // Acceleration
    accelerationLineEdit = new QLineEdit();
    velAccLayout->addWidget(accelerationLineEdit, 1, 1, 1, 1);
    accelerationLineEdit->setText(QApplication::translate("Ur5Widget", "0.5", 0));
    velAccLayout->addWidget(new QLabel("Acc"), 1, 0, 1, 1);
    velAccLayout->addWidget(new QLabel("m/s^2"), 1, 2, 1, 1);

    // Time
    velAccLayout->addWidget(new QLabel("Time"), 2, 0, 1, 1);
    timeLineEdit = new QLineEdit();
    velAccLayout->addWidget(timeLineEdit, 2, 1, 1, 1);
    timeLineEdit->setText(QApplication::translate("Ur5Widget", "0.5", 0));
    velAccLayout->addWidget(new QLabel("s"), 2, 2, 1, 1);
}

void Ur5ManualMoveTab::setCoordInfoWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Tool position");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *coordInfoLayout = new QGridLayout();
    group->setLayout(coordInfoLayout);

    coordInfoLayout->setSpacing(5);
    coordInfoLayout->setMargin(5);

    // Position label
    coordInfoLayout->addWidget(new QLabel("X"), 0, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Y"), 1, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Z"), 3, 0, 1, 1, Qt::AlignHCenter);

    // mm label
    coordInfoLayout->addWidget(new QLabel("mm"), 0, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("mm"), 1, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("mm"), 3, 3, 1, 1);

    // Ri orientation label
    coordInfoLayout->addWidget(new QLabel("RX"), 5, 0, 1, 1);
    coordInfoLayout->addWidget(new QLabel("RZ"), 7, 0, 1, 1);
    coordInfoLayout->addWidget(new QLabel("RY"), 6, 0, 1, 1);

    // Rad label
    coordInfoLayout->addWidget(new QLabel("Rad"), 5, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 6, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 7, 3, 1, 1);

    // X coordinate line edit
    xPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(xPosLineEdit, 0, 2, 1, 1);

    // Y coordinate line edit
    yPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(yPosLineEdit, 1, 2, 1, 1);

    // Line edit for Z position
    zPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(zPosLineEdit, 3, 2, 1, 1);

    // Line edit for RX orientation
    rxLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(rxLineEdit, 5, 2, 1, 1);

    // Line edit for RY orientation
    ryLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(ryLineEdit, 6, 2, 1, 1);

    // Line edit for RZ orientation
    rzLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(rzLineEdit, 7, 2, 1, 1);
}

void Ur5ManualMoveTab::setJointMoveWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move joints");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *coordInfoLayout = new QGridLayout();
    group->setLayout(coordInfoLayout);

    coordInfoLayout->setSpacing(5);
    coordInfoLayout->setMargin(5);

    q1PosButton = new QPushButton(tr("+"));
    q1NegButton = new QPushButton(tr("-"));
    q2PosButton = new QPushButton(tr("+"));
    q2NegButton = new QPushButton(tr("-"));
    q3PosButton = new QPushButton(tr("+"));
    q3NegButton = new QPushButton(tr("-"));
    q4PosButton = new QPushButton(tr("+"));
    q4NegButton = new QPushButton(tr("-"));
    q5PosButton = new QPushButton(tr("+"));
    q5NegButton = new QPushButton(tr("-"));
    q6PosButton = new QPushButton(tr("+"));
    q6NegButton = new QPushButton(tr("-"));

    q1PosButton->setAutoRepeat(true);
    q1PosButton->setToolTip("Move in positive Z direction");
    q1PosButton->setMaximumWidth(32);

    q2PosButton->setAutoRepeat(true);
    q2PosButton->setToolTip("Move in negative Z direction");
    q2PosButton->setMaximumWidth(32);

    q3PosButton->setAutoRepeat(true);
    q3PosButton->setToolTip("Move in positive Y direction");
    q3PosButton->setMaximumWidth(32);

    q4PosButton->setAutoRepeat(true);
    q4PosButton->setToolTip("Move in negative Y direction");
    q4PosButton->setMaximumWidth(32);

    q5PosButton->setAutoRepeat(true);
    q5PosButton->setToolTip("Move in positive X direction");
    q5PosButton->setMaximumWidth(32);

    q6PosButton->setAutoRepeat(true);
    q6PosButton->setToolTip("Move in negative X direction");
    q6PosButton->setMaximumWidth(32);

    q1NegButton->setAutoRepeat(true);
    q1NegButton->setToolTip("Move in positive Z direction");
    q1NegButton->setMaximumWidth(32);

    q2NegButton->setAutoRepeat(true);
    q2NegButton->setToolTip("Move in negative Z direction");
    q2NegButton->setMaximumWidth(32);

    q3NegButton->setAutoRepeat(true);
    q3NegButton->setToolTip("Move in positive Y direction");
    q3NegButton->setMaximumWidth(32);

    q4NegButton->setAutoRepeat(true);
    q4NegButton->setToolTip("Move in negative Y direction");
    q4NegButton->setMaximumWidth(32);

    q5NegButton->setAutoRepeat(true);
    q5NegButton->setToolTip("Move in positive X direction");
    q5NegButton->setMaximumWidth(32);

    q6NegButton->setAutoRepeat(true);
    q6NegButton->setToolTip("Move in negative X direction");
    q6NegButton->setMaximumWidth(32);

    q1LineEdit = new QLineEdit();
    q2LineEdit = new QLineEdit();
    q3LineEdit = new QLineEdit();
    q4LineEdit = new QLineEdit();
    q5LineEdit = new QLineEdit();
    q6LineEdit = new QLineEdit();

    int row = 0;
    coordInfoLayout->addWidget(new QLabel("Base"), row, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(q1NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q1PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q1LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Shoulder"), row, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(q2NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q2PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q2LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Elbow"), row, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(q3NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q3PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q3LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Wrist 1"), row, 0, 1, 1);
    coordInfoLayout->addWidget(q4NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q4PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q4LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Wrist 2"), row, 0, 1, 1);
    coordInfoLayout->addWidget(q5NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q5PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q5LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Wrist 3"), row, 0, 1, 1);
    coordInfoLayout->addWidget(q6NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q6PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q6LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);
}

void Ur5ManualMoveTab::coordButtonPressed(int axis, int sign)
{
    Ur5State velocity;
    velocity.jointVelocity(axis)=(sign)*velocityLineEdit->text().toDouble();
    mUr5Robot->move("speedl",velocity,accelerationLineEdit->text().toDouble(),0,timeLineEdit->text().toDouble(),0);
}

void Ur5ManualMoveTab::jointButtonPressed(int joint,int sign)
{
    Ur5State velocity;
    velocity.jointVelocity(joint)=(sign)*velocityLineEdit->text().toDouble();
    mUr5Robot->move("speedj",velocity,accelerationLineEdit->text().toDouble(),0,timeLineEdit->text().toDouble(),0);
}

void Ur5ManualMoveTab::rotButtonPressed(int angle, int sign)
{
    Ur5State velocity;
    velocity.jointVelocity(angle+3)=(sign)*velocityLineEdit->text().toDouble();
    mUr5Robot->move("speedl",velocity,accelerationLineEdit->text().toDouble(),0,0,timeLineEdit->text().toDouble());
}

void Ur5ManualMoveTab::posZButtonPressed()
{
    coordButtonPressed(2,1);
}

void Ur5ManualMoveTab::negZButtonPressed()
{
    coordButtonPressed(2,-1);
}

void Ur5ManualMoveTab::posYButtonPressed()
{
    coordButtonPressed(1,1);
}

void Ur5ManualMoveTab::negYButtonPressed()
{
    coordButtonPressed(1,-1);
}

void Ur5ManualMoveTab::posXButtonPressed()
{
   coordButtonPressed(0,1);
}

void Ur5ManualMoveTab::negXButtonPressed()
{
    coordButtonPressed(0,-1);
}

void Ur5ManualMoveTab::posRXButtonPressed()
{
    rotButtonPressed(0,1);
}

void Ur5ManualMoveTab::negRXButtonPressed()
{
    rotButtonPressed(0,-1);
}

void Ur5ManualMoveTab::posRYButtonPressed()
{
    rotButtonPressed(1,1);
}

void Ur5ManualMoveTab::negRYButtonPressed()
{
    rotButtonPressed(1,-1);
}

void Ur5ManualMoveTab::posRZButtonPressed()
{
    rotButtonPressed(2,1);
}

void Ur5ManualMoveTab::negRZButtonPressed()
{
    rotButtonPressed(2,-1);
}

void Ur5ManualMoveTab::moveButtonReleased()
{
    mUr5Robot->stopMove("stopl",accelerationLineEdit->text().toDouble());
}

void Ur5ManualMoveTab::jointButtonReleased()
{
    mUr5Robot->stopMove("stopj",accelerationLineEdit->text().toDouble());
}

void Ur5ManualMoveTab::updatePositions()
{
    Ur5State currentState;
    currentState=mUr5Robot->getCurrentState();

    xPosLineEdit->setText(QString::number(1000*(currentState.cartAxis(0)),'f',2));
    yPosLineEdit->setText(QString::number(1000*(currentState.cartAxis(1)),'f',2));
    zPosLineEdit->setText(QString::number(1000*(currentState.cartAxis(2)),'f',2));
    rxLineEdit->setText(QString::number(currentState.cartAngles(0),'f',4));
    ryLineEdit->setText(QString::number(currentState.cartAngles(1),'f',4));
    rzLineEdit->setText(QString::number(currentState.cartAngles(2),'f',4));
    q1LineEdit->setText(QString::number(currentState.jointConfiguration(0),'f',4));
    q2LineEdit->setText(QString::number(currentState.jointConfiguration(1),'f',4));
    q3LineEdit->setText(QString::number(currentState.jointConfiguration(2),'f',4));
    q4LineEdit->setText(QString::number(currentState.jointConfiguration(3),'f',4));
    q5LineEdit->setText(QString::number(currentState.jointConfiguration(4),'f',4));
    q6LineEdit->setText(QString::number(currentState.jointConfiguration(5),'f',4));
}

void Ur5ManualMoveTab::connectMovementButtons()
{
    connect(posXButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::posXButtonPressed);
    connect(posXButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(negXButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::negXButtonPressed);
    connect(negXButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(posYButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::posYButtonPressed);
    connect(posYButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(negYButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::negYButtonPressed);
    connect(negYButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(posZButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::posZButtonPressed);
    connect(posZButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(negZButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::negZButtonPressed);
    connect(negZButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(rotPosXButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::posRXButtonPressed);
    connect(rotPosXButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(rotNegXButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::negRXButtonPressed);
    connect(rotNegXButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(rotPosYButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::posRYButtonPressed);
    connect(rotPosYButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(rotNegYButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::negRYButtonPressed);
    connect(rotNegYButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(rotPosZButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::posRZButtonPressed);
    connect(rotPosZButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);

    connect(rotNegZButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::negRZButtonPressed);
    connect(rotNegZButton,&QPushButton::released,this,&Ur5ManualMoveTab::moveButtonReleased);
}

void Ur5ManualMoveTab::connectJointButtons()
{
    connect(q1PosButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q1PosButtonPressed);
    connect(q2PosButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q2PosButtonPressed);
    connect(q3PosButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q3PosButtonPressed);
    connect(q4PosButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q4PosButtonPressed);
    connect(q5PosButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q5PosButtonPressed);
    connect(q6PosButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q6PosButtonPressed);
    connect(q1NegButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q1NegButtonPressed);
    connect(q2NegButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q2NegButtonPressed);
    connect(q3NegButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q3NegButtonPressed);
    connect(q4NegButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q4NegButtonPressed);
    connect(q5NegButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q5NegButtonPressed);
    connect(q6NegButton,&QPushButton::pressed,this,&Ur5ManualMoveTab::q6NegButtonPressed);

    connect(q1PosButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q2PosButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q3PosButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q4PosButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q5PosButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q6PosButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q1NegButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q2NegButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q3NegButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q4NegButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q5NegButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
    connect(q6NegButton,&QPushButton::released,this,&Ur5ManualMoveTab::jointButtonReleased);
}

void Ur5ManualMoveTab::q1PosButtonPressed()
{
    jointButtonPressed(0,1);
}

void Ur5ManualMoveTab::q2PosButtonPressed()
{
    jointButtonPressed(1,1);
}

void Ur5ManualMoveTab::q3PosButtonPressed()
{
    jointButtonPressed(2,1);
}

void Ur5ManualMoveTab::q4PosButtonPressed()
{
    jointButtonPressed(3,1);
}

void Ur5ManualMoveTab::q5PosButtonPressed()
{
    jointButtonPressed(4,1);
}

void Ur5ManualMoveTab::q6PosButtonPressed()
{
    jointButtonPressed(5,1);
}

void Ur5ManualMoveTab::q1NegButtonPressed()
{
    jointButtonPressed(0,-1);
}

void Ur5ManualMoveTab::q2NegButtonPressed()
{
    jointButtonPressed(1,-1);
}

void Ur5ManualMoveTab::q3NegButtonPressed()
{
    jointButtonPressed(2,-1);
}

void Ur5ManualMoveTab::q4NegButtonPressed()
{
    jointButtonPressed(3,-1);
}

void Ur5ManualMoveTab::q5NegButtonPressed()
{
    jointButtonPressed(4,-1);
}

void Ur5ManualMoveTab::q6NegButtonPressed()
{
    jointButtonPressed(5,-1);
}


} // cx
