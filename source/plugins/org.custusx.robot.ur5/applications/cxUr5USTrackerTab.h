#ifndef UR5USTRACKERTAB_H
#define UR5USTRACKERTAB_H


#include "org_custusx_robot_ur5_Export.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

#include "cxBasicVideoSource.h"

#include "applications/cxUr5USTracker.h"
#include "cxUr5Robot.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5USTrackerTab: public QWidget
{
    Q_OBJECT
public:
    Ur5USTrackerTab(Ur5RobotPtr Ur5Robot, VisServicesPtr services, QWidget *parent = 0);
    ~Ur5USTrackerTab();

private slots:
    void testSlot();
    void newFrameSlot();

private:
    Ur5RobotPtr mUr5Robot;
    VisServicesPtr mServices;


    void setupUi(QWidget *parent);
    void setMoveLayout(QVBoxLayout *parent);
    void setSettingsLayout(QVBoxLayout *parent);

    QPushButton *testButton;

    BasicVideoSourcePtr mVideoSource;
};



} // cx


#endif // UR5USTRACKERTAB_H
