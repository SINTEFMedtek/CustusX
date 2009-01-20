#ifndef SSC_FORWARDDECLARARATIONS_H_
#define SSC_FORWARDDECLARARATIONS_H_

#include <boost/shared_ptr.hpp>

/**\file sscForwardDeclarations.h
 *
 * Include this file when the types and not the
 * full definitions of the ssc is needed.
 */

namespace ssc {

// data
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Mesh> MeshPtr;

// reps
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class SliceRepSW> SliceRepSWPtr;

} // namespace ssc

#endif /*SSC_FORWARDDECLARARATIONS_H_*/
