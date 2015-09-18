#ifndef CXROBOTTOOL_H
#define CXROBOTTOOL_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxToolImpl.h"
#include "cxUr5Robot.h"

namespace cx
{
typedef boost::shared_ptr<class RobotTool> RobotToolPtr;

class org_custusx_robot_ur5_EXPORT RobotTool: public ToolImpl
{
Q_OBJECT

public:
    explicit RobotTool(ToolPtr base, Ur5RobotPtr robot);
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
    virtual std::map<int, Vector3D> getReferencePoints() const;

    virtual TimedTransformMapPtr getPositionHistory() { return mBase->getPositionHistory(); }
    virtual bool isInitialized() const;
    virtual ProbePtr getProbe() const { return mBase->getProbe(); }
    virtual bool hasReferencePointWithId(int id) { return mBase->hasReferencePointWithId(id); }
    virtual TimedTransformMap getSessionHistory(double startTime, double stopTime) { return mBase->getSessionHistory(startTime, stopTime); }

    virtual void set_prMt(const Transform3D& prMt, double timestamp);
    virtual void setVisible(bool vis);

    // extensions
    ToolPtr getBase() { return mBase; }

private slots:
    void onToolTransformAndTimestamp(Transform3D matrix, double timestamp);

private:
    ToolPtr mBase;
    Transform3D m_prMt;
    Ur5RobotPtr mUr5Robot;

};

} /* namespace cx */

#endif // CXROBOTTOOL_H
