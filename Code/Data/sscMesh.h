#ifndef SSCMESH_
#define SSCMESH_

#include <boost/shared_ptr.hpp>

namespace ssc
{

/**Defines a geometric structure in space..?
 * 
 */
class Mesh
{
public:
	virtual ~Mesh();
};

typedef boost::shared_ptr<Mesh> MeshPtr;

} // namespace ssc

#endif /*SSCMESH_*/
