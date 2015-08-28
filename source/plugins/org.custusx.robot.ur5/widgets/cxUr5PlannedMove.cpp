#include "cxUr5PlannedMove.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>


namespace cx
{

Ur5PlannedMoveTab::Ur5PlannedMoveTab(Ur5RobotPtr ur5robot, QWidget *parent) :
    QWidget(parent),
    ur5Robot(ur5robot)
{
    setupUi(this);

    connect(runVTKButton,SIGNAL(clicked()),this,SLOT(runVTKfileSlot()));
    connect(goToOrigoButton,SIGNAL(clicked()),this,SLOT(goToOrigoButtonSlot()));
}

Ur5PlannedMoveTab::~Ur5PlannedMoveTab()
{

}

void Ur5PlannedMoveTab::setupUi(QWidget *parent)
{
    QVBoxLayout *vertLayout = new QVBoxLayout(this);
    vertLayout->setAlignment(Qt::AlignTop);
    QHBoxLayout *layout1 = new QHBoxLayout();

    vtkLineEdit = new QLineEdit();
    runVTKButton = new QPushButton();

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

    goToOrigoButton = new QPushButton();
    goToOrigoButton->setText("Go to origo");
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

    row++;
    // Acceleration
    accelerationLineEdit = new QLineEdit();
    mainLayout->addWidget(accelerationLineEdit, row, 1, 1, 1);
    accelerationLineEdit->setText(QApplication::translate("Ur5Widget", "0.1", 0));
    mainLayout->addWidget(new QLabel("Acceleration"), row, 0, 1, 1);
    mainLayout->addWidget(new QLabel("m/s^2"), row, 2, 1, 1);

    vertLayout->addLayout(mainLayout);
}

void Ur5PlannedMoveTab::runVTKfileSlot()
{
    ur5Robot->openVTKfile(vtkLineEdit->text());
    ur5Robot->moveProgram(ur5Robot->mProgramEncoder.poseQueue,accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble(),0);
}

void Ur5PlannedMoveTab::goToOrigoButtonSlot()
{
    ur5Robot->sendMessage(ur5Robot->mMessageEncoder.movej(Ur5State(0,0,0,0,0,0),accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble(),0));
}

} // cx
