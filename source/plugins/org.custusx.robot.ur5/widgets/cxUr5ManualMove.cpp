#include "cxUr5ManualMove.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>

namespace cx
{

Ur5ManualMoveTab::Ur5ManualMoveTab(Ur5RobotPtr Ur5Robot,QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
{
    setupUi(this);

    connectMovementButtons();

    connect(mUr5Robot.get(),&Ur5Robot::stateUpdated,this,&Ur5ManualMoveTab::updatePositionSlot);
}

Ur5ManualMoveTab::~Ur5ManualMoveTab()
{
}

void Ur5ManualMoveTab::setupUi(QWidget *parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);

    QWidget *keyWidget = new QWidget();
    QGridLayout *keyLayout = new QGridLayout(keyWidget);
    keyLayout->setSpacing(0);
    keyLayout->setMargin(0);

    QWidget *rotKeyWidget = new QWidget();
    QGridLayout *rotKeyLayout = new QGridLayout(rotKeyWidget);
    rotKeyLayout->setSpacing(0);
    rotKeyLayout->setMargin(0);

    QWidget *velAccWidget = new QWidget();
    QGridLayout *velAccLayout = new QGridLayout(velAccWidget);

    QWidget *coordInfoWidget = new QWidget();
    QGridLayout *coordInfoLayout = new QGridLayout(coordInfoWidget);

    posZButton = new QPushButton();
    negZButton = new QPushButton();
    posYButton = new QPushButton();
    negYButton = new QPushButton();
    posXButton = new QPushButton();
    negXButton = new QPushButton();

    posZButton->setAutoRepeat(true);
    negZButton->setAutoRepeat(true);
    posXButton->setAutoRepeat(true);
    negXButton->setAutoRepeat(true);
    posYButton->setAutoRepeat(true);
    negYButton->setAutoRepeat(true);


    rotPosZButton = new QPushButton();
    rotNegZButton = new QPushButton();
    rotPosYButton = new QPushButton();
    rotNegYButton = new QPushButton();
    rotPosXButton = new QPushButton();
    rotNegXButton = new QPushButton();

    rotPosXButton->setAutoRepeat(true);
    rotNegXButton->setAutoRepeat(true);
    rotPosYButton->setAutoRepeat(true);
    rotNegYButton->setAutoRepeat(true);
    rotPosZButton->setAutoRepeat(true);
    rotNegZButton->setAutoRepeat(true);

    int row=0;
    mainLayout->addWidget(new QLabel("Translational motion"),row,0,1,1,Qt::AlignCenter);
    mainLayout->addWidget(new QLabel("Rotational motion"),row,1,1,1,Qt::AlignCenter);

    row++;
    mainLayout->addWidget(keyWidget,row,0,3,1,Qt::AlignCenter);
    mainLayout->addWidget(rotKeyWidget,row,1,3,1,Qt::AlignCenter);

    row=row+3;
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout -> addWidget(line,row,0,1,2);

    row++;
    mainLayout->addWidget(velAccWidget,row,0,1,1);
    mainLayout->addWidget(coordInfoWidget,row,1,3,1);

    int krow=0;
    keyLayout->addWidget(posZButton, krow, 0, 1, 1);
    keyLayout->addWidget(negZButton, krow,2,1,1);

    krow++;
    keyLayout->addWidget(posXButton,krow,1,1,1);

    krow++;
    keyLayout->addWidget(posYButton,krow,0,1,1);
    keyLayout->addWidget(negYButton,krow,2,1,1);

    krow++;
    keyLayout->addWidget(negXButton,krow,1,1,1);

    // Positive Z Button
    QIcon icon3;
    icon3.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-up-double.png"), QSize(), QIcon::Normal, QIcon::Off);
    posZButton->setIcon(icon3);
    posZButton->setIconSize(QSize(32, 32));
    posZButton->setToolTip(QApplication::translate("Ur5Widget", "Move in positive Z direction", 0));

    // Negative Z Button
    QIcon icon2;
    icon2.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-down-double.png"), QSize(), QIcon::Normal, QIcon::Off);
    negZButton->setIcon(icon2);
    negZButton->setIconSize(QSize(32, 32));
    negZButton->setToolTip(QApplication::translate("Ur5Widget", "Move in negative Z direction", 0));

    // Positive X Button
    QIcon icon4;
    icon4.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-up.png"), QSize(), QIcon::Normal, QIcon::Off);
    posXButton->setIcon(icon4);
    posXButton->setIconSize(QSize(32, 32));

    // Negative Y Button
    QIcon icon5;
    icon5.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-right.png"), QSize(), QIcon::Normal, QIcon::Off);
    negYButton->setIcon(icon5);
    negYButton->setIconSize(QSize(32, 32));

    // Positive Y Button
    QIcon icon6;
    icon6.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-left.png"), QSize(), QIcon::Normal, QIcon::Off);
    posYButton->setIcon(icon6);
    posYButton->setIconSize(QSize(32, 32));

    // Negative X Button
    QIcon icon8;
    icon8.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-down.png"), QSize(), QIcon::Normal, QIcon::Off);
    negXButton->setIcon(icon8);
    negXButton->setIconSize(QSize(32, 32));

    // Style sheet for buttons
    posXButton->setStyleSheet("border:none");
    negXButton->setStyleSheet("border:none");
    posYButton->setStyleSheet("border:none");
    negYButton->setStyleSheet("border:none");
    posZButton->setStyleSheet("border:none");
    negZButton->setStyleSheet("border:none");


    int rkrow=0;
    rotKeyLayout->addWidget(rotNegXButton, rkrow, 0, 1, 1);
    rotKeyLayout->addWidget(rotPosXButton, rkrow,2,1,1);

    rkrow++;
    rotKeyLayout->addWidget(rotPosYButton,rkrow,1,1,1);

    rkrow++;
    rotKeyLayout->addWidget(rotNegZButton,rkrow,0,1,1);
    rotKeyLayout->addWidget(rotPosZButton,rkrow,2,1,1);

    rkrow++;
    rotKeyLayout->addWidget(rotNegYButton,rkrow,1,1,1);

    // rotPositive Z Button
    QIcon i3;
    i3.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-right.png"), QSize(), QIcon::Normal, QIcon::Off);
    rotPosZButton->setIcon(i3);
    rotPosZButton->setIconSize(QSize(32, 32));
    rotPosZButton->setToolTip(QApplication::translate("Ur5Widget", "Move in rotPositive Z direction", 0));

    // rotNegative Z Button
    QIcon i2;
    i2.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-left.png"), QSize(), QIcon::Normal, QIcon::Off);
    rotNegZButton->setIcon(i2);
    rotNegZButton->setIconSize(QSize(32, 32));
    rotNegZButton->setToolTip(QApplication::translate("Ur5Widget", "Move in rotNegative Z direction", 0));

    // rotPositive X Button
    QIcon i4;
    i4.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/edit-redo-7.ico"), QSize(), QIcon::Normal, QIcon::Off);
    rotPosXButton->setIcon(i4);
    rotPosXButton->setIconSize(QSize(32, 32));

    // rotNegative Y Button
    QIcon i5;
    i5.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-down.png"), QSize(), QIcon::Normal, QIcon::Off);
    rotNegYButton->setIcon(i5);
    rotNegYButton->setIconSize(QSize(32, 32));

    // rotPositive Y Button
    QIcon i6;
    i6.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-up.png"), QSize(), QIcon::Normal, QIcon::Off);
    rotPosYButton->setIcon(i6);
    rotPosYButton->setIconSize(QSize(32, 32));

    // rotNegative X Button
    QIcon i8;
    i8.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/edit-undo-7.ico"), QSize(), QIcon::Normal, QIcon::Off);
    rotNegXButton->setIcon(i8);
    rotNegXButton->setIconSize(QSize(32, 32));

    // Style sheet for buttons
    rotPosXButton->setStyleSheet("border:none");
    rotNegXButton->setStyleSheet("border:none");
    rotPosYButton->setStyleSheet("border:none");
    rotNegYButton->setStyleSheet("border:none");
    rotPosZButton->setStyleSheet("border:none");
    rotNegZButton->setStyleSheet("border:none");


    // Velocity
    velAccLayout->addWidget(new QLabel("Velocity"), 0, 0, 1, 1);
    velocityLineEdit = new QLineEdit();
    velAccLayout->addWidget(velocityLineEdit, 0, 1, 1, 1);
    velocityLineEdit->setText(QApplication::translate("Ur5Widget", "0.1", 0));
    velAccLayout->addWidget(new QLabel("m/s"), 0, 2, 1, 1);

    // Acceleration
    accelerationLineEdit = new QLineEdit();
    velAccLayout->addWidget(accelerationLineEdit, 1, 1, 1, 1);
    accelerationLineEdit->setText(QApplication::translate("Ur5Widget", "0.5", 0));
    velAccLayout->addWidget(new QLabel("Acceleration"), 1, 0, 1, 1);
    velAccLayout->addWidget(new QLabel("m/s^2"), 1, 2, 1, 1);

    // Time
    velAccLayout->addWidget(new QLabel("Time"), 2, 0, 1, 1);
    timeLineEdit = new QLineEdit();
    velAccLayout->addWidget(timeLineEdit, 2, 1, 1, 1);
    timeLineEdit->setText(QApplication::translate("Ur5Widget", "0.5", 0));
    velAccLayout->addWidget(new QLabel("s"), 2, 2, 1, 1);

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

void Ur5ManualMoveTab::coordButtonPressed(int axis, int sign)
{
    Ur5State velocity;
    velocity.jointAxisVelocity(axis)=(sign)*velocityLineEdit->text().toDouble();
    mUr5Robot->sendMessage(mUr5Robot->mMessageEncoder.speedl(velocity,accelerationLineEdit->text(),timeLineEdit->text()));
}

void Ur5ManualMoveTab::rotButtonPressed(int angle, int sign)
{
    Ur5State velocity;
    velocity.jointAngleVelocity(angle)=(sign)*velocityLineEdit->text().toDouble();
    mUr5Robot->sendMessage(mUr5Robot->mMessageEncoder.speedl(velocity,accelerationLineEdit->text(),timeLineEdit->text()));
}

void Ur5ManualMoveTab::posZButtonPressedSlot()
{
    emit(coordButtonPressed(2,1));
}

void Ur5ManualMoveTab::negZButtonPressedSlot()
{
    emit(coordButtonPressed(2,-1));
}

void Ur5ManualMoveTab::posYButtonPressedSlot()
{
    emit(coordButtonPressed(1,1));
}

void Ur5ManualMoveTab::negYButtonPressedSlot()
{
    emit(coordButtonPressed(1,-1));
}

void Ur5ManualMoveTab::posXButtonPressedSlot()
{
   emit(coordButtonPressed(0,1));
}

void Ur5ManualMoveTab::negXButtonPressedSlot()
{
    emit(coordButtonPressed(0,-1));
}

void Ur5ManualMoveTab::posRXButtonPressedSlot()
{
    emit(rotButtonPressed(0,1));
}

void Ur5ManualMoveTab::negRXButtonPressedSlot()
{
    emit(rotButtonPressed(0,-1));
}

void Ur5ManualMoveTab::posRYButtonPressedSlot()
{
    emit(rotButtonPressed(1,1));
}

void Ur5ManualMoveTab::negRYButtonPressedSlot()
{
    emit(rotButtonPressed(1,-1));
}

void Ur5ManualMoveTab::posRZButtonPressedSlot()
{
    emit(rotButtonPressed(2,1));
}

void Ur5ManualMoveTab::negRZButtonPressedSlot()
{
    emit(rotButtonPressed(2,-1));
}

void Ur5ManualMoveTab::moveButtonReleasedSlot()
{
    emit(mUr5Robot->sendMessage(mUr5Robot->mMessageEncoder.stopl(accelerationLineEdit->text())));
}

void Ur5ManualMoveTab::updatePositionSlot()
{
    xPosLineEdit->setText(QString::number(1000*(mUr5Robot->mCurrentState.cartAxis(0)),'f',2));
    yPosLineEdit->setText(QString::number(1000*(mUr5Robot->mCurrentState.cartAxis(1)),'f',2));
    zPosLineEdit->setText(QString::number(1000*(mUr5Robot->mCurrentState.cartAxis(2)),'f',2));
    rxLineEdit->setText(QString::number(mUr5Robot->mCurrentState.cartAngles(0),'f',4));
    ryLineEdit->setText(QString::number(mUr5Robot->mCurrentState.cartAngles(1),'f',4));
    rzLineEdit->setText(QString::number(mUr5Robot->mCurrentState.cartAngles(2),'f',4));
}

void Ur5ManualMoveTab::connectMovementButtons()
{
    connect(posXButton,SIGNAL(pressed()),this,SLOT(posXButtonPressedSlot()));
    connect(posXButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(negXButton,SIGNAL(pressed()),this,SLOT(negXButtonPressedSlot()));
    connect(negXButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(posYButton,SIGNAL(pressed()),this,SLOT(posYButtonPressedSlot()));
    connect(posYButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(negYButton,SIGNAL(pressed()),this,SLOT(negYButtonPressedSlot()));
    connect(negYButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(posZButton,SIGNAL(pressed()),this,SLOT(posZButtonPressedSlot()));
    connect(posZButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(negZButton,SIGNAL(pressed()),this,SLOT(negZButtonPressedSlot()));
    connect(negZButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotPosXButton,SIGNAL(pressed()),this,SLOT(posRXButtonPressedSlot()));
    connect(rotPosXButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotNegXButton,SIGNAL(pressed()),this,SLOT(negRXButtonPressedSlot()));
    connect(rotNegXButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotPosYButton,SIGNAL(pressed()),this,SLOT(posRYButtonPressedSlot()));
    connect(rotPosYButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotNegYButton,SIGNAL(pressed()),this,SLOT(negRYButtonPressedSlot()));
    connect(rotNegYButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotPosZButton,SIGNAL(pressed()),this,SLOT(posRZButtonPressedSlot()));
    connect(rotPosZButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotNegZButton,SIGNAL(pressed()),this,SLOT(negRZButtonPressedSlot()));
    connect(rotNegZButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));
}

} // cx
