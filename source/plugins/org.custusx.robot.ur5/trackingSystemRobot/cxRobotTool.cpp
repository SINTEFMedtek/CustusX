
#include "cxRobotTool.h"
#include "cxProbeImpl.h"
#include "cxLogger.h"



namespace cx
{

RobotTool::RobotTool(QString uid, Ur5RobotPtr robot):
    ToolImpl(uid,uid),
    mPolyData(NULL),
    mUr5Robot(robot),
    mTimestamp(0)
{
    //connect(&mTpsTimer, &QTimer::timeout, this, &RobotTool::calculateTpsSlot);
    mTypes = this->determineTypesBasedOnUid(Tool::mUid);

    this->createPolyData();
    this->toolVisibleSlot(true);
}

RobotTool::~RobotTool()
{

}

std::set<Tool::Type> RobotTool::determineTypesBasedOnUid(const QString uid) const
{
    std::set<Type> retval;
    retval.insert(TOOL_POINTER);
    return retval;
}



std::set<Tool::Type> RobotTool::getTypes() const
{
    return mTypes;
}

vtkPolyDataPtr RobotTool::getGraphicsPolyData() const
{
    return mPolyData;
}

Transform3D RobotTool::get_prMt() const
{
    return m_prMt;
}

bool RobotTool::getVisible() const
{
    return true;
}

QString RobotTool::getUid() const
{
    return Tool::mUid;
}

QString RobotTool::getName() const
{
    return Tool::mName;
}

bool RobotTool::isCalibrated() const
{
    return true;
}

double RobotTool::getTimestamp() const
{
    return mTimestamp;
}

// Just use the tool tip offset from the tool manager
double RobotTool::getTooltipOffset() const
{
    return ToolImpl::getTooltipOffset();
}

// Just use the tool tip offset from the tool manager
void RobotTool::setTooltipOffset(double val)
{
    ToolImpl::setTooltipOffset(val);
}

Transform3D RobotTool::getCalibration_sMt() const
{
    return m_sMt_calibration;
}

bool RobotTool::isInitialized() const
{
    return true;
}

void RobotTool::setVisible(bool vis)
{
    CX_LOG_WARNING() << "Cannot set visible on a openigtlink tool.";
}


void RobotTool::toolTransformAndTimestampSlot(Transform3D prMs, double timestamp)
{
    mTimestamp = timestamp;// /1000000;
    Transform3D prMt = prMs;
    Transform3D prMt_filtered = prMt;

    (*mPositionHistory)[mTimestamp] = prMt; // store original in history
    m_prMt = prMt_filtered;
    emit toolTransformAndTimestamp(m_prMt, mTimestamp);
}

void RobotTool::calculateTpsSlot()
{
}

void RobotTool::createPolyData()
{
    mPolyData = Tool::createDefaultPolyDataCone();
}

void RobotTool::toolVisibleSlot(bool on)
{
//    if (on)
//        mTpsTimer.start(1000); //calculate tps every 1 seconds
//    else
//        mTpsTimer.stop();
}


} // namespace cx
