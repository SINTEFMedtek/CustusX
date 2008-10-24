#ifndef SSCTOOL3DREP_H_
#define SSCTOOL3DREP_H_

#include <boost/shared_ptr.hpp>
#include "sscRepImpl.h"

namespace ssc
{
typedef boost::shared_ptr<class Tool> ToolPtr;

/**TODO
 */
class Tool3DRep : public RepImpl
{
public:
	Tool3DRep(const std::string& uid, const std::string& name, ToolPtr tool);
	virtual ~Tool3DRep();
};

} // namespace ssc

#endif /*SSCTOOL3DREP_H_*/
