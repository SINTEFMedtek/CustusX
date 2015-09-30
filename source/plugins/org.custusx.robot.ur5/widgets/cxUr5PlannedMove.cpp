#include "cxUr5PlannedMove.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>


namespace cx
{

Ur5PlannedMoveTab::Ur5PlannedMoveTab(Ur5RobotPtr Ur5Robot, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
{
    setupUi(this);

    connect(runVTKButton,SIGNAL(clicked()),this,SLOT(runVTKfileSlot()));
}

Ur5PlannedMoveTab::~Ur5PlannedMoveTab()
{

}

void Ur5PlannedMoveTab::setupUi(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    setMoveVTKWidget(mainLayout);
    setMoveSettingsWidget(mainLayout);
}

void Ur5PlannedMoveTab::setMoveVTKWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Follow .vtk line");
    group->setFlat(true);
    parent->addWidget(group);

    QHBoxLayout *layout1 = new QHBoxLayout();
    group->setLayout(layout1);

    vtkLineEdit = new QLineEdit();
    runVTKButton = new QPushButton();

    layout1->addWidget(new QLabel("Path to .vtk file: "));
    layout1->addWidget(vtkLineEdit);
    layout1->addWidget(runVTKButton);

    runVTKButton->setToolTip("Follow VTK line");
    runVTKButton->setText("Run");


    vtkLineEdit->setText("C:\\artery_centerline_fixed_2.vtk");
}

void Ur5PlannedMoveTab::setMoveSettingsWidget(QVBoxLayout *parent)
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
}

void Ur5PlannedMoveTab::runVTKfileSlot()
{
    mUr5Robot->openVTKfile(vtkLineEdit->text());
    mUr5Robot->moveProgram("movej",accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble(),0);
}

void Ur5PlannedMoveTab::goToOrigoButtonSlot()
{
    mUr5Robot->move("movej",Ur5State(0,0,0,0,0,0),accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble());
}

} // cx
