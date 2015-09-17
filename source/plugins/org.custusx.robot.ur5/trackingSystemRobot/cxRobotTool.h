#ifndef CXROBOTTOOL_H
#define CXROBOTTOOL_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxToolImpl.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT RobotTool: public ToolImpl
{
public:
    RobotTool();
    virtual ~RobotTool();
};

} /* namespace cx */

#endif // CXROBOTTOOL_H
