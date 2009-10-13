#ifndef SSC_FORWARDDECLARARATIONS_H_
#define SSC_FORWARDDECLARARATIONS_H_

#include <boost/shared_ptr.hpp>

/**\file sscForwardDeclarations.h
 *
 * Include this file when the types and not the
 * full definitions of the ssc is needed.
 */

namespace ssc {

class View;

// data
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class ManualTool> ManualToolPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Mesh> MeshPtr;
typedef boost::shared_ptr<class ImageTF3D> ImageTF3DPtr;
typedef boost::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;
typedef boost::shared_ptr<class GPUImageBuffer> GPUImageBufferPtr;
typedef boost::weak_ptr<class GPUImageBuffer> GPUImageBufferWeakPtr;

// reps
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;
typedef boost::shared_ptr<class ProgressiveLODVolumetricRep> ProgressiveLODVolumetricRepPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class SliceRepSW> SliceRepSWPtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class SlicePlaneRep> SlicePlaneRepPtr;

} // namespace ssc

#endif /*SSC_FORWARDDECLARARATIONS_H_*/
