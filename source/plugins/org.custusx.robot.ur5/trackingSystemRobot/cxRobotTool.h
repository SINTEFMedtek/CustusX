#ifndef CXROBOTTOOL_H
#define CXROBOTTOOL_H

#include "org_custusx_robot_ur5_Export.h"

#include "cxToolImpl.h"
#include <QTimer>

#include "cxUr5Robot.h"

namespace cx
{
typedef boost::shared_ptr<class RobotTool> RobotToolPtr;

#define PI M_PI

class org_custusx_robot_ur5_EXPORT RobotTool: public ToolImpl
{
    friend class RobotTrackingSystemService;
    Q_OBJECT

public:
    RobotTool(QString uid, Ur5RobotPtr robot, VisServicesPtr services);
    virtual ~RobotTool();

    virtual std::set<Type> getTypes() const;

    virtual vtkPolyDataPtr getGraphicsPolyData() const;
    virtual Transform3D get_prMt() const;
    virtual bool getVisible() const;
    virtual QString getUid() const;
    virtual QString getName() const;
    virtual bool isCalibrated() const;
    virtual double getTimestamp() const;
    virtual double getTooltipOffset() const;
    virtual void setTooltipOffset(double val);

    virtual Transform3D getCalibration_sMt() const;

    virtual bool isInitialized() const;

    virtual void setVisible(bool vis);

    void addRobotActors();
    void removeActors();

    Transform3D get_prMb();
    Transform3D get_eMt();

private slots:
    void toolTransformAndTimestampSlot(Transform3D bMe, double timestamp);
    void calculateTpsSlot();
    void toolVisibleSlot(bool);

private:
    void createPolyData();
    void initiateActors();
    void updateActors();
    void set_prMb_calibration();
    void set_eMt_calibration();

    std::set<Type> mTypes;

    vtkPolyDataPtr mPolyData;

    QTimer mTpsTimer;
    Transform3D m_prMt,m_sMt_calibration;
    Ur5RobotPtr mUr5Robot;
    VisServicesPtr mServices;
    double mTimestamp;

    std::set<Type> determineTypesBasedOnUid(const QString uid) const;
    QString mGraphicsFolderName = QString("/CX/source/plugins/org.custusx.robot.ur5/trackingSystemRobot/robotSTL/");

    Transform3D prMb, eMt;
    Transform3D get_rMb();

    vtkActorPtr vtkSourceToActor(QString filename);
    vtkActorPtr eeActor, baseActor, link1Actor, link2Actor, link3Actor, link4Actor, link5Actor;

    bool isRobotLinksVisualized;
};

} /* namespace cx */

#endif // CXROBOTTOOL_H
