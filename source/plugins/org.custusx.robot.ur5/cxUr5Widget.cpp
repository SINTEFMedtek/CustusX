/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#include "cxUr5Widget.h"
#include "cxUr5Connection.h"
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include "cxLogger.h"


namespace cx
{

Ur5Widget::Ur5Widget(QWidget* parent) :
    QWidget(parent)
{
    setupUi(this);
}

Ur5Widget::~Ur5Widget()
{
}

void Ur5Widget::setupUi(QWidget *Ur5Widget)
{
    Ur5Widget->setObjectName("Ur5Widget");
    Ur5Widget->setWindowTitle("UR5 Robot");

    connection = Ur5ConnectionPtr(new Ur5Connection);

    QLayout* Ur5WidgetLayout = new QVBoxLayout(Ur5Widget);

    tabWidget = new QTabWidget(Ur5Widget);
    Ur5WidgetLayout->addWidget(tabWidget);
    tabWidget->addTab(new InitializeTab(connection), tr("Initialize"));
    tabWidget->addTab(new ManualMoveTab(connection),tr("Manual movement"));
    tabWidget->addTab(new PlannedMoveTab(connection),tr("Planned movement"));
    tabWidget->addTab(new InformationTab(connection),tr("Advanced information"));

    QMetaObject::connectSlotsByName(Ur5Widget);
}

InitializeTab::InitializeTab(Ur5ConnectionPtr ur5connection,QWidget *parent) :
    QWidget(parent),
    ipLineEdit(new QLineEdit()),
    connectButton(new QPushButton()),
    disconnectButton(new QPushButton()),
    presetOrigoComboBox(new QComboBox()),
    initializeButton(new QPushButton()),
    manualCoordinatesLineEdit(new QLineEdit()),
    initializeButton_2(new QPushButton()),
    connection(ur5connection)
{
    setupUi(this);
    connect(connectButton,SIGNAL(clicked()),this,SLOT(connectButtonSlot()));
    connect(connectButton,SIGNAL(clicked()),this,SLOT(checkConnection()));
    connect(initializeButton,SIGNAL(clicked()),this,SLOT(initializeButtonSlot()));
    connect(disconnectButton,SIGNAL(clicked()),this,SLOT(disconnectButtonSlot()));
}

InitializeTab::~InitializeTab()
{
}

void InitializeTab::setupUi(QWidget *parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    int row = 0;
    mainLayout->addWidget(new QLabel("IP Address: "), row, 0, 1, 1);
    mainLayout->addWidget(ipLineEdit, row, 1,1,2);
    mainLayout->addWidget(connectButton,row,3,1,1);

    ipLineEdit->setText("169.254.62.100");

    QIcon icon;
    icon.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-idle.ico"), QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-transmit-receive.ico"), QSize(), QIcon::Normal, QIcon::On);
    connectButton->setIcon(icon);
    connectButton->setToolTip("Connect to robot");
    connectButton->setText("Connect");

    row ++;
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout -> addWidget(line,row,0,1,4);

    row ++;
    mainLayout->addWidget(new QLabel("Set origo for robot workspace"),row,0,1,2);

    row++;
    QTabWidget *setCoordinatesTab = new QTabWidget();
    QWidget *presetCoordinatesTab = new QWidget();
    QWidget *manualCoordinatesTab = new QWidget();
    setCoordinatesTab->addTab(presetCoordinatesTab,tr("Preset coordinates"));
    setCoordinatesTab->addTab(manualCoordinatesTab,tr("Manual coordinates"));
    mainLayout->addWidget(setCoordinatesTab,row,0,1,4);

    QHBoxLayout *presetCoordLayout = new QHBoxLayout(presetCoordinatesTab);
    presetCoordLayout->setAlignment(Qt::AlignTop);

    presetCoordLayout->addWidget(new QLabel("Choose origo: "));
    presetCoordLayout->addWidget(presetOrigoComboBox);
    presetCoordLayout->addWidget(initializeButton);

    presetOrigoComboBox->clear();
    presetOrigoComboBox->insertItems(0, QStringList()
                                     << QApplication::translate("Ur5Widget", "Buttom right corner", 0)
                                     << QApplication::translate("Ur5Widget", "Current position", 0)
                                     );
    //presetOrigoComboBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);

    initializeButton->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
    initializeButton->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

    QGridLayout *manualCoordLayout = new QGridLayout(manualCoordinatesTab);
    manualCoordLayout->setAlignment(Qt::AlignTop);
    manualCoordLayout->addWidget(new QLabel("Set coordinates: "), 0, 0, 1, 1);
    manualCoordLayout->addWidget(manualCoordinatesLineEdit,0,1,1,1);
    manualCoordLayout->addWidget(initializeButton_2,0,2,1,1);

    manualCoordinatesLineEdit->setText(QApplication::translate("Ur5Widget", "(x,y,z,rx,ry,rz)", 0));
    initializeButton_2->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
    initializeButton_2->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

    row++;
    mainLayout->addWidget(disconnectButton,row,3,1,1);
    QIcon icon1;
    icon1.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-offline.ico"), QSize(), QIcon::Normal, QIcon::Off);
    disconnectButton->setIcon(icon1);
    disconnectButton->setText("Disconnect");
}

void InitializeTab::connectButtonSlot()
{
    connection->setAddress(ipLineEdit->text(), 30002);
    emit(connection->tryConnectAndWait());
}

void InitializeTab::disconnectButtonSlot()
{
    emit(connection->requestDisconnect());
}

void InitializeTab::checkConnection()
{
    if(connection->isConnectedToRobot())
        initializeButton->toggle();
}

void InitializeTab::initializeButtonSlot()
{
    if(presetOrigoComboBox->currentText() == "Buttom right corner")
    {
        report("Moving to buttom right corner.");
        emit(connection->initializeWorkspace(0.00005));
    }
    else if(presetOrigoComboBox->currentText() == "Current position")
    {
       report("Setting current pos to origo");
       emit(connection->initializeWorkspace(0.000005,Ur5State(0,0,0,0,0,0),true));
    }
    else
    {
        report("Initializing did not work.");
    }
}

ManualMoveTab::ManualMoveTab(Ur5ConnectionPtr ur5Connection,QWidget *parent) :
    QWidget(parent),
    connection(ur5Connection)
{
    setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateState()));
    timer->start(100);

    posZButton->setAutoRepeat(true);
    negZButton->setAutoRepeat(true);
    posXButton->setAutoRepeat(true);
    negXButton->setAutoRepeat(true);
    posYButton->setAutoRepeat(true);
    negYButton->setAutoRepeat(true);
    rotPosXButton->setAutoRepeat(true);
    rotNegXButton->setAutoRepeat(true);
    rotPosYButton->setAutoRepeat(true);
    rotNegYButton->setAutoRepeat(true);
    rotPosZButton->setAutoRepeat(true);
    rotNegZButton->setAutoRepeat(true);

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

ManualMoveTab::~ManualMoveTab()
{
}

void ManualMoveTab::setupUi(QWidget *parent)
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

    rotPosZButton = new QPushButton();
    rotNegZButton = new QPushButton();
    rotPosYButton = new QPushButton();
    rotNegYButton = new QPushButton();
    rotPosXButton = new QPushButton();
    rotNegXButton = new QPushButton();

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

    // Line edit for RY orientation
    ryLineEdit = new QLineEdit();
    ryLineEdit->setObjectName(QStringLiteral("ryLineEdit"));
    coordInfoLayout->addWidget(ryLineEdit, 6, 2, 1, 1);

    // Line edit for RZ orientation
    rzLineEdit = new QLineEdit();
    rzLineEdit->setObjectName(QStringLiteral("rzLineEdit"));
    coordInfoLayout->addWidget(rzLineEdit, 7, 2, 1, 1);

    // Line edit for Z position
    zPosLineEdit = new QLineEdit();
    zPosLineEdit->setObjectName(QStringLiteral("zPosLineEdit"));
    coordInfoLayout->addWidget(zPosLineEdit, 3, 2, 1, 1);

    // Line edit for RX orientation
    rxLineEdit = new QLineEdit();
    rxLineEdit->setObjectName(QStringLiteral("rxLineEdit"));
    coordInfoLayout->addWidget(rxLineEdit, 5, 2, 1, 1);
}

void ManualMoveTab::updateState()
{
    if(connection->isConnectedToRobot())
    {
        emit(connection->update_currentState());
    }
    emit(updatePositionSlot());
}

void ManualMoveTab::coordButtonPressed(int axis, int sign)
{
    Ur5State velocity;
    velocity.jointAxisVelocity(axis)=(sign)*velocityLineEdit->text().toDouble();
    connection->sendMessage(connection->transmitter.speedl(velocity,accelerationLineEdit->text(),timeLineEdit->text()));
}

void ManualMoveTab::rotButtonPressed(int angle, int sign)
{
    Ur5State velocity;
    velocity.jointAngleVelocity(angle)=(sign)*velocityLineEdit->text().toDouble();
    connection->sendMessage(connection->transmitter.speedl(velocity,accelerationLineEdit->text(),timeLineEdit->text()));
}

void ManualMoveTab::posZButtonPressedSlot()
{
    emit(coordButtonPressed(2,1));
}

void ManualMoveTab::negZButtonPressedSlot()
{
    emit(coordButtonPressed(2,-1));
}

void ManualMoveTab::posYButtonPressedSlot()
{
    emit(coordButtonPressed(1,1));
}

void ManualMoveTab::negYButtonPressedSlot()
{
    emit(coordButtonPressed(1,-1));
}

void ManualMoveTab::posXButtonPressedSlot()
{
   emit(coordButtonPressed(0,1));
}

void ManualMoveTab::negXButtonPressedSlot()
{
    emit(coordButtonPressed(0,-1));
}

void ManualMoveTab::posRXButtonPressedSlot()
{
    emit(rotButtonPressed(0,1));
}

void ManualMoveTab::negRXButtonPressedSlot()
{
    emit(rotButtonPressed(0,-1));
}

void ManualMoveTab::posRYButtonPressedSlot()
{
    emit(rotButtonPressed(1,1));
}

void ManualMoveTab::negRYButtonPressedSlot()
{
    emit(rotButtonPressed(1,-1));
}

void ManualMoveTab::posRZButtonPressedSlot()
{
    emit(rotButtonPressed(2,1));
}

void ManualMoveTab::negRZButtonPressedSlot()
{
    emit(rotButtonPressed(2,-1));
}


void ManualMoveTab::moveButtonReleasedSlot()
{
    emit(connection->sendMessage(connection->transmitter.stopl(accelerationLineEdit->text())));
}

void ManualMoveTab::updatePositionSlot()
{
    xPosLineEdit->setText(QString::number(1000*(connection->currentState.cartAxis(0))));
    yPosLineEdit->setText(QString::number(1000*(connection->currentState.cartAxis(1))));
    zPosLineEdit->setText(QString::number(1000*(connection->currentState.cartAxis(2))));
    rxLineEdit->setText(QString::number(connection->currentState.cartAngles(0)));
    ryLineEdit->setText(QString::number(connection->currentState.cartAngles(1)));
    rzLineEdit->setText(QString::number(connection->currentState.cartAngles(2)));
}

PlannedMoveTab::PlannedMoveTab(Ur5ConnectionPtr ur5connection, QWidget *parent) :
    QWidget(parent),
    connection(ur5connection),
    vtkLineEdit(new QLineEdit()),
    runVTKButton(new QPushButton()),
    goToOrigoButton(new QPushButton())
{
    setupUi(this);

    connect(runVTKButton,SIGNAL(clicked()),this,SLOT(runVTKfileSlot()));
    connect(goToOrigoButton,SIGNAL(clicked()),this,SLOT(goToOrigoButtonSlot()));
}

PlannedMoveTab::~PlannedMoveTab()
{

}

void PlannedMoveTab::setupUi(QWidget *parent)
{
    QVBoxLayout *vertLayout = new QVBoxLayout(this);
    vertLayout->setAlignment(Qt::AlignTop);
    QHBoxLayout *layout1 = new QHBoxLayout();

    layout1->addWidget(new QLabel("Path to .vtk file: "));
    layout1->addWidget(vtkLineEdit);
    layout1->addWidget(runVTKButton);

    runVTKButton->setToolTip("Follow VTK line");
    runVTKButton->setText("Run");

    vtkLineEdit->setText("C:\\artery_centerline_fixed_2.vtk");

    vertLayout->addLayout(layout1);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    vertLayout->addWidget(line);

    vertLayout->addWidget(goToOrigoButton);

    QFrame *line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    vertLayout->addWidget(line1);

    QGridLayout *mainLayout = new QGridLayout();

    int row = 0;
    // Velocity
    mainLayout->addWidget(new QLabel("Velocity"), row, 0, 1, 1);
    velocityLineEdit = new QLineEdit();
    mainLayout->addWidget(velocityLineEdit, row, 1, 1, 1);
    velocityLineEdit->setText(QApplication::translate("Ur5Widget", "0.05", 0));
    mainLayout->addWidget(new QLabel("m/s"), row, 2, 1, 1);
    goToOrigoButton->setText("Go to origo");

    row++;
    // Acceleration
    accelerationLineEdit = new QLineEdit();
    mainLayout->addWidget(accelerationLineEdit, row, 1, 1, 1);
    accelerationLineEdit->setText(QApplication::translate("Ur5Widget", "0.1", 0));
    mainLayout->addWidget(new QLabel("Acceleration"), row, 0, 1, 1);
    mainLayout->addWidget(new QLabel("m/s^2"), row, 2, 1, 1);

    vertLayout->addLayout(mainLayout);
}

void PlannedMoveTab::runVTKfileSlot()
{
    connection->transmitter.openVTKfile(vtkLineEdit->text());
    connection->transmitter.movejProgram(connection->transmitter.poseQueue,accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble(),0);
    emit(connection->runProgramQueue());
}

void PlannedMoveTab::goToOrigoButtonSlot()
{
    emit(connection->sendMessage(connection->transmitter.movej(Ur5State(0,0,0,0,0,0),accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble(),0)));
}

InformationTab::InformationTab(Ur5ConnectionPtr ur5connection, QWidget *parent) :
    QWidget(parent),
    connection(ur5connection)
{
    setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateState()));
    timer->start(100);
}

InformationTab::~InformationTab()
{

}

void InformationTab::setupUi(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout1 = new QHBoxLayout();

    QGridLayout *coordInfoLayout = new QGridLayout();

    coordInfoLayout->addWidget(new QLabel("F_x"), 0, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("F_y"), 1, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("F_z"), 3, 0, 1, 1, Qt::AlignHCenter);

    coordInfoLayout->addWidget(new QLabel("N"), 0, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("N"), 1, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("N"), 3, 3, 1, 1);

    coordInfoLayout->addWidget(new QLabel("T_x"), 5, 0, 1, 1);
    coordInfoLayout->addWidget(new QLabel("T_z"), 7, 0, 1, 1);
    coordInfoLayout->addWidget(new QLabel("T_y"), 6, 0, 1, 1);

    coordInfoLayout->addWidget(new QLabel("Nm"), 5, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Nm"), 6, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Nm"), 7, 3, 1, 1);

    FxLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(FxLineEdit, 0, 2, 1, 1);

    FyLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(FyLineEdit, 1, 2, 1, 1);

    TyLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(TyLineEdit, 6, 2, 1, 1);

    TzLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(TzLineEdit, 7, 2, 1, 1);

    FzLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(FzLineEdit, 3, 2, 1, 1);

    TxLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(TxLineEdit, 5, 2, 1, 1);

    hLayout1->addLayout(coordInfoLayout);
    mainLayout->addLayout(hLayout1);
}

void InformationTab::updateState()
{
    if(connection->isConnectedToRobot())
    {
        emit(connection->update_currentState());
    }
    emit(updateForceSlot());
}

void InformationTab::updateForceSlot()
{
    FxLineEdit->setText(QString::number(connection->currentState.force(0)));
    FyLineEdit->setText(QString::number(connection->currentState.force(1)));
    FzLineEdit->setText(QString::number(connection->currentState.force(2)));
    TxLineEdit->setText(QString::number(connection->currentState.torque(0)));
    TyLineEdit->setText(QString::number(connection->currentState.torque(1)));
    TzLineEdit->setText(QString::number(connection->currentState.torque(2)));
}


} /* namespace cx */
