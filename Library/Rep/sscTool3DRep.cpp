#include "sscTool3DRep.h"
#include "sscTool.h"

namespace ssc
{

Tool3DRep::Tool3DRep(const std::string& uid, const std::string& name, ToolPtr tool) : RepImpl(uid, name)
{
}

Tool3DRep::~Tool3DRep()
{
}

} // namespace ssc
