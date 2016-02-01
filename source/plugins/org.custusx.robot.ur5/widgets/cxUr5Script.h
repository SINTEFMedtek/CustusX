#ifndef UR5SCRIPT_H
#define UR5SCRIPT_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "cxUr5Robot.h"
#include "org_custusx_robot_ur5_Export.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5ScriptTab : public QWidget
{
    Q_OBJECT
public:
    Ur5ScriptTab(Ur5RobotPtr Ur5Robot, VisServicesPtr services, QWidget *parent = 0);
    virtual ~Ur5ScriptTab();

private slots:
    void sendMessageSlot();

private:
    void setupUi(QWidget *parent);

    QPushButton *sendMessageButton;

    void setTextEditorWidget(QVBoxLayout *parent);

    QTextEdit *textEditor;


    Ur5RobotPtr mUr5Robot;
    VisServicesPtr mServices;
};


} /* namespace cx */

#endif // UR5SCRIPT_H
