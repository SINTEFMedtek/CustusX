#ifndef CX_FORWARDDECLARARATIONS_H_
#define CX_FORWARDDECLARARATIONS_H_

#include <boost/shared_ptr.hpp>
#include "sscForwardDeclarations.h"

/**\file cxForwardDeclarations.h
 *
 * Include this file when the types and not the
 * full definitions of the ssc is needed.
 */

namespace cx {
//typedef boost::shared_ptr<class InriaRep2D> InriaRep2DPtr;
typedef boost::shared_ptr<class LandmarkRep> LandmarkRepPtr;

class View2D;
class View3D;
typedef boost::shared_ptr<class ViewGroup> ViewGroupPtr;
typedef boost::shared_ptr<class ViewGroup2D> ViewGroup2DPtr;
typedef boost::shared_ptr<class ViewGroup3D> ViewGroup3DPtr;
typedef boost::shared_ptr<class ViewGroupInria> ViewGroupInriaPtr;
typedef boost::shared_ptr<class ViewWrapper> ViewWrapperPtr;


} // namespace cx

#endif /*CX_FORWARDDECLARARATIONS_H_*/
