#ifndef UR5SETTINGS_H
#define UR5SETTINGS_H

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

class org_custusx_robot_ur5_EXPORT Ur5SettingsTab : public QWidget
{
    Q_OBJECT
public:
    Ur5SettingsTab(Ur5RobotPtr Ur5Robot, VisServicesPtr services, QWidget *parent = 0);
    virtual ~Ur5SettingsTab();

private slots:
    void autoCalibrateSlot();
    void clearCalibrationSlot();

private:
    QHBoxLayout *mainLayout;
    void setupUi(QWidget *parent);

    Ur5RobotPtr mUr5Robot;
    VisServicesPtr mServices;

    void setToolConfigurationLayout(QVBoxLayout *vLayout);

    QComboBox *toolComboBox;
    QPushButton *autoCalibrateButton, *clearCalibrationButton;

    void updateCombobox();
    void createCalibrationMatrix();
};

} // cx

#endif // UR5SETTINGS_H
