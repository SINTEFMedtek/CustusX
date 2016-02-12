#include "cxUr5Script.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>

namespace cx
{

Ur5ScriptTab::Ur5ScriptTab(Ur5RobotPtr Ur5Robot, VisServicesPtr services, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot),
    mServices(services)
{
    setupUi(this);

    connect(sendMessageButton,&QPushButton::clicked,this,&Ur5ScriptTab::sendMessageSlot);
}

Ur5ScriptTab::~Ur5ScriptTab()
{

}

void Ur5ScriptTab::setupUi(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    setTextEditorWidget(mainLayout);
}

void Ur5ScriptTab::setTextEditorWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Ur5 Script");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *textEditLayout = new QGridLayout();
    group->setLayout(textEditLayout);

    textEditor = new QTextEdit();
    textEditLayout->addWidget(textEditor,0,0,2,2);
    textEditor->setText("movej([0.9019,-2.0358,2.0008,-1.5708,-1.5708, 2.3562],a=0.8,v=0.3)");

    sendMessageButton = new QPushButton(tr("Send message"));
    textEditLayout->addWidget(sendMessageButton,2,1,1,1);
}

void Ur5ScriptTab::sendMessageSlot()
{
    mUr5Robot->sendMessage(textEditor->toPlainText());
}


} // cx

