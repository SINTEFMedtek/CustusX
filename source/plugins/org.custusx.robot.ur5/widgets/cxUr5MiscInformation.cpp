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

    hLayout1->addLayout(coordInfoLayout);
    mainLayout->addLayout(hLayout1);
}

void Ur5MiscInformationTab::updateForceSlot()
{
    //FxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().force(0),'f',1));
    //FyLineEdit->setText(QString::number(mUr5Robot->getCurrentState().force(1),'f',1));
    //FzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().force(2),'f',1));
    //TxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().torque(0),'f',1));
    //TyLineEdit->setText(QString::number(mUr5Robot->getCurrentState().torque(1),'f',1));
    //TzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().torque(2),'f',1));
    //FLineEdit->setText(QString::number(mUr5Robot->getCurrentState().force.length(),'f',1));
}

void Ur5MiscInformationTab::updatePositionSlot()
{
    xPosLineEdit->setText(QString::number(1000*(mUr5Robot->getCurrentState().cartAxis(0)),'f',2));
    yPosLineEdit->setText(QString::number(1000*(mUr5Robot->getCurrentState().cartAxis(1)),'f',2));
    zPosLineEdit->setText(QString::number(1000*(mUr5Robot->getCurrentState().cartAxis(2)),'f',2));
    rxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().cartAngles(0),'f',4));
    ryLineEdit->setText(QString::number(mUr5Robot->getCurrentState().cartAngles(1),'f',4));
    rzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().cartAngles(2),'f',4));

    jxPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointConfiguration(0),'f',4));
    jyPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointConfiguration(1),'f',4));
    jzPosLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointConfiguration(2),'f',4));
    jrxLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointConfiguration(3),'f',4));
    jryLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointConfiguration(4),'f',4));
    jrzLineEdit->setText(QString::number(mUr5Robot->getCurrentState().jointConfiguration(5),'f',4));
}

} // cx

