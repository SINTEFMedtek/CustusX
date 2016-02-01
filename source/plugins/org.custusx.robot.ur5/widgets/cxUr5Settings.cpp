#include "cxUr5Settings.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QGroupBox>
#include <QList>

namespace cx
{

Ur5SettingsTab::Ur5SettingsTab(Ur5RobotPtr Ur5Robot,VisServicesPtr services, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot),
    mServices(services)
{
    setupUi(this);
}

Ur5SettingsTab::~Ur5SettingsTab()
{
}

void Ur5SettingsTab::setupUi(QWidget *parent)
{
   mainLayout = new QHBoxLayout(this);

   QWidget *leftColumnWidgets = new QWidget();
   QVBoxLayout *leftColumnLayout = new QVBoxLayout(leftColumnWidgets);

   QWidget *rightColumnWidgets = new QWidget();
   QVBoxLayout *rightColumnLayout = new QVBoxLayout(rightColumnWidgets);

   setMoveToolLayout(leftColumnLayout);
   //setCoordInfoWidget(rightColumnLayout);
   //setMoveSettingsWidget(leftColumnLayout);
   //setJointMoveWidget(rightColumnLayout);

   mainLayout->addWidget(leftColumnWidgets,0,Qt::AlignTop|Qt::AlignLeft);
   mainLayout->addWidget(rightColumnWidgets,0,Qt::AlignTop|Qt::AlignRight);

   mainLayout->setSpacing(5);
   mainLayout->setMargin(5);
}

void Ur5SettingsTab::setMoveToolLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Tool Configuration");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *keyLayout = new QGridLayout();
    group->setLayout(keyLayout);
    keyLayout->setSpacing(0);
    keyLayout->setMargin(0);
    keyLayout->setContentsMargins(0,0,0,0);
}

void Ur5SettingsTab::setMoveSettingsWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move settings");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *velAccLayout = new QGridLayout();
    group->setLayout(velAccLayout);

    velAccLayout->setSpacing(5);
    velAccLayout->setMargin(5);
}

void Ur5SettingsTab::setCoordInfoWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Tool position");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *coordInfoLayout = new QGridLayout();
    group->setLayout(coordInfoLayout);

    coordInfoLayout->setSpacing(5);
    coordInfoLayout->setMargin(5);
}

void Ur5SettingsTab::setJointMoveWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move joints");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *coordInfoLayout = new QGridLayout();
    group->setLayout(coordInfoLayout);

    coordInfoLayout->setSpacing(5);
    coordInfoLayout->setMargin(5);
}

} // cx
