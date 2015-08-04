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

    posZButton->setAutoRepeat(true);
    negZButton->setAutoRepeat(true);
    posXButton->setAutoRepeat(true);
    negXButton->setAutoRepeat(true);
    posYButton->setAutoRepeat(true);
    negYButton->setAutoRepeat(true);

    connect(connectButton,SIGNAL(clicked()),this,SLOT(connectButtonSlot()));
    connect(connectButton,SIGNAL(clicked()),this,SLOT(checkConnection()));
    connect(initializeButton,SIGNAL(clicked()),this,SLOT(initializeButtonSlot()));
    connect(posZButton,SIGNAL(pressed()),this,SLOT(posZButtonSlotPushed()));
    connect(posZButton,SIGNAL(released()),this,SLOT(moveButtonSlotReleased()));
    connect(negZButton,SIGNAL(pressed()),this,SLOT(negZButtonSlotPushed()));
    connect(negZButton,SIGNAL(released()),this,SLOT(moveButtonSlotReleased()));
    connect(posYButton,SIGNAL(pressed()),this,SLOT(posYButtonSlotPushed()));
    connect(posYButton,SIGNAL(released()),this,SLOT(moveButtonSlotReleased()));
    connect(negYButton,SIGNAL(pressed()),this,SLOT(negYButtonSlotPushed()));
    connect(negYButton,SIGNAL(released()),this,SLOT(moveButtonSlotReleased()));
    connect(posXButton,SIGNAL(pressed()),this,SLOT(posXButtonSlotPushed()));
    connect(posXButton,SIGNAL(released()),this,SLOT(moveButtonSlotReleased()));
    connect(negXButton,SIGNAL(pressed()),this,SLOT(negXButtonSlotPushed()));
    connect(negXButton,SIGNAL(released()),this,SLOT(moveButtonSlotReleased()));

    connect(disconnectButton,SIGNAL(clicked()),this,SLOT(disconnectButtonSlot()));
}

Ur5Widget::~Ur5Widget()
{
}

void Ur5Widget::posZButtonSlotPushed()
{
    Ur5State posZvel;
    posZvel.jointAxisVelocity(2)=velocityLineEdit->text().toDouble();
    emit(connection.sendMessage(connection.transmitter.speedl(posZvel,accelerationLineEdit->text(),timeLineEdit->text())));
}

void Ur5Widget::negZButtonSlotPushed()
{
    Ur5State zVel;
    zVel.jointAxisVelocity(2)=-velocityLineEdit->text().toDouble();
    emit(connection.sendMessage(connection.transmitter.speedl(zVel,accelerationLineEdit->text(),timeLineEdit->text())));
}

void Ur5Widget::posYButtonSlotPushed()
{
    Ur5State posYvel;
    posYvel.jointAxisVelocity(1)=velocityLineEdit->text().toDouble();
    emit(connection.sendMessage(connection.transmitter.speedl(posYvel,accelerationLineEdit->text(),timeLineEdit->text())));
}

void Ur5Widget::negYButtonSlotPushed()
{
    Ur5State posYvel;
    posYvel.jointAxisVelocity(1)=-velocityLineEdit->text().toDouble();
    emit(connection.sendMessage(connection.transmitter.speedl(posYvel,accelerationLineEdit->text(),timeLineEdit->text())));
}

void Ur5Widget::posXButtonSlotPushed()
{
    Ur5State posYvel;
    posYvel.jointAxisVelocity(0)=velocityLineEdit->text().toDouble();
    emit(connection.sendMessage(connection.transmitter.speedl(posYvel,accelerationLineEdit->text(),timeLineEdit->text())));
}

void Ur5Widget::negXButtonSlotPushed()
{
    Ur5State posYvel;
    posYvel.jointAxisVelocity(0)=-velocityLineEdit->text().toDouble();
    emit(connection.sendMessage(connection.transmitter.speedl(posYvel,accelerationLineEdit->text(),timeLineEdit->text())));
}


void Ur5Widget::moveButtonSlotReleased()
{
    emit(connection.sendMessage(connection.transmitter.stopl(accelerationLineEdit->text())));
}


void Ur5Widget::connectButtonSlot()
{
    connection.setAddress(ipLineEdit->text(), 30002);
    emit(connection.tryConnectAndWait());
}

void Ur5Widget::disconnectButtonSlot()
{
    emit(connection.requestDisconnect());
}

void Ur5Widget::checkConnection()
{
    if(connection.isConnectedToRobot())
        initializeButton->toggle();
}

void Ur5Widget::initializeButtonSlot()
{
    emit(connection.initializeWorkspace(0.00005));
}

void Ur5Widget::setupUi(QWidget *Ur5Widget)
{
    Ur5Widget->setObjectName("Ur5Widget");
    Ur5Widget->setWindowTitle("UR5 Robot");

    QTabWidget *tabWidget = new QTabWidget(Ur5Widget);
    tabWidget->setGeometry(QRect(2, 5, 430, 290));
    insertInitializeTab(tabWidget);
    insertManualMoveTab(tabWidget);    
    insertPlannedMoveTab(tabWidget);

    QMetaObject::connectSlotsByName(Ur5Widget);
} // setupUi

void Ur5Widget::insertPlannedMoveTab(QTabWidget *tabWidget)
{
    QWidget *plannedMoveTab = new QWidget();   
    tabWidget->addTab(plannedMoveTab, QString());
    tabWidget->setTabText(tabWidget->indexOf(plannedMoveTab), QApplication::translate("Ur5Widget", "Planned move", 0));
}

void Ur5Widget::insertInitializeTab(QTabWidget *tabWidget)
{

    QWidget *initializeTab = new QWidget(this);


    // -------------------------------------------------------

    QWidget *horizontalLayoutWidget = new QWidget(initializeTab);

    // IP address label
    QHBoxLayout *horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
    horizontalLayout->addWidget(new QLabel("IP Address: "));

    // IP address line edit
    ipLineEdit = new QLineEdit(horizontalLayoutWidget);
    horizontalLayout->addWidget(ipLineEdit);
    ipLineEdit->setText(QApplication::translate("Ur5Widget", "169.254.62.100", 0));

    // Connect button
    QHBoxLayout *horizontalLayout_2 = new QHBoxLayout();
    connectButton = new QPushButton(horizontalLayoutWidget);

    QIcon icon;
    icon.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-idle.ico"), QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-transmit-receive.ico"), QSize(), QIcon::Normal, QIcon::On);
    connectButton->setIcon(icon);
    horizontalLayout_2->addWidget(connectButton);
    horizontalLayout->addLayout(horizontalLayout_2);
    connectButton->setToolTip(QApplication::translate("Ur5Widget", "Connect to robot", 0));
    connectButton->setText(QApplication::translate("Ur5Widget", "Connect", 0));

    // -------------------------------------------------------------------------------------

    // Hard line
    QFrame *line = new QFrame(initializeTab);
    line->setGeometry(QRect(10, 40, 400, 20));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    // ---------------------------------------------------------------------------------------

    // Tab for preset coordinates and manual coordinates
    QWidget *verticalLayoutWidget = new QWidget(initializeTab);
    verticalLayoutWidget->setGeometry(QRect(10, 60, 400, 140));

    QVBoxLayout *verticalLayout = new QVBoxLayout(verticalLayoutWidget);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    // "Set origo for robot workspace" label
    verticalLayout->addWidget(new QLabel("Set origo for robot workspace"));

    // Preset origo  tab
    QTabWidget *tabWidget_2 = new QTabWidget(verticalLayoutWidget);
    QWidget *tab_4 = new QWidget();

    QWidget *horizontalLayoutWidget_3 = new QWidget(tab_4);
    horizontalLayoutWidget_3->setGeometry(QRect(10, 10, 320, 40));

    QHBoxLayout *horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_3);
    horizontalLayout_3->setContentsMargins(0, 0, 0, 0);

    // Choose origo label
    horizontalLayout_3->addWidget(new QLabel("Choose origo: "), 0, Qt::AlignLeft);

    // Preset Origo ComboBox
    presetOrigoComboBox = new QComboBox(horizontalLayoutWidget_3);
    presetOrigoComboBox->clear();
    presetOrigoComboBox->insertItems(0, QStringList()
                                     << QApplication::translate("Ur5Widget", "Buttom right corner", 0)
                                     << QApplication::translate("Ur5Widget", "Current position", 0)
                                     );
    presetOrigoComboBox->setSizePolicy(QSizePolicy::Ignored,QSizePolizy::Expanding);
    horizontalLayout_3->addWidget(presetOrigoComboBox);

    // Initialize Button in preset coordinates tab
    initializeButton = new QPushButton(horizontalLayoutWidget_3);
    horizontalLayout_3->addWidget(initializeButton);
    initializeButton->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
    initializeButton->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

    // Initialize bar
    initializeBar = new QProgressBar(tab_4);
    initializeBar->setGeometry(QRect(10, 80, 350, 20));
    initializeBar->setValue(0);

    // Add Preset coordinates tab
    tabWidget_2->addTab(tab_4, QString());
    tabWidget_2->setTabText(tabWidget_2->indexOf(tab_4), QApplication::translate("Ur5Widget", "Preset coordinates", 0));

    // Manual coordinates tab
    QWidget *tab_5 = new QWidget();
    QWidget *gridLayoutWidget = new QWidget(tab_5);
    gridLayoutWidget->setGeometry(QRect(10, 10, 330, 40));

    QGridLayout *gridLayout = new QGridLayout(gridLayoutWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    // Set coordinates label
    gridLayout->addWidget(new QLabel("Set coordinates: "), 0, 0, 1, 1);

    // Manual coordinates line edit
    manualCoordinates = new QLineEdit(gridLayoutWidget);
    gridLayout->addWidget(manualCoordinates, 0, 1, 1, 1);
    manualCoordinates->setText(QApplication::translate("Ur5Widget", "(0,0,0,0,0,0)", 0));

    // Initialize button 2
    initializeButton_2 = new QPushButton(gridLayoutWidget);
    initializeButton_2->setObjectName(QStringLiteral("initializeButton_2"));
    gridLayout->addWidget(initializeButton_2, 0, 2, 1, 1);
    initializeButton_2->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
    initializeButton_2->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

    // Add Manual coordinates tab
    tabWidget_2->addTab(tab_5, QString());
    verticalLayout->addWidget(tabWidget_2);
    tabWidget_2->setTabText(tabWidget_2->indexOf(tab_5), QApplication::translate("Ur5Widget", "Manual coordinates", 0));
    tabWidget_2->setCurrentIndex(0);

    // Disconnect button
    disconnectButton = new QPushButton(initializeTab);
    disconnectButton->setObjectName(QStringLiteral("disconnectButton"));
    disconnectButton->setGeometry(QRect(300, 220, 125, 35));
    //disconnectButton->setMaximumSize(QSize(80, 30));
    QIcon icon1;
    icon1.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-offline.ico"), QSize(), QIcon::Normal, QIcon::Off);
    disconnectButton->setIcon(icon1);
    disconnectButton->setText(QApplication::translate("Ur5Widget", "Disconnect", 0));

    // Add initializeTab
    tabWidget->addTab(initializeTab, QString());
    tabWidget->setTabText(tabWidget->indexOf(initializeTab), QApplication::translate("Ur5Widget", "Initialize", 0));
}

void Ur5Widget::insertManualMoveTab(QTabWidget *tabWidget)
{
    QWidget *manualMoveTab = new QWidget();
    manualMoveTab->setObjectName(QStringLiteral("manualMoveTab"));

    QWidget *gridLayoutWidget_2 = new QWidget(manualMoveTab);
    gridLayoutWidget_2->setObjectName(QStringLiteral("gridLayoutWidget_2"));
    gridLayoutWidget_2->setGeometry(QRect(10, 10, 160, 150));

    QGridLayout *gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
    gridLayout_2->setContentsMargins(0, 0, 0, 0);

    // Negative Z Button
    negZButton = new QPushButton(gridLayoutWidget_2);
    QIcon icon2;
    icon2.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-down-double.png"), QSize(), QIcon::Normal, QIcon::Off);
    negZButton->setIcon(icon2);
    negZButton->setIconSize(QSize(32, 32));
    gridLayout_2->addWidget(negZButton, 0, 2, 1, 1);
    negZButton->setToolTip(QApplication::translate("Ur5Widget", "Move in negative Z direction", 0));
    negZButton->setText(QString());

    // Positive Z Button
    posZButton = new QPushButton(gridLayoutWidget_2);
    QIcon icon3;
    icon3.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-up-double.png"), QSize(), QIcon::Normal, QIcon::Off);
    posZButton->setIcon(icon3);
    posZButton->setIconSize(QSize(32, 32));
    gridLayout_2->addWidget(posZButton, 0, 0, 1, 1);
    posZButton->setToolTip(QApplication::translate("Ur5Widget", "Move in positive Z direction", 0));
    posZButton->setText(QString());


    // Positive X Button
    posXButton = new QPushButton(gridLayoutWidget_2);
    QIcon icon4;
    icon4.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-up.png"), QSize(), QIcon::Normal, QIcon::Off);
    posXButton->setIcon(icon4);
    posXButton->setIconSize(QSize(32, 32));
    gridLayout_2->addWidget(posXButton, 1, 1, 1, 1);
    posXButton->setText(QString());

    // Negative Y Button
    negYButton = new QPushButton(gridLayoutWidget_2);
    QIcon icon5;
    icon5.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-right.png"), QSize(), QIcon::Normal, QIcon::Off);
    negYButton->setIcon(icon5);
    negYButton->setIconSize(QSize(32, 32));
    gridLayout_2->addWidget(negYButton, 2, 2, 1, 1);
    negYButton->setText(QString());

    // Positive Y Button
    posYButton = new QPushButton(gridLayoutWidget_2);
    QIcon icon6;
    icon6.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-left.png"), QSize(), QIcon::Normal, QIcon::Off);
    posYButton->setIcon(icon6);
    posYButton->setIconSize(QSize(32, 32));
    gridLayout_2->addWidget(posYButton, 2, 0, 1, 1);
    posYButton->setText(QString());

    // Stop Move button
    stopMove = new QPushButton(gridLayoutWidget_2);
    QIcon icon7;
    icon7.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/application-exit-4.png"), QSize(), QIcon::Normal, QIcon::Off);
    stopMove->setIcon(icon7);
    stopMove->setIconSize(QSize(32, 32));
    gridLayout_2->addWidget(stopMove, 2, 1, 1, 1);
    stopMove->setText(QString());

    // Negative X Button
    negXButton = new QPushButton(gridLayoutWidget_2);
    negXButton->setObjectName(QStringLiteral("negXButton"));
    QIcon icon8;
    icon8.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-down.png"), QSize(), QIcon::Normal, QIcon::Off);
    negXButton->setIcon(icon8);
    negXButton->setIconSize(QSize(32, 32));
    gridLayout_2->addWidget(negXButton, 3, 1, 1, 1);
    negXButton->setText(QString());

    // Style sheet for buttons
    posXButton->setStyleSheet("border:none");
    negXButton->setStyleSheet("border:none");
    posYButton->setStyleSheet("border:none");
    negYButton->setStyleSheet("border:none");
    posZButton->setStyleSheet("border:none");
    negZButton->setStyleSheet("border:none");
    stopMove->setStyleSheet("border:none");

    // Grid for coordinates and scroll bar
    QWidget *gridLayoutWidget_3 = new QWidget(manualMoveTab);
    gridLayoutWidget_3->setObjectName(QStringLiteral("gridLayoutWidget_3"));
    gridLayoutWidget_3->setGeometry(QRect(190, 10, 221, 161));

    QGridLayout *gridLayout_3 = new QGridLayout(gridLayoutWidget_3);
    gridLayout_3->setSpacing(6);
    gridLayout_3->setContentsMargins(11, 11, 11, 11);
    gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
    gridLayout_3->setContentsMargins(0, 0, 0, 0);

    // X coordinate scrollbar
    xScrollBar = new QScrollBar(gridLayoutWidget_3);
    xScrollBar->setOrientation(Qt::Horizontal);
    gridLayout_3->addWidget(xScrollBar, 0, 1, 1, 1);

    // Y coordinate scrollbar
    yScrollBar = new QScrollBar(gridLayoutWidget_3);
    yScrollBar->setOrientation(Qt::Horizontal);
    gridLayout_3->addWidget(yScrollBar, 1, 1, 1, 1);

    // Z coordinate scrollbar
    zScrollBar = new QScrollBar(gridLayoutWidget_3);
    zScrollBar->setOrientation(Qt::Horizontal);
    gridLayout_3->addWidget(zScrollBar, 3, 1, 1, 1);

    // Position label
    gridLayout_3->addWidget(new QLabel("X"), 0, 0, 1, 1, Qt::AlignHCenter);
    gridLayout_3->addWidget(new QLabel("Y"), 1, 0, 1, 1, Qt::AlignHCenter);
    gridLayout_3->addWidget(new QLabel("Z"), 3, 0, 1, 1, Qt::AlignHCenter);

    // mm label
    gridLayout_3->addWidget(new QLabel("mm"), 0, 3, 1, 1);
    gridLayout_3->addWidget(new QLabel("mm"), 1, 3, 1, 1);
    gridLayout_3->addWidget(new QLabel("mm"), 3, 3, 1, 1);

    // Ri orientation label
    gridLayout_3->addWidget(new QLabel("RX"), 5, 0, 1, 1);
    gridLayout_3->addWidget(new QLabel("RZ"), 7, 0, 1, 1);
    gridLayout_3->addWidget(new QLabel("RY"), 6, 0, 1, 1);

    // Rad label
    gridLayout_3->addWidget(new QLabel("Rad"), 5, 3, 1, 1);
    gridLayout_3->addWidget(new QLabel("Rad"), 6, 3, 1, 1);
    gridLayout_3->addWidget(new QLabel("Rad"), 7, 3, 1, 1);

    // X coordinate line edit
    xPosLineEdit = new QLineEdit(gridLayoutWidget_3);
    gridLayout_3->addWidget(xPosLineEdit, 0, 2, 1, 1);

    // Y coordinate line edit
    yPosLineEdit = new QLineEdit(gridLayoutWidget_3);
    gridLayout_3->addWidget(yPosLineEdit, 1, 2, 1, 1);

    // RX orientation scrollbar
    rxScrollBar = new QScrollBar(gridLayoutWidget_3);
    rxScrollBar->setOrientation(Qt::Horizontal);
    gridLayout_3->addWidget(rxScrollBar, 5, 1, 1, 1);

    // RY orientation scrollbar
    ryScrollBar = new QScrollBar(gridLayoutWidget_3);
    ryScrollBar->setOrientation(Qt::Horizontal);
    gridLayout_3->addWidget(ryScrollBar, 6, 1, 1, 1);

    // RZ orientation scrollbar
    rzScrollBar = new QScrollBar(gridLayoutWidget_3);
    rzScrollBar->setOrientation(Qt::Horizontal);
    gridLayout_3->addWidget(rzScrollBar, 7, 1, 1, 1);

    // Line edit for RY orientation
    ryLineEdit = new QLineEdit(gridLayoutWidget_3);
    ryLineEdit->setObjectName(QStringLiteral("ryLineEdit"));
    gridLayout_3->addWidget(ryLineEdit, 6, 2, 1, 1);

    // Line edit for RZ orientation
    rzLineEdit = new QLineEdit(gridLayoutWidget_3);
    rzLineEdit->setObjectName(QStringLiteral("rzLineEdit"));
    gridLayout_3->addWidget(rzLineEdit, 7, 2, 1, 1);

    // Line edit for Z position
    zPosLineEdit = new QLineEdit(gridLayoutWidget_3);
    zPosLineEdit->setObjectName(QStringLiteral("zPosLineEdit"));
    gridLayout_3->addWidget(zPosLineEdit, 3, 2, 1, 1);

    // Line edit for RX orientation
    rxLineEdit = new QLineEdit(gridLayoutWidget_3);
    rxLineEdit->setObjectName(QStringLiteral("rxLineEdit"));
    gridLayout_3->addWidget(rxLineEdit, 5, 2, 1, 1);

    // Hard line
    QFrame *line_2 = new QFrame(manualMoveTab);
    line_2->setGeometry(QRect(10, 170, 401, 16));
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);

    // Grid for speed, velocity and time
    QWidget *gridLayoutWidget_5 = new QWidget(manualMoveTab);
    gridLayoutWidget_5->setGeometry(QRect(10, 190, 231, 74));

    QGridLayout *gridLayout_5 = new QGridLayout(gridLayoutWidget_5);
    gridLayout_5->setSpacing(6);
    gridLayout_5->setContentsMargins(11, 11, 11, 11);
    gridLayout_5->setContentsMargins(0, 0, 0, 0);

    // Acceleration line edit
    accelerationLineEdit = new QLineEdit(gridLayoutWidget_5);
    gridLayout_5->addWidget(accelerationLineEdit, 1, 1, 1, 1);
    accelerationLineEdit->setText(QApplication::translate("Ur5Widget", "0.5", 0));

    // Velocity line edit
    velocityLineEdit = new QLineEdit(gridLayoutWidget_5);
    gridLayout_5->addWidget(velocityLineEdit, 0, 1, 1, 1);
    velocityLineEdit->setText(QApplication::translate("Ur5Widget", "0.1", 0));

    // Acceleration label
    gridLayout_5->addWidget(new QLabel("Acceleration"), 1, 0, 1, 1);

    // Velocity label
    gridLayout_5->addWidget(new QLabel("Velocity"), 0, 0, 1, 1);

    // Time label
    gridLayout_5->addWidget(new QLabel("Time"), 2, 0, 1, 1);

    // Time line edit
    timeLineEdit = new QLineEdit(gridLayoutWidget_5);
    gridLayout_5->addWidget(timeLineEdit, 2, 1, 1, 1);
    timeLineEdit->setText(QApplication::translate("Ur5Widget", "0.5", 0));

    // Labeling units
    gridLayout_5->addWidget(new QLabel("m/s"), 0, 2, 1, 1);
    gridLayout_5->addWidget(new QLabel("m/s^2"), 1, 2, 1, 1);
    gridLayout_5->addWidget(new QLabel("s"), 2, 2, 1, 1);

    // Add manual move tab
    tabWidget->addTab(manualMoveTab, QString());
    tabWidget->setTabText(tabWidget->indexOf(manualMoveTab), QApplication::translate("Ur5Widget", "Manual move", 0));
}

QString Ur5Widget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Example plugin.</h3>"
      "<p>Used for developers as a starting points for developing a new plugin</p>"
      "</html>";
}

} /* namespace cx */
