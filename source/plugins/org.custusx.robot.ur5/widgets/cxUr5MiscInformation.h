#ifndef UR5MISCINFORMATION_H
#define UR5MISCINFORMATION_H

#include <QWidget>
#include <QLineEdit>

#include "cxUr5Robot.h"
#include "org_custusx_robot_ur5_Export.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5MiscInformationTab : public QWidget
{
    Q_OBJECT
public:
    Ur5MiscInformationTab(Ur5RobotPtr Ur5Robot, QWidget *parent = 0);
    virtual ~Ur5MiscInformationTab();

    QLineEdit *FxLineEdit, *FyLineEdit, *FzLineEdit;
    QLineEdit *FLineEdit;
    QLineEdit *TxLineEdit, *TyLineEdit, *TzLineEdit;

    QLineEdit *xPosLineEdit, *yPosLineEdit, *zPosLineEdit;
    QLineEdit *rxLineEdit, *ryLineEdit, *rzLineEdit;

    QLineEdit *jxPosLineEdit, *jyPosLineEdit, *jzPosLineEdit;
    QLineEdit *jrxLineEdit, *jryLineEdit, *jrzLineEdit;

    QLineEdit *txPosLineEdit, *tyPosLineEdit, *tzPosLineEdit;
    QLineEdit *trxLineEdit, *tryLineEdit, *trzLineEdit;


protected slots:
    void updateForceSlot();
    void updatePositionSlot();

private:
    void setupUi(QWidget *parent);
    Ur5RobotPtr mUr5Robot;
};


} /* namespace cx */

#endif // UR5MISCINFORMATION_H
