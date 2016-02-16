#ifndef UR5MISCELLANEOUS_H
#define UR5MISCELLANEOUS_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollBar>
#include <QButtonGroup>
#include <QComboBox>

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5MiscellaneousTab : public QWidget
{
    Q_OBJECT
public:
    Ur5MiscellaneousTab(Ur5RobotPtr Ur5Robot, VisServicesPtr services, QWidget *parent = 0);
    virtual ~Ur5MiscellaneousTab();

private slots:
    void startLoggingSlot();
    void stopLoggingSlot();
    void logForCalibrationSlot();
    void dataLogger();

private:
    QHBoxLayout *mainLayout;
    void setupUi(QWidget *parent);

    Ur5RobotPtr mUr5Robot;
    VisServicesPtr mServices;

    void setLoggingLayout(QVBoxLayout *vLayout);

    QPushButton *startLoggingButton, *stopLoggingButton;
    QPushButton *logForCalibrationButton;
};

} // cx

#endif // UR5MISCELLANEOUS_H
