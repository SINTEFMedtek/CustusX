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
    tabWidget->addTab(new PlannedMoveTab,tr("Planned movement"));

    QMetaObject::connectSlotsByName(Ur5Widget);
}

InitializeTab::InitializeTab(Ur5ConnectionPtr ur5connection,QWidget *parent) :
    QWidget(parent),
    ipLineEdit(new QLineEdit()),
    connectButton(new QPushButton()),
    disconnectButton(new QPushButton()),
    presetOrigoComboBox(new QComboBox()),
    initializeButton(new QPushButton()),
    initializeProgressBar(new QProgressBar()),
    manualCoordinatesLineEdit(new QLineEdit()),
    initializeButton_2(new QPushButton()),
    initializeProgressBar_2(new QProgressBar()),
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

    QGridLayout *presetCoordLayout = new QGridLayout(presetCoordinatesTab);
    presetCoordLayout->addWidget(new QLabel("Choose origo: "),0,0,1,1);
    presetCoordLayout->addWidget(presetOrigoComboBox,0,1,1,1);
    presetCoordLayout->addWidget(initializeButton,0,2,1,1);

    presetOrigoComboBox->clear();
    presetOrigoComboBox->insertItems(0, QStringList()
                                     << QApplication::translate("Ur5Widget", "Buttom right corner", 0)
                                     << QApplication::translate("Ur5Widget", "Current position", 0)
                                     );
    //presetOrigoComboBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);

    initializeButton->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
    initializeButton->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

    presetCoordLayout->addWidget(initializeProgressBar,2,0,1,3);
    initializeProgressValue=0;
    initializeProgressBar->setValue(initializeProgressValue);
    initializeProgressBar->setAlignment(Qt::AlignCenter);
    initializeProgressBar->setFormat("Initializing "+QString::number(initializeProgressValue)+"%");

    QGridLayout *manualCoordLayout = new QGridLayout(manualCoordinatesTab);
    manualCoordLayout->addWidget(new QLabel("Set coordinates: "), 0, 0, 1, 1);
    manualCoordLayout->addWidget(manualCoordinatesLineEdit,0,1,1,1);
    manualCoordLayout->addWidget(initializeButton_2,0,2,1,1);
    manualCoordLayout->addWidget(initializeProgressBar_2,2,0,1,3);

    manualCoordinatesLineEdit->setText(QApplication::translate("Ur5Widget", "(x,y,z,rx,ry,rz)", 0));
    initializeButton_2->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
    initializeButton_2->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

    initializeProgressBar_2->setValue(initializeProgressValue);
    initializeProgressBar_2->setAlignment(Qt::AlignCenter);
    initializeProgressBar_2->setFormat("Initializing "+QString::number(initializeProgressValue)+"%");

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
    emit(connection->initializeWorkspace(0.00005));
}

ManualMoveTab::ManualMoveTab(Ur5ConnectionPtr ur5Connection,QWidget *parent) :
    QWidget(parent),
    connection(ur5Connection)
{
    setupUi(this);

    posZButton->setAutoRepeat(true);
    negZButton->setAutoRepeat(true);
    posXButton->setAutoRepeat(true);
    negXButton->setAutoRepeat(true);
    posYButton->setAutoRepeat(true);
    negYButton->setAutoRepeat(true);

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

    QWidget *rotKeyWidget = new QWidget();
    QGridLayout *rotKeyLayout = new QGridLayout(rotKeyWidget);
    rotKeyLayout->setSpacing(0);

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
    mainLayout->addWidget(keyWidget,row,0,1,1);
    mainLayout->addWidget(rotKeyWidget,row,1,1,1);

    row++;
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout -> addWidget(line,row,0,1,2);

    row++;
    mainLayout->addWidget(velAccWidget,row,0,1,1);
    mainLayout->addWidget(coordInfoWidget,row,1,1,1);

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

    // X coordinate scrollbar
    xScrollBar = new QScrollBar();
    xScrollBar->setOrientation(Qt::Horizontal);
    coordInfoLayout->addWidget(xScrollBar, 0, 1, 1, 1);

    // Y coordinate scrollbar
    yScrollBar = new QScrollBar();
    yScrollBar->setOrientation(Qt::Horizontal);
    coordInfoLayout->addWidget(yScrollBar, 1, 1, 1, 1);

    // Z coordinate scrollbar
    zScrollBar = new QScrollBar();
    zScrollBar->setOrientation(Qt::Horizontal);
    coordInfoLayout->addWidget(zScrollBar, 3, 1, 1, 1);

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

    // RX orientation scrollbar
    rxScrollBar = new QScrollBar();
    rxScrollBar->setOrientation(Qt::Horizontal);
    coordInfoLayout->addWidget(rxScrollBar, 5, 1, 1, 1);

    // RY orientation scrollbar
    ryScrollBar = new QScrollBar();
    ryScrollBar->setOrientation(Qt::Horizontal);
    coordInfoLayout->addWidget(ryScrollBar, 6, 1, 1, 1);

    // RZ orientation scrollbar
    rzScrollBar = new QScrollBar();
    rzScrollBar->setOrientation(Qt::Horizontal);
    coordInfoLayout->addWidget(rzScrollBar, 7, 1, 1, 1);

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

void ManualMoveTab::coordButtonPressed(int axis, int sign)
{
    Ur5State velocity;
    velocity.jointAxisVelocity(axis)=(sign)*velocityLineEdit->text().toDouble();
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

void ManualMoveTab::moveButtonReleasedSlot()
{
    emit(connection->sendMessage(connection->transmitter.stopl(accelerationLineEdit->text())));
}

PlannedMoveTab::PlannedMoveTab(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

PlannedMoveTab::~PlannedMoveTab()
{

}

void PlannedMoveTab::setupUi(QWidget *parent)
{

}

} /* namespace cx */
