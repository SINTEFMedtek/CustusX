#ifndef UR5LUNGSIMULATIONTAB_H
#define UR5LUNGSIMULATIONTAB_H


#include "org_custusx_robot_ur5_Export.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

#include "applications/cxUr5LungSimulation.h"
#include "cxUr5Robot.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5LungSimulationTab: public QWidget
{
    Q_OBJECT
public:
    Ur5LungSimulationTab(Ur5RobotPtr Ur5Robot,QWidget *parent = 0);
    ~Ur5LungSimulationTab();

private slots:
    void setStartPosLineEdit();
    void setStopPosLineEdit();
    void startSimulationSlot();
    void stopSimulationSlot();

private:
    Ur5RobotPtr mUr5Robot;
    Ur5LungSimulation *mLungSimulation;

    void setupUi(QWidget *parent);
    void setMoveLayout(QVBoxLayout *parent);
    void setSettingsLayout(QVBoxLayout *parent);

    QLineEdit *startPosLineEdit, *stopPosLineEdit;
    QPushButton *setStartPosButton, *setStopPosButton, *startMoveButton, *stopMoveButton;
    QComboBox *velocityProfileCBox;

    QLineEdit *velocityLineEdit, *accelerationLineEdit, *nCyclesLineEdit;
    QLineEdit *inspirationTimeLineEdit, *inspiratoryPauseTimeLineEdit;
    QLineEdit *expirationTimeLineEdit, *expiratoryPauseTimeLineEdit;

    Ur5MessageEncoder mMessageEncoder;
    Eigen::RowVectorXd jointStartPosition, jointStopPosition;


};



} // cx


#endif // UR5LUNGSIMULATIONTAB_H
