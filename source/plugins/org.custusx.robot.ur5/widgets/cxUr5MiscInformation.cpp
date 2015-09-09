#include "cxUr5MiscInformation.h"

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace cx
{

Ur5MiscInformationTab::Ur5MiscInformationTab(Ur5RobotPtr Ur5Robot, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
{
    setupUi(this);

    connect(mUr5Robot.get(),SIGNAL(stateUpdated()),this,SLOT(updateForceSlot()));
    connect(mUr5Robot.get(),SIGNAL(stateUpdated()),this,SLOT(updatePositionSlot()));
}

Ur5MiscInformationTab::~Ur5MiscInformationTab()
{

}

void Ur5MiscInformationTab::setupUi(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout1 = new QHBoxLayout();

    QGridLayout *coordInfoLayout = new QGridLayout();

    coordInfoLayout->addWidget(new QLabel("F_x"), 0, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("F_y"), 1, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("F_z"), 2, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("F"),1,4,1,1);

    coordInfoLayout->addWidget(new QLabel("N"), 0, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("N"), 1, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("N"), 2, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("N"),1,6,1,1);

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
    coordInfoLayout->addWidget(FzLineEdit, 2, 2, 1, 1);

    TxLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(TxLineEdit, 5, 2, 1, 1);

    FLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(FLineEdit,1,5,1,1);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    coordInfoLayout->addWidget(line,8,0,1,10);

    // Position label
    coordInfoLayout->addWidget(new QLabel("X"), 9, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Y"), 10, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Z"), 11, 0, 1, 1, Qt::AlignHCenter);

    // mm label
    coordInfoLayout->addWidget(new QLabel("mm"), 9, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("mm"), 10, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("mm"), 11, 3, 1, 1);

    // Ri orientation label
    coordInfoLayout->addWidget(new QLabel("RX"), 12, 0, 1, 1);
    coordInfoLayout->addWidget(new QLabel("RZ"), 13, 0, 1, 1);
    coordInfoLayout->addWidget(new QLabel("RY"), 14, 0, 1, 1);

    // Rad label
    coordInfoLayout->addWidget(new QLabel("Rad"), 12, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 13, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 14, 3, 1, 1);

    // X coordinate line edit
    xPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(xPosLineEdit, 9, 2, 1, 1);

    // Y coordinate line edit
    yPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(yPosLineEdit, 10, 2, 1, 1);

    // Line edit for Z position
    zPosLineEdit = new QLineEdit();
    zPosLineEdit->setObjectName(QStringLiteral("zPosLineEdit"));
    coordInfoLayout->addWidget(zPosLineEdit, 11, 2, 1, 1);

    // Line edit for RX orientation
    rxLineEdit = new QLineEdit();
    rxLineEdit->setObjectName(QStringLiteral("rxLineEdit"));
    coordInfoLayout->addWidget(rxLineEdit, 12, 2, 1, 1);

    // Line edit for RY orientation
    ryLineEdit = new QLineEdit();
    ryLineEdit->setObjectName(QStringLiteral("ryLineEdit"));
    coordInfoLayout->addWidget(ryLineEdit, 13, 2, 1, 1);

    // Line edit for RZ orientation
    rzLineEdit = new QLineEdit();
    rzLineEdit->setObjectName(QStringLiteral("rzLineEdit"));
    coordInfoLayout->addWidget(rzLineEdit, 14, 2, 1, 1);


    // JOINT

    // Position label
    coordInfoLayout->addWidget(new QLabel("Base"), 9, 4, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Shoulder"), 10, 4, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Elbow"), 11, 4, 1, 1, Qt::AlignHCenter);

    // mm label
    coordInfoLayout->addWidget(new QLabel("Rad"), 9, 6, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 10, 6, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 11, 6, 1, 1);

    // Ri orientation label
    coordInfoLayout->addWidget(new QLabel("Wrist 1"), 12, 4, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Wrist 2"), 13, 4, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Wrist 3"), 14, 4, 1, 1);

    // Rad label
    coordInfoLayout->addWidget(new QLabel("Rad"), 12, 6, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 13, 6, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 14, 6, 1, 1);

    // X coordinate line edit
    jxPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(jxPosLineEdit, 9, 5, 1, 1);

    // Y coordinate line edit
    jyPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(jyPosLineEdit, 10, 5, 1, 1);

    // Line edit for Z position
    jzPosLineEdit = new QLineEdit();
    jzPosLineEdit->setObjectName(QStringLiteral("zPosLineEdit"));
    coordInfoLayout->addWidget(jzPosLineEdit, 11, 5, 1, 1);

    // Line edit for RX orientation
    jrxLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(jrxLineEdit, 12, 5, 1, 1);

    // Line edit for RY orientation
    jryLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(jryLineEdit, 13, 5, 1, 1);

    // Line edit for RZ orientation
    jrzLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(jrzLineEdit, 14, 5, 1, 1);


    // TCP

    // Position label
    coordInfoLayout->addWidget(new QLabel("X"), 9, 7, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Y"), 10, 7, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Z"), 11, 7, 1, 1, Qt::AlignHCenter);

    // mm label
    coordInfoLayout->addWidget(new QLabel("mm"), 9, 9, 1, 1);
    coordInfoLayout->addWidget(new QLabel("mm"), 10, 9, 1, 1);
    coordInfoLayout->addWidget(new QLabel("mm"), 11, 9, 1, 1);

    // Ri orientation label
    coordInfoLayout->addWidget(new QLabel("RX"), 12, 7, 1, 1);
    coordInfoLayout->addWidget(new QLabel("RZ"), 13, 7, 1, 1);
    coordInfoLayout->addWidget(new QLabel("RY"), 14, 7, 1, 1);

    // Rad label
    coordInfoLayout->addWidget(new QLabel("Rad"), 12, 9, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 13, 9, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 14, 9, 1, 1);

    // X coordinate line edit
    txPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(txPosLineEdit, 9, 8, 1, 1);

    // Y coordinate line edit
    tyPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(tyPosLineEdit, 10, 8, 1, 1);

    // Line edit for Z position
    tzPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(tzPosLineEdit, 11, 8, 1, 1);

    // Line edit for RX orientation
    trxLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(trxLineEdit, 12, 8, 1, 1);

    // Line edit for RY orientation
    tryLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(tryLineEdit, 13, 8, 1, 1);

    // Line edit for RZ orientation
    trzLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(trzLineEdit, 14, 8, 1, 1);

    hLayout1->addLayout(coordInfoLayout);
    mainLayout->addLayout(hLayout1);
}


void Ur5MiscInformationTab::updateForceSlot()
{
    FxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().force(0),'f',1));
    FyLineEdit->setText(QString::number(mUr5Robot->getCurrentState().force(1),'f',1));
    FzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().force(2),'f',1));
    TxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().torque(0),'f',1));
    TyLineEdit->setText(QString::number(mUr5Robot->getCurrentState().torque(1),'f',1));
    TzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().torque(2),'f',1));
    FLineEdit->setText(QString::number(mUr5Robot->getCurrentState().force.length(),'f',1));
}

void Ur5MiscInformationTab::updatePositionSlot()
{
    xPosLineEdit->setText(QString::number(1000*(mUr5Robot->getCurrentState().cartAxis(0)),'f',2));
    yPosLineEdit->setText(QString::number(1000*(mUr5Robot->getCurrentState().cartAxis(1)),'f',2));
    zPosLineEdit->setText(QString::number(1000*(mUr5Robot->getCurrentState().cartAxis(2)),'f',2));
    rxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().cartAngles(0),'f',4));
    ryLineEdit->setText(QString::number(mUr5Robot->getCurrentState().cartAngles(1),'f',4));
    rzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().cartAngles(2),'f',4));

    jxPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointPosition(0),'f',4));
    jyPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointPosition(1),'f',4));
    jzPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointPosition(2),'f',4));
    jrxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointPosition(3),'f',4));
    jryLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointPosition(4),'f',4));
    jrzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointPosition(5),'f',4));

    txPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().tcpAxis(0),'f',2));
    tyPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().tcpAxis(1),'f',2));
    tzPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().tcpAxis(2),'f',2));
    trxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().tcpAngles(0),'f',4));
    tryLineEdit->setText(QString::number(mUr5Robot->getCurrentState().tcpAngles(1),'f',4));
    trzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().tcpAngles(2),'f',4));
}

} // cx

