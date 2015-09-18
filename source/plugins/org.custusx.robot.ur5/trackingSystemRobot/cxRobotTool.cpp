#include "cxRobotTool.h"
#include "cxTypeConversions.h"
#include "cxDoubleProperty.h"

namespace cx
{

RobotTool::RobotTool(ToolPtr base, Ur5RobotPtr robot):
    ToolImpl(base->getUid(), "Robot tracker" +base->getName()),
    mBase(base),
    mUr5Robot(robot)
{
    connect(mBase.get(), &Tool::toolProbeSector, this, &Tool::toolProbeSector);
    connect(mBase.get(), &Tool::tooltipOffset, this, &Tool::tooltipOffset);
    connect(mBase.get(), &Tool::toolTransformAndTimestamp, this, &RobotTool::onToolTransformAndTimestamp);
    connect(mBase.get(), &Tool::toolVisible, this, &Tool::toolVisible);
    connect(mBase.get(), &Tool::tooltipOffset, this, &Tool::tooltipOffset);
    connect(mBase.get(), &Tool::tps, this, &Tool::tps);
}

RobotTool::~RobotTool()
{

}

void RobotTool::onToolTransformAndTimestamp(Transform3D prMt, double timestamp)
{
    m_prMt = mUr5Robot->getCurrentState().baseMee;
    emit toolTransformAndTimestamp(m_prMt, timestamp);
}

std::set<Tool::Type> RobotTool::getTypes() const
{
    return mBase->getTypes();
}

vtkPolyDataPtr RobotTool::getGraphicsPolyData() const
{
    return mBase->getGraphicsPolyData();
}

Transform3D RobotTool::get_prMt() const
{
    return m_prMt;
}

bool RobotTool::getVisible() const
{
    return mBase->getVisible();
}

QString RobotTool::getUid() const
{
    return mUid;
}

QString RobotTool::getName() const
{
    return mName;
}

bool RobotTool::isCalibrated() const
{
    return mBase->isCalibrated();
}

double RobotTool::getTimestamp() const
{
    return mBase->getTimestamp();
}

// Just use the tool tip offset from the tool manager
double RobotTool::getTooltipOffset() const
{
    return mBase->getTooltipOffset();
}

// Just use the tool tip offset from the tool manager
void RobotTool::setTooltipOffset(double val)
{
    mBase->setTooltipOffset(val);
}

Transform3D RobotTool::getCalibration_sMt() const
{
    return mBase->getCalibration_sMt();
}

std::map<int, Vector3D> RobotTool::getReferencePoints() const
{
    return mBase->getReferencePoints();
}

bool RobotTool::isInitialized() const
{
    return mBase->isInitialized();
}

void RobotTool::set_prMt(const Transform3D& prMt, double timestamp)
{
    mBase->set_prMt(prMt, timestamp);

}

void RobotTool::setVisible(bool vis)
{
    mBase->setVisible(vis);
}



} // namespace cx
